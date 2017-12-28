#ifndef __FIELD_MASK_ALGO_H__
#define __FIELD_MASK_ALGO_H__

#include <algo/random_algo/random_algo.h>

class FieldMaskAlgo 
	: public virtual RandomAlgo
{
public:
	FieldMaskAlgo(const DSBAlgoGenricData& gdata);

	virtual std::string get_algo_name() const override { return "field-mask"; }

	virtual DSBAlgoApi* clone(const DSBAlgoGenricData& gdata) const override 
	{
		return new FieldMaskAlgo(gdata);
	}

	virtual AlgoStepRes get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes get_next_shot(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes apply_shot_result(const FieldCoords& coords, ShotResult res) override
	{
		return RandomAlgo::apply_shot_result(coords, res);
	}
private:
	void insert_tier_point(const FieldCoords& coords, unsigned short s);
	void fill_shot_hints(int hint_color, const FewFieldCoords& current_tier_coords, ShotHints& shot_hints);
	FewFieldCoords* get_current_tier_points(int& tier);

	// Rank some part of points by tiers
	FewFieldCoords _tier1_points;
	FewFieldCoords _tier2_points;
	FewFieldCoords _tier3_points;
};

#endif // __FIELD_MASK_ALGO_H__
