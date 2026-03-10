#include "filterChain.h"
#include <atomic>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>
#include <jack/jack.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

void openJackClient(FilterChain *filterChain, std::atomic<bool> *running,
                    std::atomic<float> *sampleRate);

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
                                               
         ,--.                                  
       ,--.'|    ,---,.  ,----..   ,-.----.    
   ,--,:  : |  ,'  .' | /   /   \  \    /  \   
,`--.'`|  ' :,---.'   ||   :     : ;   :    \  
|   :  :  | ||   |   .'.   |  ;. / |   | .\ :  
:   |   \ | ::   :  |-,.   ; /--`  .   : |: |  
|   : '  '; |:   |  ;/|;   | ;  __ |   |  \ :  
'   ' ;.    ;|   :   .'|   : |.' .'|   : .  /  
|   | | \   ||   |  |-,.   | '_.' :;   | |  \  
'   : |  ; .''   :  ;/|'   ; : \  ||   | ;\  \ 
|   | '`--'  |   |    \'   | '/  .':   ' | \.' 
'   : |      |   :   .'|   :    /  :   : :-'   
;   |.'      |   | ,'   \   \ .'   |   |.'     
'---'        `----'      `---`     `---'       
 
                                                              )") |
                   ftxui::color(ftxui::Color::Red);
  ;

  std::vector<std::tuple<std::string, float, float, float>> slider_options(
      NUM_SLIDERS);

  for (int i = 0; i < NUM_SLIDERS; ++i) {
    slider_options[i] = std::make_tuple(std::string(""), -12.0f, 12.0f, 1.0f);
  }

  auto updateFilters = [&]() {
    if (shared_sampleRate.load() > 0) {
      filterChain.configure(shared_sampleRate.load(), slider_values);
    }
  };

  std::vector<ftxui::Component> slider_components;
  for (int i = 0; i < NUM_SLIDERS; ++i) {
    ftxui::SliderOption<float> option;
    option.value = &slider_values[i];
    option.min = std::get<1>(slider_options[i]);
    option.max = std::get<2>(slider_options[i]);
    option.increment = std::get<3>(slider_options[i]);
    option.on_change = updateFilters;
    slider_components.push_back(ftxui::Slider(option));
  }

  auto sliders = ftxui::Container::Vertical(slider_components);

  auto component = ftxui::Container::Vertical({
      sliders,
  });

  auto renderer = ftxui::Renderer(component, [&] {
    auto content = ftxui::vbox({
        ascii_art,
        ftxui::text("") | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 2),
        sliders->Render(),
    });

    return ftxui::hcenter(content | ftxui::yflex |
                          ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 80));
  });

  auto screen = ftxui::ScreenInteractive::Fullscreen();

  screen.Loop(renderer);

  running = false;
  if (t.joinable())
    t.join();

  return 0;
}
