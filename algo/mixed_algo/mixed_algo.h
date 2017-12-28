#ifndef __MIXED_ALGO_H__
#define __MIXED_ALGO_H__

#include <algo/common/basic_algo.h>
#include <algo/random_algo/random_algo.h>
#include <algo/eclipsed_algo/eclipsed_algo.h>
#include <algo/field_mask_algo/field_mask_algo.h>

class MixedAlgo 
	: public EclipsedAlgo
	, public FieldMaskAlgo
{
public:
	MixedAlgo(const DSBAlgoGenricData& gdata)
		: BasicAlgo(gdata)
		, RandomAlgo(gdata)
		, EclipsedAlgo(gdata)
		, FieldMaskAlgo(gdata)
	{ }

	virtual std::string get_algo_name() const override { return "mixed"; }

	virtual DSBAlgoApi* clone(const DSBAlgoGenricData& gdata) const override
	{
		return new MixedAlgo(gdata);
	}

	virtual AlgoStepRes get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes get_next_shot(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes apply_shot_result(const FieldCoords& coords, ShotResult res) override
	{
		return BasicAlgo::apply_shot_result(coords, res);
	}
};

#endif // __MIXED_ALGO_H__
