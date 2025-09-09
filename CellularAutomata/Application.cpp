#include "Application.h"
#include <SFML/Graphics.hpp>

#include "core/CoreTypes.h"
#include "input/Parser.h"

namespace App
{
    // numbers from 1 to 9 on keyboard
    constexpr size_t maxMatters = 9;

    constexpr int textScale = 3;
    constexpr int maxBrushSize = 4;

}


void Application::load()
{
    font.openFromFile("resources/arial.ttf");
    
    Parser parser = Parser();
    parser.parse();

    auto [w,h] = parser.getDimensions();
    pixelSize = parser.getPixelSize();

    width = w * pixelSize;
    height = h * pixelSize;
    
    grid.initialize(w, h);
    grid.loadCellTypes(parser.getCells());

    matterNames = grid.getCellNames();
    matterNames.resize(std::min(matterNames.size(), App::maxMatters));
}

void Application::tryChangeActiveMatter(int newActiveMatter)
{
    const int minMatter = std::min(matterNames.size(), App::maxMatters);
    if (newActiveMatter < minMatter && newActiveMatter >= 0)
    {
        activeMatter = newActiveMatter;
    }
}

const std::string& Application::getActiveMatterName() const
{
    return matterNames[activeMatter];
}

void Application::changeBrushSize(int delta)
{
    brushSize = std::clamp(brushSize + delta, 1, App::maxBrushSize);
}

std::pair<sf::Vector2i, sf::Vector2i> Application::getBrushBounds(const sf::RenderWindow& window) const
{
    sf::Vector2i startPosition = sf::Mouse::getPosition(window);
    startPosition.x = startPosition.x - (startPosition.x % pixelSize) - (brushSize - 1) * pixelSize;
    startPosition.y = startPosition.y - (startPosition.y % pixelSize) - (brushSize - 1) * pixelSize;
    sf::Vector2i endPosition = startPosition + (sf::Vector2i(pixelSize, pixelSize) * (2 * (brushSize - 1) + 1));

    return std::make_pair(startPosition, endPosition);
}

void Application::drawGrid(sf::RenderWindow& window)
{
    const int rows = height / pixelSize;
    const int columns = width / pixelSize;
    for(int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            if (grid.getCell(r, c))
            {
                sf::CircleShape shape(pixelSize / 2, 5);
                auto col = grid.getCell(r, c)->getTraits().color;
                shape.setFillColor(sf::Color(col[0], col[1], col[2]));
                shape.setPosition(sf::Vector2f(c * pixelSize, r * pixelSize));

                window.draw(shape);
            }
        }
    }
}

void Application::drawInfo(sf::RenderWindow& window)
{
    // draw material
    const std::string& selectedMatterName = getActiveMatterName();
    const std::unique_ptr<Cell>& matterDefaults = grid.getCellDefault(selectedMatterName);
    if (!matterDefaults)
    {
        return;
    }
    
    sf::Text selectedMatterText(font);
    selectedMatterText.setString(selectedMatterName);
    auto col = matterDefaults->getTraits().color;
    selectedMatterText.setFillColor(sf::Color(col[0], col[1], col[2]));
    selectedMatterText.setCharacterSize(pixelSize * App::textScale);
    window.draw(selectedMatterText);

    // draw brush
    auto [startPosition, endPosition] = getBrushBounds(window);
    std::array line = {
        sf::Vertex{sf::Vector2f(startPosition.x, startPosition.y)},
        sf::Vertex{sf::Vector2f(startPosition.x, endPosition.y)},
        sf::Vertex{sf::Vector2f(endPosition.x, endPosition.y)},
        sf::Vertex{sf::Vector2f(endPosition.x, startPosition.y)},
        sf::Vertex{sf::Vector2f(startPosition.x, startPosition.y)},
    };
    window.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);
}

void Application::handleMouse(sf::RenderWindow& window)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        auto [startPosition, endPosition] = getBrushBounds(window);
        for (int i = startPosition.x; i < endPosition.x; i += pixelSize)
        {
            for (int j = startPosition.y; j < endPosition.y; j += pixelSize)
            {
                const int r = j / pixelSize;
                const int c = i / pixelSize;
                grid.createCell(r, c, getActiveMatterName());
            }
        }
    }
}

void Application::handleEvents(sf::RenderWindow& window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode >= sf::Keyboard::Scancode::Num1 && keyPressed->scancode <= sf::Keyboard::Scancode::Num9)
            {
                tryChangeActiveMatter(static_cast<int>(keyPressed->scancode) - static_cast<int>(sf::Keyboard::Scancode::Num1));
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::NumpadPlus)
            {
                changeBrushSize(1);
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::NumpadMinus)
            {
                changeBrushSize(-1);
            }
        } 
    }
}

void Application::run()
{
    sf::RenderWindow  window(sf::VideoMode({width, height}), "CellularAutomata");
    while(window.isOpen())
    {
        handleEvents(window);

        window.clear();

        handleMouse(window);

        grid.step();

        drawGrid(window);
        drawInfo(window);

        window.display();
    }
}
