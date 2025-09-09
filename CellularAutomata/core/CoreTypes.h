#pragma once
#include <cstdint>
#include <string>

enum class CellType : uint8_t
{
    Solid,
    Grain,
    Liquid,
    Gas
};

CellType fromStr(std::string str);