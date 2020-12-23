#include "BezierCurve.h"


#include <cmath>

#include <iostream>

#include "Utility.h"


/*
 * SEGMENT
 */

BezierCurve::Segment::Segment()
{
    setRenderable(true);
}

BezierCurve::Segment::Segment(const sf::Vector2f &pos)
    :
        Segment()
{
    m_pos.x = pos.x;
    m_pos.y = pos.y;
}

void BezierCurve::Segment::update(sf::Event &event,
        const sf::Vector2f &windowSize)
{
}

void BezierCurve::Segment::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    m_shape.setPosition(unitToPixel(windowSize, m_pos));
    window.draw(m_shape);
}

/*
 * HANDLE
 */

BezierCurve::Handle::Handle()
{
    m_shape.setFillColor(sf::Color::Yellow);
    m_shape.setSize({6, 6});
    setRenderable(false);
    m_selected = false;
}

BezierCurve::Handle::Handle(const sf::Vector2f &pos)
    :
        Handle()
{
    m_pos.x = pos.x;
    m_pos.y = pos.y;
}

void BezierCurve::Handle::update(sf::Event &event,
        const sf::Vector2f &windowSize)
{
    /* Can't interact with if not visible */
    if (!isRenderable())
        return;
    switch (event.type) {
        case sf::Event::MouseButtonPressed:
        {
            if (m_shape.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                /* LMB to highlight handle (drag to move),
                 * RMB to unhighlight */
                if (event.mouseButton.button == sf::Mouse::Left) {
                    m_selected = true;
                    m_lastDragPos.x = event.mouseButton.x;
                    m_lastDragPos.y = event.mouseButton.y;
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    m_selected = false;
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
            m_selected = false;
            break;
        case sf::Event::MouseMoved:
            if (m_selected) {
                /* Clicked on and dragged, move with the mouse */
                sf::Vector2f delta(pixelToUnit(windowSize,
                        sf::Vector2f(event.mouseMove.x, event.mouseMove.y)) -
                        pixelToUnit(windowSize, m_lastDragPos));
                
                m_pos += delta;
                
                m_lastDragPos.x = event.mouseMove.x;
                m_lastDragPos.y = event.mouseMove.y;
            }
            break;
    }
}

void BezierCurve::Handle::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    m_shape.setPosition(unitToPixel(windowSize, m_pos));
    window.draw(m_shape);
}



BezierCurve::BezierCurve(int fidelity, std::list<sf::Vector2f> controlPoints)
    :
        m_fidelity(fidelity)
{
    for (std::list<sf::Vector2f>::iterator it = controlPoints.begin();
            it != controlPoints.end(); it++) {
        std::cout << "Adding POS: " << *it << std::endl;
        m_handles.emplace_back(*it);
        std::cout << "Added POS: " << m_handles.back().m_pos << std::endl;
    }
    m_selected = false;
    m_moving = false;
    m_constructed = false;
    setRenderable(true);
}

sf::Vector2f BezierCurve::GetPointOnCurve(float t)
{
    std::list<std::list<sf::Vector2f>> hierarchy(m_handles.size());
    /* Add the first layer (the handle positions) */
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        hierarchy.front().push_back(it->m_pos);
    }
    /* Step into each layer, calculating the current parametric bezier position
     * with respect to the upper layer */
    for (std::list<std::list<sf::Vector2f>>::iterator it = std::next(hierarchy.begin());
            it != hierarchy.end(); it++) {
        std::list<sf::Vector2f> &upperLayer = *std::prev(it);
        /* Find the parametric point on the inner layer */
        for (std::list<sf::Vector2f>::iterator iit = upperLayer.begin();
                std::next(iit) != upperLayer.end(); iit++) {
            it->emplace_back((1.0f - t) * *iit + t * *std::next(iit));
        }
    }
    return hierarchy.back().back();
}

void BezierCurve::ConstructCurve(const sf::Vector2f& windowSize)
{
    std::cout << "Constructing Bezier " << m_segments.size() << std::endl;
    int sizeDiff = m_fidelity - m_segments.size();
    if (sizeDiff > 0) {
        // Add the missing segments
        std::cout << "Filling " << sizeDiff << " Segments" << std::endl;
        for (int i = 0; i < sizeDiff; i++) {
            std::cout << "Adding segment" << std::endl;
            m_segments.emplace_back();
        }
    } else if (sizeDiff < 0) {
        std::cout << "Removing " << sizeDiff << " Segments" << std::endl;
        // Remove the extra segments
        for (int i = 0; i > sizeDiff; i--) {
            m_segments.pop_back();
        }
    }
    
    std::cout << "WINDOW SIZE " << windowSize << std::endl;
    
    std::cout << "Start x: " << m_handles.front().m_pos.x << ", y: " <<
            m_handles.front().m_pos.y << std::endl;
            
    std::cout << "End x: " << m_handles.back().m_pos.x << ", y: " <<
            m_handles.back().m_pos.y << std::endl;
    
    float stepSize = 1.0f / m_fidelity;
    //Temp
    //sf::Vector2f start = unitToPixel(windowSize, m_endpoints.first);
    //sf::Vector2f end = unitToPixel(windowSize, m_endpoints.second);
    //std::cout << "Start " << start << std::endl;
    //std::cout << "End " << end << std::endl;
    //float diffX = (end.x - start.x) * stepSize;
    //float diffY = (end.y - start.y) * stepSize;
    //float length = std::sqrt(std::pow(diffX, 2) + std::pow(diffY, 2));
    //
    //std::cout << "stepSize: " << stepSize << std::endl;
    //std::cout << "diffx: " << diffX << ", diffy: " << diffY << std::endl;
    
    //std::cout << "Length " << length << std::endl;
            
    sf::Vector2f start;
    sf::Vector2f end = GetPointOnCurve(0);
    int i = 0;
    for (std::list<Segment>::iterator it = m_segments.begin();
            it != m_segments.end(); it++) {
        float t = stepSize * i;
        start = end;
        end = GetPointOnCurve(t + stepSize);
        
        
        sf::Vector2f diff = unitToPixel(windowSize, end) - unitToPixel(windowSize, start);
        
        it->m_shape.setSize(sf::Vector2f(
                std::sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2)), 4));
        it->m_shape.setOrigin(0, it->m_shape.getSize().y / 2);
        it->m_shape.setRotation((180 / M_PI) * std::atan2(diff.y, diff.x));
        //sf::Vector2f pos = unitToPixel(windowSize, m_handles.front().m_pos + (float) i * diff);
        
        std::cout << "Segment " << i << ": " << start << std::endl;
        
        it->m_pos = start;
        //it->m_shape.setPosition(unitToPixel(windowSize, start));
        i++;
    }
    m_constructed = true;
    std::cout << "Rendered " << i << " shapes" << std::endl;
}

void BezierCurve::render(sf::RenderWindow& window, const sf::Vector2f &windowSize)
{
    sf::Color fillColor;
    if (m_selected)
        fillColor = sf::Color(100, 100, 255);
    else
        fillColor = sf::Color::Blue;
    
    int i = 0;
    for (std::list<Segment>::iterator it = m_segments.begin();
            it != m_segments.end(); it++) {
        if (it->isRenderable()) {
            it->m_shape.setFillColor(fillColor);
            it->render(window, windowSize);
        }
        //std::cout << "Drawing num " << i++ << std::endl;
        //std::cout << "X: " << it->getSize().x << ", Y: " << it->getSize().y << std::endl;
    }
    
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        //std::cout << "Handle " << it->m_pos << std::endl;
        if (it->isRenderable()) {
            it->m_shape.setFillColor(sf::Color::Yellow);
            it->render(window, windowSize);
        }
        //std::cout << "Drawing num " << i++ << std::endl;
        //std::cout << "X: " << it->getSize().x << ", Y: " << it->getSize().y << std::endl;
    }
}

void BezierCurve::update(sf::Event& event, const sf::Vector2f& windowSize)
{
    if (!m_constructed)
        ConstructCurve(windowSize);
    
    bool selectedHandle = false;
    /* Process all the handles (as they have priority for click and drag if
     * visible) */
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        it->update(event, windowSize);
        if (it->m_selected) {
            std::cout << "Selected Handle" << std::endl;
            selectedHandle = true;
            /* Only select one to prevent issues with dragging multiple
             * overlapping points */
            break;
        }
    }
    
    switch (event.type) {
        case sf::Event::MouseButtonPressed:
        {
            if (selectedHandle)
                break; /* Clicking handles has priority (it's drawn on top) */
            
            for (std::list<Segment>::iterator it = m_segments.begin();
                    it != m_segments.end(); it++) {
                if (it->m_shape.getGlobalBounds().contains(event.mouseButton.x,
                        event.mouseButton.y)) {
                    /* LMB to highlight bezier (drag to move),
                     * RMB to unhighlight */
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        m_moving = true;
                        m_lastDragPos.x = event.mouseButton.x;
                        m_lastDragPos.y = event.mouseButton.y;
                        
                        m_selected = true;
                        /* Only render handles when selected */
                        for (std::list<Handle>::iterator it = m_handles.begin();
                                it != m_handles.end(); it++) {
                            it->setRenderable(true);
                        }
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        m_selected = false;
                        /* Only render handles when selected */
                        for (std::list<Handle>::iterator it = m_handles.begin();
                                it != m_handles.end(); it++) {
                            it->setRenderable(false);
                        }
                    }
                    break;
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
            m_moving = false;
            break;
        case sf::Event::MouseMoved:
            if (selectedHandle) {
                /* Redraw bezier as handles can change the shape */
                ConstructCurve(windowSize);
            } else if (m_moving) {
                
                
                sf::Vector2f delta(pixelToUnit(windowSize,
                        sf::Vector2f(event.mouseMove.x, event.mouseMove.y)) -
                        pixelToUnit(windowSize, m_lastDragPos));
                std::cout << "Delta: " << delta << std::endl;
                for (std::list<Segment>::iterator it = m_segments.begin();
                        it != m_segments.end(); it++) {
                    it->m_pos += delta;
                }
                int i = 0;
                for (std::list<Handle>::iterator it = m_handles.begin();
                        it != m_handles.end(); it++) {
                    if (i == 0)
                        std::cout << "prev " << it->m_pos << std::endl;
                    it->m_pos += delta;
                    if (i == 0)
                        std::cout << "after " << it->m_pos << std::endl;
                    i ++;
                }
                m_lastDragPos.x = event.mouseMove.x;
                m_lastDragPos.y = event.mouseMove.y;
                //m_selectedNode->x = pixelToUnit(windowSize.x, m_nodeWidth, event.mouseMove.x);
                //m_selectedNode->y = pixelToUnit(windowSize.y, m_nodeWidth, event.mouseMove.y);
            }
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Up:
                    if (m_fidelity < 300)
                        m_fidelity++;
                    ConstructCurve(windowSize);
                    break;
                case sf::Keyboard::Down:
                    if (m_fidelity >= 2)
                        m_fidelity--;
                    ConstructCurve(windowSize);
                    break;
                default:
                    break;
            }
    }
}