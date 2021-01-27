#include "Config.h"

#if _TEST_ == 1
#include <iostream>
#include <map>
#include <typeinfo>
#include <vector>

#include "plog/Log.h"
#include "plog/Init.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Appenders/ConsoleAppender.h"

#include "SFML/Graphics.hpp"

#include "LayeredRenderable.h"
#include "Renderable.h"
#include "Utility.h"

bool LayerIteratorForwardTest();
bool LayerIteratorReverseTest();


/* Stub Renderable class with identifier */
class Test : public Renderable
{
public:
    int id;
    Test() : Test(0) {};
    Test(int id) : id(id) {};
    /* Renderable interface */
    void update(sf::Event *event, const sf::Vector2f &windowSize,
            bool clickedOn) {};
    void render(sf::RenderWindow &window, const sf::Vector2f &windowSize) {};
    bool contains(float x, float y) { return false; };
protected:
    void streamOut(std::ostream &os) const
    {
        os << id;
    };
};

int main(int argc, char **argv)
{
    static plog::ConsoleAppender<plog::TxtFormatter> debugConsole;
    plog::init(plog::Severity::debug, &debugConsole);
    
    
    Test t;
    PLOGD << "t: " << typeid(t).name();
    PLOGD << "Test: " << typeid(Test).name();
    PLOGD << "Equals: " << (typeid(t) == typeid(Test));
    
    
    PLOGD << "Starting LayerIteratorForwardTest";
    if (LayerIteratorForwardTest())
        PLOGI << "Passed LayerIterator test";
    else
        PLOGE << "Failed LayerIterator test";
    
    PLOGD << "Starting LayerIteratorReverseTest";
    if (LayerIteratorReverseTest())
        PLOGI << "Passed LayerReverseIterator test";
    else
        PLOGE << "Failed LayerReverseIterator test";
}


bool LayerIteratorForwardTest()
{
    /* SET UP */
    /* Structure of layers:
     * 0:  0,  1,  2, 3, 4
     * 1:  5,  6,  7, 8
     * 2:  9, 10, 11
     * 3: 12, 13
     * 4: 14
     */
    /* Work out how many nodes this requires */
    std::vector<int> numNodesPerLayer;
    int numLayers = 5, numItems = 0;
    for (int i = numLayers; i > 0; i--) {
        numNodesPerLayer.push_back(i);
        numItems += i;
    }
    /* To hold the test data */
    std::vector<Test> testholder(numItems);
    for (int i = 0; i < numItems; i++) {
        testholder[i].id = i;
    }
    
    
    /* TEST */
    LayeredRenderable test;
    int id = 0;
    for (int layerNum = 0; layerNum < 5; layerNum++) {
        PLOGD << "Adding Layer " << layerNum;
        test.addLayer(layerNum);
        for (int j = 0; j < numNodesPerLayer[layerNum]; j++) {
            if (test.addToLayer(layerNum, testholder[id])) {
                PLOGD << " Added item " << id << " to layer " << layerNum;
            } else {
                PLOGE << "Failed to add item " << id << " to layer " << layerNum;
                return false;
            }
            id++;
        }
    }
    if (test.getNumLayers() != numLayers) {
        PLOGE << "Number of layers added is inconsistent. Expected " <<
                numLayers << ", Got " << test.getNumLayers();
        return false;
    }
    if (test.size() != numItems) {
        PLOGE << "Number of items added is inconsistent. Expected " <<
                numItems << ", Got " << test.size();
        return false;
    }
    
    /* Check the iterator matches that in testholder */
    int itemNum = 0;
    PLOGD << "Unmodified iterator:";
    for (LayeredRenderable::iterator it = test.begin(); it != test.end(); ++it) {
        int itID = ((Test*)(*it))->id;
        PLOGD << " ID: " << itID;
        if (itID != testholder[itemNum].id) {
            PLOGE << "ID Mismatch in unmodified iterator. Expected " << 
                    testholder[itemNum].id << ", Got " << itID;
            return false;
        }
        itemNum++;
    }
    
    
    /* Check that modifying the underlying objects is reflected in the iterator
     * (ensuring the objects added actually are the ones in testholder */
    testholder[5].id = 20;
    itemNum = 0;
    for (LayeredRenderable::iterator it = test.begin(); it != test.end(); ++it) {
        int itID = ((Test*)(*it))->id;
        PLOGD << " ID: " << itID;
        if (itID != testholder[itemNum].id) {
            PLOGE << "ID Mismatch in modified iterator, expected " << 
                    testholder[itemNum].id << ", got " << itID;
            return false;
        }
        itemNum++;
    }
    return true;
}


bool LayerIteratorReverseTest()
{
    /* SET UP */
    /* Structure of layers:
     * 0:  0,  1,  2, 3, 4
     * 1:  5,  6,  7, 8
     * 2:  9, 10, 11
     * 3: 12, 13
     * 4: 14
     */
    /* Work out how many nodes this requires */
    std::vector<int> numNodesPerLayer;
    int numLayers = 5, numItems = 0;
    for (int i = numLayers; i > 0; i--) {
        numNodesPerLayer.push_back(i);
        numItems += i;
    }
    numItems++;
    /* To hold the test data */
    std::vector<Test> testholder(numItems);
    for (int i = 0; i < numItems; i++) {
        testholder[i].id = i;
    }
    
    
    /* TEST */
    LayeredRenderable test;
    int id = 0;
    for (int layerNum = 0; layerNum < 5; layerNum++) {
        PLOGD << "Adding Layer " << layerNum;
        test.addLayer(layerNum);
        for (int j = 0; j < numNodesPerLayer[layerNum]; j++) {
            if (test.addToLayer(layerNum, testholder[id])) {
                PLOGD << " Added item " << id << " to layer " << layerNum;
            } else {
                PLOGE << "Failed to add item " << id << " to layer " << layerNum;
                return false;
            }
            id++;
        }
    }
    if (test.addToLayer(4, testholder[testholder.size() - 1])) {
        PLOGD << " Added item " << testholder[testholder.size() - 1] << " to layer " << 4;
    } else {
        PLOGE << "Failed to add item " << testholder[testholder.size() - 1] << " to layer " << 4;
        return false;
    }
    if (test.getNumLayers() != numLayers) {
        PLOGE << "Number of layers added is inconsistent. Expected " <<
                numLayers << ", Got " << test.getNumLayers();
        return false;
    }
    if (test.size() != numItems) {
        PLOGE << "Number of items added is inconsistent. Expected " <<
                numItems << ", Got " << test.size();
        return false;
    }
    
    /* Check the iterator matches that in testholder */
    int itemNum = testholder.size() - 1;
    PLOGD << "itemNum " << itemNum;
    PLOGD << "Unmodified iterator:";
    for (LayeredRenderable::reverse_iterator it = test.rbegin(); it != test.rend(); ++it) {
        int itID = ((Test*)(*it))->id;
        PLOGD << " ID: " << itID;
        if (itID != testholder[itemNum].id) {
            PLOGE << "ID Mismatch in unmodified iterator. Expected " << 
                    testholder[itemNum] << ", Got " << itID;
            return false;
        }
        itemNum--;
    }
    
    
    /* Check that modifying the underlying objects is reflected in the iterator
     * (ensuring the objects added actually are the ones in testholder */
    testholder[5].id = 20;
    itemNum = testholder.size() - 1;
    for (LayeredRenderable::reverse_iterator it = test.rbegin(); it != test.rend(); ++it) {
        int itID = ((Test*)(*it))->id;
        PLOGD << " ID: " << itID;
        if (itID != testholder[itemNum].id) {
            PLOGE << "ID Mismatch in modified iterator, expected " << 
                    testholder[itemNum].id << ", got " << itID;
            return false;
        }
        itemNum--;
    }
    return true;
}

#endif