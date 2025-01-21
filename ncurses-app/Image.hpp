#pragma once

#include <boost/algorithm/algorithm.hpp>
#include <boost/locale.hpp>
#include <boost/json.hpp>

#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <exception>
#include <cmath>

#include "Pixel.hpp"
#include "Vector2.hpp"
#include "LoggerIncludes.hpp"


namespace traffic_lights {
namespace ncurses {

using namespace traffic_lights::model;

class Image {
private:
	std::vector<Pixel> pixels_;
	Vector2i size_;

	Pixel& pixel(const Vector2i& position) { return pixels_[GetPixelIndex(position)]; }

	Image(const Vector2i& size, const Pixel& pixel) : size_(size), pixels_(size.x * size.y, pixel) {}

	int GetPixelIndex(const Vector2i& position) const { return position.y * size_.x + position.x; }

	static Color GetColorFromCharacter(const std::string& character) {
		switch (character[0]) {
			case 'W': return Color::kWhite;
			case 'R': return Color::kRed;
			case 'G': return Color::kGreen;
			case 'B': return Color::kBlue;
			case 'C': return Color::kCyan;
			case 'M': return Color::kMagenta;
			case 'Y': return Color::kYellow;
			case 'D': return Color::kBlack;
			default: return Color::kTransparent;
		}
	}

	static wchar_t Utf8CharacterToWchar(const std::string& utf8_character) {
		constexpr wchar_t kUnknownCharacter = L'\uFFFD';
		
		auto wchar_character = boost::locale::conv::utf_to_utf<wchar_t>(utf8_character);

		if (wchar_character.size() == 1)
			return wchar_character[0];
		else
			return kUnknownCharacter;
	}

	template <class ActionT>
	static void ParseFile(const Vector2i& size, const std::filesystem::path& file, ActionT action) {
		std::ifstream characters_stream(file);
		std::string characters = {
			std::istreambuf_iterator<char>(characters_stream),
			std::istreambuf_iterator<char>()
		};
		characters = boost::locale::normalize(
			characters,
			boost::locale::norm_type::norm_nfc,
			boost::locale::generator()("")
		);

		Vector2i position(0, 0);
		boost::locale::boundary::segment_index character_segments{
			boost::locale::boundary::character,
			characters.begin(),
			characters.end(),
			boost::locale::generator()("")
		};
		for (const auto& character_segment : character_segments) {
			std::string character(character_segment);
			if (character[0] == '\n') {
				position.y++;
				position.x = 0;
				if (position.y >= size.y)
					break;
			} else if (position.x < size.x && position.y < size.y) {
				//pixel(position).character = character;
				action(position, character);
				position.x++;
			}
		}
	}

	void ParseCharacters(const Vector2i& size, const std::filesystem::path& file) {	
		ParseFile(size, file, [this](const Vector2i& position, const std::string& character) -> void {
			pixel(position).character = character;
		});
	}

	void ParseColors(const Vector2i& size, const std::filesystem::path& file) {
		ParseFile(size, file, [this](const Vector2i& position, const std::string& character) -> void {
			pixel(position).color = GetColorFromCharacter(character);
		});
	}

	void ParseBackgroundColors(const Vector2i& size, const std::filesystem::path& file) {
		ParseFile(size, file, [this](const Vector2i& position, const std::string& character) -> void {
			pixel(position).background_color = GetColorFromCharacter(character);
		});
	}


public:
	Image() = delete;

	const Pixel& pixel(const Vector2i& position) const { return pixels_[GetPixelIndex(position)]; }


	Vector2i size() const { return size_; }


	void AddOverlay(const Image& overlay, Vector2d position, Vector2d overlay_center = { 0.5, 0.5 }) {
		Vector2d offset(
			overlay.size().x * -overlay_center.x,
			overlay.size().y * -overlay_center.y
		);

		Vector2i overlay_position(0, 0);
		for (overlay_position.x = 0; overlay_position.x < overlay.size().x; overlay_position.x++) {
			for (overlay_position.y = 0; overlay_position.y < overlay.size().y; overlay_position.y++) {
				Vector2d overlay_position_double(
					static_cast<double>(overlay_position.x),
					static_cast<double>(overlay_position.y)
				);

				Vector2d pixel_position_double = position + overlay_position_double + offset;
				
				Vector2i pixel_position(
					std::round(pixel_position_double.x),
					std::round(pixel_position_double.y)
				);
				if (0 <= pixel_position.x && pixel_position.x < size().x && 0 <= pixel_position.y && pixel_position.y < size().y) {
					Pixel old_pixel = pixel(pixel_position);
					Pixel overlay_pixel = overlay.pixel(overlay_position);
					Pixel new_pixel = old_pixel.Mix(overlay_pixel);
					pixel(pixel_position) = new_pixel;
				}
			}
		}
	}

	void RenderTo(WINDOW* window) {
		werase(window);

		Vector2i image_position(0, 0);
		for (image_position.x = 0; image_position.x < size().x; image_position.x++) {
			for (image_position.y = 0; image_position.y < size().y; image_position.y++) {
				Pixel to_render = pixel(image_position);

				if (to_render.background_color == Color::kTransparent)
					to_render.background_color = Color::kBlack;

				if (to_render.color == Color::kTransparent)
					to_render.color = Color::kBlack;

				Vector2i window_position = image_position;


#ifdef NCURSES
				attron(COLOR_PAIR(GetColorPair(to_render.color, to_render.background_color)));
				mvwprintw(
					window,
					window_position.y,
					window_position.x,
					to_render.character.c_str()
				);
#elif PDCURSES
				mvwaddch(
					window,
					window_position.y,
					window_position.x,
					Utf8CharacterToWchar(to_render.character) | COLOR_PAIR(GetColorPair(to_render.color, to_render.background_color))
				);

#endif
			}
		}

		wrefresh(window);
	}

	static Image Rectangle(
		const Vector2i& size,
		Color background_color = Color::kBlack,
		const std::string& character = " ",
		Color color = Color::kWhite
	) {
		return Image(size, Pixel(character, color, background_color));
	}

	static Image Dot(
		Color color = Color::kWhite,
		bool filled = true,
		Color background_color = Color::kTransparent
	) {
		std::string character = filled ? "\xE2\x97\x8F" : "\xE2\x97\x8B";
		return Image(Vector2i(1, 1), Pixel(character, color, background_color));
	}

	static Image FromFiles(const Vector2i& size, const std::filesystem::path& directory) {
		Image result(size, Pixel(" ", Color::kTransparent));

		result.ParseCharacters(size, directory / "characters.txt");
		result.ParseColors(size, directory / "colors.txt");
		result.ParseBackgroundColors(size, directory / "background-colors.txt");

		return result;
	}

	static short GetColorPair(Color color, Color background_color) {
		constexpr short kOffset = 1;
		constexpr short kNumberOfColors = 8;
		return kOffset + static_cast<short>(color) * kNumberOfColors + static_cast<short>(background_color);
	}

	static void InitColorPairs() {
		static const std::vector<Color> colors = {
			Color::kBlack,
			Color::kRed,
			Color::kGreen,
			Color::kBlue,
			Color::kCyan,
			Color::kMagenta,
			Color::kYellow,
			Color::kWhite
		};

		for (const auto& color : colors) {
			for (const auto& background_color : colors) {
				init_pair(
					GetColorPair(color, background_color),
					static_cast<short>(color),
					static_cast<short>(background_color)
				);
			}
		}
	}

};

} // namespace ncurses
} // namespace traffic_lights

