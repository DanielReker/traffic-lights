#pragma once

#include <boost/json.hpp>

#include <string>
#include <cmath>
#include <numbers>
#include <map>

#include "CubicSpline2.hpp"
#include "Node.hpp"


namespace traffic_lights {
namespace model {

class Path {
private:
	std::string name_;
	const Node& from_, to_;
	CubicSpline2d spline_;

	template <Numeric T>
	static T DegreesToRadians(T degrees) {
		return degrees / 180.0 * std::numbers::pi;
	}

	CubicSpline2d MakeSpline(const boost::json::value& path, const std::map<std::string, std::shared_ptr<Node>>& nodes) {
		//const Node& from = *nodes.at(path.at_pointer("/from/node").as_string().c_str());
		//const Node& to = *nodes.at(path.at_pointer("/to/node").as_string().c_str());

		Vector2d from_direction = Vector2d(
			std::cos(DegreesToRadians(from_.angle())),
			std::sin(DegreesToRadians(from_.angle()))
		) * path.at_pointer("/from/directionCoefficient").to_number<double>();

		Vector2d to_direction = Vector2d(
			std::cos(DegreesToRadians(to_.angle())),
			std::sin(DegreesToRadians(to_.angle()))
		) * path.at_pointer("/to/directionCoefficient").to_number<double>();

		return CubicSpline2d(from_.position(), from_direction, to_.position(), to_direction);
	}

public:
	Path(const boost::json::value& path, const std::map<std::string, std::shared_ptr<Node>>& nodes)
		: name_(path.at_pointer("/name").as_string().c_str()),
	      from_(*nodes.at(path.at_pointer("/from/node").as_string().c_str())),
	      to_(*nodes.at(path.at_pointer("/to/node").as_string().c_str())),
		  spline_(MakeSpline(path, nodes)) { }

	const auto& name() const { return name_; }
	const auto& spline() const { return spline_; }
	const auto& from() const { return from_; }
	const auto& to() const { return to_; }
};

} // namespace model
} // namespace traffic_lights

