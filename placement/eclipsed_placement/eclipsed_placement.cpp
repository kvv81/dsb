#include "eclipsed_placement.h"

#include <deque>
#include <utility>
#include <limits>

#include <common/coords.h>					// for FIELD_SIZE
#include <common/field_bitmap.h>			// for FieldBitmap
#include <common/custom_params_parser.h>	// for CustomParamsParser

constexpr PositionScore::value_type MAX_SCORE = std::numeric_limits<PositionScore::value_type>::max();

#define DEBUG 0 // Increment for debugging

bool EclipsedPlacement::process_custom_params(const std::string& params)
{
	CustomParamsParser p(params);
	unsigned short umax_eclipse_tradeoff = 0;
	if (p.parse_var("max_eclipse_tradeoff", umax_eclipse_tradeoff)) {
		_max_eclipse_tradeoff = static_cast<signed short>(umax_eclipse_tradeoff);
		std::cout << "EclipsedPlacement accepted custom parameter max_eclipse_tradeoff=" << _max_eclipse_tradeoff << std::endl;
	}

	if (p.parse_var("min_eclipse_trick", _min_eclipse_trick)) {
		std::cout << "EclipsedPlacement accepted custom parameter min_eclipse_trick=" << std::boolalpha << _min_eclipse_trick << std::endl;
	}
	
	return true;
}

static void
process_horizontal_boat(const FieldBitmap& eclipse, unsigned int size, PositionScore& score_map, bool is_transponated,
	signed short& min_score, signed short& max_score)
{
	FieldRow boat_mask = FieldBitmap::get_boat_initial_mask(size);
	FieldRow margin_mask = FieldBitmap::get_margin_initial_mask(size);

	max_score = 0;
	min_score = MAX_SCORE;

	for (int x=0; x < FIELD_SIZE-size+1; x++) {
		for (int y=0; y < FIELD_SIZE; y++) {
			signed short score = -1;
			if (!eclipse.is_intersected(y, boat_mask)) {
				signed short score = static_cast<signed short>(eclipse.popcount3(y, margin_mask));
				if (is_transponated) {
					score_map._info[y][x] = score;
				} else {
					score_map._info[x][y] = score;
				}
				if (score > max_score) {
					max_score = score;
				}
				if (score < min_score) {
					min_score = score;
				}
			} else {
/*
				if (is_transponated) {
					score_map._info[y][x] = -1;
				} else {
					score_map._info[x][y] = -1;
				}
*/
			}
		}

		boat_mask = boat_mask << 1;
		margin_mask = margin_mask << 1;
	}

#if 0 /* This step is not needed because we check out-of-bounds condition for placement outside */
	for (int x=FIELD_SIZE-size+1; x<FIELD_SIZE; x++) {
		for (int y=0; y < FIELD_SIZE; y++) {
			if (is_transponated) {
				score_map._info[y][x] = -1;
			} else {
				score_map._info[x][y] = -1;
			}
		}
	}
#endif

}

bool EclipsedPlacement::put_boat(PlacementInfo& field, unsigned int size, bool is_max_eclipse /* = true */)
{
	// Step 1 - process normal fields and horizontal boats
	FieldBitmap eclipse;
	eclipse.set_border();
	eclipse.add_eclipse(field);
#if DEBUG>1
	//eclipse.dump();
#endif

	FieldBitmap field_bmp(field, /* is_transponate = */ false);

	PositionScore score_map_h(-1);
	signed short max_score_h;
	signed short min_score_h;

	process_horizontal_boat(eclipse, size, score_map_h, /* is_transponated = */ false, min_score_h, max_score_h);

#if DEBUG>1
	std::cout << "Boat size = " << size << ", H score:\n";
	score_map_h.dump();
#endif

	std::unique_ptr<PositionScore> pscore_map_v; // WIll be used only for case size>1
	signed short max_score_v = 0;

	if (size > 1) {
		// Step 2 - process transponated fields + horizontal boats (equal to vertical boats)
		// For 1-sized boats no reason to do this because horizontal and vertial scores are equal
		FieldBitmap eclipse_t(/* is_init= */ false);
		eclipse_t.get_transponated(eclipse);

		signed short dummy_min_score_v;
		pscore_map_v.reset(new PositionScore(-1)); // Init by -1
		process_horizontal_boat(eclipse_t, size, *pscore_map_v, /* is_transponated = */ true, dummy_min_score_v, max_score_v);

#if DEBUG>1
		std::cout << "Boat size = " << size << ", V score:\n";
		pscore_map_v->dump();
#endif
	}

	typedef std::pair<FieldCoords,bool> BoatPlacement; // coords of top-left corner and is_vertical can describe the position of the boat of known size
	std::deque<BoatPlacement> best_eclipse;

	signed short max_score;
	signed short score_tradeoff;
	if (is_max_eclipse) {
		max_score = (max_score_h > max_score_v) ? max_score_h : max_score_v;

		// Allow small score trafe-off between the best eclipse and the more unpredictable position
		// The best placement (by eclipse) is quite predictable so it is very easy to kill all boats
		// By using trade-off factor we lower the eclipse ratio but more varations for placement are possible
		score_tradeoff = (max_score > _max_eclipse_tradeoff ? _max_eclipse_tradeoff : max_score);

		for (unsigned int y=0; y < FIELD_SIZE; y++) {
			for (unsigned int x=0; x < FIELD_SIZE; x++) {
				FieldCoords coord(x,y);

				/* For out-of-bounds placemnt, do not process the score at all (we does not set it, it might be zeroed or uninited) */
				if (x <= FIELD_SIZE - size) {
					if (score_map_h.get(coord) >= max_score-score_tradeoff) {
						best_eclipse.push_back(std::make_pair(coord, true));
					}
				}

				if (size > 1 && y<= FIELD_SIZE - size) {
					if (pscore_map_v->get(coord) >= max_score-score_tradeoff) {
						best_eclipse.push_back(std::make_pair(coord, false));
					}
				}
			}
		}
	} else { /* Min eclipse trick - search for cell with minimal eclipse instead */
		assert(size == 1); // code below can handle only 1-sized boat
		for (unsigned int y=0; y < FIELD_SIZE; y++) {
			for (unsigned int x=0; x < FIELD_SIZE; x++) {
				FieldCoords coord(x,y);
				if (score_map_h.get(coord) == min_score_h) {
					best_eclipse.push_back(std::make_pair(coord, true));
				}
			}
		}
	}

	size_t sz = best_eclipse.size();
	assert(sz > 0);

	BoatPlacement p = best_eclipse[random() % sz];
#if DEBUG>0
	if (is_max_eclipse) {
		std::cout << "Boat size=" << size << ", the best score = "<< max_score << ", score_tradeoff = " << score_tradeoff << ", amount of optimal placements=" << sz <<
			", choosen coord = (" << p.first._x << "," << p.first._y << ", is_horiz=" << p.second << ")\n"; 
	} else {
		std::cout << "The minimal score = "<< min_score_h << ", amount of minimal placements=" << sz << ", choosen coord = (" << p.first._x << "," << p.first._y << ")\n"; 
	}
#endif

	CoordsSeq boat(/* coord= */ p.first, /* is_horizontal = */ p.second, size);
	boat.mark_a_boat(field, true);

	return true;
}

// bool _min_eclipse_trick:
// Putting all boats with big/the best eclipse results in very predictable placement;
// it is quite easy to learn the way to kill all boats in few shots.
// To solve this issue, we can do the trick of placing 1-size boat in unexpected way (with minimal eclipse, in contrast to other boats with max eclipse).
// The effect of uneclipsed placement is not so big but will force the opponent to scan many uneclipsed cells before the finial kill.

bool EclipsedPlacement::get_placement(PlacementInfo& field)
{
	for (unsigned int size = 4; size > 0; --size) {
		const int boats_count = 5 - size;
		for (int i = 0; i<boats_count; ++i) {
			// Apply the trick for the last 1-size boat
			bool is_min_eclipse = ((_min_eclipse_trick) && (size == 1) && (i == 3));
			
			if (!put_boat(field, size, /* is_max_eclipse = */ !is_min_eclipse)) {
				return false;
			}
		}
	}

	return true;
}
