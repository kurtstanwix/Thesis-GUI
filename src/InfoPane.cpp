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
    
    /* Temporary text item using the given font to allow text distances to be known */
    sf::Text tempText(std::string(m_content), FontManager::getInstance().getFont(),
            CHAR_SIZE);
    
    /* Every new line is it's own text item to allow for more flexible
     * positioning */
    int numChars = m_content.length();
    int currI = 0, currWidth = 0, startI = 0, widthOffset = 0;
    bool isHighlighted = false;


    m_textDisplay.emplace_back();
    std::list<std::pair<bool, sf::Text>> *currRow = &m_textDisplay.back();

    while (currI < numChars) {
        if (tempText.getString()[currI] == '`') {
            sf::String temp = tempText.getString();
            temp.replace(currI, 1, "");
            tempText.setString(temp);
            numChars--;
            if (currI != startI) {
                /* Previous iteration will guarantee that the text before this character will fit on the line */
                currRow->emplace_back();
                std::pair<bool, sf::Text> &textPair = currRow->back();
                textPair.first = isHighlighted;
                isHighlighted = !isHighlighted;
                textPair.second.setFont(FontManager::getInstance().getFont());
                textPair.second.setCharacterSize(CHAR_SIZE);
                textPair.second.setString(tempText.getString().substring(startI, currI - startI));
                startI = currI;
            }
            continue;
        }
        currWidth = tempText.findCharacterPos(currI).x - widthOffset;
        if (currWidth >= CONTENT_WIDTH || tempText.getString()[currI] == '\n') {
            //m_textDisplay.emplace_back();
            //std::pair<bool, std::list<sf::Text>> textPair = m_textDisplay.back();
            currRow->emplace_back();
            std::pair<bool, sf::Text> &textPair = currRow->back();
            textPair.second.setFont(FontManager::getInstance().getFont());
            textPair.second.setCharacterSize(CHAR_SIZE);
            //textItem.
            
            std::size_t endI = currI;
            /* Want to end line at end of previous word */
            if (currWidth >= CONTENT_WIDTH)
                endI = tempText.getString().toAnsiString().rfind(' ', currI - 1);
            
            if (endI > startI || tempText.getString()[currI] == '\n') {
                /* Space found on line or new line */
                textPair.second.setString(tempText.getString().substring(startI, endI - startI));
                startI = endI + 1;
            } else { /* Word does not fit on just one line */
                textPair.second.setString(tempText.getString().substring(startI, currI - startI - 1));
                startI = currI - 1;
            }
            textPair.first = isHighlighted;
            m_textDisplay.emplace_back();
            currRow = &m_textDisplay.back();
            
            currI = startI;
            widthOffset = tempText.findCharacterPos(startI).x;
            currWidth = 0;
        } else {
            currI++;
        }
    }
    if (startI != currI) { /* Add any remaining characters to final line */
        currRow->emplace_back(isHighlighted,
                sf::Text(tempText.getString().substring(startI, currI - startI),
                        FontManager::getInstance().getFont(), CHAR_SIZE));
    }
    setExpanded(m_expanded);
    // updateContents();
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
    
    float lineSpacing =
            FontManager::getInstance().getFont().getLineSpacing(CHAR_SIZE);
    /* Offset for each text line to keep in bounding box */
    float yOffset = (lineSpacing - CHAR_SIZE) / 2;
    if (m_textDisplay.size() > 1) {
        if (m_expanded) {
            int prevHeight = lineSpacing;
            for (std::list<std::list<std::pair<bool, sf::Text>>>::iterator it = m_textDisplay.begin();
                    it != m_textDisplay.end(); it++) {
                float xOffset = 0;
                for (std::list<std::pair<bool, sf::Text>>::iterator itt = it->begin();
                        itt != it->end(); itt++) {
                    if (itt->first) {
                        itt->second.setColor(m_backgroundColor + m_selectedColorOffset);
                    } else {
                        itt->second.setColor(m_textColor);
                    }
                    itt->second.setPosition(m_pos.x + xOffset, m_pos.y + prevHeight - yOffset);
                    xOffset += itt->second.findCharacterPos(-1).x - itt->second.findCharacterPos(0).x;
                    window.draw(itt->second);
                }
                prevHeight += lineSpacing;
            }
        }
        m_expandIcon.setPosition(m_pos.x + m_background.getSize().x / 2,
                m_pos.y + lineSpacing / 2);
        window.draw(m_expandIcon);
    } else {
        if (m_textDisplay.size() == 1) {
            float xOffset = 0;
            for (std::list<std::pair<bool, sf::Text>>::iterator it = m_textDisplay.front().begin();
                    it != m_textDisplay.front().end(); it++) {
                if (it->first) {
                    it->second.setColor(m_backgroundColor + m_selectedColorOffset);
                } else {
                    it->second.setColor(m_textColor);
                }
                it->second.setPosition(m_pos.x + xOffset, m_pos.y - yOffset);
                xOffset += it->second.findCharacterPos(-1).x - it->second.findCharacterPos(0).x;
                window.draw(it->second);
            }
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
    /*
    setContent("type", "This is a test, te AV let's keep `trying AH` this abcdefghijklmnopqrstuvwxyz123456789 it kinda works but may actually not");
    m_content["type"].second.setExpanded(true);
    
    //m_content.emplace("example", "Yet another little test for this");
    setContent("example", "Yet another little test for this");
    //m_content["example"].setExpanded(true);
    //setContent("example", "Temp");
    
    //setContent("one test", "");
    setContent("one test", "testing");
    
    setContent("listTest", "This is a list:\n\n -Hey\n -How's it going\n -Oh no, looks like we have a multiline piece here");
    setTitle("Testing Title");
    
    setPosition(1200 / 2, 1200 / 2);
    //setBackgroundColor(sf::Color::Yellow);
    */
    m_title.setFont(FontManager::getInstance().getFont());
    m_title.setCharacterSize(TITLE_CHAR_SIZE);
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

bool InfoPane::setContent(const std::string &label, const std::string &content, bool keepPosition)
{
    std::map<std::string, std::pair<sf::Text, InfoContent>>::iterator val =
            m_content.find(label);
    if (val == m_content.end()) {
        // Content hasn't been added before
        m_content.emplace(std::make_pair(label,
                std::pair<sf::Text, InfoContent>(std::piecewise_construct,
                std::forward_as_tuple(label,
                FontManager::getInstance().getFont(), CHAR_SIZE),
                std::forward_as_tuple(content))));
        m_contentOrder.push_back(label);
    } else {
        // Content has been added before, replace its value
        val->second.first.setString(label);
        val->second.second.setContent(content);
        if (!keepPosition) {
            // Remove the old ordering
            m_contentOrder.remove(label);
            m_contentOrder.push_back(label);
        }
    }
    updateContents();

    
    
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
    for (std::list<std::string>::iterator it = m_contentOrder.begin();
            it != m_contentOrder.end(); ++it) {
        std::pair<sf::Text, InfoContent> &val = m_content[*it];
        val.second.m_pos.y = m_pos.y + size.y;
        val.first.setPosition(m_pos.x + CONTENT_BUFFER, m_pos.y + size.y);
        size.y += val.second.getSize().y + CONTENT_BUFFER;
        maxLabelWidth = std::max(maxLabelWidth, val.first.getLocalBounds().width);
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
    
    for (std::list<std::string>::iterator it = m_contentOrder.begin();
            it != m_contentOrder.end(); ++it) {
        std::pair<sf::Text, InfoContent> &val = m_content[*it];
        bool expanded = val.second.m_expanded;
        val.second.update(event, windowSize, clickedOn);
        if (expanded != val.second.m_expanded)
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

