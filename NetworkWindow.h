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
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool clickedOn = false);
    void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    bool contains(float x, float y) { return false; };
protected:
    void streamOut(std::ostream& os) const {};
};

#endif