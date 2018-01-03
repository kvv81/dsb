#include "eclipsed_algo.h"

#include <limits>	// for std::numeric_limits
#include <iterator>	// for std::advance
#include <cstdlib>	// for random()
#include <cmath>	// for ceilf

#include <common/coords.h>					// for FIELD_SIZE
#include <common/field_bitmap.h>			// for FieldBitmap

#define DEBUG 0 // Increment for debugging

constexpr PositionScore::value_type MAX_SCORE = std::numeric_limits<PositionScore::value_type>::max();

static void
process_horizontal_boat(const FieldBitmap& eclipse, const FieldBitmap& denied_pos,
	unsigned int size, PositionScore& score_map, bool is_transponated)
{
	FieldRow boat_mask = FieldBitmap::get_boat_initial_mask(size);
	FieldRow margin_mask = FieldBitmap::get_margin_initial_mask(size);

	for (int x=0; x < FIELD_SIZE-size+1; x++) {
		for (int y=0; y < FIELD_SIZE; y++) {
			if (denied_pos.is_intersected(y, boat_mask)) continue;

			signed short score = static_cast<signed short>(eclipse.popcount3(y, margin_mask));
			if (is_transponated) {
				for (int i=0; i<size; ++i) {
					score_map._info[y][x+i] += score;
				}
			} else {
				for (int i=0; i<size; ++i) {
					score_map._info[x+i][y] += score;
				}
			}
		}

		boat_mask = boat_mask << 1;
		margin_mask = margin_mask << 1;
	}
}

void EclipsedAlgo::get_score4boat(PositionScore& score_map, unsigned int size)
{
	// Step 1 - process normal fields and horizontal boats
	FieldBitmap eclipse;
	eclipse.set_border();
	eclipse.add_eclipse(_gdata._field, /* is_set_missed = */ false);
	FieldBitmap denied_pos;
	denied_pos.add_eclipse(_gdata._field, /* is_set_missed = */ true);

#if DEBUG>1
	//eclipse.dump();
#endif

	FieldBitmap field_bmp(_gdata._field, /* is_transponate = */ false);

	process_horizontal_boat(eclipse, denied_pos, size, score_map, /* is_transponated = */ false);

#if DEBUG>2
	std::cout << "Boat size = " << size << ", H score:\n";
	score_map.dump();
#endif

	if (size > 1) {
		// Step 2 - process transponated fields + horizontal boats (equal to vertical boats)
		FieldBitmap eclipse_t(/* is_init = */ false);
		eclipse_t.get_transponated(eclipse);

		FieldBitmap denied_pos_t(/* is_init = */ false);
		denied_pos_t.get_transponated(denied_pos);

		process_horizontal_boat(eclipse_t, denied_pos_t, size, score_map, /* is_transponated = */ true);

#if DEBUG>2
		std::cout << "Boat size = " << size << ", TOTAL score:\n";
		score_map.dump();
#endif
	}
}

void EclipsedAlgo::fill_shot_hints(const PositionScore& score_map, const FewFieldCoords& good_shots, ShotHints& shot_hints)
{
	for (int x = 0; x < FIELD_SIZE; ++x) {
		for (int y=0; y<FIELD_SIZE; ++y) {
			signed short score = score_map.get(x, y);
			if (score > 0) {
				FieldCoords coord(x, y);
				bool is_good = false;

				for (auto it=good_shots.begin(); it != good_shots.end(); ++it) {
					if (*it == coord) {
						is_good = true;
						break;
					}
				}
				ShotHintData data = { SH_NUMBERED, score, 0};
				if (is_good) {
					data.hint_flags = SH_COLORED_AND_NUMBERED;
					data.hint_color = 1;
				}
				shot_hints[coord] = data;
			}
		}
	}
}

AlgoStepRes EclipsedAlgo::get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints)
{
	signed short max_score = 0;
	signed short min_score = MAX_SCORE;

	// Try to get eclipse score of boats of each size (if we still have such boats alive)
	PositionScore score_map;
	for (unsigned int size = 4; size > 0; --size) {
		const signed int total_boats_count = 5 - size;
		const int killed_boats_count = _gdata._killed_boats_of_size[size-1];
		const signed int remained_boats = total_boats_count - killed_boats_count;
		assert(remained_boats >= 0);

		if (remained_boats > 0) {
			PositionScore score_map_of_size_x;
			get_score4boat(score_map_of_size_x, size);

			score_map.sum(score_map_of_size_x, /* factor = */ remained_boats, min_score, max_score);
		}
	}

#if DEBUG>1
	std::cout << "TOTAL score map:\n";
	score_map.dump();
#endif

	if (max_score > 0) {
		FewFieldCoords good_shots;
		float deviation = 0.02f; // allowed deviation from the best choice to be less predictible
		// calculate threshold of the score what is good enough to shoot
		signed int thr_score = ceilf((max_score-min_score)*(1.0f-deviation)) + min_score;
		assert(thr_score>0);

		for (int x = 0; x<FIELD_SIZE; ++x) {
			for (int y = 0; y<FIELD_SIZE; ++y) {
				if (score_map.get(x, y) >= thr_score) {
					FieldCoords cur_coord(x, y);
					good_shots.push_back(cur_coord);
				}
			}
		}

		if (shot_hints != NULL) {
			fill_shot_hints(score_map, good_shots, *shot_hints);
		}

		assert(good_shots.size()>0);
		coords = good_shots[random() % good_shots.size()];
#if DEBUG>0
		std::cout << "Eclipsed algo got min_score=" << min_score << ", max_score=" << max_score << ", thres=" << thr_score <<
			", coords within 'good enough' range: " << good_shots.size() << ", chosen coord=(" << coords._x << ',' << coords._y << ")\n";
#endif

		return ASR_OK;
	}

#if DEBUG>0
	std::cout << "Eclipsed algo has not found any eclipse profit, bailing to next algo...\n";
#endif

	return ASR_NO_GUESS;
}

AlgoStepRes EclipsedAlgo::get_next_shot(FieldCoords& coords, ShotHints* shot_hints)
{
	// Stage1: in case previous shot has harmed some ship we need to finish it
	if ( _field_m.has_harmed_boat()) {
		return _field_m.get_next_short_for_harmed_boat(coords, shot_hints);
	}

	// Stage2: no harmed boats - try shoot via field mask
	AlgoStepRes res = get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage3: no eclipse profit - try to find remained unknown cells randomly
	res = RandomAlgo::get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage4: if random cannot find unknown cell for too long, bail to base dummy algo
	return BasicAlgo::get_next_shot_or_bail(coords, shot_hints);
}

