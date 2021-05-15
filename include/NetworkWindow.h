#ifndef _NETWORKWINDOW_H
#define _NETWORKWINDOW_H 1

#include <list>

#include "LayeredRenderable.h"
#include "NetworkTopology.h"
#include "Interface.h"

class NetworkWindow : public LayeredRenderable
{
private:
    std::list<std::reference_wrapper<Renderable>> m_components;
    Interface *m_interface;
    
public:
    NetworkTopology *m_nettop;
    
    NetworkWindow();
    
    bool init(const sf::Vector2f &windowSize, const std::string &fileName,
            int nodeWidth = 100);
    
    void registerButton(const std::string &name, const sf::Vector2f &windowSize,
        void (*onClickCallback)(InterfaceButton &caller),
        const sf::Color &col, const sf::Vector2f &pos)
    {
        m_interface->registerButton(name, windowSize, onClickCallback, col, pos);
    }
    /*
    bool setNodeActive(int nodeID, bool state);
    bool setLinkActive(int nodeID1, int nodeID2, bool state);
    */
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    bool contains(float x, float y) { return false; };
protected:
    void streamOut(std::ostream& os) const {};
};

#endif