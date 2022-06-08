#include "edge.hpp"

Edge::Edge (int i) :
	id (i) {
	for (auto& t : tiles)
		t = nullptr;
	for (auto& c : corners)
		c = nullptr;
}