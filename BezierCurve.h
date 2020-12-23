#ifndef _BEZIERCURVE_H
#define _NETWORKWINDOW_H 1

#include <list>

#include "SFML/Graphics.hpp"
#include "Renderable.h"

class BezierCurve : public Renderable
{
private:
    struct Segment : public Renderable
    {
    public:
        sf::Vector2f m_pos;
        sf::RectangleShape m_shape;
        
        Segment();
        //virtual ~Segment() {};
        Segment(const sf::Vector2f &pos);
        /* Renderable interface */
        void update(sf::Event &event, const sf::Vector2f &windowSize);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    };
    
    struct Handle : public Renderable
    {
    public:
        sf::Vector2f m_pos;
        sf::RectangleShape m_shape;
        bool m_selected;
        sf::Vector2f m_lastDragPos;
        
        Handle();
        //virtual ~Handle() {};
        Handle(const sf::Vector2f &pos);
        /* Renderable interface */
        void update(sf::Event &event, const sf::Vector2f &windowSize);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    };
    
    
    bool m_constructed;
    bool m_moving;
    bool m_selected;
    std::list<Handle> m_handles;
    
    int m_fidelity;
    std::list<Segment> m_segments;
    
    sf::Vector2f m_lastDragPos;
    //std::list<
    //std::pair<sf::RectangleShape, sf::RectangleShape> m_handleShapes;
    //std::pair<sf::RectangleShape, sf::RectangleShape> m_handleLines;
    
    void ConstructCurve(const sf::Vector2f& windowSize);
    sf::Vector2f GetPointOnCurve(float t);
    
public:
    BezierCurve(int fidelity, std::list<sf::Vector2f> controlPoints);

    /* Renderable interface */
    void update(sf::Event &event, const sf::Vector2f &windowSize);
    void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
};

#endif