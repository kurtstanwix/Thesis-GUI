#ifndef _RENDERABLE_H
#define _RENDERABLE_H 1

#include "SFML/Graphics.hpp"

class Renderable
{
public:
    virtual void update(sf::Event &event, const sf::Vector2f &windowSize) = 0;
    virtual void render(sf::RenderWindow& window,
            const sf::Vector2f &windowSize) = 0;
            
    bool isRenderable(void) {
        return m_renderable;
    }
    
    void setRenderable(bool renderable) {
        m_renderable = renderable;
    }
    
protected:
    bool m_renderable;
};

#endif