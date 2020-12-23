#include "NetworkTopology.h"

#include <ctime>
#include <stdlib.h>
#include <set>
#include <fstream>

#include <cmath>

#include <nlohmann/json.hpp>

#include "Utility.h"

using json = nlohmann::json;

#define INVALID_ID -1

NetworkTopology::Node NetworkTopology::Node::null_node(INVALID_ID, 0);




NetworkTopology::Link::Link(Node &end1, Node &end2) :
        m_end1(end1),
        m_end2(end2),
        shape(40, {end1.shape.getPosition(),
                {(end1.x + end2.x) / 2.0f, (end1.y + end2.y) / 2.0f},
                end2.shape.getPosition()})
{
    std::cout << "Making link bezier " << sf::Vector2f(end1.x, end1.y) << " - " <<
            sf::Vector2f((end1.x + end2.x) / 2.0f, (end1.y + end2.y) / 2.0f) << " - " <<
            sf::Vector2f(end2.x, end2.y) << std::endl;
    m_end1.links.push_back(*this);
    end1Arrow.setPointCount(3);
    end2Arrow.setPointCount(3);
    m_renderable = true;
}


void NetworkTopology::Link::update(sf::Event *event,
        const sf::Vector2f &windowSize) {
    shape.update(event, windowSize);
}

void NetworkTopology::Link::render(sf::RenderWindow &window,
        const sf::Vector2f &windowSize) {
    sf::Color fillColor;
    if (activated) {
        fillColor = sf::Color::Yellow;
    } else {
        fillColor = sf::Color(125, 0, 0);
    }
    shape.SetColor(fillColor);
    
    sf::Vector2f end1Pos(unitToPixel(windowSize.x, m_end1.x),
            unitToPixel(windowSize.y, m_end1.y));
    sf::Vector2f end2Pos(unitToPixel(windowSize.x, m_end2.x),
            unitToPixel(windowSize.y, m_end2.y));
    
    float rot = (180 / M_PI) * std::atan2(end2Pos.y - end1Pos.y,
            end2Pos.x - end1Pos.x);
    
    float r = 1.8f * shape.getWidth();
    float rx = -r * std::cos(rot * M_PI / 180);
    float ry = -r * std::sin(rot * M_PI / 180);
    
    /* Get the offset of the endpoint of the link's "from" side from the centre
     * point of the node */
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
    
    
    /* Set the length and add appropriate endpoint icons */
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
            /* Draw from the edge of outbound node as it's also inbound */
            origin += offset;
            
            end1Arrow.setRadius(r);
            end1Arrow.setFillColor(fillColor);
            end1Arrow.setOrigin(r, r);
            
            end1Arrow.setPosition(end1Pos);
            end1Arrow.setRotation(rot - 90);
            window.draw(end1Arrow);
        }
            /* FALL-THROUGH */
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
    
    //shape.setSize(sf::Vector2f(length, 4));
    //shape.setPosition(origin);
    
    
    //shape.setRotation(rot);
    //shape.setOrigin(0, shape.getSize().y / 2.0f);
    
    //window.draw(arrowHead);
    //window.draw(shape);
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
    m_renderable = true;
}

void NetworkTopology::Node::update(sf::Event *event,
        const sf::Vector2f &windowSize) {
    
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
    
    shape.setPosition(unitToPixel(windowSize.x, x),
            unitToPixel(windowSize.y, y));
    window.draw(shape);
}

//NetworkTopology::NetworkTopology()

//Default constructor
NetworkTopology::NetworkTopology(int numNodes, int nodeWidth,
        const sf::Vector2f &windowSize)
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
        node->x = capInitial(windowSize.x, nodeWidth, std::cos(divisionAngle * i));
        node->y = capInitial(windowSize.y, nodeWidth, -std::sin(divisionAngle * i));
        
        if (i == 0) {
            std::cout << "width: " << nodeWidth << std::endl;
            std::cout << "windowSize.x: " << windowSize.x << ", windowSize.y: " << windowSize.y << std::endl;
            std::cout << "cos: " << std::cos(divisionAngle * i) << ", sin: " << -std::sin(divisionAngle * i) << std::endl;
            std::cout << "x: " << node->x << ", y: " << node->y << std::endl;
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
        int nodeWidth, const sf::Vector2f &windowSize)
    :
        m_numNodes(nodeLinks.size()),
        m_nodes(),
        m_nodeWidth(nodeWidth),
        m_selectedNode(NULL)
{
    std::cout <<"GOOOOOD INIT" << std::endl;
    /* Need to add all nodes dynamically as they are around for lifetime of this */
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        m_nodes.insert(*(new Node(it->first, nodeWidth)));
    }
    
    /* Initial layout for nodes will be an evenly spaced circle */
    float divisionAngle = 2 * M_PI / m_numNodes;
    int pos = 0;
    for (std::map<int, std::set<int>>::iterator it = nodeLinks.begin();
            it != nodeLinks.end(); it++) {
        Node *node = getNode(it->first);
        if (node == &Node::null_node) {
            /* How could this even happen? */
            std::cout << "Node " << it->first << " not found" << std::endl;
            continue;
        }
        std::cout << "NodeNum: " << *node;
        //std::set<int> linkIndices;
        for (std::set<int>::iterator iit = it->second.begin();
                iit != it->second.end(); iit++) {
            Node *linkedNode = getNode(*iit);
            if (linkedNode != &Node::null_node &&
                    node != linkedNode) { // Can't link to itself or an invalid node
                //linkIndices.insert(linkIndex);
                //std::cout << ", " << linkIndex;
                addLink(*node, *linkedNode);
                std::cout << ", " << *iit;
            }
        }
        std::cout << std::endl;
        
        /* Position in an evenly spaced circle */
        std::cout << "Div: " << divisionAngle * pos << std::endl;
        node->x = capInitial(windowSize.x, nodeWidth, std::cos(divisionAngle * pos));
        node->y = capInitial(windowSize.y, nodeWidth, -std::sin(divisionAngle * pos));
        
        if (pos == 0) {
            std::cout << "width: " << nodeWidth << std::endl;
            std::cout << "windowSize.x: " << windowSize.x << ", windowSize.y: " << windowSize.y << std::endl;
            std::cout << "cos: " << std::cos(divisionAngle * pos) << ", sin: " << -std::sin(divisionAngle * pos) << std::endl;
            std::cout << "x: " << node->x << ", y: " << node->y << std::endl;
        }
        pos++;
        
        
    }
    
    m_renderable = true;
    
    
    std::cout << std::endl;
    
    
    //std::unique_ptr<int[]> array(new int[size]);
    
}

//#include <stdio.h>
#include <algorithm>
#include <unistd.h>

NetworkTopology* NetworkTopology::createTopology(int numNodes,
        int nodeWidth, const sf::Vector2f &windowSize)
{
    std::ifstream i("../test1");
    json j;
    i >> j;
    std::map<int, std::set<int>> nodeLinks;
    for (json::const_iterator it = j.begin(); it != j.end(); it++) {
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
        std::cout << "id: " << it->first << ", links: [";
        for (std::set<int>::iterator iit = it->second.begin();
                iit != it->second.end(); iit++) {
            std::cout << *iit << ", ";
        }
        std::cout << "]" << std::endl;
        //(*it)["id"] << std::endl;
    }
    
    std::cout << "SIZE: " << nodeLinks.size() << std::endl;
    
    NetworkTopology *test = new NetworkTopology(nodeLinks, nodeWidth, windowSize);
    //NetworkTopology *test = new NetworkTopology(numNodes, nodeWidth, windowSize);
    for (std::list<std::reference_wrapper<Link>>::iterator it = test->m_links.begin();
            it != test->m_links.end(); it++) {
        std::cout << "Link: " << it->get() << std::endl;
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

NetworkTopology::Link* NetworkTopology::getLink(Node& n1, Node& n2) {
    std::cout << std::endl << "Checking " << n1 << ", " << n2 << " exists ";
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        Link& l = it->get();
        if ((&l.m_end1 == &n1 && &l.m_end2 == &n2) ||
                (&l.m_end1 == &n2 && &l.m_end2 == &n1)) {
            return &l;
        }
    }
    return NULL;
}

void NetworkTopology::addLink(Node &n1, Node &n2) {
    Link *existingLink = getLink(n1, n2);
    if (existingLink == NULL) {
        Link &l = *(new Link(n1, n2));
        m_links.push_back(l);
        std::cout << "Add Link: " << l;
    } else {
        std::cout << "Bidirectional " << *existingLink << std::endl;
        existingLink->m_isBidirectional = true;
        n1.links.push_back(*existingLink);
    }
}

void NetworkTopology::removeNode(Node &node) {
    // Need to remove links with this node on either end. Do this without
    // iterating list twice for each element
    std::cout <<"Removing node " << node.id << std::endl;
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        Link &link = it->get();
        Node &otherEnd = link.getOtherEnd(node);
        if (&otherEnd == &Node::null_node) {
            std::cout << "No good " <<link << std::endl;
            continue;
        }
        Node &toDelete = Node::null_node;
        
        if (&link.m_end1 == &node) {
            std::cout << "Removing Link to " << otherEnd.id << std::endl;
            node.links.remove(link);
            toDelete = otherEnd;
        } else {
            std::cout << "Removing Link from " << otherEnd.id << std::endl;
            otherEnd.links.remove(link);
            toDelete = node;
        }
        if (link.m_isBidirectional) {
            std::cout << "is bidirectional" << std::endl;
            toDelete.links.remove(link);
        } else
            std::cout << "isn't bidirectional" << std::endl;
        it = m_links.erase(it);
        it--;
        delete &(link);
    }
    m_nodes.erase(node);
    delete &node;
}

void NetworkTopology::activateNode(Node& node) {
    node.activated = true;
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = node.links.begin();
            it != node.links.end(); it++) {
        it->get().activated = true;
    }
}

void NetworkTopology::deactivateNode(Node& node) {
    node.activated = false;
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = node.links.begin();
            it != node.links.end(); it++) {
        it->get().activated = false;
    }
}


void NetworkTopology::update(sf::Event *event, const sf::Vector2f &windowSize)
{
    if (event != nullptr) {
        switch (event->type) {
            case sf::Event::MouseButtonPressed:
            {
                std::cout << "Mouse Press: x " << event->mouseButton.x << ", y " <<
                        event->mouseButton.y << std::endl;
                
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
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
                m_selectedNode = NULL;
                break;
            case sf::Event::MouseMoved:
                if (m_selectedNode != NULL) {
                    std::cout << "Mouse moved x: " << event->mouseMove.x << ", y: " << event->mouseMove.y << std::endl;
                    
                    m_selectedNode->x = pixelToUnit(windowSize.x, event->mouseMove.x);
                    m_selectedNode->y = pixelToUnit(windowSize.y, event->mouseMove.y);
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
    }
}


void NetworkTopology::render(sf::RenderWindow& window,
        const sf::Vector2f &windowSize)
{
    for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
            it != m_nodes.end(); it++) {
        if (it->get().isRenderable())
            it->get().render(window, windowSize);
    }
    
    for (std::list<std::reference_wrapper<Link>>::iterator it = m_links.begin();
            it != m_links.end(); it++) {
        if (it->get().isRenderable())
            it->get().render(window, windowSize);
    }
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
