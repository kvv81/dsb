#include "random_algo.h"

#include <cstdlib>	// for random()

#define DEBUG 0 // Increment for debugging

AlgoStepRes RandomAlgo::get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints)
{
	// Speed optimization: if we have too many unknown locations on the field, it is more efficient
	// to pick a random one and verify status (and NOT scanning all field)
	if (_gdata._step_number < FIELD_SIZE * FIELD_SIZE / 2) {
		// Factor 5 for all cells on the field
		const unsigned int max_tries = 5 * FIELD_SIZE * FIELD_SIZE;

		unsigned int tries = 0;
		do {
#if DEBUG > 0
			// Track amount of tries needed to locate unknown location
			std::cout << "*";
#endif
			if (tries > max_tries) {
				// bail if randomizer cannot find unknown cell for too long (do not stuck infinitely on placement error)
				return ASR_NO_GUESS;
			}
			coords._x = random() % FIELD_SIZE;
			coords._y = random() % FIELD_SIZE;
			++tries;
		} while (_field_m._field_m.get(coords) != FPIM_UNKNOWN);

		return ASR_OK;
	}

	// Ok, we have too little unknown cells, scan all filed an gather them
	std::deque<FieldCoords> unknown_cells;
	for (int y=0;y<FIELD_SIZE; y++) {
		for (int x=0; x<FIELD_SIZE; x++) {
			FieldCoords pt(x, y);
			if (_field_m._field_m.get(pt) == FPIM_UNKNOWN) {
				unknown_cells.push_back(pt);
			}
		}
	}

	if (unknown_cells.empty()) {
		// If we have no any unknown cells, this isome stupid mistake made earlier (all boats must be already killed)
		return ASR_FAILURE;
	}

	// Get a random location from the list of unknown cells
	coords = unknown_cells[random() % unknown_cells.size()];
	return ASR_OK;
}

AlgoStepRes RandomAlgo::get_next_shot(FieldCoords& coords, ShotHints* shot_hints)
{
	// Stage1: in case previous shot has harmed some ship we need to finish it
	if ( _field_m.has_harmed_boat()) {
		return _field_m.get_next_short_for_harmed_boat(coords, shot_hints);
	}

	// Stage2: no harmed boats - try to find next unknown cell randomly
	AlgoStepRes res = get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage3: if random cannot find unknown cell for too long, bail to base dummy algo
	return BasicAlgo::get_next_shot_or_bail(coords, shot_hints);
}
