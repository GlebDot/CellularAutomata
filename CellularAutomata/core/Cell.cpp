#include "Cell.h"

#include "CellGrid.h"
#include "CoreTypes.h"


void Cell::load(const CellTraits& inTraits)
{
    traits = inTraits;
}

void Cell::updatePosition(int r, int c)
{
    row = r;
    column = c;
}

std::unique_ptr<Cell> Cell::clone() const
{
    return std::unique_ptr<Cell>(cloneImpl());
}

const CellTraits& Cell::getTraits() const
{
    return traits;
}

const int Cell::getInertia() const
{
    return inertia;
}

Cell* SolidCell::cloneImpl() const
{
    return new SolidCell(*this);
}

void SolidCell::step(CellGrid* inGrid)
{
}

Cell* GrainCell::cloneImpl() const
{
    return new GrainCell(*this);
}

bool GrainCell::isFreeHorizontally(const CellGrid* inGrid, int dir) const
{
    return !inGrid->getCell(row, column + dir) || inGrid->getCell(row, column + dir)->getTraits().type != CellType::Solid;
}

void GrainCell::step(CellGrid* inGrid)
{
    constexpr int dirs[3][2] = {{1, 0}, {1, 1}, {1, -1}};
    for (const auto dir : dirs)
    {
        const int newRow = row + dir[0];
        const int newColumn = column + dir[1];
        if (inGrid->isValidCellIndex(newRow, newColumn))
        {
            if (dir[1] == 0 || isFreeHorizontally(inGrid, dir[1]))
            {
                if (!inGrid->getCell(newRow, newColumn))
                {
                    inGrid->swapCells(row, column, newRow, newColumn);
                    return;
                }

                const auto& newCellTraits = inGrid->getCell(newRow, newColumn)->getTraits();
                if (newCellTraits.type == CellType::Liquid || newCellTraits.type == CellType::Gas)
                {
                    if (inGrid->getCell(newRow, newColumn)->getTraits().density < getTraits().density)
                    {
                        inGrid->swapCells(row, column, newRow, newColumn);
                        return;
                    }
                }
            }
        }
    }
}

Cell* LiquidCell::cloneImpl() const
{
    return new LiquidCell(*this);
}

void LiquidCell::step(CellGrid* inGrid)
{
    constexpr int dirs[3][2] = {{1, 0}, {1, 1}, {1, -1}};
    for (const auto dir : dirs)
    {
        const int newRow = row + dir[0] * gravity;
        const int newColumn = column + dir[1];
        if (inGrid->isValidCellIndex(newRow, newColumn))
        {
            if (!inGrid->getCell(newRow, newColumn))
            {
                inGrid->swapCells(row, column, newRow, newColumn);
                return;
            }

            const auto& newCellTraits = inGrid->getCell(newRow, newColumn)->getTraits();
            if (newCellTraits.type != CellType::Solid && newCellTraits.type != CellType::Grain)
            {
                if (gravity * newCellTraits.density < gravity * getTraits().density)
                {
                    inGrid->swapCells(row, column, newRow, newColumn);
                    return;
                }
            }
        }
    }
    const int newColumn = column + inertia;
    if (inGrid->isValidCellIndex(row, newColumn))
    {
        if (!inGrid->getCell(row, newColumn))
        {
            inGrid->swapCells(row, column, row, newColumn);
            return;
        }

        CellType newCellType = inGrid->getCell(row, newColumn)->getTraits().type;
        if (newCellType == getTraits().type)
        {
            inertia = inGrid->getCell(row, newColumn)->getInertia();
            inGrid->addPendingCell(row, column);
        }
        else
        {
            inertia = -inertia;
        }
    }
    else
    {
        inertia = -inertia;
    }
}

GasCell::GasCell() 
{
    gravity = -1;
}

Cell* GasCell::cloneImpl() const
{
    return new GasCell(*this);
}
