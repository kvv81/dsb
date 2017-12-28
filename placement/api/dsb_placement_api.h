#ifndef __DSB_PLACEMENT_API_H__
#define __DSB_PLACEMENT_API_H__

#include <string>			// for std::string
#include <common/field.h>	// for PlacementInfo

class DSBPlacementApi {
public:
	virtual std::string get_placement_name() const = 0;

	virtual bool process_custom_params(const std::string& params)
	{
		return true; // Ignore custom params by default; derived placement classes can override this if params are needed
	}
	virtual std::string get_custom_params_usage() const
	{
		return std::string(); // empty string means "no usage"
	}

	virtual DSBPlacementApi* clone() const = 0;
	virtual bool get_placement(PlacementInfo& field) = 0;
	virtual ~DSBPlacementApi() {}
};

#endif // __DSB_PLACEMENT_API_H__

