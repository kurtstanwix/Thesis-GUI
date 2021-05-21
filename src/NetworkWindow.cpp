#include "NetworkWindow.h"

#include <memory>

#include "plog/Log.h"
#include "SFML/Graphics/Text.hpp"

#include "Interface.h"
#include "NetworkTopology.h"
#include "Utility.h"


#define UI_LAYER_ID 0
#define NETWORK_LAYER_ID 1


NetworkWindow::NetworkWindow()
{
    addLayer(UI_LAYER_ID);
    addLayer(NETWORK_LAYER_ID);
}

// Load in the network file and set up the rendering layers
bool NetworkWindow::init(const sf::Vector2f &windowSize,
        const std::string &fileName, int nodeWidth)
{
    m_nettop = NetworkTopology::createTopology(windowSize,
            fileName, nodeWidth);
    
    if (m_nettop == nullptr) {
        PLOGF << "Failed to create topology from file \"" << fileName << "\"";
        return false;;
    }
    
    /* Add the topology window and the ui to the components queue */
    m_interface = new Interface(windowSize, *m_nettop);
    
    addToLayer(UI_LAYER_ID, *m_interface);
    addToLayer(NETWORK_LAYER_ID, *m_nettop);
    
    return true;
}

// Render all components to the window
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

// Process the update logic of all components in the window
void NetworkWindow::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    for (iterator it = begin(); it != end(); ++it) {
        (*it)->update(event, windowSize, clickedOn);
    }
}