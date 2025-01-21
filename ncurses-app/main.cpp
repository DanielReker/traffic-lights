#include "NCursesIncludes.hpp"
#include "Image.hpp"
#include "LoggerIncludes.hpp"
#include "ImageStorage.hpp"
#include "CrossroadRenderer.hpp"

#include <Crossroad.hpp>

#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>


using namespace std::chrono_literals;
using namespace traffic_lights::model;

std::atomic<bool> running(true);

void control_crossroad(Crossroad& crossroad) {
    while (running) {
        std::this_thread::sleep_for(5ms);
        crossroad.Update();
    }
}

int main() {
    using namespace traffic_lights::model;
    using namespace traffic_lights::ncurses;

    setlocale(LC_ALL, "");

    auto file_logger = spdlog::basic_logger_mt("file_logger", "logs.txt", true);
    spdlog::flush_every(1s);
    spdlog::set_default_logger(file_logger);

#ifndef NDEBUG
    file_logger->set_level(spdlog::level::trace);
#endif

    spdlog::info("NCurses Traffic Lights started");

    SPDLOG_TRACE("Trace logging is enabled");
    SPDLOG_DEBUG("Debug logging is enabled");

#ifdef NCURSES
    spdlog::info("Using NCurses as rendering backend");
#elif PDCURSES
    spdlog::info("Using PDCurses as rendering backend");
#endif

    initscr();
    start_color();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);
    Image::InitColorPairs();

    spdlog::info("Curses initialized");


    CrossroadRenderer crossroad_renderer(ImageStorage(std::filesystem::current_path() / "assets" / "images"));

    std::ifstream crossroad1_stream(std::filesystem::current_path() / "assets" / "crossroads" / "crossroad1.json");
    auto crossroad1_json = boost::json::parse(crossroad1_stream);
    Crossroad crossroad(crossroad1_json);
    
    std::thread model_thread(control_crossroad, std::ref(crossroad));

    while (running) {
        crossroad_renderer.Render(crossroad, stdscr);

        int ch = getch();
        if (ch == 'C' || ch == 'c') {
            crossroad.GenerateCar();
        } else if (ch == 'G' || ch == 'g') {
            crossroad_renderer.render_graph = !crossroad_renderer.render_graph;
        } else if (ch == 27) { // Escape
            running = false;
        }
    }

    model_thread.join();

    endwin();

    spdlog::info("NCurses traffic lights finished");
    return 0;
}
