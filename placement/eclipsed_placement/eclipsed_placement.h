#ifndef __ECLIPSED_PLACEMENT_H__
#define __ECLIPSED_PLACEMENT_H__

#include <placement/api/dsb_placement_api.h>	// for DSBPlacementApi

class EclipsedPlacement
	: public DSBPlacementApi
{
public:
	EclipsedPlacement()
		: _max_eclipse_tradeoff(2)
		, _min_eclipse_trick(true)
	{ }

	virtual std::string get_placement_name() const
	{
		return "eclipsed";
	}
	virtual std::string get_custom_params_usage() const
	{
		std::string cpu_str("max_eclipse_tradeoff=<uint,default=");
		cpu_str += std::to_string(_max_eclipse_tradeoff);
		cpu_str += ">:min_eclipse_trick=<bool,default=";
		cpu_str += _min_eclipse_trick ? "true" : "false";
		cpu_str += '>';
		return cpu_str;
	}

	virtual DSBPlacementApi* clone() const
	{
		return new EclipsedPlacement(*this);
	}

	virtual bool process_custom_params(const std::string& params);
	virtual bool get_placement(PlacementInfo& field);

private:
	bool put_boat(PlacementInfo& field, unsigned int size, bool is_max_eclipse = true);

	unsigned short	_max_eclipse_tradeoff;
	bool			_min_eclipse_trick;
};

#endif // __ECLIPSED_PLACEMENT_H__
