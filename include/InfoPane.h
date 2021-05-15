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
        sf::CircleShape m_expandIcon;
        
        int m_numLines;
        std::list<std::list<std::pair<bool, sf::Text>>> m_textDisplay;
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
    
    
    sf::Text m_title;
    std::map<std::string, std::pair<sf::Text, InfoContent>> m_content;
    std::list<std::string> m_contentOrder;
    sf::Vector2f m_pos;
    
    sf::RectangleShape m_background;
    sf::RectangleShape m_dividerLine;
    sf::Color m_backgroundColor;
    sf::Color m_textColor;
    sf::Color m_contentColor;
    
    sf::RectangleShape m_closeBox;
    sf::RectangleShape m_closeCross1;
    sf::RectangleShape m_closeCross2;
public:
    InfoPane();
    
    void setBackgroundColor(const sf::Color &col);
    bool setContent(const std::string &label, const std::string &content, bool keepPosition=false);
    void setPosition(int x, int y);
    void setPosition(const sf::Vector2f &pos);
    void setTitle(const std::string &title);
    void updateContents();
    
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize);
    bool contains(float x, float y);
protected:
    void streamOut(std::ostream& os) const {};
};






#endif