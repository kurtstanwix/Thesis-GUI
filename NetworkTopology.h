
/* Only include this header file once. */
#ifndef _NETWORKTOPOLOGY_H_
#define _NETWORKTOPOLOGY_H_ 1

#include <iostream>
#include <set>
#include <list>
#include <memory>
#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"

#include "BezierCurve.h"
#include "LayeredRenderable.h"

enum nodeLayout
{
    Custom = 0,
    Circle = 1,
    Grid = 2
};

class NetworkTopology : public LayeredRenderable
{
private:
    struct Link;
    struct Node;
    
    int m_numNodes;
    std::set<std::reference_wrapper<Node>> m_nodes;
    int m_nodeWidth;
    nodeLayout m_layout;
    
    std::list<std::reference_wrapper<Link>> m_links;
    
    Node *m_selectedNode;
    
    
    Link* getLink(Node& n1, Node& n2);
    Link* addLink(Node& n1, Node& n2);
    void removeNode(Node &node);
    
    Node* getNode(int id) {
        for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
                it != m_nodes.end(); it++) {
            if (id == it->get().id) {
                return &it->get();
            }
        }
        return NULL;
    }
    
    struct Node : public Renderable {
        //Node(int id) : id(id) {}
        int id;
        sf::Vector2f m_pos; // in unit coordinates
        std::list<std::reference_wrapper<Link>> links;
        sf::RectangleShape shape;
        sf::Text m_label;
        bool activated;
        
        void init(int id, int width);
        Node() : Node(0, 0) {};
        Node(int id, int width);
        //virtual ~Node();
        
        void setActive(bool state);
        
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize,
                bool &clickedOn);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
        bool contains(float x, float y);
        
        static Node null_node;
        
        //friend std::ostream& operator<<(std::ostream& os, const Node& node);
        
        friend bool operator<(const Node& lhs, const Node& rhs) {
            return lhs.id < rhs.id;
        }
    protected:
        void streamOut(std::ostream& os) const {
            os << this->id;
        }
    };
    
    struct Link : public Renderable {
        //Node(int id) : id(id) {}
        std::vector<std::reference_wrapper<Node>> m_ends;
        BezierCurve shape;
        std::vector<sf::CircleShape> endArrows;
        bool m_isBidirectional;
        bool activated;
        
        void setActive(bool state)
        {
            activated = state;
        }
        
        Node& getOtherEnd(Node& n) const {
            if (&m_ends[0].get() == &n) {
                return m_ends[1];
            } else if (&m_ends[1].get() == &n) {
                return m_ends[0];
            } else {
                return Node::null_node;
            }
        };
        
        friend bool operator==(const Link &lhs, const Link &rhs) {
            return &lhs == &rhs;
        }
        
        
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize,
                bool &clickedOn);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
        bool contains(float x, float y);
        
        Link(Node& m_end1, Node& m_end2);
        //virtual ~Link();
    protected:
        void streamOut(std::ostream& os) const {
            os << "(" << this->m_ends[0] << "," << this->m_ends[1] << ")";
        }
    };
    
    
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    // Private to avoid direct creation
    NetworkTopology(std::map<int, std::set<int>> nodeLinks,
            int nodeWidth, const sf::Vector2f &windowSize,
            nodeLayout layout = Circle);
    NetworkTopology(int numNodes, int nodeWidth,
            const sf::Vector2f &windowSize, nodeLayout layout = Circle);
public:
    ~NetworkTopology();
        
    //NetworkTopology(NetworkTopology&& source) = default;
    static NetworkTopology* createTopology(int numNodes, int nodeWidth,
            const sf::Vector2f &windowSize);
    
    void save(const std::string &fileName);
    void print();
    
    bool setNodeActive(int nodeID, bool state);
    bool setLinkActive(int nodeID1, int nodeID2, bool state);
    
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn);
    void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
    bool contains(float x, float y) { return false; };
protected:
    void streamOut(std::ostream& os) const {};
};


//std::ostream& operator<<(std::ostream& os, const Node& node) {
//    
//    return os;
//}


#endif