#pragma once
#include <SFML/Graphics/Font.hpp>

#include "core/CellGrid.h"

namespace sf
{
    class RenderWindow;
}

class Application
{
public:
    void load();
    void run();
private:
    CellGrid grid {};
    int pixelSize =  0;
    unsigned width = 0;
    unsigned height = 0;
    sf::Font font;

    int brushSize = 1;

    int activeMatter = 0;
    std::vector<std::string> matterNames;

    void tryChangeActiveMatter(int newActiveMatter);

    const std::string& getActiveMatterName() const;

    void changeBrushSize(int delta);

    std::pair<sf::Vector2i, sf::Vector2i> getBrushBounds(const sf::RenderWindow& window) const;
    
    void drawGrid(sf::RenderWindow& window);
    void drawInfo(sf::RenderWindow& window);
    void handleMouse(sf::RenderWindow& window);
    void handleEvents(sf::RenderWindow& window);
};
