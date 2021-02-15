#ifndef _INFOPANE_H
#define _INFOPANE_H

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "LayeredRenderable.h"

/*
union InfoContent
{
    int integer,
    std::string text
};
*/

class InfoPane : public LayeredRenderable
{
private:
    
    class InfoContent : public Renderable
    {
    private:
        std::string m_content;
        
        sf::RectangleShape m_background;
        sf::Color m_backgroundColor;
        
        int m_numLines;
        std::list<sf::Text> m_textDisplay;
        sf::Color m_textColor;
    public:
        InfoContent() {};
        InfoContent(const std::string &text);
        
        sf::Vector2f m_pos;
        bool m_expanded;
        
        void setBackgroundColor(const sf::Color &col);
        void setContent(const std::string &text);
        void setExpanded(bool state);
        
        const sf::Vector2f& getSize();
        
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize,
                bool &clickedOn);
        void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
        bool contains(float x, float y);
    protected:
        void streamOut(std::ostream& os) const {};
    };
    
    
    std::map<std::string, InfoContent> m_content;
    sf::Vector2f m_pos;
    
    sf::RectangleShape m_background;
    sf::Color m_backgroundColor;
    sf::Color m_contentColor;
public:
    InfoPane();
    
    void setBackgroundColor(const sf::Color &col);
    void setPosition(int x, int y);
    void updateContents();
    
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
    bool contains(float x, float y) { return false; };
protected:
    void streamOut(std::ostream& os) const {};
};






#endif