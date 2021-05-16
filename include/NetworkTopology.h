
/* Only include this header file once. */
#ifndef _NETWORKTOPOLOGY_H_
#define _NETWORKTOPOLOGY_H_ 1

#include <chrono>
#include <iostream>
#include <set>
#include <list>
#include <memory>
#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"

#include "BezierCurve.h"
#include "InfoPane.h"
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
    Node* addNode(Node& n);
    void removeNode(Node &node);
    
    Node* getNode(int id)
    {
        for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
                it != m_nodes.end(); it++) {
            if (id == it->get().id) {
                return &it->get();
            }
        }
        return nullptr;
    }
    
    Link* getLink(int end1ID, int end2ID)
    {
        Node *end1 = getNode(end1ID);
        Node *end2 = getNode(end2ID);
        if (end1 == nullptr || end2 == nullptr)
            return nullptr;
        return getLink(*end1, *end2);
    }
    
    struct Node : public Renderable {
        //Node(int id) : id(id) {}
        int id;
        sf::Vector2f m_pos; // in unit coordinates
        std::list<std::reference_wrapper<Link>> links;
        sf::RectangleShape shape;
        sf::Text m_label;
        //bool activated;
        
        InfoPane m_info;
        
        void init(int id, int width);
        Node() : Node(0, 0) {};
        Node(int id, int width);
        //virtual ~Node();
        
        //void setActive(bool state);
        
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
        //bool activated;
        
        InfoPane m_info;
        
        void setSelected(bool state)
        {
            m_selected = state;
            shape.setSelected(state);
        }
        
        Node& getOtherEnd(const Node& n) const {
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
public:
    ~NetworkTopology();
        
    //NetworkTopology(NetworkTopology&& source) = default;
    static NetworkTopology* createTopology(const sf::Vector2f &windowSize,
            const std::string &fileName, int nodeWidth = 100);
    
    void save(const std::string &fileName);
    void print();
    
    void addInfoPane(InfoPane &info);

    std::vector<int> getNodeIds();
    
    /* Returns true if Node with nodeID exists and if so, inList will contain
     * the nodeIDs of all Nodes that can access this Node. Otherwise returns
     * false and inList is unmodified
     */
    bool getNodesIn(int nodeID, std::vector<int> &inList);
    
    /* Returns true if Node with nodeID exists and if so, outList will contain
     * the nodeIDs of all Nodes that can be accessed from this Node. Otherwise
     * returns false and outList is unmodified
     */
    bool getNodesOut(int nodeID, std::vector<int> &outList);
    
    bool setLinkSelected(int end1ID, int end2ID, bool state);
    bool setNodeSelected(int nodeID, bool state);
    
    /* Node property setters */
    bool setNodeColor(int nodeID, const sf::Color &col);
    
    /* Node InfoPane property setters */
    bool setNodeInfoColor(int nodeID, const sf::Color &col);
    bool setNodeInfoParameter(int nodeID, const std::string &label,
            const std::string &content, bool keepPosition = false);
    bool setNodeInfoTitle(int nodeID, const std::string &title);
    
    /* Link property setters */
    bool setLinkColor(int end1ID, int end2ID, const sf::Color &col);

    /* Link property getters */
    const sf::Color getLinkColor(int end1ID, int end2ID);
    
    /* Link InfoPane property setters */
    bool setLinkInfoColor(int end1ID, int end2ID, const sf::Color &col);
    bool setLinkInfoParameter(int end1ID, int end2ID, const std::string &label,
            const std::string &content, bool keepPosition = false);
    bool setLinkInfoTitle(int end1ID, int end2ID, const std::string &title);
    
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