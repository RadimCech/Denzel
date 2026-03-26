#include "filterChain.h"
#include <atomic>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <jack/jack.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

void openJackClient(FilterChain *filterChain, std::atomic<bool> *running,
                    std::atomic<float> *sampleRate);

class Graph {
public:
  Graph(FilterChain *fc) : filterChain(fc) {}

  std::vector<int> operator()(int width, int height) const {
    std::vector<int> output(width);

    const float *buffer = filterChain->getScopeBuffer();
    int writeIdx = filterChain->getScopeWriteIndex();

    int startIdx = (writeIdx - width + FilterChain::kScopeBufferSize) %
                   FilterChain::kScopeBufferSize;

    for (int i = 0; i < width; ++i) {
      int idx = (startIdx + i) % FilterChain::kScopeBufferSize;
      float sample = buffer[idx];

      float v = (sample * 0.5f + 0.5f) * height;
      if (v < 0)
        v = 0;
      if (v >= height)
        v = height - 1;
      output[i] = static_cast<int>(v);
    }
    return output;
  }

private:
  FilterChain *filterChain;
};

int main() {
  const int NUM_SLIDERS = 10;
  std::atomic<bool> running(true);
  std::atomic<float> shared_sampleRate{44100.0f};

  FilterChain filterChain;
  std::vector<float> slider_values(NUM_SLIDERS, 0.0f);
  std::thread t([&filterChain, &running, &shared_sampleRate]() {
    openJackClient(&filterChain, &running, &shared_sampleRate);
  });

  auto ascii_art = ftxui::paragraph(R"(
 ______                     __ 
|      |____ ____ ____ ____|  |
|  |   |  -_|    |-- _|  -_|  |
|_____/|____|__|_|____|____|__|
 

                                )") |
                   ftxui::color(ftxui::Color::LightSkyBlue1);
  ;

  std::vector<std::string> slider_labels = {"31Hz",  "63Hz", "125Hz", "250Hz",
                                            "500Hz", "1kHz", "2kHz",  "4kHz",
                                            "8kHz",  "16kHz"};

  std::vector<std::tuple<std::string, float, float, float>> slider_options(
      NUM_SLIDERS);

  for (int i = 0; i < NUM_SLIDERS; ++i) {
    slider_options[i] = std::make_tuple(std::string(""), -12.0f, 12.0f, 2.0f);
  }

  auto updateFilters = [&]() {
    if (shared_sampleRate.load() > 0) {
      filterChain.configure(shared_sampleRate.load(), slider_values);
    }
  };

  std::vector<ftxui::Component> slider_containers;
  for (int i = 0; i < NUM_SLIDERS; ++i) {
    ftxui::SliderOption<float> option;
    option.value = &slider_values[i];
    option.min = std::get<1>(slider_options[i]);
    option.max = std::get<2>(slider_options[i]);
    option.increment = std::get<3>(slider_options[i]);
    option.on_change = updateFilters;
    option.color_active = ftxui::Color::LightSkyBlue1;
    option.direction = ftxui::Direction::Up;

    auto slider = ftxui::Slider(option) |
                  ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 10) |
                  ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 4) |
                  ftxui::border;

    auto label = ftxui::Renderer([i, &slider_labels] {
      return ftxui::text(slider_labels[i]) | ftxui::center;
    });

    auto container = ftxui::Container::Vertical({slider, label});
    slider_containers.push_back(container);
  }

  auto sliders = ftxui::Container::Horizontal(slider_containers);

  Graph graph_data(&filterChain);

  auto component = ftxui::Container::Vertical({
      sliders,
  });

  auto screen = ftxui::ScreenInteractive::Fullscreen();

  auto renderer = ftxui::Renderer(component, [&] {
    screen.RequestAnimationFrame();
    auto content = ftxui::vbox({
        ascii_art,
        sliders->Render(),
        ftxui::graph(std::ref(graph_data)) |
            ftxui::color(ftxui::Color::GrayLight) |
            ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 8),
    });

    return ftxui::hcenter(content | ftxui::yflex |
                          ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 80));
  });

  screen.Loop(renderer);

  running = false;
  if (t.joinable())
    t.join();

  return 0;
}
