#pragma once


#include <cwctype>

#include "Color.hpp"
#include "NCursesIncludes.hpp"


namespace traffic_lights {
namespace ncurses {

struct Pixel {
	Color background_color;
	Color color;
	std::string character;

	Pixel(const std::string& character, Color color, Color background_color = Color::kTransparent)
		: character(character), color(color), background_color(background_color) { }

	Pixel Mix(Pixel upper) {
		// Non-transparent background of upper pixel completely hides underlying pixel
		if (upper.background_color != Color::kTransparent)
			return upper;

		// Upper pixel is completely transparent so it does not affect underlying pixel
		if (upper.color == Color::kTransparent || std::iswspace(upper.character[0]))
			return *this;

		Pixel result = *this;
		result.color = upper.color;
		result.character = upper.character;

		return result;
	}
};

} // namespace ncurses
} // namespace traffic_lights

