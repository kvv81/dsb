#include "random_placement.h"

#include <cstdlib>		// for random(), srandom()
#include <algorithm>	// for std::swap
#include <common/coords.h>	// for FIELD_SIZE

// Check if the boat can be placed on the field (no other boat is adjacent/collided)
bool RandomPlacement::is_placement_available(const PlacementInfo& field, const CoordsSeq& boat) const
{
	FieldCoords c1;
	FieldCoords c2;
	boat.get_margin(c1, c2);

	for (unsigned int xp = c1._x; xp <= c2._x; ++xp) {
		for (unsigned int yp = c1._y; yp <= c2._y; ++yp) {
			if (field._info[xp][yp]) return false;
		}
	}
	return true;
}

bool RandomPlacement::put_boat(PlacementInfo& field, unsigned int size)
{
	const unsigned int max_tries = 1000;
	for (unsigned int tries = 0; tries < max_tries; ++tries) {	
		// choose proper range for horizontal placement
		unsigned int x = random() % (FIELD_SIZE - size + 1);
		unsigned int y = random() % FIELD_SIZE;

		bool is_x_seq = (bool) (random() % 2);
		if (!is_x_seq) {
			std::swap(x, y);
		}
		FieldCoords head(x, y);
		FieldCoords tail(head);

		if (is_x_seq) {
			tail._x += (size - 1);
		} else {
			tail._y += (size - 1);
		}

		CoordsSeq boat(head, tail);
		if (!is_placement_available(field, boat)) continue;

		boat.mark_a_boat(field, true);

		return true;
	}

	// oops, too many retries failed... is it a bug ?
	return false;
}

bool RandomPlacement::get_placement(PlacementInfo& field)
{

	for (unsigned int size = 4; size > 0; --size) {
		const int boats_count = 5 - size;
		for (int i = 0; i<boats_count; ++i) {
			if (!put_boat(field, size)) {
				return false;
			}
		}
	}
	return true;
}
