#ifndef _INTERFACE_H_
#define _INTERFACE_H_ 1

#include "SFML/Graphics.hpp"
#include "NetworkTopology.h"
#include "Renderable.h"

class Interface : public Renderable
{
    private:
        sf::RectangleShape m_exitButton;
        sf::RectangleShape m_saveButton;
        
        NetworkTopology &m_nettop;
    public:
        
        
        Interface(const sf::Vector2f &windowSize, NetworkTopology &nettop);
        
        /* Renderable interface */
        void update(sf::Event &event, const sf::Vector2f &windowSize);
        void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
};

#endif