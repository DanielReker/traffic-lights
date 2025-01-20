#pragma once

#include "Numeric.hpp"
#include "Vector2.hpp"


namespace traffic_lights {
namespace model {

template<Numeric T>
class CubicSpline {
private:
	T a_, b_, c_, d_;


public:
	CubicSpline(T from, T from_derivative, T to, T to_derivative)
		: a_(from_derivative + to_derivative + 2 * from - 2 * to),
		  b_(3 * to - 3 * from - 2 * from_derivative - to_derivative),
		  c_(from_derivative),
		  d_(from) { }
	
	T value(T point) const {
		// ax^3 + bx^2 + cx + d
		return a_ * point * point * point + b_ * point * point + c_ * point + d_;
	}

	T derivative(T point) const {
		// 3ax^2 + 2bx + c
		return 3 * a_ * point * point + 2 * b_ * point + c_;
	}
};

} // namespace model
} // namespace traffic_lights

