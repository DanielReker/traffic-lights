#pragma once


#include <concepts>

namespace traffic_lights {
namespace model {

template<class T> concept Numeric = std::is_arithmetic_v<T>;

} // namespace model
} // namespace traffic_lights

