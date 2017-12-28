#include "mixed_algo.h"

AlgoStepRes MixedAlgo::get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints)
{
	// Mixed algo has no own logic, do not reuse it!
	return ASR_NO_GUESS;
}

AlgoStepRes MixedAlgo::get_next_shot(FieldCoords& coords, ShotHints* shot_hints)
{
	// Stage1: in case previous shot has harmed some ship we need to finish it
	if ( _field_m.has_harmed_boat()) {
		return _field_m.get_next_short_for_harmed_boat(coords, shot_hints);
	}

	// Stage2: no harmed boats - try shoot via field mask
	AlgoStepRes res = FieldMaskAlgo::get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage3: field-mask is already shooted - try eclipsed algo
	res = EclipsedAlgo::get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage4: no eclipse - try random
	res = RandomAlgo::get_next_shot_or_bail(coords, shot_hints);
	if (res != ASR_NO_GUESS) return res;

	// Stage5: if random cannot find unknown cell for too long, bail to base dummy algo
	return BasicAlgo::get_next_shot_or_bail(coords, shot_hints);
}
