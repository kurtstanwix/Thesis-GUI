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
        Segment(const sf::Vector2f &pos);
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize,
                bool &clickedOn);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
        bool contains(float x, float y);
    protected:
        void streamOut(std::ostream& os) const;
    };
    
    struct Handle : public Renderable
    {
    public:
        sf::Vector2f m_pos;
        sf::RectangleShape m_shape;
        sf::Vector2f m_lastDragPos;
        float m_length;
        
        Handle();
        Handle(const sf::Vector2f &pos);
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize,
                bool &clickedOn);
        void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
        bool contains(float x, float y);
    protected:
        void streamOut(std::ostream& os) const;
    };
    
    float m_width;
    bool m_constructed;
    int m_fidelity;
    std::list<Segment> m_segments;
    
    void ConstructCurve();
    sf::Vector2f GetPointOnCurve(float t);
    
    std::list<Handle> m_handles;
    BezierCurve(int fidelity, std::list<sf::Vector2f> controlPoints);
    
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
    
    sf::Vector2f getDominantHandleEnd()
    {
        return std::next(m_handles.rbegin())->m_pos;
    }
    
    sf::Vector2f getDominantHandleStart()
    {
        return std::next(m_handles.begin())->m_pos;
    }
    
    void setSelected(bool state);

    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
    bool contains(float x, float y);
protected:
    void streamOut(std::ostream& os) const;
};

#endif