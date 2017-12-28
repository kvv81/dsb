#ifndef __DUMMY_ALGO_H__
#define __DUMMY_ALGO_H__

#include <algo/common/basic_algo.h>

class DummyAlgo 
	: public virtual BasicAlgo
{
public:
	DummyAlgo(const DSBAlgoGenricData& gdata)
		: BasicAlgo(gdata)
	{ }

	virtual std::string get_algo_name() const override { return "dummy"; }

	virtual DSBAlgoApi* clone(const DSBAlgoGenricData& gdata) const override
	{
		return new DummyAlgo(gdata);
	}


	// DummyAlgo does not improve BasicAlgo so get_next_shot/apply_shot_result is called from the parent class
};

#endif // __DUMMY_ALGO_H__
