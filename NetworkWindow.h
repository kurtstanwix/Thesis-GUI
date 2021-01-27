#ifndef _NETWORKWINDOW_H
#define _NETWORKWINDOW_H 1

#include <list>

#include "LayeredRenderable.h"

class NetworkWindow : public LayeredRenderable
{
private:
    std::list<std::reference_wrapper<Renderable>> m_components;
    
public:
    NetworkWindow(int numNodes, int nodeWidth, const sf::Vector2f &windowSize);
    
    bool setNodeActive(int nodeID, bool state);
    bool setLinkActive(int nodeID1, int nodeID2, bool state);
    
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    bool contains(float x, float y) { return false; };
protected:
    void streamOut(std::ostream& os) const {};
};

#endif