#include "Interface.h"

#include <iostream>
#include <list>

Interface::Interface(const sf::Vector2f &windowSize, NetworkTopology &nettop)
    :
        m_nettop(nettop),
        m_bezier(4, {{0, 0}, {0.25, 0}, {0.5, 0}, {0.5, 0.25}, {0.5, 0.5}})
{
    /* 1/20th (5%) of screen width and height */
    m_exitButton.setSize(windowSize / 20.0f);
    m_exitButton.setOrigin(m_exitButton.getSize() / 2.0f);
    m_exitButton.setFillColor(sf::Color(200, 30, 30));
    m_exitButton.setOutlineColor(sf::Color::Black);
    
    m_saveButton.setSize(windowSize / 20.0f);
    m_saveButton.setOrigin(m_exitButton.getSize() / 2.0f);
    m_saveButton.setFillColor(sf::Color(30, 200, 30));
    m_saveButton.setOutlineColor(sf::Color::Black);
    
    m_renderable = true;
}

void Interface::update(sf::Event *event, const sf::Vector2f &windowSize) {
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                if (m_exitButton.getGlobalBounds().contains(event->mouseButton.x,
                        event->mouseButton.y)) {
                        exit(0);
                } else if (m_saveButton.getGlobalBounds().contains(event->mouseButton.x,
                        event->mouseButton.y)) {
                        std::cout << "Saving" << std::endl;
                        m_nettop.save("../test2");
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
                break;
            case sf::Event::MouseMoved:
                break;
            default:
                break;
        }
    }
    m_bezier.update(event, windowSize);
}

void Interface::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    const sf::Vector2f &exitButtonSize = m_exitButton.getSize();
    m_exitButton.setPosition(windowSize.x - exitButtonSize.x, exitButtonSize.y);
    
    const sf::Vector2f &saveButtonSize = m_saveButton.getSize();
    m_saveButton.setPosition(saveButtonSize.x, saveButtonSize.y);
    
    if (m_bezier.isRenderable()) {
        m_bezier.render(window, windowSize);
    }
    
    window.draw(m_exitButton);
    window.draw(m_saveButton);
}