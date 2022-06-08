#include "corner.hpp"

Corner::Corner (int i) :
	id (i) {
	for (auto& t : tiles)
		t = nullptr;
	for (auto& c : corners)
		c = nullptr;
	for (auto& e : edges)
		e = nullptr;
}