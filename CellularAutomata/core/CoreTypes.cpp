#include "CoreTypes.h"

CellType fromStr(std::string str)
{

    if (str == "gr")
    {
        return CellType::Grain;
    }
    if (str == "l")
    {
        return CellType::Liquid;
    }
    if (str == "s")
    {
        return CellType::Solid;
    }
    if (str == "g")
    {
        return CellType::Gas;
    }


    return CellType::Grain;
}
