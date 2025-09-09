#pragma once
#include <memory>
#include <string>

class CellGrid;
enum class CellType : uint8_t;

struct CellTraits
{
    std::string name;
    CellType type;
    int density;
    int color[3]; 
};

class Cell
{
public:
    Cell() = default;
    Cell(const Cell& other) = default;
    Cell(Cell&& other) noexcept = default;
    Cell& operator=(const Cell& other) = default;
    Cell& operator=(Cell&& other) noexcept = default;
    virtual ~Cell() = default;
    
    virtual void load(const CellTraits& inTraits);
    virtual void updatePosition(int r, int c);
    virtual void step(CellGrid* inGrid) = 0;
    
    std::unique_ptr<Cell> clone() const;
    const CellTraits& getTraits() const;
    const int getInertia() const;

protected:
    virtual Cell* cloneImpl() const = 0;

    
    CellTraits traits = {};
    int row = 0;
    int column = 0;

    // a variable to store either left or right direction on the surface
    int inertia = 1;
};

class SolidCell : public Cell
{
public:
    virtual void step(CellGrid* inGrid) override;
    
protected:
    virtual Cell* cloneImpl() const override;
};

class GrainCell : public Cell
{
public:
    virtual void step(CellGrid* inGrid) override;
    
protected:
    virtual Cell* cloneImpl() const override;
    bool isFreeHorizontally(const CellGrid* inGrid, int dir) const;
};

class LiquidCell : public Cell
{
public:
    virtual void step(CellGrid* inGrid) override;
protected:
    int gravity = 1; 
    virtual Cell* cloneImpl() const override;
};

class GasCell : public LiquidCell
{
public:
    GasCell();
protected:
    virtual Cell* cloneImpl() const override;
};