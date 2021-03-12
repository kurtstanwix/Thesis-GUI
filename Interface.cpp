#include "Interface.h"

#include <iostream>
#include <list>

#include "plog/Log.h"

#include "Utility.h"

#define BUTTON_LAYER 0
#define SHAPE_LAYER 1

InterfaceButton::InterfaceButton(std::string name,
        const sf::Vector2f &windowSize,
        void (*onClickCallback)(InterfaceButton &caller), Interface &parent)
    :
        m_name(name),
        m_onClick(onClickCallback),
        m_parent(parent)
{
    m_shape.setSize(windowSize / 20.0f);
    m_shape.setOrigin(getSize() / 2.0f);
    m_label.setFont(FontManager::getInstance().getFont());
    setText(m_label, name);
}

void InterfaceButton::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize)
{
    m_shape.setFillColor(m_color);
    m_shape.setPosition(m_pos);
    m_label.setPosition(m_pos);
    window.draw(m_shape);
    window.draw(m_label);
}

void InterfaceButton::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn)
{
    PLOGD << "Button update";
    if (event != nullptr) {
        if (!clickedOn && event->type == sf::Event::MouseButtonPressed &&
                contains(event->mouseButton.x, event->mouseButton.y)) {
            PLOGD<<"TRUE";
            clickedOn = true;
            if (event->mouseButton.button == sf::Mouse::Left)
                m_onClick(*this);
        } else
            PLOGD<<"FALSE";
    }
}

bool InterfaceButton::contains(float x, float y)
{
    return m_shape.getGlobalBounds().contains(x, y);
}


void InterfaceButton::streamOut(std::ostream& os) const
{
    os << m_name << " Button";
}





bool Interface::contains(float x, float y)
{
    for (iterator it = begin(); it != end(); ++it) {
        if ((*it)->contains(x, y))
            return true;
    }
    return false;
}

Interface::Interface(const sf::Vector2f &windowSize, NetworkTopology &nettop)
    :
        m_nettop(nettop),
        m_bezier(4, {{0, 0}, {0.25, 0}, {0.5, 0}, {0.5, 0.25}, {0.5, 0.5}})
{
    PLOGD << "Creating interface";
    std::list<InterfaceButton>::iterator exit =
            m_buttons.emplace(m_buttons.end(), "Exit", windowSize, &exitAction,
            *this);
    
    /* 1/20th (5%) of screen width and height */
    exit->setColor(sf::Color(200, 30, 30));
    exit->setOutlineColor(sf::Color::Black);
    exit->m_pos = { windowSize.x - exit->getSize().x, exit->getSize().y };
    
    std::list<InterfaceButton>::iterator save =
            m_buttons.emplace(m_buttons.end(), "Save", windowSize, &saveAction,
            *this);
    save->setColor(sf::Color(30, 200, 30));
    save->setOutlineColor(sf::Color::Black);
    save->m_pos = { save->getSize().x, save->getSize().y };
    
    addLayer(BUTTON_LAYER);
    addLayer(SHAPE_LAYER);
    
    addToLayer(BUTTON_LAYER, *exit);
    addToLayer(BUTTON_LAYER, *save);
    addToLayer(SHAPE_LAYER, m_bezier);
    
    m_renderable = true;
}

void Interface::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    PLOGD << "Updating interface";
    for (iterator it = begin(); it != end(); ++it) {
        PLOGD<<"Iterate";
        (*it)->update(event, windowSize, clickedOn);
    }
}

void Interface::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    PLOGD << "Rendering interface";
    for (reverse_iterator it = rbegin(); it != rend(); ++it) {
        (*it)->render(window, windowSize);
    }
}