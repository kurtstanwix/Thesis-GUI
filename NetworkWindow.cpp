#include "NetworkWindow.h"

#include <memory>

#include "Interface.h"
#include "NetworkTopology.h"



NetworkWindow::NetworkWindow(int numNodes, int nodeWidth,
                const sf::Vector2f &windowSize)
{
    std::cout << "Test" << std::endl;
    std::unique_ptr<NetworkTopology> test(NetworkTopology::createTopology(
            numNodes, nodeWidth, windowSize));
    
    if (test == NULL) {
        std::cout << "Bad file" << std::endl;
        exit(0);
    }
    m_components.push_back(std::move(test));
    
    auto interface = std::make_unique<Interface>(windowSize, *test);
    m_components.push_back(std::move(interface));
}

void NetworkWindow::render(sf::RenderWindow &window, const sf::Vector2f &windowSize)
{
    for (std::list<std::unique_ptr<Renderable>>::iterator it = m_components.begin();
            it != m_components.end(); it++) {
        if (it->get()->isRenderable())
            it->get()->render(window, windowSize);
    }
}

void NetworkWindow::update(sf::Event *event, const sf::Vector2f &windowSize)
{
    for (std::list<std::unique_ptr<Renderable>>::iterator it = m_components.begin();
            it != m_components.end(); it++) {
        it->get()->update(event, windowSize);
    }
}

