
/* Only include this header file once. */
#ifndef _NETWORKTOPOLOGY_H_
#define _NETWORKTOPOLOGY_H_ 1

#include <iostream>
#include <set>
#include <list>
#include <memory>
#include "SFML/Graphics.hpp"

#include "BezierCurve.h"
#include "Renderable.h"

class NetworkTopology : public Renderable
{
    private:
        struct Link;
        struct Node;
        
        int m_numNodes;
        std::set<std::reference_wrapper<Node>> m_nodes;
        int m_nodeWidth;
        
        std::list<std::reference_wrapper<Link>> m_links;
        
        Node *m_selectedNode;
        
        
        Link* getLink(Node& n1, Node& n2);
        void addLink(Node& n1, Node& n2);
        void removeNode(Node &node);
        void activateNode(Node& node);
        void deactivateNode(Node& node);
        
        
        struct Node : public Renderable {
            //Node(int id) : id(id) {}
            int id;
            float x; // in unit coordinates
            float y;
            std::list<std::reference_wrapper<Link>> links;
            sf::RectangleShape shape;
            bool activated;
            
            void init(int id, int width);
            Node() : Node(0, 0) {};
            Node(int id, int width);
            //virtual ~Node();
            
            /* Renderable interface */
            void update(sf::Event *event, const sf::Vector2f &windowSize);
            void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
            
            static Node null_node;
            
            friend std::ostream& operator<<(std::ostream& os, const Node& node) {
                os << node.id;
                return os;
            }
            
            friend bool operator<(const Node& lhs, const Node& rhs) {
                return lhs.id < rhs.id;
            }
        };
        
        struct Link : public Renderable {
            //Node(int id) : id(id) {}
            Node& m_end1;
            Node& m_end2;
            BezierCurve shape;
            sf::CircleShape end1Arrow;
            sf::CircleShape end2Arrow;
            bool m_isBidirectional;
            bool activated;
            
            Node& getOtherEnd(Node& n) const {
                if (&m_end1 == &n) {
                    return m_end2;
                } else if (&m_end2 == &n) {
                    return m_end1;
                } else {
                    return Node::null_node;
                }
            };
            
            friend std::ostream& operator<<(std::ostream& os, const Link& link) {
                os << "(" << link.m_end1 << "," << link.m_end2 << ")";
                return os;
            }
            
            friend bool operator==(const Link &lhs, const Link &rhs) {
                return &lhs == &rhs;
            }
            
            
            /* Renderable interface */
            void update(sf::Event *event, const sf::Vector2f &windowSize);
            void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
            
            Link(Node& m_end1, Node& m_end2);
            //virtual ~Link();
        };
        
        
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        // Private to avoid direct creation
        NetworkTopology(std::map<int, std::set<int>> nodeLinks,
                int nodeWidth, const sf::Vector2f &windowSize);
        NetworkTopology(int numNodes, int nodeWidth,
                const sf::Vector2f &windowSize);
    public:
        ~NetworkTopology();
            
        //NetworkTopology(NetworkTopology&& source) = default;
        static NetworkTopology* createTopology(int numNodes, int nodeWidth,
                const sf::Vector2f &windowSize);
        Node* getNode(int id) {
            for (std::set<std::reference_wrapper<Node>>::iterator it = m_nodes.begin();
                    it != m_nodes.end(); it++) {
                if (id == it->get().id) {
                    return &it->get();
                }
            }
            return NULL;
        }
        void save(const std::string &fileName);
        void print();
        
        /* Renderable interface */
        void update(sf::Event *event, const sf::Vector2f &windowSize);
        void render(sf::RenderWindow& window, const sf::Vector2f &windowSize);
};




#endif