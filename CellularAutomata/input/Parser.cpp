#include "Parser.h"

#include "../core/CoreTypes.h"


constexpr char DEFAULT_CONFIG[] = "config.txt";

Parser::Parser()
{
    loadConfig(DEFAULT_CONFIG);
}

Parser::Parser(const std::string& inConfigName)
{
    loadConfig(inConfigName);
}

Parser::~Parser()
{
    fileHandle.close();
}

void Parser::parse()
{
    std::string line = "";
    bool startedMatter = false;
    while (std::getline(fileHandle, line))
    {
        if (line.empty())
        {
            continue;
        }
        size_t delPos = line.find(':');
        std::string trait = line.substr(0, delPos);
        if (!startedMatter)
        {
            if (trait == "h")
            {
                height  = std::stoi(line.substr(delPos + 1));
            }
            if (trait == "w")
            {
                width  = std::stoi(line.substr(delPos + 1));
            }
            if (trait == "s")
            {
                pixelSize  = std::stoi(line.substr(delPos + 1));
            }
            if (trait == "matter")
            {
                startedMatter = true;
            }
        }
        else
        {
            std::vector<std::string> matterTraits = {line};
            while(std::getline(fileHandle, line))
            {
                if (line.empty())
                {
                    break;
                }
                matterTraits.push_back(line);
            }
            parseMatter(matterTraits);
        }
    }
}

int Parser::getPixelSize() const
{
    return pixelSize;
}

std::pair<int, int> Parser::getDimensions() const
{
    return {width, height};
}

const std::vector<CellTraits>& Parser::getCells() const
{
    return cells;
}

void Parser::loadConfig(const std::string& inConfigName)
{
    fileHandle.open(inConfigName, std::ios_base::in);
}

void Parser::parseColor(std::string colorStr, CellTraits& outConfig)
{
    size_t startPos = 0;
    for (int& i : outConfig.color)
    {
        size_t endPos = colorStr.find(',', startPos);
        std::string comp = colorStr.substr(startPos, endPos);
        i = std::stoi(comp);
        startPos = endPos + 1;
    }
}

void Parser::parseMatter(const std::vector<std::string>& matterTraits)
{
    CellTraits matterConfig;
    for (int i = 0; i < matterTraits.size(); ++i)
    {
        size_t delPos = matterTraits[i].find(':');
        std::string trait = matterTraits[i].substr(0, delPos);

        if (trait == "n")
        {
            matterConfig.name = matterTraits[i].substr(delPos + 1);
        }
        if (trait == "t")
        {
            matterConfig.type = fromStr(matterTraits[i].substr(delPos + 1));
        }
        if (trait == "d")
        {
            matterConfig.density = std::stoi(matterTraits[i].substr(delPos + 1));
        }
        if (trait == "c")
        {
            parseColor(matterTraits[i].substr(delPos + 1), matterConfig);
        }

    }
    cells.emplace_back(matterConfig);
}
