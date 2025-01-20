#pragma once

#include <boost/json.hpp>

#include <string>
#include <optional>
#include <memory>

#include "Vector2.hpp"
#include "TrafficLights.hpp"


namespace traffic_lights {
namespace model {

class Node {
private:
	std::string name_;
	Vector2d position_;
	double angle_;
	std::optional<std::shared_ptr<TrafficLights>> traffic_lights_;


public:
	Node(const boost::json::value& node, const std::map<std::string, std::shared_ptr<TrafficLights>>& traffic_lights)
		: name_(node.at_pointer("/name").as_string().c_str()),
		  position_(Vector2d(
			  node.at_pointer("/position/x").to_number<double>(),
			  node.at_pointer("/position/y").to_number<double>()
		  )),
		  angle_(node.at_pointer("/angle").to_number<double>()),
		  traffic_lights_() {
	
		auto traffic_lights_json = node.try_at_pointer("/trafficLights");
		if (traffic_lights_json.has_value()) {
			std::string traffic_lights_name = traffic_lights_json.value().as_string().c_str();
			traffic_lights_ = traffic_lights.at(traffic_lights_name);
		}
	}

	Vector2d position() const { return position_; }
	double angle() const { return angle_; }
	const auto& name() const { return name_; }
	const auto& traffic_lights() const { return traffic_lights_; }

	bool CanMove() const {
		return !traffic_lights_.has_value() || traffic_lights_.value()->state() == "green";
	}
};

} // namespace model
} // namespace traffic_lights

