#ifndef __RANDOM_ALGO_H__
#define __RANDOM_ALGO_H__

#include <algo/common/basic_algo.h>

class RandomAlgo 
	: public virtual BasicAlgo
{
public:
	RandomAlgo(const DSBAlgoGenricData& gdata)
		: BasicAlgo(gdata)
	{ }

	virtual std::string get_algo_name() const override { return "random"; }

	virtual DSBAlgoApi* clone(const DSBAlgoGenricData& gdata) const override
	{
		return new RandomAlgo(gdata);
	}

	virtual AlgoStepRes get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes get_next_shot(FieldCoords& coords, ShotHints* shot_hints) override;
};

#endif // __RANDOM_ALGO_H__
