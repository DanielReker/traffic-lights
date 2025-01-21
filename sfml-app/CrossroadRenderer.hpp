#pragma once

#include <cmath>

#include <SFML/Graphics.hpp>

#include <Crossroad.hpp>
#include <TrafficLights.hpp>
#include <Path.hpp>
#include <CubicSpline2.hpp>


namespace traffic_lights {
namespace sfml {

using namespace traffic_lights::model;

class CrossroadRenderer {
private:
	std::map<std::string, sf::Texture> textures_;
	std::map<std::string, sf::Sprite> sprites_;

	static void ResizeSprite(sf::Sprite& sprite, const sf::Vector2f& size, bool center_origin = true) {
		sprite.setScale(
			size.x / sprite.getTexture()->getSize().x,
			size.y / sprite.getTexture()->getSize().y
		);

		if (center_origin) {
			sprite.setOrigin(sprite.getTexture()->getSize().x / 2.0f, sprite.getTexture()->getSize().y / 2.0f);
		}
	}


public:
	bool render_graph;

	CrossroadRenderer(const std::string& textures_directory) :
		render_graph(false) {

		constexpr float kCarWidth = 2.0f;
		constexpr float kCarLength = 4.0f;

		const static sf::Vector2f kTrafficLightsSize(5.0f, 1.0f);

		textures_["car"].loadFromFile(textures_directory + "car.png");
		textures_["car"].setSmooth(true);
		sprites_["car"].setTexture(textures_["car"]);
		ResizeSprite(sprites_["car"], sf::Vector2f(kCarLength, kCarWidth));

		textures_["traffic-lights-red"].loadFromFile(textures_directory + "traffic-lights-red.png");
		textures_["traffic-lights-red"].setSmooth(true);
		sprites_["traffic-lights-red"].setTexture(textures_["traffic-lights-red"]);
		ResizeSprite(sprites_["traffic-lights-red"], kTrafficLightsSize);

		textures_["traffic-lights-yellow"].loadFromFile(textures_directory + "traffic-lights-yellow.png");
		textures_["traffic-lights-yellow"].setSmooth(true);
		sprites_["traffic-lights-yellow"].setTexture(textures_["traffic-lights-yellow"]);
		ResizeSprite(sprites_["traffic-lights-yellow"], kTrafficLightsSize);

		textures_["traffic-lights-green"].loadFromFile(textures_directory + "traffic-lights-green.png");
		textures_["traffic-lights-green"].setSmooth(true);
		sprites_["traffic-lights-green"].setTexture(textures_["traffic-lights-green"]);
		ResizeSprite(sprites_["traffic-lights-green"], kTrafficLightsSize);

		textures_["crossroad1"].loadFromFile(textures_directory + "crossroad1.png");
		textures_["crossroad1"].setSmooth(true);
		sprites_["crossroad1"].setTexture(textures_["crossroad1"]);
	}

	void Render(const Crossroad& crossroad, sf::RenderWindow& window) {
		ResizeSprite(sprites_["crossroad1"], sf::Vector2f(
			crossroad.bounds_max().x - crossroad.bounds_min().x,
			crossroad.bounds_max().y - crossroad.bounds_min().y
		), false);

		window.clear(sf::Color::Black);
		window.draw(sprites_["crossroad1"]);

		sf::View view(sf::FloatRect(
			crossroad.bounds_min().x,
			crossroad.bounds_max().y,
			crossroad.bounds_max().x - crossroad.bounds_min().x,
			crossroad.bounds_min().y - crossroad.bounds_max().y
		));
		window.setView(view);

		for (const auto& [traffic_lights_name, traffic_lights] : crossroad.traffic_lights()) {
			if (traffic_lights->state() == "red") {
				sprites_["traffic-lights-red"].setPosition(traffic_lights->position().x, traffic_lights->position().y);
				sprites_["traffic-lights-red"].setRotation(traffic_lights->angle());
				window.draw(sprites_["traffic-lights-red"]);
			} else if (traffic_lights->state() == "yellow") {
				sprites_["traffic-lights-yellow"].setPosition(traffic_lights->position().x, traffic_lights->position().y);
				sprites_["traffic-lights-yellow"].setRotation(traffic_lights->angle());
				window.draw(sprites_["traffic-lights-yellow"]);
			} else if (traffic_lights->state() == "green") {
				sprites_["traffic-lights-green"].setPosition(traffic_lights->position().x, traffic_lights->position().y);
				sprites_["traffic-lights-green"].setRotation(traffic_lights->angle());
				window.draw(sprites_["traffic-lights-green"]);
			}
		}

		for (const auto& car : crossroad.cars()) {
			if (!car.IsActive())
				continue;

			sprites_["car"].setPosition(car.position().x, car.position().y);
			sprites_["car"].setRotation(car.angle());
			window.draw(sprites_["car"]);
		}

		sf::CircleShape dot(1.0f);
		dot.setOrigin(dot.getRadius(), dot.getRadius());
		dot.setFillColor(sf::Color::Red);

		if (render_graph) {
			for (const auto& [node_name, node] : crossroad.nodes()) {
				dot.setPosition(node->position().x, node->position().y);
				window.draw(dot);
			}

			/*for (const auto& [path_name, path] : crossroad.paths()) {
				auto dot = Image::Dot(Color::kRed);
				const CubicSpline2d& spline = path->spline();
				for (double t = 0; t <= 1; t += 0.001) {
					auto coordinates = TransformCoordinates(spline.point(t), crossroad, Vector2d(field.size().x, field.size().y));
					auto direction = spline.direction(t);
					field.AddOverlay(dot, coordinates);
				}
			}*/
		}


		window.display();
	}

};

} // namespace sfml
} // namespace traffic_lights

