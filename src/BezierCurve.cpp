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
                // Can't interact with if not visible
                if (isRenderable() && !clickedOn) {
                    if (contains(event->mouseButton.x, event->mouseButton.y)) {
                        clickedOn = true;
                        /* LMB to highlight handle (drag to move),
                         * RMB to unhighlight */
                        if (event->mouseButton.button == sf::Mouse::Left) {
                            m_selected = true;
                            m_moving = true;
                        } else if (event->mouseButton.button == sf::Mouse::Right) {
                            m_selected = false;
                        }
                    }
                }
                break;
            case sf::Event::MouseButtonReleased:
                // If mouse is released, will always stop moving
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                // Can't interact with if not visible
                if (isRenderable() && m_moving) {
                    // Clicked on and dragged, move with the mouse
                    m_pos = pixelToUnit(windowSize,
                            {event->mouseMove.x, event->mouseMove.y});
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

// Fidelity represents how many segments make a bezier curve, higher amounts will
// lead to a smooth curve but require more processing
BezierCurve::BezierCurve(int fidelity, std::list<sf::Vector2f> controlPoints)
    :
        m_fidelity(fidelity),
        m_constructed(false),
        m_width(4)
{
    m_color = sf::Color(0, 150, 0);
    m_moving = false;
    for (std::list<sf::Vector2f>::iterator it = controlPoints.begin();
            it != controlPoints.end(); it++) {
        PLOGD << "Adding POS: " << *it;
        m_handles.emplace_back(*it);
        PLOGD << "Added POS: " << m_handles.back().m_pos;
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

// Calculate the position on the bezier curve using the handle positions. t must be [0, 1]
// (Can be viewed as percentage along the curve)
sf::Vector2f BezierCurve::GetPointOnCurve(float t)
{
    std::list<std::list<sf::Vector2f>> hierarchy(m_handles.size());
    // Add the first layer (the handle positions)
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        hierarchy.front().push_back(it->m_pos);
    }
    /* Step into each layer, calculating the current parametric bezier position
     * with respect to the upper layer */
    for (std::list<std::list<sf::Vector2f>>::iterator it = std::next(hierarchy.begin());
            it != hierarchy.end(); it++) {
        std::list<sf::Vector2f> &upperLayer = *std::prev(it);
        // Find the parametric point on the inner layer
        for (std::list<sf::Vector2f>::iterator iit = upperLayer.begin();
                std::next(iit) != upperLayer.end(); iit++) {
            it->emplace_back((1.0f - t) * *iit + t * *std::next(iit));
        }
    }
    return hierarchy.back().back();
}

void BezierCurve::ConstructCurve()
{
    PLOGD << "Constructing Bezier " << m_segments.size();
    int sizeDiff = m_fidelity - m_segments.size();
    if (sizeDiff > 0) {
        // Add the missing segments
        PLOGD << "Filling " << sizeDiff << " Segments";
        for (int i = 0; i < sizeDiff; i++) {
            m_segments.emplace_back();
        }
    } else if (sizeDiff < 0) {
        PLOGD << "Removing " << sizeDiff << " Segments";
        // Remove the extra segments
        for (int i = 0; i > sizeDiff; i--) {
            m_segments.pop_back();
        }
    }
    
    
    PLOGD << "Start x: " << m_handles.front().m_pos.x << ", y: " <<
            m_handles.front().m_pos.y;
            
    PLOGD << "End x: " << m_handles.back().m_pos.x << ", y: " <<
            m_handles.back().m_pos.y;
    
    float stepSize = 1.0f / m_fidelity;
            
    sf::Vector2f start;
    sf::Vector2f end = GetPointOnCurve(0);
    int i = 0;
    for (std::list<Segment>::iterator it = m_segments.begin();
            it != m_segments.end(); it++) {
        float t = stepSize * i;
        start = end;
        end = GetPointOnCurve(t + stepSize);
        
        // Rotate the segment to point in the direction of the step
        it->m_shape.setOrigin(0, m_width / 2);
        it->m_shape.setRotation((180 / M_PI) * std::atan2(end.y - start.y,
                end.x - start.x));
        
        it->m_pos = start;
        i++;
    }
    m_constructed = true;
    PLOGD << "Constructed " << i << " shapes";
}

bool BezierCurve::contains(float x, float y)
{
    for (std::list<Segment>::iterator it = m_segments.begin();
            it != m_segments.end(); it++) {
        if (it->contains(x, y))
            return true;
    }
    // Handles are only included if the curve is selected (hidden otherwise)
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
    int i = 0;
    // Render all segments if selected based on the constructed positions
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
            it->m_shape.setFillColor(m_color);
            it->render(window, windowSize);
        }
    }
    
    // Render all handles if selected
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        if (it->isRenderable()) {
            it->m_shape.setFillColor(sf::Color::Yellow);
            it->render(window, windowSize);
        }
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
        if (it->isMoving())
            selectedHandle = true;
            /* Only select one to prevent issues with dragging multiple
             * overlapping points */
    }
    
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                bool toDeselect = !selectedHandle;
                // If a handle wasn't selected, check if the remainder was
                if (!clickedOn && !selectedHandle) {
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
                // Can hold down CTRL to select multiple items
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
                    // Redraw bezier as handles can change the shape
                    ConstructCurve();
                } else if (m_moving) {
                    sf::Vector2f delta(pixelToUnit(windowSize,
                            sf::Vector2f(event->mouseMove.x, event->mouseMove.y)) -
                            pixelToUnit(windowSize, m_lastDragPos));
                    PLOGV << "Delta: " << delta;
                    // Move all the curve segments as one
                    for (std::list<Segment>::iterator it = m_segments.begin();
                            it != m_segments.end(); it++) {
                        it->m_pos += delta;
                    }
                    int i = 0;
                    // Move all handles with the curve
                    for (std::list<Handle>::iterator it = m_handles.begin();
                            it != m_handles.end(); it++) {
                        it->m_pos += delta;
                        i ++;
                    }
                    m_lastDragPos.x = event->mouseMove.x;
                    m_lastDragPos.y = event->mouseMove.y;
                }
                break;
            case sf::Event::KeyPressed:
                switch (event->key.code) {
                    case sf::Keyboard::Up:
                        // Increase the fidelity of the curve
                        if (m_selected) {
                            if (m_fidelity < 300)
                                m_fidelity++;
                            ConstructCurve();
                        }
                        break;
                    case sf::Keyboard::Down:
                        // Decrease the fidelity of the curve
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
    // Only render handles when selected
    for (std::list<Handle>::iterator it = m_handles.begin();
            it != m_handles.end(); it++) {
        it->setRenderable(state);
    }
}