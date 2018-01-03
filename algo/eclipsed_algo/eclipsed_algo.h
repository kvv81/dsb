#ifndef __ECLIPSED_ALGO_H__
#define __ECLIPSED_ALGO_H__ 

#include <algo/random_algo/random_algo.h>

class EclipsedAlgo 
	: public virtual RandomAlgo
{
public:
	EclipsedAlgo(const DSBAlgoGenricData& gdata)
		: BasicAlgo(gdata)
		, RandomAlgo(gdata)
	{ }

	virtual std::string get_algo_name() const override { return "eclipsed"; }

	virtual DSBAlgoApi* clone(const DSBAlgoGenricData& gdata) const override
	{
		return new EclipsedAlgo(gdata);
	}

	virtual AlgoStepRes get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes get_next_shot(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes apply_shot_result(const FieldCoords& coords, ShotResult res) override
	{
		return RandomAlgo::apply_shot_result(coords, res);
	}
private:
	void get_score4boat(PositionScore& score_map, unsigned int size);
	void fill_shot_hints(const PositionScore& score_map, const FewFieldCoords& good_shots, ShotHints& shot_hints);
};

#endif // __ECLIPSED_ALGO_H__
