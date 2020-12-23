#ifndef _BEZIERCURVE_H
#define _BEZIERCURVE_H 1

#include <list>

#include "SFML/Graphics.hpp"
#include "Renderable.h"

class BezierCurve : public Renderable
{
public:
    struct Segment : public Renderable
    {
    public:
        sf::Vector2f m_pos;
        sf::RectangleShape m_shape;
        
        Segment();
        //virtual ~Segment() {};
        Segment(const sf::Vector2f &pos);
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    };
    
    struct Handle : public Renderable
    {
    public:
        sf::Vector2f m_pos;
        sf::RectangleShape m_shape;
        bool m_selected;
        sf::Vector2f m_lastDragPos;
        float m_rot;
        float m_length;
        
        Handle();
        //virtual ~Handle() {};
        Handle(const sf::Vector2f &pos);
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize);
        void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
    };
    
    sf::Color m_color;
    float m_width;
    
    bool m_constructed;
    bool m_moving;
    bool m_selected;
    //std::list<Handle> m_handles;
    
    int m_fidelity;
    std::list<Segment> m_segments;
    
    sf::Vector2f m_lastDragPos;
    //std::list<
    //std::pair<sf::RectangleShape, sf::RectangleShape> m_handleShapes;
    //std::pair<sf::RectangleShape, sf::RectangleShape> m_handleLines;
    
    void ConstructCurve();
    sf::Vector2f GetPointOnCurve(float t);
    
//public:
    std::list<Handle> m_handles;
    BezierCurve(int fidelity, std::list<sf::Vector2f> controlPoints);
    
    void SetColor(sf::Color color)
    {
        m_color = color;
    };
    
    sf::Vector2f getStart()
    {
        return m_handles.front().m_pos;
    }
    
    void setStart(const sf::Vector2f &start)
    {
        m_handles.begin()->m_pos = start;
        ConstructCurve();
    }
    
    sf::Vector2f getEnd()
    {
        return m_handles.back().m_pos;
    }
    
    void setEnd(const sf::Vector2f &end)
    {
        m_handles.rbegin()->m_pos = end;
        ConstructCurve();
    }
    
    void setWidth(const float width) { m_width = width; };
    float getWidth() { return m_width; };

    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize);
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
};

#endif