#ifndef grid_hpp
#define grid_hpp

#include <deque>
#include <raylib.h>
#include "tile.hpp"
#include "corner.hpp"
#include "edge.hpp"
#include <math.h>

class Grid
{
public:
    Grid(int s);
    int size;
    std::deque<Corner> corners;
    std::deque<Tile> tiles;
    std::deque<Edge> edges;
};


#endif