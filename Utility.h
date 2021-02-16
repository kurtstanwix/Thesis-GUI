#ifndef _UTILITY_H
#define _UTILITY_H 1

#include <chrono>
#include <cmath>

#include "SFML/Graphics.hpp"
#include <plog/Log.h>

#include "Config.h"

#include <SFML/Graphics/Font.hpp>




#include <iostream>


constexpr std::chrono::milliseconds DOUBLE_CLICK_DURATION { 600 };


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
    //FontManager
    sf::Font m_font;
    
    FontManager() {
        m_font.loadFromFile("../coolvetica rg.ttf");
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

inline sf::Vector2f operator/(sf::Vector2f const &lhs, sf::Vector2f const &rhs)
{
    return sf::Vector2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

/* Color overloads */
inline plog::Record& operator<<(plog::Record &record, const sf::Color &col)
{
    return record << "(" << unsigned(col.r) << "," << unsigned(col.g) << "," <<
            unsigned(col.b) << "," << unsigned(col.a) << ")";
}

inline plog::Record& operator<<(plog::Record &record, const sf::Vector2f& vec)
{
    return record << "(" << vec.x << "," << vec.y << ")";
}
    
/*
inline sf::Vector& operator=(const ClassName& other)
{
    a = other.a;
    b = other.b;
    return *this;
}
*/

inline bool floatEquals(const float a, const float b)
{
    return std::abs(a - b) <= 0.01f;
}

inline bool floatEquals(const sf::Vector2f &a, const sf::Vector2f &b)
{
    return std::abs(a.x - b.x) <= 0.01f && std::abs(a.y - b.y) <= 0.01f;
}



inline sf::Vector2f unitToPixel(const sf::Vector2f& max,
        const sf::Vector2f& scale)
{
    return (scale + 1.0f) * max / 2.0f;
}

inline float unitToPixel(float max, float scale)
{
    return (scale + 1) * max / 2;
}

inline sf::Vector2f pixelToUnit(const sf::Vector2f& max,
        const sf::Vector2f& pixel)
{
    return (2.0f * pixel) / max - 1.0f;
}

inline float pixelToUnit(float max, int pixel)
{
    return (2 * pixel) / max - 1;
}

inline sf::Vector2f unitToPixelVector(const sf::Vector2f& max,
        const sf::Vector2f& scale)
{
    return scale * max / 2.0f;
}

inline float unitToPixelVector(float max, float scale)
{
    return scale * max / 2;
}

inline sf::Vector2f pixelToUnitVector(const sf::Vector2f& max,
        const sf::Vector2f& pixel)
{
    return (2.0f * pixel) / max;
}

inline float pixelToUnitVector(float max, int pixel)
{
    return (2.0f * pixel) / max;
}

inline float capInitial(float max, int objectSize, float initial)
{
    return initial * (1 - 2 * objectSize / max);
}

inline sf::Vector2f capInitial(const sf::Vector2f max, sf::Vector2f objectSize,
        sf::Vector2f initial)
{
    return initial * (- 2.0f * objectSize / max + 1.0f);
}

#endif