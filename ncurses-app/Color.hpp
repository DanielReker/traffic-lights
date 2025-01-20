#pragma once

#include "NCursesIncludes.hpp"


namespace traffic_lights {
namespace ncurses {

enum class Color {
	kTransparent = -1,
	kBlack = COLOR_BLACK,
	kRed = COLOR_RED,
	kGreen = COLOR_GREEN,
	kBlue = COLOR_BLUE,
	kCyan = COLOR_CYAN,
	kMagenta = COLOR_MAGENTA,
	kYellow = COLOR_YELLOW,
	kWhite = COLOR_WHITE
};

} // namespace ncurses
} // namespace traffic_lights

