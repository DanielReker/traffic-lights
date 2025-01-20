#pragma once

#include <string>
#include <map>
#include <random>

#include "Node.hpp"
#include "Path.hpp"
#include "TrafficLights.hpp"
#include "Car.hpp"


namespace traffic_lights {
namespace model {

class Crossroad {
private:
	std::string name_;
	Vector2d bounds_min_, bounds_max_;
	std::map<std::string, std::shared_ptr<Node>> nodes_;
	std::map<std::string, std::shared_ptr<Path>> paths_;
	std::map<std::string, std::shared_ptr<TrafficLights>> traffic_lights_;
	std::vector<Car> cars_;
	std::vector<std::shared_ptr<Node>> source_nodes_;

	std::map<std::string, std::vector<std::shared_ptr<Path>>> paths_from_node_;

	std::mt19937 random_gen_;

public:
	Crossroad(const boost::json::value& crossroad)
		: name_(crossroad.at_pointer("/name").as_string().c_str()),
		  bounds_min_(Vector2d(
			  crossroad.at_pointer("/bounds/min/x").to_number<double>(),
			  crossroad.at_pointer("/bounds/min/y").to_number<double>()
		  )), 
		  bounds_max_(Vector2d(
			  crossroad.at_pointer("/bounds/max/x").to_number<double>(),
			  crossroad.at_pointer("/bounds/max/y").to_number<double>()
		  )),
	      random_gen_(std::random_device{}()) {
		
		auto now = std::chrono::steady_clock::now();
		std::map<std::string, std::vector<std::shared_ptr<Path>>> paths_to_node;

		for (const auto& traffic_lights_json : crossroad.at_pointer("/trafficLights").as_array()) {
			auto traffic_lights = std::make_shared<TrafficLights>(traffic_lights_json, now);
			traffic_lights_[traffic_lights->name()] = traffic_lights;
		}

		for (const auto& node_json : crossroad.at_pointer("/nodes").as_array()) {
			auto node = std::make_shared<Node>(node_json, traffic_lights_);
			nodes_[node->name()] = node;
		}

		for (const auto& path_json : crossroad.at_pointer("/paths").as_array()) {
			auto path = std::make_shared<Path>(path_json, nodes_);
			paths_[path->name()] = path;

			paths_from_node_[path->from().name()].push_back(path);
			paths_to_node[path->to().name()].push_back(path);
		}

		for (const auto& [node_name, node] : nodes_) {
			if (!paths_to_node.contains(node->name())) {
				source_nodes_.push_back(node);
			}
		}

	}

	void GenerateCar() {
		constexpr double min_speed_kmh = 10;
		constexpr double max_speed_kmh = 30;

		std::uniform_real_distribution<double> speed_dist(min_speed_kmh, max_speed_kmh);
		std::uniform_int_distribution<std::size_t> index_dist(0, source_nodes_.size() - 1);

		cars_.push_back(Car(
			paths_from_node_,
			speed_dist(random_gen_),
			*source_nodes_[index_dist(random_gen_)],
			random_gen_
		));

		spdlog::info("Spawned car at ({}, {})", cars_.back().position().x, cars_.back().position().y);
	}

	void Update(double time_elapsed_sec) {
		for (auto& car : cars_) {
			car.Move(time_elapsed_sec);
		}
	}

	const auto& name() const { return name_; }
	const auto& nodes() const { return nodes_; }
	const auto& paths() const { return paths_; }
	const auto& bounds_min() const { return bounds_min_; }
	const auto& bounds_max() const { return bounds_max_; }
	const auto& traffic_lights() const { return traffic_lights_; }
	const auto& cars() const { return cars_; }
};

} // namespace model
} // namespace traffic_lights

