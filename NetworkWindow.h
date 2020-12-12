#ifndef _NETWORKWINDOW_H
#define _NETWORKWINDOW_H 1

#include <list>

#include "Renderable.h"

class NetworkWindow : public Renderable
{
private:
    std::list<std::unique_ptr<Renderable>> m_components;
    
public:
    NetworkWindow(int numNodes, int nodeWidth, const sf::Vector2f &windowSize);

    /* Renderable interface */
    void update(sf::Event &event, const sf::Vector2f &windowSize);
    void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
};

#endif