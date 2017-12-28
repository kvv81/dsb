#include "basic_algo.h"

AlgoStepRes BasicAlgo::get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints)
{
	if (!_field_m.get_first_unknown(coords)) {
		return ASR_FAILURE;
	}
	return ASR_OK;
}

AlgoStepRes BasicAlgo::get_next_shot(FieldCoords& coords, ShotHints* shot_hints)
{
	// in case previous shot has harmed some ship we need to finish it
	if ( _field_m.has_harmed_boat()) {
		return _field_m.get_next_short_for_harmed_boat(coords, shot_hints);
	}
	return get_next_shot_or_bail(coords, shot_hints);
}

AlgoStepRes BasicAlgo::apply_shot_result(const FieldCoords& coords, ShotResult res)
{
	if (res == SR_KILLED && _gdata._killed_boats == ALL_BOATS_COUNT) {
		return ASR_WON;
	}

	return _field_m.apply_shot_result(coords, res);
}

