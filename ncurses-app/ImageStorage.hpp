#pragma once

#include <map>
#include <filesystem>

#include "Image.hpp"


namespace traffic_lights {
namespace ncurses {

class ImageStorage {
private:
	std::map<std::string, std::vector<std::pair<double, Image>>> images_;


public:
	const Image& GetImage(const std::string& name, double preferred_angle = 0.0) {
		const Image* result = nullptr;
		double min_angle_diff = std::numeric_limits<double>::max();

		for (const auto& [angle, image] : images_[name]) {
			double angle_diff = std::abs(preferred_angle - angle);
			if (angle_diff < min_angle_diff) {
				min_angle_diff = angle_diff;
				result = &image;
			}
		}

		if (result)
			return *result;
		else
			throw std::invalid_argument(std::format("Image '{}' not found", name));
	}

	ImageStorage(const std::filesystem::path& directory) {
		for (const auto& dir_entry : std::filesystem::recursive_directory_iterator{ directory }) {
			if (
				dir_entry.is_directory() &&
				std::filesystem::directory_entry(dir_entry.path() / "metadata.json").exists() &&
				!std::filesystem::directory_entry(dir_entry.path() / "metadata.json").is_directory()
			) {
				auto image_directory = dir_entry.path();
				spdlog::info("Loading image from {}", image_directory.string());

				std::ifstream metadata_stream(image_directory / "metadata.json");
				auto metadata = boost::json::parse(metadata_stream);
				std::string name = metadata.at_pointer("/name").as_string().c_str();
				auto angle = metadata.at_pointer("/angle").to_number<double>();
				Vector2i size(
					metadata.at_pointer("/size/x").to_number<int>(),
					metadata.at_pointer("/size/y").to_number<int>()
				);

				spdlog::info("Image | Name: {}, angle: {}, size: ({}, {})", name, angle, size.x, size.y);

				images_[name].push_back({ angle, Image::FromFiles(size, image_directory) });
			}
		}
			
	}
};

} // namespace ncurses
} // namespace traffic_lights

