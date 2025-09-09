#include "CellGrid.h"

#include <iostream>
#include <iterator>
#include <ostream>

#include "CoreTypes.h"

CellGrid::~CellGrid()
{
    resetCellDefaults();
}

void CellGrid::initialize(int w, int h)
{
    width = w;
    heigth = h;
    grid = GridType(h);
    for (int r = 0; r < h; ++r)
    {
        grid[r] = std::vector<std::unique_ptr<Cell>>(w);
    }
}

void CellGrid::loadCellTypes(const std::vector<CellTraits>& cellTraits)
{
    resetCellDefaults();
    for (auto& cellTrait : cellTraits)
    {
        if (cellDefaults.find(cellTrait.name) == cellDefaults.end())
        {
            addCellDefault(cellTrait);
        }
    }
}

void CellGrid::createCell(int r, int c, const std::string& cellName)
{
    if (cellDefaults.find(cellName) == cellDefaults.end() || r < 0 || c < 0 || r >= heigth || c >= width)
    {
        return;
    }
    grid[r][c] = cellDefaults[cellName]->clone();
    grid[r][c]->updatePosition(r, c);

    addPendingCell(r, c);
}

void CellGrid::step()
{
    UniqueQueue<int> localUpdates = pendingUpdates;
    pendingUpdates.clear();


    while (!localUpdates.empty())
    {
        const int updateIndex = localUpdates.front();
        localUpdates.pop();
        performCellUpdate(updateIndex);
    }

}

const std::unique_ptr<Cell>& CellGrid::getCell(int r, int c) const
{
    if (r < 0 || c < 0 || r >= heigth || c >= width)
    {
        return nullptr;
    }
    return grid[r][c];
}

const std::unique_ptr<Cell>& CellGrid::getCellDefault(const std::string& cellName) const
{
    if (cellDefaults.count(cellName) != 0)
    {
        return cellDefaults.at(cellName);
    }

    return nullptr;
}

bool CellGrid::isValidCellIndex(int r, int c) const
{
    return r >= 0 && c >= 0 && r < heigth && c < width;
}

bool CellGrid::isValidCell(int r, int c) const
{
    return isValidCellIndex(r, c) && grid[r][c] != nullptr;
}

void CellGrid::swapCells(int r1, int c1, int r2, int c2)
{
    if (!isValidCellIndex(r1, c1) || !isValidCellIndex(r2, c2))
    {
        return;
    }
    auto temp = std::move(grid[r1][c1]);
    grid[r1][c1] = std::move(grid[r2][c2]);
    grid[r2][c2] = std::move(temp);
    
    if (grid[r1][c1])
    {
        grid[r1][c1]->updatePosition(r1, c1);
        addPendingCell(r1, c1);
        propagateDormancy(r1, c1);
    }
    if (grid[r2][c2])
    {
        grid[r2][c2]->updatePosition(r2, c2);
        addPendingCell(r2, c2);
        propagateDormancy(r2, c2);
    }
}

std::vector<std::string> CellGrid::getCellNames() const
{
    std::vector<std::string> cellNames;
    cellNames.reserve(cellDefaults.size());
    for (auto& [k, v] : cellDefaults)
    {
        cellNames.push_back(k);
    }
    return cellNames;
}

void CellGrid::performCellUpdate(int index)
{
    // we want to filter out the possibility of updating the same cell multiple times in one frame.
    // if we have a cell with the same index already in pending updates - we will update it next frame
    if (pendingUpdates.contains(index))
    {
        return;
    }
    const int r = index / width;
    const int c = index % width;
    if (!isValidCell(r, c))
    {
        return;
    }

    grid[r][c]->step(this);
}

void CellGrid::propagateDormancy(int r, int c)
{
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            if (isValidCell(r + i, c + j) && (i != 0 || j != 0))
            {
                addPendingCell(r + i, c + j);
            }
        }
    }
}

void CellGrid::addCellDefault(const CellTraits& trait)
{
    std::unique_ptr<Cell> newCell = nullptr;
    switch (trait.type)
    {
        case CellType::Solid:
            newCell = std::make_unique<SolidCell>();
            break;
        case CellType::Gas:
            newCell = std::make_unique<GasCell>();
            break;
        case CellType::Grain:
            newCell = std::make_unique<GrainCell>();
            break;
        case CellType::Liquid:
            newCell = std::make_unique<LiquidCell>();
            break;
    }
    
    if (!newCell)
    {
        return;
    }

    newCell->load(trait);
    cellDefaults[trait.name] = std::move(newCell);

}

void CellGrid::resetCellDefaults()
{
    cellDefaults.clear();
}

void CellGrid::addPendingCell(int r, int c)
{
    if (!isValidCell(r, c))
    {
        return;
    }
    const int index = r * width + c;
    pendingUpdates.push(index);
}
