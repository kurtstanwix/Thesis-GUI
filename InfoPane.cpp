#include "InfoPane.h"

#include <iostream>

#include <plog/Log.h>
#include <SFML/System/Vector2.hpp>

#include "Utility.h"

#define CONTENT_WIDTH 200
#define CONTENT_BUFFER 10
#define CHAR_SIZE 15
#define TITLE_CHAR_SIZE 30

InfoPane::InfoContent::InfoContent(const std::string &text)
{
    setContent(text);
    m_expandIcon.setPointCount(3);
    int r = FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE) / 3;
    m_expandIcon.setRadius(r);
    m_expandIcon.setOrigin(r, r);
    setExpanded(false);
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
    if (state && numLines != 1) {
        m_background.setSize(sf::Vector2f(CONTENT_WIDTH,
                (numLines + 1) *
                FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE)));
        m_expandIcon.setRotation(0);
    } else {
        m_background.setSize(sf::Vector2f(CONTENT_WIDTH,
                FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE)));
        m_expandIcon.setRotation(180);
    }
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
    
    m_expandIcon.setFillColor(m_textColor);
    
    int lineSpacing =
            FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE);
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
        }
        m_expandIcon.setPosition(m_pos.x + m_background.getSize().x / 2,
                m_pos.y + lineSpacing / 2);
        window.draw(m_expandIcon);
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
    m_moving = false;
    m_renderable = false;
    //m_content.emplace("type", "This is a test, te AV let's keep trying AH this abcdefghijklmnopqrst it kinda works but may actually not");
    setContent("type", "This is a test, te AV let's keep trying AH this abcdefghijklmnopqrstuvwxyz123456789 it kinda works but may actually not");
    m_content["type"].second.setExpanded(true);
    
    //m_content.emplace("example", "Yet another little test for this");
    setContent("example", "Yet another little test for this");
    //m_content["example"].setExpanded(true);
    //setContent("example", "Temp");
    
    setContent("one test", "");
    setContent("one test", "testing");
    setTitle("Testing Title");
    
    m_title.setFont(FontManager::getInstance().getFont());
    m_title.setCharacterSize(TITLE_CHAR_SIZE);
    setPosition(1200 / 2, 1200 / 2);
    //setBackgroundColor(sf::Color::Yellow);
    setBackgroundColor(sf::Color(175, 0, 175, 255));
    
    
    m_closeCross1.setFillColor(sf::Color::Red);
    m_closeCross2.setFillColor(sf::Color::Red);
    m_closeCross1.setSize(sf::Vector2f(TITLE_CHAR_SIZE / 2, 3));
    m_closeCross2.setSize(sf::Vector2f(TITLE_CHAR_SIZE / 2, 3));
    m_closeCross1.setOrigin(m_closeCross1.getSize() / 2.0f);
    m_closeCross2.setOrigin(m_closeCross2.getSize() / 2.0f);
    m_closeCross1.setRotation(45);
    m_closeCross2.setRotation(-45);
    
    m_closeBox.setSize(sf::Vector2f(TITLE_CHAR_SIZE / 1.5,
            TITLE_CHAR_SIZE / 1.5));
}

void InfoPane::setBackgroundColor(const sf::Color &col)
{
    m_backgroundColor = col;
    m_contentColor = col - sf::Color(100, 100, 100, 0);
    
    /* Determine text colour for best contrast on background, found from:
     * https://stackoverflow.com/questions/3942878/how-to-decide-font-color-in-white-or-black-depending-on-background-color
     */
    if ((col.r * 0.299 + col.g * 0.587 + col.b * 0.114) > 186)
        m_textColor = sf::Color::Black;
    else
        m_textColor = sf::Color::White;
}

bool InfoPane::setContent(const std::string &label, const std::string &content)
{
    std::map<std::string, std::pair<sf::Text, InfoContent>>::iterator val =
            m_content.find(label);
    if (val == m_content.end())
        m_content.emplace(std::make_pair(label,
                std::pair<sf::Text, InfoContent>(std::piecewise_construct,
                std::forward_as_tuple(label,
                FontManager::getInstance().getFont(), CHAR_SIZE),
                std::forward_as_tuple(content))));
    else
        val->second = std::pair<sf::Text, InfoContent>(
                std::piecewise_construct, std::forward_as_tuple(label,
                FontManager::getInstance().getFont(), CHAR_SIZE),
                std::forward_as_tuple(content));
    
    
    
        /*m_content.emplace(std::piecewise_construct,
                std::forward_as_tuple(label), std::forward_as_tuple(
                std::piecewise_construct, std::forward_as_tuple(label,
                FontManager::getInstance().getFont(), CHAR_SIZE),
                std::forward_as_tuple(content)));*/
    //else
    //    m_content
    
    
}

void InfoPane::setPosition(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
    
    updateContents();
}

void InfoPane::setPosition(const sf::Vector2f &pos)
{
    setPosition(pos.x, pos.y);
}

void InfoPane::setTitle(const std::string &title)
{
    m_title.setString(title);
}

void InfoPane::updateContents()
{
    /* There are 4 horizontal buffers:
     * before/after label, before/after content */
    sf::Vector2f size(CONTENT_WIDTH + 4 * CONTENT_BUFFER, 2 * CONTENT_BUFFER);
    float maxLabelWidth = 0;
    int titleHeight = m_title.getLocalBounds().height;
    size.y += titleHeight; // Title height
    /* First pass sets label and content vertical spacing, label x positions
     * and calculates max label width */
    for (std::map<std::string, std::pair<sf::Text, InfoContent>>::iterator it =
            m_content.begin(); it != m_content.end(); it++) {
        it->second.second.m_pos.y = m_pos.y + size.y;
        it->second.first.setPosition(m_pos.x + CONTENT_BUFFER,
                m_pos.y + size.y);
        size.y += it->second.second.getSize().y + CONTENT_BUFFER;
        maxLabelWidth = std::max(maxLabelWidth,
                it->second.first.getLocalBounds().width);
    }
    
    m_dividerLine.setSize(sf::Vector2f(CONTENT_BUFFER / 3,
            size.y - (3 * CONTENT_BUFFER + titleHeight)));
    m_dividerLine.setPosition(m_pos.x + 2 * CONTENT_BUFFER + maxLabelWidth,
            m_pos.y + 2 * CONTENT_BUFFER + titleHeight);
    
    /* Second pass sets the horizontal spacing of all contents according to the
     * max label width */
    for (std::map<std::string, std::pair<sf::Text, InfoContent>>::iterator it =
            m_content.begin(); it != m_content.end(); it++) {
        it->second.second.m_pos.x = m_pos.x + 3 * CONTENT_BUFFER +
                m_dividerLine.getSize().x + maxLabelWidth;
    }
    
    size.x += maxLabelWidth + m_dividerLine.getSize().x;
    m_background.setSize(size);
    m_background.setPosition(m_pos);
    m_title.setPosition(m_pos.x +
            (size.x - m_title.getLocalBounds().width) / 2,
            m_pos.y + CONTENT_BUFFER - titleHeight / 2);
    
    m_closeBox.setPosition(m_pos.x + size.x - CONTENT_BUFFER -
            m_closeBox.getSize().x, m_pos.y + CONTENT_BUFFER);
    m_closeCross1.setPosition(m_closeBox.getPosition() +
            m_closeBox.getSize() / 2.0f);
    m_closeCross2.setPosition(m_closeBox.getPosition() +
            m_closeBox.getSize() / 2.0f);
}

void InfoPane::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    if (!m_renderable)
        return;
    
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
                if (!clickedOn) {
                    /* No elements were clicked, process a move event */
                    if (m_closeBox.getGlobalBounds().contains(
                            event->mouseButton.x, event->mouseButton.y)) {
                        clickedOn = true;
                        setRenderable(false);
                        return;
                    }
                }
        }
    }
    
    for (std::map<std::string, std::pair<sf::Text, InfoContent>>::iterator it =
            m_content.begin(); it != m_content.end(); it++) {
        bool expanded = it->second.second.m_expanded;
        it->second.second.update(event, windowSize, clickedOn);
        if (expanded != it->second.second.m_expanded)
            updateContents();
    }
    
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
                if (!clickedOn) {
                    /* No elements were clicked, process a move event */
                    if (contains(event->mouseButton.x, event->mouseButton.y)) {
                        clickedOn = true;
                        if (event->mouseButton.button == sf::Mouse::Left) {
                            m_moving = true;
                            m_lastDragPos.x = event->mouseButton.x;
                            m_lastDragPos.y = event->mouseButton.y;
                        }
                    }
                }
                break;
            case sf::Event::MouseButtonReleased:
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                if (m_moving) {
                    setPosition(m_pos.x + event->mouseMove.x - m_lastDragPos.x,
                            m_pos.y + event->mouseMove.y - m_lastDragPos.y);
                    m_lastDragPos.x = event->mouseMove.x;
                    m_lastDragPos.y = event->mouseMove.y;
                }
                //if (m_moving)
                //    setPosition(event->mouseMove.x, event->mouseMove.y);
        }
    }
    
}

void InfoPane::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize)
{
    if (!m_renderable)
        return;
    
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineThickness(3);
    m_background.setOutlineColor(
            m_backgroundColor - sf::Color(100, 100, 100, 0));
    m_dividerLine.setFillColor(m_background.getOutlineColor());
    m_title.setColor(m_textColor);
    m_closeBox.setFillColor(m_textColor);
    
    window.draw(m_background);
    window.draw(m_dividerLine);
    window.draw(m_title);
    
    window.draw(m_closeBox);
    window.draw(m_closeCross1);
    window.draw(m_closeCross2);
    //PLOGD << "Rendering Pane";
    
    for (std::map<std::string, std::pair<sf::Text, InfoContent>>::iterator it =
            m_content.begin(); it != m_content.end(); it++) {
        it->second.second.setBackgroundColor(m_contentColor);
        it->second.second.render(window, windowSize);
        it->second.first.setColor(m_textColor);
        window.draw(it->second.first);
    }
    //m_content["type"].render(window, windowSize);
}

bool InfoPane::contains(float x, float y)
{
    return m_background.getGlobalBounds().contains(x, y);
}

