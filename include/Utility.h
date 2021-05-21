#ifndef _UTILITY_H
#define _UTILITY_H 1

#include <chrono>
#include <cmath>

#include <nlohmann/json.hpp>
#include <plog/Log.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Config.h"

#include <iostream>

// Time window for a second click to be processed as a double click
constexpr std::chrono::milliseconds DOUBLE_CLICK_DURATION { 600 };

// Wrapper to not have to refer to the path of resources when used
#define RESOURCE(NAME) (std::string("../res/").append(NAME))

/**
 * @class FontManager
 * @author Kurt Stanwix
 * @date 01/25/21
 * @file Config.h
 * @brief A font manager implemented as a singleton.
 */
class FontManager
{
protected:
    sf::Font m_font;
    
    FontManager() {
        m_font.loadFromFile(RESOURCE("coolvetica rg.ttf"));
    };
public:
    static FontManager& getInstance()
    {
        static FontManager instance;
        return instance;
    };

    /**
     * Cannot clone a singleton
     */
    FontManager(FontManager &other) = delete;
    void operator=(const FontManager &rhs) = delete;
    
    bool setFont(std::string &fileName)
    {
        if (m_font.loadFromFile(fileName))
            return true;
        return false;
    };
    
    sf::Font& getFont()
    {
        return m_font;
    };
};

inline void setText(sf::Text &text, const std::string &string)
{
    text.setString(string);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2 + bounds.left,
            bounds.height / 2 + bounds.top);
}

inline std::ostream& operator<<(std::ostream& os, const sf::Vector2f& vec) {
    os << "(" << vec.x << "," << vec.y << ")";
    return os;
}

// Scalar operations on 2D float vectors
inline sf::Vector2f operator+(sf::Vector2f const &lhs, float const &rhs)
{
    return sf::Vector2f(lhs.x + rhs, lhs.y + rhs);
}

inline sf::Vector2f operator-(sf::Vector2f const &lhs, float const &rhs)
{
    return sf::Vector2f(lhs.x - rhs, lhs.y - rhs);
}

// Piecewise operations on two 2D float vectors
inline sf::Vector2f operator*(sf::Vector2f const &lhs, sf::Vector2f const &rhs)
{
    return sf::Vector2f(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline sf::Vector2f operator/(sf::Vector2f const &lhs, sf::Vector2f const &rhs)
{
    return sf::Vector2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

// Plog printing operators for logging commonly used objects
inline plog::Record& operator<<(plog::Record &record, const sf::Color &col)
{
    return record << "(" << unsigned(col.r) << "," << unsigned(col.g) << "," <<
            unsigned(col.b) << "," << unsigned(col.a) << ")";
}

inline plog::Record& operator<<(plog::Record &record, const sf::Vector2f &vec)
{
    return record << "(" << vec.x << "," << vec.y << ")";
}

template <class T>
inline plog::Record& operator<<(plog::Record &record,
        const std::vector<T> &vec)
{
    record << "(";
    for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        record << *it << ",";
    }
    return record << ")";
}

template <class T1, class T2>
inline plog::Record& operator<<(plog::Record &record, const std::pair<T1, T2> &pair)
{
    return record << "(" << pair.first << "," << pair.second << ")";
}

// Float equality with 0.01 tolerance
inline bool floatEquals(const float a, const float b)
{
    return std::abs(a - b) <= 0.01f;
}

// 2D float vector equality with 0.01 tolerance
inline bool floatEquals(const sf::Vector2f &a, const sf::Vector2f &b)
{
    return std::abs(a.x - b.x) <= 0.01f && std::abs(a.y - b.y) <= 0.01f;
}

// Convert a unit 2D float vector to a 2D float vector representing a pixel position
inline sf::Vector2f unitToPixel(const sf::Vector2f& max,
        const sf::Vector2f& scale)
{
    return (scale + 1.0f) * max / 2.0f;
}

// Convert a unit float value to its value in pixel coords
inline float unitToPixel(float max, float scale)
{
    return (scale + 1) * max / 2;
}

// Convert a 2D float vector representing a pixel position to a unit 2D float vector
inline sf::Vector2f pixelToUnit(const sf::Vector2f& max,
        const sf::Vector2f& pixel)
{
    return (2.0f * pixel) / max - 1.0f;
}

// Convert a value in pixel coords to its unit float value
inline float pixelToUnit(float max, int pixel)
{
    return (2 * pixel) / max - 1;
}

// Convert a unit direction 2D float vector to a direction vector in pixel coords
inline sf::Vector2f unitToPixelVector(const sf::Vector2f& max,
        const sf::Vector2f& scale)
{
    return scale * max / 2.0f;
}

// Convert a unit direction float value to its direction value in pixel coords
inline float unitToPixelVector(float max, float scale)
{
    return scale * max / 2;
}

// Convert a direction vector in pixel coords to its unit direction 2D float vector
inline sf::Vector2f pixelToUnitVector(const sf::Vector2f& max,
        const sf::Vector2f& pixel)
{
    return (2.0f * pixel) / max;
}

// Convert a direction value in pixel coords to its unit direction float value
inline float pixelToUnitVector(float max, int pixel)
{
    return (2.0f * pixel) / max;
}

// Scales a value to be within the max and with a buffer of objectSize
inline float capInitial(float max, int objectSize, float initial)
{
    return initial * (1 - 2 * objectSize / max);
}

// Scales a vector to be within the max and with a buffer of objectSize
inline sf::Vector2f capInitial(const sf::Vector2f max, sf::Vector2f objectSize,
        sf::Vector2f initial)
{
    return initial * (- 2.0f * objectSize / max + 1.0f);
}

#endif