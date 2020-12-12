#ifndef _UTILITY_H
#define _UTILITY_H 1

inline std::ostream& operator<<(std::ostream& os, const sf::Vector2f& vec) {
    os << "(" << vec.x << "," << vec.y << ")";
    return os;
}

inline int unitToPixel(float max, float scale) {
    return (scale + 1) * max / 2;
}

inline float capInitial(float max, int objectSize, float initial) {
    return initial * (1 - 2 * objectSize / max);
}

inline float pixelToUnit(float max, int objectSize, int pixel) {
    return (2 * pixel) / max - 1;
}

#endif