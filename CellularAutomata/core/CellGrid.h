#pragma once
#include <memory>
#include <map>
#include <set>
#include <vector>

#include "Cell.h"
#include "../utils/UniqueQueue.h"

class Cell;

class CellGrid
{
typedef std::vector<std::vector<std::unique_ptr<Cell>>> GridType;

public:
    ~CellGrid();
    void initialize(int w, int h);
    void loadCellTypes(const std::vector<CellTraits>& cellTraits);
    void createCell(int r, int c, const std::string& cellName);

    void step();

    const std::unique_ptr<Cell>& getCell(int r, int c) const;
    const std::unique_ptr<Cell>& getCellDefault(const std::string& cellName) const;
    bool isValidCellIndex(int r, int c) const;
    bool isValidCell(int r, int c) const;
    void swapCells(int r1, int c1, int r2, int c2);
    void addPendingCell(int r, int c);
    std::vector<std::string> getCellNames() const;

private:
    GridType grid;
    std::map<std::string, std::unique_ptr<Cell>> cellDefaults {};
    int width = 0;
    int heigth = 0;

    UniqueQueue<int> pendingUpdates;

    void propagateDormancy(int r, int c);

    void performCellUpdate(int index);
    void addCellDefault(const CellTraits& trait);
    void resetCellDefaults();
};
