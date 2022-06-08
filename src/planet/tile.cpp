#include "tile.hpp"

Tile::Tile (int i, int e) :
	id (i), edge_count (e) {
	tiles.resize(edge_count, nullptr);
	corners.resize(edge_count, nullptr);
	edges.resize(edge_count, nullptr);
}