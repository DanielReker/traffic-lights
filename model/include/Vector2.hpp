#pragma once

#include "Numeric.hpp"

#include <ostream>
#include <cmath>
#include <numbers>


namespace traffic_lights {
namespace model {

template<Numeric T>
struct Vector2 {
	T x, y;

	Vector2(const T& x, const T& y) : x(x), y(y) { }

	friend Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
		return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	friend Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
		return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	friend Vector2 operator*(const T& scalar, const Vector2& vector) {
		return Vector2(scalar * vector.x, scalar * vector.y);
	}

	friend Vector2 operator*(const Vector2& vector, const T& scalar) {
		return scalar * vector;
	}


	friend Vector2 operator/(const Vector2& vector, const T& scalar) {
		return Vector2(vector.x / scalar, vector.y / scalar);
	}

	Vector2 operator-() const {
		return Vector2(-this->x, -this->y);
	}

	friend std::ostream& operator<<(std::ostream& out, const Vector2& vector) {
		out << "(" << vector.x << ", " << vector.y << ")";
		return out;
	}
	
	T length_squared() const {
		return x * x + y * y;
	}

	T length() const {
		return std::sqrt(length_squared());
	}

	Vector2<T> normalize() const {
		return *this / length();
	}

	T angle() const {
		constexpr T cos45 = 1 / std::numbers::sqrt2_v<T>;
		auto norm = normalize();

		T x_ = norm.x, y_ = norm.y;

		T radians = 0;
		if (x_ < -cos45 || y_ < -cos45) {
			radians += std::numbers::pi_v<T>;
			x_ *= -1;
			y_ *= -1;
		}

		if (-cos45 <= x_ && x_ <= cos45)
			radians += std::numbers::pi_v<T> / 2 - std::atan(x_ / y_);
		else
			radians += std::atan(y_ / x_);

		return radians / std::numbers::pi_v<T> * 180;
	}
};

using Vector2d = Vector2<double>;
using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

} // namespace model
} // namespace traffic_lights

