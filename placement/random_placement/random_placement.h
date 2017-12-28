#ifndef __RANDOM_PLACEMENT_H__
#define __RANDOM_PLACEMENT_H__

#include <placement/api/dsb_placement_api.h>	// for DSBPlacementApi
#include <common/all.h>							// for CoordsSeq

class RandomPlacement
	: public DSBPlacementApi
{
public:
	virtual std::string get_placement_name() const
	{
		return "random";
	}

	virtual DSBPlacementApi* clone() const
	{
		return new RandomPlacement();
	}
		
	virtual bool get_placement(PlacementInfo& field);
private:
	bool put_boat(PlacementInfo& field, unsigned int size);
	bool is_placement_available(const PlacementInfo& field, const CoordsSeq& boat) const;
};

#endif // __RANDOM_PLACEMENT_H__
