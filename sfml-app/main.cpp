#include <SFML/Graphics.hpp>

#include <fstream>
#include <filesystem>

#include "CrossroadRenderer.hpp"


using namespace std::chrono_literals;
using namespace traffic_lights::sfml;
using namespace traffic_lights::model;

std::atomic<bool> running(true);

void control_crossroad(Crossroad& crossroad) {
    while (running) {
        std::this_thread::sleep_for(5ms);
        crossroad.Update();
    }
}

int main() {
    auto file_logger = spdlog::basic_logger_mt("file_logger", "logs.txt", true);
    spdlog::flush_every(1s);
    spdlog::set_default_logger(file_logger);

#ifndef NDEBUG
    file_logger->set_level(spdlog::level::trace);
#endif

    spdlog::info("SFML Traffic Lights started");

    SPDLOG_TRACE("Trace logging is enabled");
    SPDLOG_DEBUG("Debug logging is enabled");

    CrossroadRenderer crossroad_renderer("assets/images/");

    std::ifstream crossroad1_stream(std::filesystem::current_path() / "assets" / "crossroads" / "crossroad1.json");
    auto crossroad1_json = boost::json::parse(crossroad1_stream);
    Crossroad crossroad(crossroad1_json);

    std::thread model_thread(control_crossroad, std::ref(crossroad));

    sf::RenderWindow window(sf::VideoMode(750, 400), "Traffic lights");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.scancode == sf::Keyboard::Scan::Escape) {
                    running = false;
                    window.close();
                } else if (event.key.scancode == sf::Keyboard::Scan::C) {
                    crossroad.GenerateCar();
                } else if (event.key.scancode == sf::Keyboard::Scan::G) {
                    crossroad_renderer.render_graph = !crossroad_renderer.render_graph;
                }
            }
        }

        crossroad_renderer.Render(crossroad, window);
    }

    model_thread.join();

    spdlog::info("SFML Traffic Lights finished");
    return 0;
}