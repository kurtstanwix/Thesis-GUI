#ifndef _INTERFACE_H_
#define _INTERFACE_H_ 1

#include "SFML/Graphics.hpp"
#include "BezierCurve.h"
#include "LayeredRenderable.h"
#include "NetworkTopology.h"
#include "Renderable.h"

class Interface;
class InterfaceButton;


void exitAction(InterfaceButton &caller);
void saveAction(InterfaceButton &caller);

class InterfaceButton : public Renderable
{
private:
    sf::RectangleShape m_shape;
    sf::Text m_label;
    std::string m_name;
    void (*m_onClick)(InterfaceButton &caller);
    
public:
    sf::Vector2f m_pos;
    Interface &m_parent;
    
    InterfaceButton(std::string name, void (*onClickCallback)(InterfaceButton &caller),
            Interface &parent, const sf::Vector2f &size);
    
    void setOutlineColor(const sf::Color &color)
    {
        m_shape.setOutlineColor(color);
    };
    
    void setSize(const sf::Vector2f &size)
    {
        m_shape.setSize(size);
    };
    
    const sf::Vector2f& getSize() const
    {
        return m_shape.getSize();
    };
    
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
    bool contains(float x, float y);
protected:
    void streamOut(std::ostream& os) const;
};

class Interface : public LayeredRenderable
{
private:
    std::list<InterfaceButton> m_buttons;
    
public:
    NetworkTopology &m_nettop;
    
    Interface(const sf::Vector2f &windowSize, NetworkTopology &nettop);

    void registerButton(const std::string &name,
        void (*onClickCallback)(InterfaceButton &caller),
        const sf::Color &col, const sf::Vector2f &pos,
        const sf::Vector2f &size);
    
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
    bool contains(float x, float y);
protected:
    void streamOut(std::ostream& os) const {};
};

#endif