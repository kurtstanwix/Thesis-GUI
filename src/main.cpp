#include <limits.h>

#include <iostream>

#include <chrono>
#include <ctime>
#include <stdlib.h>

#include "plog/Log.h"
#include "plog/Init.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Appenders/ConsoleAppender.h"

#include "SFML/Graphics.hpp"

#include "LayeredRenderable.h"
#include "NetworkWindow.h"


#include "Config.h"
#include "NetworkTopology.h"
#include "Utility.h"


using namespace std;

#include "InfoPane.h"


#include "Interface.h"
void exitAction(InterfaceButton &caller)
{
    //exit(0);
}

void saveAction(InterfaceButton &caller)
{
    //PLOGI << "Saving";
    //caller.m_parent.m_nettop.save("../test2");
}

typedef pair<size_t, size_t> sizePair;

string actions[] = {"UP", "DOWN", "LEFT", "RIGHT"};

// Example usage of library. If used as a library this will be filtered out

#if _TEST_ == 0
int main(int argc, char **argv)
{
    
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    cout << std::string( result, (count > 0) ? count : 0 ) << endl;
    
    static plog::ConsoleAppender<plog::TxtFormatter> debugConsole;
    plog::init(plog::info, &debugConsole);
    
    using namespace std::chrono;
    
    int squareWidth = 100;
    int width = 1200;
    int height = 1200;
    
    
    sf::RenderWindow window(sf::VideoMode(width, height), "MDP Policy");
    
    srand(time(0));
    
    NetworkWindow networkWindow;
    if (!networkWindow.init(sf::Vector2f(window.getSize()), RESOURCE("MDPExample1.json"), squareWidth))
        exit(1);
    
    static const microseconds step(1000000 / 60);
    system_clock::time_point oldTime = system_clock::now();

    int ticks = 0;
    networkWindow.m_nettop->setNodeInfoTitle(2, "Custom Title");
    networkWindow.m_nettop->setNodeInfoColor(2, sf::Color::Green);
    networkWindow.m_nettop->setNodeInfoParameter(2, "Dynamic Add", "This is a test of dynamic adding through API");
    
    while (window.isOpen())
    {
        sf::Vector2f windowSize(window.getSize());
        if (duration_cast<microseconds>(system_clock::now() - oldTime).count() >= step.count()) {
            ticks++;
            // Update logic
            sf::Event event;
            bool clickedOn = false;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                else
                    networkWindow.update(&event, windowSize, clickedOn);
            }
            networkWindow.update(nullptr, windowSize, clickedOn);
            
            oldTime = system_clock::now();
        }
        
        window.clear();
        networkWindow.render(window, windowSize);
        window.display();
    }
}

#endif