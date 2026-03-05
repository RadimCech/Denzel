#include <jack/jack.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <string>
#include <vector>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/component/screen_interactive.hpp>


void openJackClient(float *volume, std::atomic<bool> *running);

int main() {
    const int NUM_SLIDERS = 10;
    float volume = 0.5f;
    std::atomic<bool> running(true);

    std::thread t([&volume, &running]() { openJackClient(&volume, &running); });

    std::string text1;
    
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
_____________________________________________________ 
                                                              )") | ftxui::color(ftxui::Color::Red);;

    std::vector<float> slider_values(NUM_SLIDERS, 1.0f);
    std::vector<std::tuple<std::string, float, float, float>> slider_options(NUM_SLIDERS);
    
    for (int i = 0; i < NUM_SLIDERS; ++i) {
        slider_options[i] = std::make_tuple(std::string(10, 'A' + i), 0.5f, 1.5f, 0.1f);
        slider_values[i] = 1;
    }
    
    std::vector<ftxui::Component> slider_components;
    for (int i = 0; i < NUM_SLIDERS; ++i) {
        slider_components.push_back(
            ftxui::Slider(std::get<0>(slider_options[i]) + ":", 
                         &slider_values[i], 
                         std::get<1>(slider_options[i]),
                         std::get<2>(slider_options[i]),
                         std::get<3>(slider_options[i]))
        );
    }
    
    auto sliders = ftxui::Container::Vertical(slider_components);
    
    auto component = ftxui::Container::Vertical({
        sliders,
    });

    auto renderer = ftxui::Renderer(component, [&] {
        auto content = ftxui::vbox({
            ascii_art,
            sliders->Render(), 
        });
        
        return ftxui::hcenter(content | ftxui::yflex | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 80));
    });

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    screen.Loop(renderer);

    running = false;
    if (t.joinable()) t.join();

    return 0;
}
