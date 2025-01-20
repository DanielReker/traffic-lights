#pragma once

#include <string>
#include <random>
#include <cmath>

#include "Path.hpp"
#include "Node.hpp"


namespace traffic_lights {
namespace model {

class Car {
private:
	const std::map<std::string, std::vector<std::shared_ptr<Path>>>& paths_from_node_;
	double speed_mps_;
	std::vector<std::shared_ptr<Path>> route_;
	int current_path_index_;
	double current_t_;
	std::mt19937& random_gen_;


	std::shared_ptr<Path> ChooseRandomPath(const Node& from) {
		if (!paths_from_node_.contains(from.name()) || paths_from_node_.at(from.name()).size() == 0)
			return nullptr;

		const auto& available_paths = paths_from_node_.at(from.name());

		std::uniform_int_distribution<std::size_t> dist(0, available_paths.size() - 1);
		auto index = dist(random_gen_);
		return available_paths[index];
	}

	bool NextPath() {
		if (current_path_index_ + 1 >= route_.size() || !route_[current_path_index_ + 1]->from().CanMove()) {
			current_t_ = 1;
			return false;
		}

		current_path_index_++;
		current_t_ = 0;
		return true;
	}

public:
	Car(
		const std::map<std::string, std::vector<std::shared_ptr<Path>>>& paths_from_node,
		double speed_kmh,
		const Node& from,
		std::mt19937& random_gen
	)
		: paths_from_node_(paths_from_node),
		  speed_mps_(speed_kmh / 3.6),
	      current_path_index_(0),
	      current_t_(0),
	      random_gen_(random_gen) {

		auto path = ChooseRandomPath(from);
		while (path) {
			route_.push_back(path);
			path = ChooseRandomPath(path->to());
		}

	}

	void Move(double time_seconds) {
		constexpr double t_step = 0.001;
		double moved_distance_meters = speed_mps_ * time_seconds;

		while (moved_distance_meters > 0) {
			const auto& from = route_[current_path_index_]->spline().point(current_t_);
			const auto& to = route_[current_path_index_]->spline().point(current_t_ + t_step);
			double diff_len = (to - from).length();
			moved_distance_meters -= diff_len;
			current_t_ += t_step;

			if (current_t_ >= 1 && !NextPath()) {
				return;
			}
		}
	}

	bool IsActive() const { return !(current_path_index_ + 1 == route_.size() && current_t_ >= 1); }


	const Path& current_path() const { return *(route_[current_path_index_]); }
	Vector2d position() const { return route_[current_path_index_]->spline().point(current_t_); }
	double angle() const {
		auto direction = route_[current_path_index_]->spline().direction(current_t_);
		return (direction.x == 0 && direction.y == 0) ? route_[current_path_index_]->from().angle() : direction.angle();
	}
};

} // namespace model
} // namespace traffic_lights

