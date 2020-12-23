#ifndef _UTILITY_H
#define _UTILITY_H 1

#include "SFML/Graphics.hpp"

inline std::ostream& operator<<(std::ostream& os, const sf::Vector2f& vec) {
    os << "(" << vec.x << "," << vec.y << ")";
    return os;
}

inline sf::Vector2f operator+(sf::Vector2f const &lhs, float const &rhs)
{
    return sf::Vector2f(lhs.x + rhs, lhs.y + rhs);
}

inline sf::Vector2f operator-(sf::Vector2f const &lhs, float const &rhs)
{
    return sf::Vector2f(lhs.x - rhs, lhs.y - rhs);
}

inline sf::Vector2f operator*(sf::Vector2f const &lhs, sf::Vector2f const &rhs)
{
    return sf::Vector2f(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline sf::Vector2f operator/(sf::Vector2f const &lhs, float const &rhs)
{
    return sf::Vector2f(lhs.x / rhs, lhs.y / rhs);
}

inline sf::Vector2f operator/(sf::Vector2f const &lhs, sf::Vector2f const &rhs)
{
    return sf::Vector2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

/*
inline sf::Vector& operator=(const ClassName& other)
{
    a = other.a;
    b = other.b;
    return *this;
}
*/



inline sf::Vector2f unitToPixel(const sf::Vector2f& max,
        const sf::Vector2f& scale) {
    return (scale + 1.0f) * max / 2.0f;
}

inline float unitToPixel(float max, float scale) {
    return (scale + 1) * max / 2;
}

inline sf::Vector2f pixelToUnit(const sf::Vector2f& max,
        const sf::Vector2f& pixel) {
    return (2.0f * pixel) / max - 1.0f;
}

inline float pixelToUnit(float max, int pixel) {
    return (2 * pixel) / max - 1;
}

inline float capInitial(float max, int objectSize, float initial) {
    return initial * (1 - 2 * objectSize / max);
}

#endif