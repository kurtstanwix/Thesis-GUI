#ifndef _LAYEREDRENDERABLE_H
#define _LAYEREDRENDERABLE_H 1

#include <list>
#include <map>
#include <memory>

#include "plog/Log.h"
#include "SFML/Graphics.hpp"

#include "Renderable.h"

class Layer
{
private:
    std::list<Renderable*> m_renderables;
public:
    typedef std::list<Renderable*>::size_type size_type;
    typedef std::list<Renderable*>::iterator iterator;
    typedef std::list<Renderable*>::reverse_iterator reverse_iterator;
    
    //virtual void update(sf::Event *event, const sf::Vector2f &windowSize) = 0;
    //virtual void render(sf::RenderWindow& window,
    //        const sf::Vector2f &windowSize) = 0;
    //{
        //for (std::list<Renderable>::iterator it = m_renderables.begin())
    //
    
    
    
    const iterator begin()
    {
        return m_renderables.begin();
    }
    
    const iterator end()
    {
        return m_renderables.end();
    }
    
    const reverse_iterator rbegin()
    {
        return m_renderables.rbegin();
    }
    
    const reverse_iterator rend()
    {
        return m_renderables.rend();
    }
    
    size_type size() const
    {
        return m_renderables.size();
    }
    
    void add(Renderable &item)
    {
        PLOGD << "Adding renderable " << item << " @ " << &item;
        m_renderables.push_back(&item);
        PLOGD << "Size " << m_renderables.size();
        PLOGD << "Last: " << *m_renderables.back() << " @ " << m_renderables.back();
    }
};







/*
 * Seperates the rendering and updating into layers of objects, with the lowest
 * layer being drawn on top.
 */
class LayeredRenderable : public Renderable
{
private:
    std::map<int, Layer> m_layers;
    
public:
    class iterator :
            public std::iterator<std::bidirectional_iterator_tag, Renderable*>
    {
    private:
        /* Increments the outerloop to the next Layer with renderables in it.
         * Returns true if there are renderables in the current outerloop
         * position, returns false if there are no non-empty Layers left */
        bool advanceOuterLoop()
        {
            m_parentPos++;
            while (m_parentPos != m_parent.end()) {
                if (m_parentPos->second.size() != 0) {
                    return true;
                }
                m_parentPos++;
            }
            return false;
        }
        /* Decrements the outerloop to the next Layer with renderables in it.
         * Returns true if there are renderables in the current outerloop
         * position, returns false if there are no non-empty Layers left */
        bool reverseOuterLoop()
        {
            if (m_parentPos == m_parent.rend().base())
                return false; /* Stop moving if the outer loop is done */
            do {
                m_parentPos--;
                if (m_parentPos->second.size() != 0) {
                    return true;
                }
            } while (m_parentPos != m_parent.rend().base());
            return false;
        }
    public:
        iterator(std::map<int, Layer> &parent,
                const std::map<int, Layer>::iterator &parentPos,
                const std::list<value_type>::iterator &p)
            :
                m_parent(parent),
                m_parentPos(parentPos),
                m_curr(p) {};
        iterator(iterator *source)
            :
                m_parent(source->m_parent),
                m_parentPos(source->m_parentPos),
                m_curr(source->m_curr) {};
        
        iterator& operator=(iterator &other)
        {
            m_parent = *(&other.m_parent);
            m_parentPos = other.m_parentPos;
            m_curr = other.m_curr;
            return *this;
        };
        
        reference operator*() const { return *m_curr; };
        value_type operator->() const { return *m_curr; };
        
        iterator& operator++()
        {
            m_curr++;
            /* Check if at the end of the inner loop, if so, go to next list */
            if (m_curr == m_parentPos->second.end()) {
                if (advanceOuterLoop()) /* Go through the next outer list */
                    m_curr = m_parentPos->second.begin();
                else /* No More left */
                    m_curr = m_parent.rbegin()->second.end();
            }
                
            return *this;
        }
        
        iterator& operator--()
        {
            /* Check if at the end of the inner loop, if so, go to next list */
            if (m_curr == m_parentPos->second.rend().base()) {
                if (reverseOuterLoop()) { /* Go through the next outer list */
                    Layer::reverse_iterator tmp = m_parentPos->second.rbegin();
                    m_curr = (++tmp).base();
                }
                else /* No More left */
                    m_curr = m_parent.rend()->second.rend().base();
            } else {
                m_curr--;
            }
                
            return *this;
        }
        
        friend bool operator==(const iterator &a, const iterator &b)
        {
            if (a.m_parentPos != b.m_parentPos)
                return false;
            
            if (a.m_curr != b.m_curr)
                return false;
            
            return true;
        }
        
        friend bool operator!=(const iterator &a, const iterator &b)
        {
            return !(a == b);
        }
    protected:
        std::map<int, Layer> &m_parent;
        std::map<int, Layer>::iterator m_parentPos;
        std::list<value_type>::iterator m_curr;
    };
    typedef std::reverse_iterator<iterator> reverse_iterator;

    /*
     * Return true if item is added to list, false if list does not exist
     */
    bool addToLayer(int id, Renderable &item)
    {
        std::map<int, Layer>::iterator it = m_layers.find(id);
        if (it == m_layers.end())
            return false;
        it->second.add(item);
        return true;
    }

    void addLayer(int id)
    {
        m_layers.emplace(id, Layer());
    }

    virtual void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool &clickedOn) = 0;
    
    virtual void render(sf::RenderWindow& window,
            const sf::Vector2f &windowSize) = 0;
    
    virtual bool contains(float x, float y) = 0;
    
    const iterator begin()
    {
        /* Find the first non-empty layer */
        std::map<int, Layer>::iterator it;
        for (it = m_layers.begin(); it != m_layers.end(); it++) {
            if (it->second.size() != 0)
                break;
        }
        if (it == m_layers.end())
            return end();
        else
            return iterator(m_layers, it, it->second.begin());
    }
    
    const iterator end()
    {
        return iterator(m_layers, m_layers.end(),
                std::prev(m_layers.end())->second.end());
    }
    
    const reverse_iterator rbegin()
    {
        /* Find the first non-empty layer */
        std::map<int, Layer>::reverse_iterator it;
        for (it = m_layers.rbegin(); it != m_layers.rend(); it++) {
            if (it->second.size() != 0)
                break;
        }
        if (it == m_layers.rend())
            return rend();
        else {
            Layer::reverse_iterator rev = it->second.rbegin();
            PLOGV << " Found non empty outer layer: " << it->first;
            return reverse_iterator(iterator(m_layers, (++it).base(),
                    (rev).base()));
        }
    }
    
    const reverse_iterator rend()
    {
        return reverse_iterator(iterator(m_layers, m_layers.begin(),
                m_layers.begin()->second.rend().base()));
        //return m_layers.begin()->rend();
    }
    
    Layer::size_type size() const
    {
        Layer::size_type numLayers = 0;
        for (std::map<int, Layer>::const_iterator it = m_layers.begin();
                it != m_layers.end(); it++) {
            numLayers += it->second.size();
        }
        return numLayers;
    }
    
    std::map<int, Layer>::size_type getNumLayers() const
    {
        return m_layers.size();
    }
    
    Layer* getLayer(int layerID)
    {
        try {
            return &m_layers.at(layerID);
        } catch(const std::out_of_range &e) {
            return nullptr;
        }
    }
protected:
    LayeredRenderable()
    {
        /* Has an initial layer that can't be deleted */
        m_layers.emplace(0, Layer());
    }
    void streamOut(std::ostream &os) const {};
};

#endif