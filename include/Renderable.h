#ifndef _RENDERABLE_H
#define _RENDERABLE_H 1

#include <chrono>
#include "SFML/Graphics.hpp"

class Renderable
{
public:
    /**
     * @brief Processes any events that have occured and updates the
     *        renderable's logic.
     * @param event the event to process, if applicable, can be nullptr
     * @param windowSize the size of the window the renderable is in
     * @param clickedOn true if this item has been clicked on. Used for order
     *        as an items on top of each other will both return true for a
     *        contains call, so the parent can use this to tell which one
     *        should actually process the click event.
     */
    virtual void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn) = 0;
    
    /**
     * @brief Renders this renderable onto the window if m_renderable is set.
     *        Will draw onto the top so the order of this call is important.
     * @param window screen to draw to
     * @param windowSize the size of the screen in pixels, can be found from
     *        window but is passed in as a reference for efficiency due to the
     *        frequency of this call.
     */
    virtual void render(sf::RenderWindow& window,
            const sf::Vector2f &windowSize) = 0;
    
    /**
     * @brief Check if the point (x, y) is inside the renderable
     * @param x the x coordinate to check
     * @param y the y coordinate to check
     * @return  true if (x, y) is in the shape, flase otherwise
     */
    virtual bool contains(float x, float y) = 0;
    
    /**
     * @brief Overloads the stream out operator in order to allow classes that
     *        inherit this to implement their own friendly stream out by
     *        implementing the streamOut method
     * @param os
     * @param r
     * @return 
     */
    friend std::ostream& operator<<(std::ostream& os, const Renderable& r) {
        r.streamOut(os);
        return os;
    }
    
    bool isRenderable(void)
    {
        return m_renderable;
    }
    
    void setRenderable(bool renderable)
    {
        m_renderable = renderable;
    }
    
    bool isMoving(void)
    {
        return m_moving;
    }
    
    void setColor(const sf::Color &col)
    {
        m_color = col;
    }

    const sf::Color getColor()
    {
        return m_color;
    }
    
    virtual void setSelected(bool state)
    {
        m_selected = state;
    }
    
    bool isSelected(void)
    {
        return m_selected;
    }
    
    
protected:
    static inline const sf::Color m_selectedColorOffset = sf::Color(100, 100, 100, 0);
    sf::Color m_color;
    sf::Vector2f m_lastDragPos;
    bool m_moving;
    bool m_renderable;
    bool m_selected = false;
    std::chrono::system_clock::time_point m_lastClickTime;
    
    virtual void streamOut(std::ostream& os) const = 0;
};

#endif