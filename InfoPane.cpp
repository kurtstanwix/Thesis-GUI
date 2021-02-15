#include "InfoPane.h"

#include <iostream>

#include <plog/Log.h>
#include <SFML/System/Vector2.hpp>

#include "Utility.h"

#define CONTENT_WIDTH 100
#define CHAR_SIZE 15

InfoPane::InfoContent::InfoContent(const std::string &text)
    :
        m_expanded(false)
{
    
    setContent(text);
}

const sf::Vector2f& InfoPane::InfoContent::getSize()
{
    return m_background.getSize();
}

void InfoPane::InfoContent::setContent(const std::string &text)
{
    m_content.assign(text);
    m_textDisplay.clear(); /* Remove all current lines */
    
    sf::Text tempText(std::string(m_content), FontManager::getInstance().getFont(),
            CHAR_SIZE);
    
    int numChars = m_content.length();
    int currI = 0, currWidth = 0, startI = 0, widthOffset = 0;
    while (currI < numChars) {
        currWidth = tempText.findCharacterPos(currI).x - widthOffset;
        if (currWidth >= CONTENT_WIDTH) {
            m_textDisplay.emplace_back();
            sf::Text &textItem = m_textDisplay.back();
            textItem.setFont(FontManager::getInstance().getFont());
            textItem.setCharacterSize(CHAR_SIZE);
            
            /* Want to end line at end of previous word */
            std::size_t endI = m_content.rfind(' ', currI - 1);
            
            if (endI <= startI) { /* Word does not fit on just one line */
                textItem.setString(m_content.substr(startI, currI - startI - 1));
                startI = currI - 1;
            } else {
                textItem.setString(m_content.substr(startI, endI - startI));
                startI = endI + 1;
            }
            
            currI = startI;
            widthOffset = tempText.findCharacterPos(startI).x;
            currWidth = 0;
        } else {
            currI++;
        }
    }
    if (startI != numChars) { /* Add any remaining characters to final line */
        m_textDisplay.emplace_back(m_content.substr(startI, numChars - startI),
                FontManager::getInstance().getFont(), CHAR_SIZE);
    }
    PLOGD << "Num Lines: " << m_textDisplay.size();
}

void InfoPane::InfoContent::setBackgroundColor(const sf::Color &col)
{
    m_backgroundColor = col;
    /* Determine text colour for best contrast on background, found from:
     * https://stackoverflow.com/questions/3942878/how-to-decide-font-color-in-white-or-black-depending-on-background-color
     */
    if ((col.r * 0.299 + col.g * 0.587 + col.b * 0.114) > 186)
        m_textColor = sf::Color::Black;
    else
        m_textColor = sf::Color::White;
}

void InfoPane::InfoContent::setExpanded(bool state)
{
    int numLines = m_textDisplay.size();
    if (state && numLines != 1)
        m_background.setSize(sf::Vector2f(CONTENT_WIDTH,
                (numLines + 1) *
                FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE)));
    else
        m_background.setSize(sf::Vector2f(CONTENT_WIDTH,
                FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE)));
    PLOGD << "Set Expanded " << m_expanded << ", Size: " << m_background.getSize();
    m_expanded = state;
}

void InfoPane::InfoContent::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn)
{
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                //bool toDeselect = true;
                if (!clickedOn) {
                    if (contains(event->mouseButton.x, event->mouseButton.y)) {
                        clickedOn = true;
                        PLOGD << "Mouse Press on Content";
                        //toDeselect = false;
                        setExpanded(!m_expanded);
                    }
                }
                break;
            }
            /*
            case sf::Event::MouseButtonReleased:
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                if (m_moving) {
                    PLOGD << "Mouse moved x: " << event->mouseMove.x <<
                            ", y: " << event->mouseMove.y;
                    m_pos = pixelToUnit(windowSize,
                            {event->mouseMove.x, event->mouseMove.y});
                }*/
        }
    }
}

void InfoPane::InfoContent::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize)
{
    // Temp ///
    //m_pos.x = windowSize.x / 2 - m_background.getSize().x / 2;
    //m_pos.y = windowSize.y / 2 - m_background.getSize().y / 2;
    ///////////
    
    
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineThickness(3);
    m_background.setOutlineColor(
            m_backgroundColor - sf::Color(100, 100, 100, 0));
    m_background.setPosition(m_pos);
    window.draw(m_background);
    
    int lineSpacing =
            m_textDisplay.front().getFont()->getLineSpacing(CHAR_SIZE);
    /* Offset for each text line to keep in bounding box */
    int offset = (lineSpacing - CHAR_SIZE) / 2;
    if (m_textDisplay.size() > 1) {
        if (m_expanded) {
            int prevHeight = lineSpacing;
            for (std::list<sf::Text>::iterator it = m_textDisplay.begin();
                    it != m_textDisplay.end(); it++) {
                it->setColor(m_textColor);
                it->setPosition(m_pos.x, m_pos.y + prevHeight - offset);
                prevHeight += lineSpacing;
                window.draw(*it);
            }
            /* Draw up (collapse) arrow */
        } else {
            /* Draw down (expand) arrow */
        }
    } else {
        if (m_textDisplay.size() == 1) {
            sf::Text &tempText = m_textDisplay.front();
            tempText.setColor(m_textColor);
            tempText.setPosition(m_pos.x, m_pos.y - offset);
            window.draw(tempText);
        }
    }
    
    
}

bool InfoPane::InfoContent::contains(float x, float y)
{
    return m_background.getGlobalBounds().contains(x, y);
}


InfoPane::InfoPane()
{
    m_content.emplace("type", "This is a test, te AV let's keep trying AH this abcdefghijklmnopqrst it kinda works but may actually not");
    m_content["type"].setExpanded(true);
    
    m_content.emplace("example", "Yet another little test for this");
    m_content["example"].setExpanded(true);
    
    
    setPosition(1200 / 2, 1200 / 2);
    setBackgroundColor(sf::Color(175, 0, 175, 255));
}

void InfoPane::setBackgroundColor(const sf::Color &col)
{
    m_backgroundColor = col;
    m_contentColor = col - sf::Color(100, 100, 100, 0);
}

void InfoPane::setPosition(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
    
    updateContents();
}

void InfoPane::updateContents()
{
    sf::Vector2f size(400, 0);
    size.y += 30; // Title height
    for (std::map<std::string, InfoContent>::iterator it = m_content.begin();
            it != m_content.end(); it++) {
        it->second.m_pos = m_pos + sf::Vector2f(size.x / 2, size.y);
        size.y += it->second.getSize().y + 10;
    }
    
    m_background.setSize(size);
}

void InfoPane::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    for (std::map<std::string, InfoContent>::iterator it = m_content.begin();
            it != m_content.end(); it++) {
        bool expanded = it->second.m_expanded;
        it->second.update(event, windowSize, clickedOn);
        if (expanded != it->second.m_expanded)
            updateContents();
    }
}

void InfoPane::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize)
{
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineThickness(3);
    m_background.setOutlineColor(
            m_backgroundColor - sf::Color(100, 100, 100, 0));
    m_background.setPosition(m_pos);
    window.draw(m_background);
    //PLOGD << "Rendering Pane";
    
    for (std::map<std::string, InfoContent>::iterator it = m_content.begin();
            it != m_content.end(); it++) {
        it->second.setBackgroundColor(m_contentColor);
        it->second.render(window, windowSize);
    }
    //m_content["type"].render(window, windowSize);
}


