#include "NetworkTopology.h"

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
using namespace std::chrono;

#define INVALID_ID -1

#define NODE_LAYER_ID 2
#define LINK_LAYER_ID 1
#define INFO_LAYER_ID 0

NetworkTopology::Node NetworkTopology::Node::null_node(INVALID_ID, 0);

NetworkTopology::Link::Link(Node &end1, Node &end2) :
        m_ends({end1, end2}),
        m_isBidirectional(false),
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
    m_color = sf::Color::Blue;
    PLOGD << "Making link bezier " << end1.m_pos << " - " <<
            (end1.m_pos + end2.m_pos) / 2.0f << " - " << end2.m_pos;
    m_ends[0].get().links.push_back(*this);
    for (std::vector<sf::CircleShape>::iterator it = endArrows.begin();
            it != endArrows.end(); it++) {
        it->setPointCount(3);
    }
    m_renderable = true;
}


void NetworkTopology::Link::update(sf::Event *event,
        const sf::Vector2f &windowSize, bool &clickedOn) {
    bool clickedOnBefore = clickedOn;
    shape.update(event, windowSize, clickedOn);
    setSelected(shape.isSelected());
    if (!clickedOnBefore && clickedOn) { // Shape was clicked on
        // Check if double click, if so, show info pane
        if (duration_cast<milliseconds>(system_clock::now() -
                m_lastClickTime).count() < DOUBLE_CLICK_DURATION.count()) {
            m_info.setPosition(unitToPixel(windowSize,
                    shape.GetPointOnCurve(0.5)));
            m_info.setRenderable(true);
        }
        m_lastClickTime = system_clock::now();
    }
}

void NetworkTopology::Link::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    sf::Color fillColor = m_color;
    if (m_selected)
        fillColor += m_selectedColorOffset;
    
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
        endPos += offset;
        
        // This is for bidirectional links
        //if (m_isBidirectional || i == 1) {
        // This is for unidirection only links
        if (i == 1) {
            endArrows[i].setRadius(r);
            endArrows[i].setFillColor(fillColor);
            endArrows[i].setOrigin(r, r);
            
            endArrows[i].setPosition(endPos);
            endArrows[i].setRotation(rot + 90);
            
            window.draw(endArrows[i]);
        }
        endsPos.push_back(endPos);
    }
    
    shape.setColor(fillColor);
    // It has changed endpoints since last checked
    if (!floatEquals(unitToPixel(windowSize, shape.getStart()), endsPos[0]) ||
            !floatEquals(unitToPixel(windowSize, shape.getEnd()), endsPos[1])) {
        shape.setStart(pixelToUnit(windowSize, endsPos[0]));
        shape.setEnd(pixelToUnit(windowSize, endsPos[1]));
    }
    shape.render(window, windowSize);
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
    shape.setSize(sf::Vector2f(width, width));
    shape.setOrigin(width / 2, width / 2);
    m_color = sf::Color(0, 0, 200);
    shape.setOutlineColor(sf::Color::Black);
    m_label.setFont(FontManager::getInstance().getFont());
    setText(m_label, "Node " + std::to_string(id));
    m_renderable = true;
    m_moving = false;
    m_lastClickTime = system_clock::now();
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
                                    (m_selected ? "YES" : "NO");
                            setSelected(true);
                            m_moving = true;
                            m_lastDragPos.x = event->mouseButton.x;
                            m_lastDragPos.y = event->mouseButton.y;
                            // Check if double click, if so, show info pane
                            if (duration_cast<milliseconds>(
                                    system_clock::now() -
                                    m_lastClickTime).count() <
                                    DOUBLE_CLICK_DURATION.count()) {
                                m_info.setPosition(unitToPixel(windowSize, m_pos));
                                m_info.setRenderable(true);
                            }
                            m_lastClickTime = system_clock::now();
                        } else if (event->mouseButton.button == sf::Mouse::Right) {
                            PLOGD << "Right click on Node " << *this;
                            PLOGD << "Already activated: " <<
                                    (m_selected ? "YES" : "NO");
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
                // If mouse is released, will always stop moving
                m_moving = false;
                break;
            case sf::Event::MouseMoved:
                if (m_moving) {
                    sf::Vector2f delta(pixelToUnit(windowSize,
                            sf::Vector2f(event->mouseMove.x, event->mouseMove.y)) -
                            pixelToUnit(windowSize, m_lastDragPos));
                    m_pos += delta;
                    m_lastDragPos.x = event->mouseMove.x;
                    m_lastDragPos.y = event->mouseMove.y;
                }
        }
    }
}

void NetworkTopology::Node::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    if (id == INVALID_ID) {
        return;
    }
    sf::Color fillColor = m_color;
    if (m_selected)
        fillColor += m_selectedColorOffset;
    
    shape.setFillColor(fillColor);
    
    shape.setPosition(unitToPixel(windowSize, m_pos));
    m_label.setPosition(unitToPixel(windowSize, m_pos));
    window.draw(shape);
    window.draw(m_label);
}

bool NetworkTopology::Node::contains(float x, float y)
{
    return shape.getGlobalBounds().contains(x, y);
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
        m_selectedNode(nullptr)
{
    /* Set up the render/update layers */
    addLayer(NODE_LAYER_ID);
    addLayer(LINK_LAYER_ID);
    addLayer(INFO_LAYER_ID);
    /* Need to add all nodes dynamically as they are around for lifetime of this */
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        addNode(*(new Node(it->first, nodeWidth)));
    }
    
    
    /* Calculate constants for each node layout */
    /* Seperation angle for an evenly spaced circle */
    float divisionAngle = 2 * M_PI / m_numNodes;
    /* Largest square creatable with the number of nodes */
    int largestSquare = int(std::sqrt(m_numNodes));
    /* How many nodes remain after making this square */
    int dist = m_numNodes - largestSquare * largestSquare;
    int i = 0;
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        Node *node = getNode(it->first);
        switch (m_layout) {
            case Circle:
                /* Position in an evenly spaced circle */
                node->m_pos = capInitial(windowSize, {nodeWidth, nodeWidth},
                        {std::cos(divisionAngle * i), -std::sin(divisionAngle * i)});
                break;
            case Grid:
                PLOGI << "Dist: " << dist;
                sf::Vector2f pos;
                if (dist == 0) { /* Num nodes makes a perfect square */
                    pos.x = i % largestSquare;
                    pos.y = i / largestSquare;
                    PLOGI << pos;
                    pos = pos / (0.5f * (largestSquare - 1)) - 1.0f;
                } else if (dist > largestSquare) {
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
                node->m_pos = capInitial(windowSize, {nodeWidth, nodeWidth}, pos);
        }
        node->m_info.setPosition(unitToPixel(windowSize, node->m_pos));
        i++;
    }
    
    /* Add all the links between nodes */
    PLOGV << "Node Links:";
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        Node *node = getNode(it->first);
        if (node == &Node::null_node) {
            /* How could this even happen? */
            PLOGW << " Node " << it->first << " not found";
            continue;
        }
        //addToLayer(NODE_LAYER_ID, *node);
        std::stringstream ss;
        ss << " Node " << *node << ":";
        std::string sep = "";
        for (std::set<int>::iterator iit = it->second.begin();
                iit != it->second.end(); iit++) {
            Node *linkedNode = getNode(*iit);
            if (linkedNode != &Node::null_node &&
                    node != linkedNode) { // Can't link to itself or an invalid node
                Link &link = *addLink(*node, *linkedNode);
                
                link.m_info.setPosition(unitToPixel(windowSize,
                        link.shape.GetPointOnCurve(0.5)));
                ss << sep << *iit;
                sep = ", ";
            }
        }
        PLOGV << ss.str();
    }
    
    m_renderable = true;
    
    PLOGD << "Number of renderables in topology: " << size();
}

//#include <stdio.h>
//#include <algorithm>
//#include <unistd.h>

NetworkTopology* NetworkTopology::createTopology(const sf::Vector2f &windowSize,
        const std::string &fileName, int nodeWidth)
{
    NetworkTopology *result;
    try {
        std::ifstream i(fileName);
        json j;
        i >> j;
        
        nodeLayout layout = Grid;
        if (j.contains("layout")) {
            layout = j["layout"];
        }
        PLOGD << "Topology Layout " << layout;
        
        if (!j.contains("nodes")) {
            PLOGE << "Contains no nodes";
            return nullptr;
        }
        json nodes = j["nodes"];
        std::map<int, std::set<int>> nodeLinks;
        for (json::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
            if (!it->contains("id")) {
                PLOGE << "Node does not contain required field \"id\"";
                return nullptr;
            }
            std::set<int> linkIDs;
            if (it->contains("links")) {
                json links = (*it)["links"];
                for (json::const_iterator itt = links.begin(); itt != links.end();
                        itt++) {
                    if (!itt->contains("otherNodeID")) {
                        PLOGE << "Link on Node " << (int)(*it)["id"] << " does not contain required field \"otherNodeID\"";
                        return nullptr;
                    }
                    if (!linkIDs.insert((int)(*itt)["otherNodeID"]).second) {
                        PLOGE << "Node " << (int)(*it)["id"] << " has duplicate link to Node " <<
                                (int)(*itt)["otherNodeID"];
                        return nullptr;
                    }
                }
            }
            if (!nodeLinks.insert(std::pair<int, std::set<int>>(
                    (*it)["id"], linkIDs)).second) {
                /* This was a duplicate node, bad file */
                PLOGE << "Contains duplicate Node " << (int)(*it)["id"];
                return nullptr;
            }
        }
        
        // Print out the connections
        for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
                it != nodeLinks.end(); it++) {
            std::stringstream ss;
            ss << "id: " << it->first << ", links: [";
            for (std::set<int>::iterator iit = it->second.begin();
                    iit != it->second.end(); iit++) {
                ss << *iit << ", ";
            }
            PLOGD << ss.str() << "]";
        }
        PLOGD << "SIZE: " << nodeLinks.size();
        
        result = new NetworkTopology(nodeLinks, nodeWidth,
                windowSize, layout);
    } catch (const std::exception &e) {
        PLOGF << e.what();
        return nullptr;
    }
    return result;
}

// WARNING, out of date, will corrupt state
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
    return nullptr;
}
*/

// This is for unidirectional links where n1-n2 != n2-n1 (but both can exist)
NetworkTopology::Link* NetworkTopology::getLink(Node& n1, Node& n2) {
    PLOGD << "Checking Link " << n1 << ", " << n2 << " exists";
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        Link& l = it->get();
        if (&l.m_ends[0].get() == &n1 && &l.m_ends[1].get() == &n2)
            return &l;
    }
    return nullptr;
}

NetworkTopology::Link* NetworkTopology::addLink(Node &n1, Node &n2) {
    Link *existingLink = getLink(n1, n2);
    if (existingLink == nullptr) {
        Link &l = *(new Link(n1, n2));
        m_links.push_back(l);
        addToLayer(LINK_LAYER_ID, l);
        addInfoPane(l.m_info);
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

NetworkTopology::Node* NetworkTopology::addNode(Node &n)
{
    if (m_nodes.insert(n).second == false)
        return nullptr;
    addToLayer(NODE_LAYER_ID, n);
    addInfoPane(n.m_info);
    return &n;
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

bool NetworkTopology::removeLink(int end1ID, int end2ID)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return false;
    
    // Remove the infopane associated with the list
    for (iterator it = begin(); it != end(); ++it) {
        if (*it == &(link->m_info)) {
            erase(it);
            break;
        }
    }
    
    for (iterator it = begin(); it != end(); ++it) {
        if (*it == link) {
            // Find the iterator to the link (can only delete by iterator, not value)
            std::list<std::reference_wrapper<Link>>::iterator li;
            for (li = m_links.begin(); li != m_links.end(); ++li) {
                if (&(li->get()) == link)
                    break;
            }
            // Remove references to link in every node
            for (std::vector<std::reference_wrapper<Node>>::iterator n = li->get().m_ends.begin();
                    n != li->get().m_ends.end(); ++n) {
                for (std::list<std::reference_wrapper<Link>>::iterator l = n->get().links.begin();
                        l != n->get().links.end(); ++l) {
                    if (&(l->get()) == &(li->get())) {
                        l = n->get().links.erase(l);
                        break;
                    }
                }
            }
            // Remove from network state and the renderable list
            m_links.erase(li);
            delete(link);
            erase(it);
            break;
        }
    }
}

void NetworkTopology::addInfoPane(InfoPane &info)
{
    addToLayer(INFO_LAYER_ID, info);
}

std::vector<int> NetworkTopology::getNodeIds()
{
    std::vector<int> result;
    for (std::set<std::reference_wrapper<Node>>::iterator n = m_nodes.begin();
            n != m_nodes.end(); ++n)
    {
        result.push_back(n->get().id);
    }
    return result;
}

bool NetworkTopology::getNodesIn(int nodeID, std::vector<int> &inList)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    inList.clear();
    for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
            it != m_nodes.end(); ++it) {
        Node &otherNode = it->get();
        if (otherNode.id == node->id) /* Nodes can't link to themselves */
            continue;
        for (std::list<std::reference_wrapper<Link>>::iterator itt = otherNode.links.begin();
            itt != otherNode.links.end(); ++itt) {
            if (itt->get().getOtherEnd(otherNode).id == node->id)
                inList.push_back(otherNode.id);
        }
    }
    return true;
}

bool NetworkTopology::getNodesOut(int nodeID, std::vector<int> &outList)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    outList.clear();
    for (std::list<std::reference_wrapper<Link>>::iterator it = node->links.begin();
            it != node->links.end(); ++it) {
        outList.push_back(it->get().getOtherEnd(*node).id);
    }
    return true;
}

bool NetworkTopology::setLinkSelected(int end1ID, int end2ID, bool state)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return false;
    link->setSelected(state);
    return true;
}

bool NetworkTopology::setNodeSelected(int nodeID, bool state)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    node->setSelected(state);
    return true;
}

bool NetworkTopology::setLinkColor(int end1ID, int end2ID, const sf::Color &col)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return false;
    link->setColor(col);
    return true;
}

const sf::Color NetworkTopology::getLinkColor(int end1ID, int end2ID)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return sf::Color::Transparent;
    return link->getColor();
}


bool NetworkTopology::setLinkInfoColor(int end1ID, int end2ID,
        const sf::Color &col)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return false;
    link->m_info.setBackgroundColor(col);
    return true;
}

bool NetworkTopology::setLinkInfoParameter(int end1ID, int end2ID,
        const std::string &label, const std::string &content, bool keepPosition)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return false;
    link->m_info.setContent(label, content, keepPosition);
    return true;
}

bool NetworkTopology::setLinkInfoTitle(int end1ID, int end2ID,
        const std::string &title)
{
    Link *link = getLink(end1ID, end2ID);
    if (link == nullptr)
        return false;
    link->m_info.setTitle(title);
    return true;
}

bool NetworkTopology::setNodeColor(int nodeID, const sf::Color &col)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    node->setColor(col);
    return true;
}

bool NetworkTopology::setNodeInfoColor(int nodeID, const sf::Color &col)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    node->m_info.setBackgroundColor(col);
    return true;
}

bool NetworkTopology::setNodeInfoParameter(int nodeID,
        const std::string &label, const std::string &content, bool keepPosition)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    node->m_info.setContent(label, content, keepPosition);
    return true;
}

bool NetworkTopology::setNodeInfoTitle(int nodeID, const std::string &title)
{
    Node *node = getNode(nodeID);
    if (node == nullptr)
        return false;
    node->m_info.setTitle(title);
    return true;
}

// Processes update logic of all objects in the network from top to bottom to keep click order
// (if two objects are on top of each other, the click will be processed by the object on top)
void NetworkTopology::update(sf::Event *event, const sf::Vector2f &windowSize,
        bool &clickedOn)
{
    for (iterator it = begin(); it != end(); ++it) {
        /* This will be true if another item has already processed a click
         * event (it was on top of this object) */
        bool clickedBefore = clickedOn;
        (*it)->update(event, windowSize, clickedOn);
        // If something is clicked, bring it to the front of its layer
        if (clickedOn && !clickedBefore) {
            iterator oldI = it;
            --it;
            if (!oldI.moveToFront())
                it = oldI;
        }
    }
}

// Render all objects in the network from bottom to top
void NetworkTopology::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    for (reverse_iterator it = rbegin(); it != rend(); ++it) {
        if ((*it)->isRenderable()) {
            (*it)->render(window, windowSize);
            
        }
    }
}
