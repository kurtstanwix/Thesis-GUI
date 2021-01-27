#include "NetworkWindow.h"

#include <memory>

#include "plog/Log.h"
#include "SFML/Graphics/Text.hpp"

#include "Interface.h"
#include "NetworkTopology.h"
#include "Utility.h"


#define UI_LAYER_ID 0
#define NETWORK_LAYER_ID 1


NetworkWindow::NetworkWindow(int numNodes, int nodeWidth,
                const sf::Vector2f &windowSize)
{
    std::cout << "Test" << std::endl;
    NetworkTopology *test = NetworkTopology::createTopology(numNodes,
            nodeWidth, windowSize);
    
    if (test == NULL) {
        PLOGF << "Bad file";
        exit(0);
    }
    
    addLayer(UI_LAYER_ID);
    addLayer(NETWORK_LAYER_ID);
    /* Add the topology window and the ui to the components queue */
    Interface *interface = new Interface(windowSize, *test);
    addToLayer(UI_LAYER_ID, *interface);
    
    addToLayer(NETWORK_LAYER_ID, *test);
}

void NetworkWindow::render(sf::RenderWindow &window, const sf::Vector2f &windowSize)
{
    PLOGV << "Num components " << m_components.size();
    for (reverse_iterator it = rbegin(); it != rend(); ++it) {
        if ((*it)->isRenderable()) {
            PLOGV << "Rendering to NetworkWindow";
            (*it)->render(window, windowSize);
        } else {
            PLOGV << "Not Renderable";
        }
    }
}

void NetworkWindow::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    for (iterator it = begin(); it != end(); ++it) {
        (*it)->update(event, windowSize, clickedOn);
    }
}

bool NetworkWindow::setNodeActive(int nodeID, bool state)
{
    return ((NetworkTopology*)*getLayer(NETWORK_LAYER_ID)->begin())->setNodeActive(nodeID, state);
}

bool NetworkWindow::setLinkActive(int nodeID1, int nodeID2, bool state)
{
    return ((NetworkTopology*)*getLayer(NETWORK_LAYER_ID)->begin())->setLinkActive(nodeID1, nodeID2, state);
}

