#ifndef tile_hpp
#define tile_hpp

#include <raylib.h>
#include <vector>

class Corner;
class Edge;

class Tile
{
public:
    Tile(int id, int edge_count);

    int id;
    int edge_count;
    Vector3 v;
    std::vector<const Tile *> tiles;
    std::vector<const Corner *> corners;
    std::vector<const Edge *> edges;
};

#endif