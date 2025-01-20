#pragma once

#include <cmath>

#include "NCursesIncludes.hpp"
#include "ImageStorage.hpp"

#include <Crossroad.hpp>
#include <TrafficLights.hpp>
#include <Path.hpp>
#include <CubicSpline2.hpp>


namespace traffic_lights {
namespace ncurses {

using namespace traffic_lights::model;

class CrossroadRenderer {
private:
	ImageStorage image_storage_;


	static double TransformCoordinate(
		double from_point,
		double from_bound_min,
		double from_bound_max,
		double to_bound_min,
		double to_bound_max
	) {
		double t = (from_point - from_bound_min) / (from_bound_max - from_bound_min);
		return std::lerp(to_bound_min, to_bound_max, t);
	}

	static Vector2d TransformCoordinates(
		const Vector2d& field_coordinates,
		const Crossroad& crossroad,
		const Vector2d& image_size
	) {
		return Vector2d(
			TransformCoordinate(field_coordinates.x, crossroad.bounds_min().x, crossroad.bounds_max().x, -0.5, image_size.x - 0.5),
			TransformCoordinate(field_coordinates.y, crossroad.bounds_min().y, crossroad.bounds_max().y, image_size.y - 0.5, -0.5)
		);
	}


public:
	bool render_graph;

	CrossroadRenderer(const ImageStorage& image_storage)
		: image_storage_(image_storage),
		render_graph(false) {
	}

	void Render(const Crossroad& crossroad, WINDOW* window) {
		auto field = image_storage_.GetImage(crossroad.name());
		
		for (const auto& [traffic_lights_name, traffic_lights] : crossroad.traffic_lights()) {
			auto coordinates = TransformCoordinates(traffic_lights->position(), crossroad, Vector2d(field.size().x, field.size().y));
			field.AddOverlay(image_storage_.GetImage("traffic-lights-casing", traffic_lights->angle()), coordinates);
			if (traffic_lights->state() == "red")
				field.AddOverlay(image_storage_.GetImage("traffic-lights-red", traffic_lights->angle()), coordinates);
			if (traffic_lights->state() == "yellow")
				field.AddOverlay(image_storage_.GetImage("traffic-lights-yellow", traffic_lights->angle()), coordinates);
			if (traffic_lights->state() == "green")
				field.AddOverlay(image_storage_.GetImage("traffic-lights-green", traffic_lights->angle()), coordinates);
		}

		for (const auto& car : crossroad.cars()) {
			if (!car.IsActive()) 
				continue;

			auto coordinates = TransformCoordinates(car.position(), crossroad, Vector2d(field.size().x, field.size().y));
			field.AddOverlay(image_storage_.GetImage("car", car.angle()), coordinates);
		}

		if (render_graph) {
			for (const auto& [node_name, node] : crossroad.nodes()) {
				auto coordinates = TransformCoordinates(node->position(), crossroad, Vector2d(field.size().x, field.size().y));
				field.AddOverlay(image_storage_.GetImage("car", node->angle()), coordinates);
			}

			for (const auto& [path_name, path] : crossroad.paths()) {
				auto dot = Image::Dot(Color::kRed);
				const CubicSpline2d& spline = path->spline();
				for (double t = 0; t <= 1; t += 0.001) {
					auto coordinates = TransformCoordinates(spline.point(t), crossroad, Vector2d(field.size().x, field.size().y));
					auto direction = spline.direction(t);
					field.AddOverlay(dot, coordinates);
				}
			}
		}

		field.RenderTo(window);
	}

};

} // namespace ncurses
} // namespace traffic_lights

