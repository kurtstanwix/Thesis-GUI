#ifndef _RENDERABLE_H
#define _RENDERABLE_H 1

#include "SFML/Graphics.hpp"

class Renderable
{
public:
    virtual void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool clickedOn = false) = 0;
    virtual void render(sf::RenderWindow& window,
            const sf::Vector2f &windowSize) = 0;
    virtual bool contains(float x, float y) = 0;
    
    friend std::ostream& operator<<(std::ostream& os, const Renderable& r) {
        r.streamOut(os);
        return os;
    }
    
    bool isRenderable(void)
    {
        return m_renderable;
    }
    
    void setRenderable(bool renderable)
    {
        m_renderable = renderable;
    }
    
    bool isMoving(void)
    {
        return m_moving;
    }
    
protected:
    bool m_moving;
    bool m_renderable;
    
    virtual void streamOut(std::ostream& os) const = 0;
};

#endif