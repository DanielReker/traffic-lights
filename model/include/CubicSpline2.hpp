#pragma once

#include <algorithm>

#include "Numeric.hpp"
#include "CubicSpline.hpp"


namespace traffic_lights {
namespace model {

template<Numeric T>
class CubicSpline2 {
private:
	CubicSpline<T> spline_x_, spline_y_;
	T length_;

	void CalculateLength() {
		constexpr T step = 0.001;

		length_ = 0;
		for (T t = 0; t <= 1; t += step) {
			auto begin = point(std::clamp<T>(t, 0, 1));
			auto end = point(std::clamp<T>(t + step, 0, 1));

			auto diff = end - begin;

			length_ += diff.length();
		}
	}


public:
	CubicSpline2(Vector2<T> from, Vector2<T> from_direction, Vector2<T> to, Vector2<T> to_direction)
		: spline_x_(from.x, from_direction.x, to.x, to_direction.x),
		  spline_y_(from.y, from_direction.y, to.y, to_direction.y) {
		
		CalculateLength();
	}

	Vector2<T> point(T t) const {
		return Vector2<T>(spline_x_.value(t), spline_y_.value(t));
	}

	Vector2<T> direction(T t) const {
		return Vector2<T>(spline_x_.derivative(t), spline_y_.derivative(t));
	}

	T length() const { return length_; }
};

using CubicSpline2d = CubicSpline2<double>;
using CubicSpline2f = CubicSpline2<float>;
using CubicSpline2i = CubicSpline2<int>;

} // namespace model
} // namespace traffic_lights

