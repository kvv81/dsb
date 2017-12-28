#include "field_mask_algo.h"

#include <iterator>	// for std::advance
#include <cstdlib>	// for random()

void FieldMaskAlgo::insert_tier_point(const FieldCoords& coords, unsigned short s)
{
	unsigned short sum = coords._x + coords._y + s;
	if ((sum%4) == 0) {
		_tier3_points.push_back(coords);
	} else {
		_tier2_points.push_back(coords);
	}
}

FieldMaskAlgo::FieldMaskAlgo(const DSBAlgoGenricData& gdata)
	: BasicAlgo(gdata)
	, RandomAlgo(gdata)
{
	for (unsigned short s=0; s<FIELD_SIZE; s+=3) {
		for (unsigned short x = s; x<FIELD_SIZE; x++) {
			FieldCoords coords1(x, x-s);
			insert_tier_point(coords1, s);

			if (s != 0) {
				FieldCoords coords2(x-s, x);
				insert_tier_point(coords2, s);
			}
		}
	}
}

void FieldMaskAlgo::fill_shot_hints(int hint_color, const FewFieldCoords& current_tier_coords, ShotHints& shot_hints)
{
	for ( auto coord : current_tier_coords) {
		ShotHintData data = { SH_COLORED, 0, hint_color };
		shot_hints[coord] = data;
	}
}

FewFieldCoords* FieldMaskAlgo::get_current_tier_points(int& tier)
{
	FewFieldCoords* ps = &_tier1_points;
	tier = 1;

	if (ps->size() == 0) {
		ps = &_tier2_points;
		tier = 2;
		if (ps->size() == 0) {
			tier = 3;
			ps = &_tier3_points;
		}
	}
	return ps;
}

AlgoStepRes FieldMaskAlgo::get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints)
{
	while (_tier1_points.size() + _tier2_points.size() + _tier3_points.size() > 0) {
		int hint_color = -1;
		FewFieldCoords* ps = get_current_tier_points(hint_color);
		assert(ps->size() > 0); // We have to find a point in some tier because the sum of sizes is greater than 0

		size_t i = random() % ps->size();
		if (_field_m._field_m.get((*ps)[i]) == FPIM_UNKNOWN) {
			if (shot_hints != NULL) {
				fill_shot_hints(hint_color, *ps, *shot_hints);
			}
			coords = (*ps)[i];
			return ASR_OK;
		}
		FewFieldCoords::iterator it = ps->begin();
		std::advance(it, i);
		ps->erase(it);
	}
	return ASR_NO_GUESS;

}

AlgoStepRes FieldMaskAlgo::get_next_shot(FieldCoords& coords, ShotHints* shot_hints)
{
	// Stage1: in case previous shot has harmed some ship we need to finish it
	if ( _field_m.has_harmed_boat()) {
		return _field_m.get_next_short_for_harmed_boat(coords, shot_hints);
	}

	// Stage2: no harmed boats - try shoot via field mask
	AlgoStepRes res = get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage3: field-mask is already shooted - try random
	res = RandomAlgo::get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage4: if random cannot find unknown cell for too long, bail to base dummy algo
	return BasicAlgo::get_next_shot_or_bail(coords, shot_hints);
}
