#ifndef __BASIC_ALGO_H__
#define __BASIC_ALGO_H__

#include <algo/api/dsb_algo_api.h>
#include <algo/common/margined_field.h>

class BasicAlgo 
	: public DSBAlgoApi
{
public:
	BasicAlgo(const DSBAlgoGenricData& gdata)
		: DSBAlgoApi(gdata)
	{ }

	// Basic algo defines the very primitive shooting algorithm and standard handling of margined field.
	// get_next_shot_or_bail() might be used by derived classes if more complicated logic
	// is not possible (for example eclipsed algorithm cannot find any eclipse any more OR field-mask algorithm has already shooted via whole mask).
	// apply_shot_result() can be re-defined if more actions are needed by specific algo

	virtual AlgoStepRes get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes get_next_shot(FieldCoords& coords, ShotHints* shot_hints) override;
	virtual AlgoStepRes apply_shot_result(const FieldCoords& coords, ShotResult res) override;
protected:
	MarginedField _field_m;
};

#endif // __BASIC_ALGO_H__
