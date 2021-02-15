#include "NetworkTopology.h"

#include <ctime>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <set>
#include <typeinfo>

#include <cmath>

#include "plog/Log.h"
#include <nlohmann/json.hpp>

#include "Utility.h"

using json = nlohmann::json;

#define INVALID_ID -1

#define NODE_LAYER_ID 1
#define LINK_LAYER_ID 0

NetworkTopology::Node NetworkTopology::Node::null_node(INVALID_ID, 0);




NetworkTopology::Link::Link(Node &end1, Node &end2) :
        m_ends({end1, end2}),
        m_isBidirectional(false),
        activated(false),
        endArrows(m_ends.size()),
        shape(40, {end1.shape.getPosition(),
                sf::Vector2f((end1.m_pos.x + end2.m_pos.x) / 2.0f + 
                        std::sin(std::atan2(end1.m_pos.y - end2.m_pos.y,
                                end1.m_pos.x - end2.m_pos.x)) / 6.0f,
                        (end1.m_pos.y + end2.m_pos.y) / 2.0f -
                        std::cos(std::atan2(end1.m_pos.y - end2.m_pos.y,
                                end1.m_pos.x - end2.m_pos.x)) / 6.0f),
                
                end2.shape.getPosition()})
{
    std::cout << "Making link bezier " << end1.m_pos << " - " <<
            (end1.m_pos + end2.m_pos) / 2.0f << " - " <<
            end2.m_pos << std::endl;
    m_ends[0].get().links.push_back(*this);
    for (std::vector<sf::CircleShape>::iterator it = endArrows.begin();
            it != endArrows.end(); it++) {
        it->setPointCount(3);
    }
    m_renderable = true;
}


void NetworkTopology::Link::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn) {
    shape.update(event, windowSize, clickedOn);
}

void NetworkTopology::Link::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    sf::Color fillColor;
    if (activated) {
        fillColor = sf::Color::Yellow;
    } else {
        fillColor = sf::Color(125, 0, 0);
    }
    //shape.SetColor(fillColor);
    
    std::vector<sf::Vector2f> endsPos;
    
    for (int i = 0; i < m_ends.size(); i++) {
        Node &curr = m_ends[i];
        sf::Vector2f endPos = unitToPixel(windowSize, curr.m_pos);
        
        sf::Vector2f dominantHandle;
        if (i == 0)
            dominantHandle = shape.getDominantHandleStart();
        else
            dominantHandle = shape.getDominantHandleEnd();
        float rot = (180 / M_PI) * std::atan2(curr.m_pos.y - dominantHandle.y,
                curr.m_pos.x - dominantHandle.x);
        
        float r = 1.8f * shape.getWidth();
        float rx = -r * std::cos(rot * M_PI / 180);
        float ry = -r * std::sin(rot * M_PI / 180);
        
        /* Get the offset of the endpoint of the link's "from" side from the centre
         * point of the node */
        sf::Vector2f offset;
        if ((rot <= -45 && rot >= -135) || (rot >= 45 && rot <= 135)) { // Above and Below
            if (rot < 0) // Above
                offset.y = curr.shape.getSize().y / 2 + ry;
            else // Below
                offset.y = -curr.shape.getSize().y / 2 + ry;
            offset.x = offset.y * std::tan(-(rot - 90) * M_PI / 180);
        } else { // Right and Left
            if (rot >= -45 && rot <= 45) // Right
                offset.x = -curr.shape.getSize().x / 2 + rx;
            else // Left
                offset.x = curr.shape.getSize().x / 2 + rx;
            offset.y = offset.x * std::tan(rot * M_PI / 180);
        }
        
        //float overlapLength = std::sqrt(std::pow(offset.x, 2) + std::pow(offset.y, 2));
        
        
        endPos += offset;
        
        // This is for bidirectional links
        //if (m_isBidirectional || i == 1) {
        // This is for unidirection only links
        if (i == 1) {
            if (m_isBidirectional) {
                PLOGI << "Bidirectional link";
                fillColor = sf::Color::Magenta;
            }
            
            
            endArrows[i].setRadius(r);
            endArrows[i].setFillColor(fillColor);
            endArrows[i].setOrigin(r, r);
            
            endArrows[i].setPosition(endPos);
            endArrows[i].setRotation(rot + 90);
            
            window.draw(endArrows[i]);
        }
        endsPos.push_back(endPos);
    }
    
    shape.SetColor(fillColor);
    /* It has changed endpoints since last checked */
    if (!floatEquals(unitToPixel(windowSize, shape.getStart()), endsPos[0]) ||
            !floatEquals(unitToPixel(windowSize, shape.getEnd()), endsPos[1])) {
        shape.setStart(pixelToUnit(windowSize, endsPos[0]));
        shape.setEnd(pixelToUnit(windowSize, endsPos[1]));
    }
    shape.render(window, windowSize);
    /*
    
    std::pair<sf::Vector2f, sf::Vector2f> endsPos(
            unitToPixel(windowSize, m_ends[0].get().m_pos),
            unitToPixel(windowSize, m_ends[1].get().m_pos));
    
    //float rot = (180 / M_PI) * std::atan2(end2Pos.y - end1Pos.y,
    //        end2Pos.x - end1Pos.x);
    //float rot = 
    
    float r = 1.8f * shape.getWidth();
    float rx = -r * std::cos(rot * M_PI / 180);
    float ry = -r * std::sin(rot * M_PI / 180);
    
    * Get the offset of the endpoint of the link's "from" side from the centre
     * point of the node *
    sf::Vector2f offset;
    if ((rot <= -45 && rot >= -135) || (rot >= 45 && rot <= 135)) { // Above and Below
        if (rot < 0) // Above
            offset.y = -m_end1.shape.getSize().y / 2 - ry;
        else // Below
            offset.y = m_end1.shape.getSize().y / 2 - ry;
        offset.x = offset.y * std::tan(-(rot - 90) * M_PI / 180);
    } else { // Right and Left
        if (rot >= -45 && rot <= 45) // Right
            offset.x = m_end1.shape.getSize().x / 2 - rx;
        else // Left
            offset.x = -m_end1.shape.getSize().x / 2 - rx;
        offset.y = offset.x * std::tan(rot * M_PI / 180);
    }
    
    
    * Set the length and add appropriate endpoint icons *
    //float length = std::sqrt(std::pow(end1Pos.x - end2Pos.x, 2) +
    //                std::pow(end1Pos.y - end2Pos.y, 2));
    float overlapLength = std::sqrt(std::pow(offset.x, 2) + std::pow(offset.y, 2));
    sf::Vector2f origin = end1Pos;
    end1Pos += offset;
    end2Pos -= offset;
    switch (m_isBidirectional) {
        case true:
        {
            //fillColor = sf::Color::Magenta;
            shape.SetColor(fillColor);
            //length -= overlapLength;
            * Draw from the edge of outbound node as it's also inbound *
            origin += offset;
            
            end1Arrow.setRadius(r);
            end1Arrow.setFillColor(fillColor);
            end1Arrow.setOrigin(r, r);
            
            end1Arrow.setPosition(end1Pos);
            end1Arrow.setRotation(rot - 90);
            window.draw(end1Arrow);
        }
            * FALL-THROUGH *
        case false:
            //length -= overlapLength;
            
            end2Arrow.setRadius(r);
            end2Arrow.setFillColor(fillColor);
            end2Arrow.setOrigin(r, r);
            
            end2Arrow.setPosition(end2Pos);
            end2Arrow.setRotation(rot + 90);
            window.draw(end2Arrow);
    }
    
    if (!floatEquals(unitToPixel(windowSize, shape.getStart()), end1Pos) ||
            !floatEquals(unitToPixel(windowSize, shape.getEnd()), end2Pos)) {
        shape.setStart(pixelToUnit(windowSize, end1Pos));
        shape.setEnd(pixelToUnit(windowSize, end2Pos));
    }
    shape.render(window, windowSize);
    */
    //shape.setSize(sf::Vector2f(length, 4));
    //shape.setPosition(origin);
    
    
    //shape.setRotation(rot);
    //shape.setOrigin(0, shape.getSize().y / 2.0f);
    
    //window.draw(arrowHead);
    //window.draw(shape);
}

bool NetworkTopology::Link::contains(float x, float y)
{
    return shape.contains(x, y);
}

NetworkTopology::Node::Node(int id, int width) {
    init(id, width);
}

void NetworkTopology::Node::init(int id, int width) {
    this->id = id;
    activated = false;
    shape.setSize(sf::Vector2f(width, width));
    shape.setOrigin(width / 2, width / 2);
    shape.setFillColor(sf::Color(0, 0, 200));
    shape.setOutlineColor(sf::Color::Black);
    m_label.setFont(FontManager::getInstance().getFont());
    setText(m_label, "Node " + std::to_string(id));
    m_renderable = true;
    m_moving = false;
}

void NetworkTopology::Node::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn)
{
    if (clickedOn) {
        
    }
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                bool toDeselect = true;
                if (!clickedOn) {
                    if (contains(event->mouseButton.x, event->mouseButton.y)) {
                        clickedOn = true;
                        toDeselect = false;
                        if (event->mouseButton.button == sf::Mouse::Left) {
                            PLOGD << "Right click on Node " << *this;
                            PLOGD << "Already activated: " <<
                                    (activated ? "YES" : "NO");
                            setActive(true);
                            m_moving = true;
                        } else if (event->mouseButton.button == sf::Mouse::Right) {
                            PLOGD << "Right click on Node " << *this;
                            PLOGD << "Already activated: " <<
                                    (activated ? "YES" : "NO");
                            setActive(false);
                        }
                    }
                }
                if (toDeselect && 
                        !(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
                    setActive(false);
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                if (m_moving) {
                    PLOGD << "Mouse moved x: " << event->mouseMove.x <<
                            ", y: " << event->mouseMove.y;
                    m_pos = pixelToUnit(windowSize,
                            {event->mouseMove.x, event->mouseMove.y});
                }
        }
    }
}

void NetworkTopology::Node::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    if (id == INVALID_ID) {
        return;
    }
    if (activated)
        shape.setFillColor(sf::Color(0, 200, 200));
    else
        shape.setFillColor(sf::Color(0, 0, 200));
    
    shape.setPosition(unitToPixel(windowSize, m_pos));
    m_label.setPosition(unitToPixel(windowSize, m_pos));
    window.draw(shape);
    window.draw(m_label);
}

bool NetworkTopology::Node::contains(float x, float y)
{
    return shape.getGlobalBounds().contains(x, y);
}

void NetworkTopology::Node::setActive(bool state)
{
    activated = state;
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = links.begin();
            it != links.end(); it++) {
        it->get().activated = state;
    }
}

//NetworkTopology::NetworkTopology()

//Default constructor
NetworkTopology::NetworkTopology(int numNodes, int nodeWidth,
        const sf::Vector2f &windowSize, nodeLayout layout)
    :
        m_numNodes(numNodes),
        m_nodes(),
        m_nodeWidth(nodeWidth),
        m_selectedNode(NULL)
{
    /* Need to add all nodes dynamically as they are around for lifetime of this */
    for (int i = 0; i < numNodes; i++) {
        m_nodes.insert(*(new Node(i + 20, nodeWidth)));
    }
    
    /* Initial layout for nodes will be an evenly spaced circle */
    float divisionAngle = 2 * M_PI / numNodes;
    for (int i = 20; i < numNodes + 20; i++) {
        Node *node = getNode(i);
        if (node == &Node::null_node) {
            std::cout << "Node " << i << " not found" << std::endl;
            continue;
        }
        std::cout << "NodeNum: " << *node;//get(i);
        /* */
        int numLinks = rand() % numNodes / 3;
        //std::set<int> linkIndices;
        for (int j = 0; j < numLinks; j++) {
            int linkId = rand() % m_numNodes + 20;
            Node *linkedNode = getNode(linkId);
            if (linkedNode != &Node::null_node &&
                    node != linkedNode) { // Can't link to itself or an invalid node
                //linkIndices.insert(linkIndex);
                //std::cout << ", " << linkIndex;
                addLink(*node, *linkedNode);
                std::cout << ", " << linkId;
            }
        }
        std::cout << std::endl;
        
        std::cout << "Div: " << divisionAngle * i << std::endl;
        node->m_pos = capInitial(windowSize, {nodeWidth, nodeWidth},
                {std::cos(divisionAngle * i), -std::sin(divisionAngle * i)});
        
        if (i == 0) {
            std::cout << "width: " << nodeWidth << std::endl;
            std::cout << "windowSize.x: " << windowSize.x << ", windowSize.y: " << windowSize.y << std::endl;
            std::cout << "cos: " << std::cos(divisionAngle * i) << ", sin: " << -std::sin(divisionAngle * i) << std::endl;
            std::cout << "pos: " << node->m_pos << std::endl;
        }
        
        
    }
    
    m_renderable = true;
    
    
    std::cout << std::endl;
    
    
    //std::unique_ptr<int[]> array(new int[size]);
    
    
}

NetworkTopology::~NetworkTopology()
{
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        delete &(*it);
    }
}

NetworkTopology::NetworkTopology(std::map<int, std::set<int>> nodeLinks,
        int nodeWidth, const sf::Vector2f &windowSize, nodeLayout layout)
    :
        m_numNodes(nodeLinks.size()),
        m_layout(layout),
        m_nodes(),
        m_nodeWidth(nodeWidth),
        m_selectedNode(NULL)
{
    /* Set up the render/update layers */
    addLayer(NODE_LAYER_ID);
    addLayer(LINK_LAYER_ID);
    std::cout <<"GOOOOOD INIT" << std::endl;
    /* Need to add all nodes dynamically as they are around for lifetime of this */
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        m_nodes.insert(*(new Node(it->first, nodeWidth)));
    }
    
    
    /* Initial layout for nodes will be an evenly spaced circle */
    float divisionAngle = 2 * M_PI / m_numNodes;
    
    int largestSquare = int(std::sqrt(m_numNodes));
    PLOGI << "square " << largestSquare;
    int dist = m_numNodes - largestSquare * largestSquare;
    int i = 0;
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        Node *node = getNode(it->first);
        switch (m_layout) {
            case Circle:
                /* Position in an evenly spaced circle */
                std::cout << "Div: " << divisionAngle * i << std::endl;
                node->m_pos = capInitial(windowSize, {nodeWidth, nodeWidth},
                        {std::cos(divisionAngle * i), -std::sin(divisionAngle * i)});
                break;
            case Grid:
                PLOGI << "Dist: " << dist;
                sf::Vector2f pos;
                if (dist == 0) { /* Num nodes makes a perfect square */
                    pos.x = i % largestSquare;
                    pos.y = i / largestSquare;
                    std::stringstream ss;
                    ss << pos;
                    PLOGI << ss.str();
                    pos = pos / (0.5f * (largestSquare - 1)) - 1.0f;
                } else if (dist > largestSquare + 1) {
                    /* Num nodes fill up all rows but the bottom */
                    pos.x = i % (largestSquare + 1);
                    pos.y = i / (largestSquare + 1);
                    pos = pos / (0.5f * largestSquare) - 1.0f;
                } else {
                    if (i / (largestSquare + 1.0f) > dist) {
                        pos.x = (i - dist) % largestSquare;
                        pos.y = (i - dist) / largestSquare;
                    } else {
                        pos.x = i % (largestSquare + 1);
                        pos.y = i / (largestSquare + 1);
                    }
                    pos.x = pos.x / (0.5f * largestSquare) - 1.0f;
                    pos.y = pos.y / (0.5f * (largestSquare - 1)) - 1.0f;
                }
                node->m_pos = capInitial(windowSize, {nodeWidth, nodeWidth},
                            pos);
        }
        
        if (i == 0) {
            std::cout << "width: " << nodeWidth << std::endl;
            std::cout << "windowSize.x: " << windowSize.x << ", windowSize.y: " << windowSize.y << std::endl;
            std::cout << "cos: " << std::cos(divisionAngle * i) << ", sin: " << -std::sin(divisionAngle * i) << std::endl;
            std::cout << "pos: " << node->m_pos << std::endl;
        }
        i++;
    }
    
    /* Add all the links between nodes */
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        Node *node = getNode(it->first);
        if (node == &Node::null_node) {
            /* How could this even happen? */
            PLOGW << "Node " << it->first << " not found";
            continue;
        }
        addToLayer(NODE_LAYER_ID, *node);
        std::stringstream ss;
        ss << "NodeNum: " << *node;
        //std::set<int> linkIndices;
        for (std::set<int>::iterator iit = it->second.begin();
                iit != it->second.end(); iit++) {
            Node *linkedNode = getNode(*iit);
            if (linkedNode != &Node::null_node &&
                    node != linkedNode) { // Can't link to itself or an invalid node
                //linkIndices.insert(linkIndex);
                //std::cout << ", " << linkIndex;
                Link &link = *addLink(*node, *linkedNode);
                ss << ", " << *iit;
                if (linkedNode == &link.m_ends[1].get()) {
                    addToLayer(LINK_LAYER_ID, link);
                }
            }
        }
        PLOGD << ss.str();
    }
    
    
    m_renderable = true;
    
    PLOGD << "Number of renderables in topology: " << size();
    
    std::cout << std::endl;
    
    
    //std::unique_ptr<int[]> array(new int[size]);
    
}

//#include <stdio.h>
#include <algorithm>
#include <unistd.h>

NetworkTopology* NetworkTopology::createTopology(int numNodes,
        int nodeWidth, const sf::Vector2f &windowSize)
{
    std::ifstream i("../gridNetwork.json");
    json j;
    i >> j;
    
    nodeLayout layout = j["layout"];
    PLOGI << "Layout " << layout;
    
    json nodes = j["nodes"];
    std::map<int, std::set<int>> nodeLinks;
    for (json::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
        if (nodeLinks.insert(std::pair<int, std::set<int>>(
                (*it)["id"],
                (*it)["links"])).second == false) {
            // This was a duplicate node, bad file
            return NULL;
        }
        //(*it)["id"] << std::endl;
    }
    
    
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        std::stringstream ss;
        ss << "id: " << it->first << ", links: [";
        for (std::set<int>::iterator iit = it->second.begin();
                iit != it->second.end(); iit++) {
            ss << *iit << ", ";
        }
        PLOGD << ss.str() << "]";
        //(*it)["id"] << std::endl;
    }
    
    PLOGD << "SIZE: " << nodeLinks.size();
    
    NetworkTopology *test = new NetworkTopology(nodeLinks, nodeWidth,
            windowSize, layout);
    //NetworkTopology *test = new NetworkTopology(numNodes, nodeWidth, windowSize);
    for (std::list<std::reference_wrapper<Link>>::iterator it = test->m_links.begin();
            it != test->m_links.end(); it++) {
        PLOGD << "Link: " << it->get();
    }
    return test;
}

void NetworkTopology::save(const std::string &fileName) {
    json toSave;
    for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
            it != m_nodes.end(); it++) {
        Node &node = it->get();
        json nodeObj = json::object();
        nodeObj["id"] = node.id;
        
        json nodeLinks = json::array();
        for (std::list<std::reference_wrapper<Link>>::iterator iit = node.links.begin();
                iit != node.links.end(); iit++) {
            const Node &otherNode = iit->get().getOtherEnd(node);
            if (&otherNode != &Node::null_node) {
                nodeLinks.push_back(otherNode.id);
            }
        }
        nodeObj["links"] = nodeLinks;
        
        toSave.push_back(nodeObj);
    }
    std::ofstream out(fileName);
    out << toSave.dump(4);
    out.close();
}

/* This is for bidirection links where link n1-n2 == n2-n1
NetworkTopology::Link* NetworkTopology::getLink(Node& n1, Node& n2) {
    std::cout << std::endl << "Checking " << n1 << ", " << n2 << " exists ";
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        Link& l = it->get();
        if ((&l.m_ends[0].get() == &n1 && &l.m_ends[1].get() == &n2) ||
                (&l.m_ends[0].get() == &n2 && &l.m_ends[1].get() == &n1)) {
            return &l;
        }
    }
    return NULL;
}
*/
// This is for unidirectional links where n1-n2 != n2-n1 (but both can exist)
NetworkTopology::Link* NetworkTopology::getLink(Node& n1, Node& n2) {
    std::cout << std::endl << "Checking " << n1 << ", " << n2 << " exists ";
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        Link& l = it->get();
        if (&l.m_ends[0].get() == &n1 && &l.m_ends[1].get() == &n2)
            return &l;
    }
    return NULL;
}

NetworkTopology::Link* NetworkTopology::addLink(Node &n1, Node &n2) {
    Link *existingLink = getLink(n1, n2);
    if (existingLink == NULL) {
        Link &l = *(new Link(n1, n2));
        m_links.push_back(l);
        PLOGD << "Added Link: " << l;
        return &l;
    } else {
        PLOGD << "Attempted to add the same link";
        return nullptr;
        /* Bi directional links
        PLOGD << "Link is Bidirectional: " << *existingLink;
        existingLink->m_isBidirectional = true;
        n1.links.push_back(*existingLink);
        return existingLink;
        */
    }
}

void NetworkTopology::removeNode(Node &node) {
    // Need to remove links with this node on either end. Do this without
    // iterating list twice for each element
    PLOGI <<"Removing node " << node.id;
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        Link &link = it->get();
        Node &otherEnd = link.getOtherEnd(node);
        if (&otherEnd == &Node::null_node) {
            PLOGD << "No good " << link;
            continue;
        }
        Node &toDelete = Node::null_node;
        
        if (&link.m_ends[0].get() == &node) {
            PLOGD << "Removing Link to " << otherEnd.id;
            node.links.remove(link);
            toDelete = otherEnd;
        } else {
            PLOGD << "Removing Link from " << otherEnd.id;
            otherEnd.links.remove(link);
            toDelete = node;
        }
        if (link.m_isBidirectional) {
            PLOGD << "is bidirectional";
            toDelete.links.remove(link);
        } else
            PLOGD << "isn't bidirectional";
        it = m_links.erase(it);
        it--;
        delete &(link);
    }
    m_nodes.erase(node);
    delete &node;
}

bool NetworkTopology::setNodeActive(int nodeID, bool state)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    node->setActive(state);
}

bool NetworkTopology::setLinkActive(int nodeID1, int nodeID2, bool state)
{
    Node *node1 = getNode(nodeID1);
    Node *node2 = getNode(nodeID2);
    if (node1 == nullptr || node2 == nullptr)
        return false;
    Link *link = getLink(*node1, *node2);
    if (link == nullptr)
        return false;
    link->setActive(state);
    return true;
}

void NetworkTopology::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    //bool mousePressEvent = false, alreadyClickedAnObject = false;
    //if (event != nullptr) {
    //    if (event->type == sf::Event::MouseButtonPressed)
    //        mousePressEvent = true;
    //}
    for (iterator it = begin(); it != end(); ++it) {
        //bool clickedThisObject = false;
        /*if (mousePressEvent && !alreadyClickedAnObject) {
            if ((*it)->contains(event->mouseButton.x, event->mouseButton.y)) {
                PLOGD << "Clicked on something";
                alreadyClickedAnObject = true;
                clickedThisObject = true;
            }
        }*/
        /* This will be true if another item has already processed a click
         * event (it was on top of this object) */
        //bool anotherItemClicked = clickedOn;
        (*it)->update(event, windowSize, clickedOn);
        //if (!anotherItemClicked && clickedOn) { /* This item was clicked */
         /*   if (typeid(**it) == typeid(Node)) {
                PLOGD << "It was a node";
                Node *tmp = (Node*) *it;
                if (tmp->activated)
                    activateNode(*tmp);
                else
                    deactivateNode(*tmp);
            }
        }*/
    }
    /*if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                PLOGD << "Mouse Press: x " << event->mouseButton.x << ", y " <<
                        event->mouseButton.y;
                Renderable *clicked = nullptr;
                for (iterator it = begin(); it != end(); ++it) {
                    if (it->contains(event->mouseButton.x,
                            event->mouseButton.y))
                        clicked = *it;
                }
                
                if (clicked != nullptr) {
                    PLOGD << "Something was clicked";
                    if (typeid(*clicked) == typeid(Node)) {
                        m_selectedNode = clicked;
                        if (event->mouseButton.button == sf::Mouse::Left) {
                            std::cout << "Left click on Node " << *clicked << std::endl;
                            std::cout << "Already activated: " <<
                                    (nodeClicked->activated ? "YES" : "NO") << std::endl;
                            
                            activateNode(*nodeClicked);
                        }
                        else if (event->mouseButton.button == sf::Mouse::Right) {
                            std::cout << "Right click on Node " << *clicked << std::endl;
                            std::cout << "Already activated: " <<
                                    (nodeClicked->activated ? "YES" : "NO") << std::endl;
                            
                            deactivateNode(*nodeClicked);
                        }
                    }
                }
                
                /*
                
                
                
                
                
                std::cout << "Mouse Press: x " << event->mouseButton.x << ", y " <<
                        event->mouseButton.y << std::endl;
                /* Check if a node was selected *
                Node *nodeClicked = NULL;
                for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
                        it != m_nodes.end(); it++) {
                    if (it->get().shape.getGlobalBounds().contains(event->mouseButton.x,
                            event->mouseButton.y)) {
                        nodeClicked = &it->get();
                        break;
                    }
                }
                if (nodeClicked != NULL) {
                    /* Node was selected *
                    m_selectedNode = nodeClicked;
                    if (event->mouseButton.button == sf::Mouse::Left) {
                        std::cout << "Left click on Node " << *nodeClicked << std::endl;
                        std::cout << "Already activated: " <<
                                (nodeClicked->activated ? "YES" : "NO") << std::endl;
                        
                        activateNode(*nodeClicked);
                    }
                    else if (event->mouseButton.button == sf::Mouse::Right) {
                        std::cout << "Right click on Node " << *nodeClicked << std::endl;
                        std::cout << "Already activated: " <<
                                (nodeClicked->activated ? "YES" : "NO") << std::endl;
                        
                        deactivateNode(*nodeClicked);
                    }
                }*
                break;
            }
            case sf::Event::MouseButtonReleased:
                m_selectedNode = NULL;
                break;
            case sf::Event::MouseMoved:
                if (m_selectedNode != NULL) {
                    std::cout << "Mouse moved x: " << event->mouseMove.x << ", y: " << event->mouseMove.y << std::endl;
                    
                    m_selectedNode->m_pos = pixelToUnit(windowSize,
                            {event->mouseMove.x, event->mouseMove.y});
                }
                break;
            case sf::Event::KeyPressed:
                if (event->key.code == sf::Keyboard::Delete) {
                    if (m_selectedNode != NULL) {
                        removeNode(*m_selectedNode);
                        m_selectedNode = NULL;
                    }
                }
                
            default:
                break;
        }
    }
    for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
            it != m_nodes.end(); it++) {
        it->get().update(event, windowSize);
    }
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        it->get().update(event, windowSize);
    }*/
}


void NetworkTopology::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    //Layer &l = m_layers[1];
    for (reverse_iterator it = rbegin(); it != rend(); ++it) {
        if ((*it)->isRenderable()) {
            (*it)->render(window, windowSize);
            
        }
    }
    /*
    for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
            it != m_nodes.end(); it++) {
        if (it->get().isRenderable())
            it->get().render(window, windowSize);
    }
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        if (it->get().isRenderable())
            it->get().render(window, windowSize);
    }*/
}

void NetworkTopology::print()
{
    std::cout << "Links:" << std::endl;
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        std::cout << " " << *it << std::endl;
    }
    
    
    for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
            it != m_nodes.end(); it++) {
        Node &node = it->get();
        std::cout << "ID " << node;
        //std::cout << ", " << node.links.back().get();
        for (std::list<std::reference_wrapper<Link>>::iterator it = node.links.begin();
                it != node.links.end(); it++) {
            std::cout << ", " << it->get().getOtherEnd(node);
        }
        std::cout << std::endl;
    }
}
