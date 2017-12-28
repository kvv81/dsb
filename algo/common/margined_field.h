#ifndef __MARGINED_FILED_H__
#define __MARGINED_FILED_H__ 

#include <deque>
#include <cstdint>
#include <common/coords.h>	// for FieldCoords
#include <common/field.h>	// for FieldInfoTpl, values of FieldPosInfo

#include <algo/api/dsb_algo_api.h>	// for AlgoStepRes

// FieldPosInfoMargined extends FieldPosInfo by adding additional 'margin' type of cell
// where placement of ships is not possible due mandatory margin between ships
enum FieldPosInfoMargined {FPIM_UNKNOWN = FPI_UNKNOWN, FPIM_MISSED = FPI_MISSED, FPIM_HARMED = FPI_HARMED, FPIM_KILLED = FPI_KILLED, FPIM_MARGIN};

typedef std::deque<FieldCoords> FewFieldCoords;

class MarginedField
{
public:
	MarginedField()
		: _harm_counter(0)
	{ }

	bool has_harmed_boat() const
	{
		return (_harm_counter > 0);
	}

	bool get_first_unknown(FieldCoords& coords) const;
	AlgoStepRes get_next_short_for_harmed_boat(FieldCoords& coords, ShotHints* shot_hints) const;
	
	AlgoStepRes apply_shot_result(const FieldCoords& coords, ShotResult res);

	typedef FieldInfoTpl<FieldPosInfoMargined> FieldInfoMargined;
	FieldInfoMargined _field_m;

private:
	bool is_good_for_shot(const FieldCoords coords,
		signed int dx, signed int dy, FieldCoords& dst) const;
	AlgoStepRes get_next_shorts_for_harmed_boat(FieldCoords& coords, FewFieldCoords& res) const;
	AlgoStepRes update_harmed_boat(const FieldCoords& coords);
	void mark_killed_boat();

	unsigned int _harm_counter;
	FieldCoords _harmed_boat_head;
	FieldCoords _harmed_boat_tail;
};

#endif // __MARGINED_FILED_H__
