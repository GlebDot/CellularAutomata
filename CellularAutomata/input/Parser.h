#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "../core/Cell.h"

enum class CellType : uint8_t;

class Parser
{
public:
    Parser();
    explicit Parser(const std::string& inConfigName);
    ~Parser();

    void parse();

    int getPixelSize() const;
    std::pair<int, int> getDimensions() const;
    const std::vector<CellTraits>& getCells() const;

private:
    std::ifstream fileHandle;

    int width = 0;
    int height = 0;
    int pixelSize = 0;

    std::vector<CellTraits> cells;
    
    void loadConfig(const std::string& inConfigName);
    void parseColor(std::string colorStr, CellTraits& outConfig);
    void parseMatter(const std::vector<std::string>& matterTraits);
};
