#include "Interface.h"

#include <iostream>
#include <list>

#include "plog/Log.h"

#include "Utility.h"

#define BUTTON_LAYER 0
#define SHAPE_LAYER 1

InterfaceButton::InterfaceButton(std::string name,
        void (*onClickCallback)(InterfaceButton &caller), Interface &parent,
        const sf::Vector2f &size)
    :
        m_name(name),
        m_onClick(onClickCallback),
        m_parent(parent)
{
    m_shape.setSize(size);
    m_shape.setOrigin(getSize() / 2.0f);
    m_shape.setOutlineThickness(4.0f);
    m_label.setFont(FontManager::getInstance().getFont());
    setText(m_label, name);
}

void InterfaceButton::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize)
{
    m_shape.setFillColor(m_color);
    m_shape.setOutlineColor(m_color - m_selectedColorOffset);
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
            clickedOn = true;
            if (event->mouseButton.button == sf::Mouse::Left)
                m_onClick(*this);
        }
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
        m_nettop(nettop)
{
    PLOGD << "Creating interface";

    addLayer(BUTTON_LAYER);
    addLayer(SHAPE_LAYER);
    
    m_renderable = true;
}

// Register a clickable button which calls the callback function on click
void Interface::registerButton(const std::string &name,
        void (*onClickCallback)(InterfaceButton &caller),
        const sf::Color &col, const sf::Vector2f &pos,
        const sf::Vector2f &size)
{
    std::list<InterfaceButton>::iterator button =
            m_buttons.emplace(m_buttons.end(), name, onClickCallback, *this, size);
    
    button->setColor(col);
    button->setOutlineColor(col - m_selectedColorOffset);
    button->m_pos = pos;

    addToLayer(BUTTON_LAYER, *button);
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