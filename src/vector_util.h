#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H

#include <iostream>

#include <SFML/System/Vector2.hpp>

template <typename T>
constexpr sf::Vector2<T> operator*(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
    return sf::Vector2<T>(a.x * b.x, a.y * b.y);
}

template <typename T>
constexpr sf::Vector2<T> operator/(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
    assert(b.x != 0);
    assert(b.y != 0);
    return sf::Vector2<T>(a.x / b.x, a.y / b.y);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
    return os << "Vector2{ " << v.x << ", " << v.y << " }";
}

#endif  // VECTOR_UTIL_H
