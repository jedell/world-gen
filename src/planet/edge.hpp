#ifndef edge_h
#define edge_h

#include <array>
class Tile;
class Corner;

class Edge {
public:
	Edge (int id);
	
	int id;
	std::array<const Tile*, 2> tiles;
	std::array<Corner*, 2> corners;
};

#endif