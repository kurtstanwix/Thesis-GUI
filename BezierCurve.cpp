#include "BezierCurve.h"


#include <cmath>
#include <iostream>

#include "plog/Log.h"

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

void BezierCurve::Segment::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn)
{
}

void BezierCurve::Segment::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    m_shape.setPosition(unitToPixel(windowSize, m_pos));
    window.draw(m_shape);
}

void BezierCurve::Segment::streamOut(std::ostream& os) const
{
    os << m_pos;
}

bool BezierCurve::Segment::contains(float x, float y)
{
    return m_shape.getGlobalBounds().contains(x, y);
}

/*
 * HANDLE
 */

BezierCurve::Handle::Handle()
{
    m_shape.setFillColor(sf::Color::Yellow);
    m_shape.setSize({15, 15});
    setRenderable(false);
    m_selected = false;
    m_moving = false;
    m_shape.setOrigin(m_shape.getSize() / 2.0f);
}

BezierCurve::Handle::Handle(const sf::Vector2f &pos)
    :
        Handle()
{
    m_pos.x = pos.x;
    m_pos.y = pos.y;
}

void BezierCurve::Handle::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn)
{
    if(event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
                /* Can't interact with if not visible */
                if (isRenderable() && !clickedOn) {
                    PLOGI << "asdasdghjahsfghjagsfjhgajfsgajsfg";
                    if (contains(event->mouseButton.x, event->mouseButton.y)) {
                        PLOGI << "Handle contains mouse";
                        clickedOn = true;
                        /* LMB to highlight handle (drag to move),
                         * RMB to unhighlight */
                        if (event->mouseButton.button == sf::Mouse::Left) {
                            m_selected = true;
                            m_moving = true;
                            //m_lastDragPos.x = event->mouseButton.x;
                            //m_lastDragPos.y = event->mouseButton.y;
                        } else if (event->mouseButton.button == sf::Mouse::Right) {
                            m_selected = false;
                        }
                    }
                }
                break;
            case sf::Event::MouseButtonReleased:
                PLOGI << "Release that handle";
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                /* Can't interact with if not visible */
                if (isRenderable() && m_moving) {
                    PLOGI << "Moving Bezier handle";
                    /* Clicked on and dragged, move with the mouse */
                    //sf::Vector2f delta = pixelToUnitVector(windowSize,
                    //        sf::Vector2f(event->mouseMove.x, event->mouseMove.y) -
                    //        m_lastDragPos);
                    m_pos = pixelToUnit(windowSize,
                            {event->mouseMove.x, event->mouseMove.y});
                    //m_pos += delta;
                    
                    //m_lastDragPos.x = event->mouseMove.x;
                    //m_lastDragPos.y = event->mouseMove.y;
                }
                break;
        }
    }
}

void BezierCurve::Handle::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    m_shape.setPosition(unitToPixel(windowSize, m_pos));
    window.draw(m_shape);
}

void BezierCurve::Handle::streamOut(std::ostream& os) const
{
    os << m_pos;
}

bool BezierCurve::Handle::contains(float x, float y)
{
    return m_shape.getGlobalBounds().contains(x, y);
}





BezierCurve::BezierCurve(int fidelity, std::list<sf::Vector2f> controlPoints)
    :
        m_fidelity(fidelity),
        m_selected(false),
        m_constructed(false),
        m_color(150,50,20),
        m_width(4)
{
    m_moving = false;
    for (std::list<sf::Vector2f>::iterator it = controlPoints.begin();
            it != controlPoints.end(); it++) {
        std::cout << "Adding POS: " << *it << std::endl;
        m_handles.emplace_back(*it);
        std::cout << "Added POS: " << m_handles.back().m_pos << std::endl;
    }
    
    setRenderable(true);
}

void BezierCurve::streamOut(std::ostream& os) const
{
    os << "(";
    for (std::list<Handle>::const_iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        os << *it << ", ";
    }
    os << ")";
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

void BezierCurve::ConstructCurve()
{
    std::cout << "Constructing Bezier " << m_segments.size() << std::endl;
    int sizeDiff = m_fidelity - m_segments.size();
    if (sizeDiff > 0) {
        // Add the missing segments
        std::cout << "Filling " << sizeDiff << " Segments" << std::endl;
        for (int i = 0; i < sizeDiff; i++) {
            m_segments.emplace_back();
        }
    } else if (sizeDiff < 0) {
        std::cout << "Removing " << sizeDiff << " Segments" << std::endl;
        // Remove the extra segments
        for (int i = 0; i > sizeDiff; i--) {
            m_segments.pop_back();
        }
    }
    
    
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
        
        
        it->m_shape.setOrigin(0, m_width / 2);
        it->m_shape.setRotation((180 / M_PI) * std::atan2(end.y - start.y,
                end.x - start.x));
        //sf::Vector2f pos = unitToPixel(windowSize, m_handles.front().m_pos + (float) i * diff);
        
        
        it->m_pos = start;
        //it->m_shape.setPosition(unitToPixel(windowSize, start));
        i++;
    }
    m_constructed = true;
    std::cout << "Rendered " << i << " shapes" << std::endl;
}

bool BezierCurve::contains(float x, float y)
{
    for (std::list<Segment>::iterator it = m_segments.begin();
            it != m_segments.end(); it++) {
        if (it->contains(x, y))
            return true;
    }
    /* Handles are only included if the curve is selected */
    if (m_selected) {
        for (std::list<Handle>::iterator it = m_handles.begin();
                it != m_handles.end(); it++) {
            if (it->contains(x, y)) {
                return true;
            }
        }
    }
    return false;
}

void BezierCurve::render(sf::RenderWindow& window, const sf::Vector2f &windowSize)
{
    sf::Color fillColor;
    if (m_selected)
        fillColor = sf::Color(std::min(m_color.r + 100, 255), 
                std::min(m_color.g + 100, 255),
                std::min(m_color.b + 100, 255));
    else
        fillColor = m_color;
    
        
    int i = 0;
    for (std::list<Segment>::iterator it = m_segments.begin();
            it != m_segments.end(); it++) {
        if (it->isRenderable()) {
            sf::Vector2f diff;
            std::list<Segment>::iterator nextIT = std::next(it);
            if (nextIT == m_segments.end())
                diff = unitToPixelVector(windowSize, 
                        m_handles.back().m_pos - it->m_pos);
            else
                diff = unitToPixelVector(windowSize,
                        nextIT->m_pos - it->m_pos);
            
            it->m_shape.setSize({std::sqrt(std::pow(diff.x, 2) +
                    std::pow(diff.y, 2)), m_width});
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

void BezierCurve::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    if (!m_constructed)
        ConstructCurve();
    bool selectedHandle = false;
    /* Process all the handles (as they have priority for click and drag if
     * visible) */
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        it->update(event, windowSize, clickedOn);
        if (it->isMoving()) {
            PLOGI<<"HOW";
            selectedHandle = true;
            /* Only select one to prevent issues with dragging multiple
             * overlapping points */
            break;
        }
    }
    
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                bool toDeselect = !selectedHandle;
                /* If a handle wasn't selected, check if the remainder was */
                if (!clickedOn && !selectedHandle) {
                    PLOGD<<"Dooooing";
                    if (contains(event->mouseButton.x, event->mouseButton.y)) {
                        clickedOn = true;
                        if (event->mouseButton.button == sf::Mouse::Left) {
                            toDeselect = false;
                            m_moving = true;
                            m_lastDragPos.x = event->mouseButton.x;
                            m_lastDragPos.y = event->mouseButton.y;
                            setSelected(true);
                        } else if (event->mouseButton.button == sf::Mouse::Right) {
                            setSelected(false);
                        }
                    }
                }
                if (toDeselect &&
                        !(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
                    setSelected(false);
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                if (selectedHandle) {
                    /* Redraw bezier as handles can change the shape */
                    ConstructCurve();
                } else if (m_moving) {
                    sf::Vector2f delta(pixelToUnit(windowSize,
                            sf::Vector2f(event->mouseMove.x, event->mouseMove.y)) -
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
                    m_lastDragPos.x = event->mouseMove.x;
                    m_lastDragPos.y = event->mouseMove.y;
                    //m_selectedNode->x = pixelToUnit(windowSize.x, m_nodeWidth, event->mouseMove.x);
                    //m_selectedNode->y = pixelToUnit(windowSize.y, m_nodeWidth, event->mouseMove.y);
                }
                break;
            case sf::Event::KeyPressed:
                switch (event->key.code) {
                    case sf::Keyboard::Up:
                        if (m_selected) {
                            if (m_fidelity < 300)
                                m_fidelity++;
                            ConstructCurve();
                        }
                        break;
                    case sf::Keyboard::Down:
                        if (m_selected) {
                            if (m_fidelity >= 2)
                                m_fidelity--;
                            ConstructCurve();
                        }
                        break;
                    default:
                        break;
                }
        }
    }
}

void BezierCurve::setSelected(bool state)
{
    m_selected = state;
    /* Only render handles when selected */
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        //if (it != m_handles.begin() && it != std::prev(m_handles.end())) {
            it->setRenderable(state);
        //}
    }
}