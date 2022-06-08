#ifndef corner_hpp
#define corner_hpp

#include <raylib.h>
#include <array>

class Tile;
class Edge;

class Corner
{
public:
    Corner(int id);
    int id;
    Vector3 v;
    std::array<const Tile*, 3> tiles;
	std::array<const Corner*, 3> corners;
	std::array<const Edge*, 3> edges;
};

#endif