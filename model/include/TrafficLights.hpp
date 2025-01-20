#pragma once

#include <boost/json.hpp>

#include <string>
#include <chrono>

#include "Vector2.hpp"


namespace traffic_lights {
namespace model {

class TrafficLights {
private:
	std::string name_;
	Vector2d position_;
	double angle_;
	std::chrono::steady_clock::time_point cycle_begin_;
	double cycle_duration_seconds_;
	std::vector<std::pair<double, std::string>> phases_;


public:
	TrafficLights(
		const boost::json::value& traffic_lights,
		std::chrono::steady_clock::time_point cycle_begin
	)
		: name_(traffic_lights.at_pointer("/name").as_string().c_str()),
		  position_(Vector2d(
			  traffic_lights.at_pointer("/position/x").to_number<double>(),
			  traffic_lights.at_pointer("/position/y").to_number<double>()
		  )),
		  angle_(traffic_lights.at_pointer("/angle").to_number<double>()),
		  cycle_duration_seconds_(0),
		  cycle_begin_(cycle_begin) {

		for (const auto& phase : traffic_lights.at_pointer("/phases").as_array()) {
			std::string phase_state = phase.at_pointer("/state").as_string().c_str();
			double phase_duration_seconds = phase.at_pointer("/durationSeconds").to_number<double>();

			cycle_duration_seconds_ += phase_duration_seconds;
			phases_.push_back({ cycle_duration_seconds_, phase_state });
		}
	}

	std::string state() const {
		auto elapsed = std::chrono::steady_clock::now() - cycle_begin_;
		double elapsed_seconds = std::chrono::duration<double>(elapsed).count();

		int cycles_elapsed = static_cast<int>(elapsed_seconds / cycle_duration_seconds_);
		double cycle_position_seconds = elapsed_seconds - cycles_elapsed * cycle_duration_seconds_;

		for (const auto& [phase_end_time_seconds, state] : phases_) {
			if (cycle_position_seconds < phase_end_time_seconds)
				return state;
		}

		return phases_.back().second;
	}

	Vector2d position() const { return position_; }
	double angle() const { return angle_; }
	const auto& name() const { return name_; }

};

} // namespace model
} // namespace traffic_lights

