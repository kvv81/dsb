#include "margined_field.h"

#include <cstdlib>	// for random()

// Get the first unknown cell (for backup plan)
bool MarginedField::get_first_unknown(FieldCoords& coords) const
{
	for (int y=0; y<FIELD_SIZE; ++y) {
		for (int x=0; x<FIELD_SIZE; ++x) {
			if (_field_m.get(x, y) == FPIM_UNKNOWN) {
				coords = FieldCoords(x, y);
				return true;
			}
		}
	}
	return false;
}

// check if the coords are in range and cell is still unknown
bool MarginedField::is_good_for_shot(const FieldCoords coords,
	signed int dx, signed int dy, FieldCoords& dst) const
{
	signed int x = coords._x + dx;
	signed int y = coords._y + dy;
	if (!_field_m.in_range(x, y)) return false;

	dst = FieldCoords((unsigned int) x, (unsigned int) y);
	return (_field_m.get(dst) == FPIM_UNKNOWN);
}

AlgoStepRes MarginedField::get_next_shorts_for_harmed_boat(FieldCoords& coords, FewFieldCoords& res) const
{
	// Must be called only after harming the boat (but not killing it completely)
	assert(_harm_counter>0);

	FieldCoords dst;

	if (_harm_counter == 1) {
		// the 1st harm, we have 4 cells to guess in general
		if (is_good_for_shot(_harmed_boat_head, -1, 0, dst)) res.push_back(dst);
		if (is_good_for_shot(_harmed_boat_head, 1, 0, dst)) res.push_back(dst);
		if (is_good_for_shot(_harmed_boat_head, 0, -1, dst)) res.push_back(dst);
		if (is_good_for_shot(_harmed_boat_head, 0, 1, dst)) res.push_back(dst);
	} else {
		// well, now we do see direction of the ship, we have only two cells to guess
		CoordsSeq boat_part(_harmed_boat_head, _harmed_boat_tail);
		if (boat_part.is_x_seq()) {
			FieldCoords head = boat_part.cur();
			if (is_good_for_shot(head, -1, 0, dst)) res.push_back(dst);
			FieldCoords tail = boat_part.tail();
			if (is_good_for_shot(tail, 1, 0, dst)) res.push_back(dst);
		} else {
			FieldCoords head = boat_part.cur();
			if (is_good_for_shot(head, 0, -1, dst)) res.push_back(dst);
			FieldCoords tail = boat_part.tail();
			if (is_good_for_shot(tail, 0, 1, dst)) res.push_back(dst);
		}
	}

	if (res.size() == 0) return ASR_FAILURE;
	return ASR_OK;
}

// Randomly choose any suitable short when the boat is harmed
AlgoStepRes MarginedField::get_next_short_for_harmed_boat(FieldCoords& coords, ShotHints* shot_hints) const
{
	FewFieldCoords ffc;
	AlgoStepRes res = get_next_shorts_for_harmed_boat(coords, ffc);
	if (res != ASR_OK) return res;

	assert(ffc.size() > 0);
	coords = ffc[random() % ffc.size()];

	if (shot_hints != NULL) {
		ShotHintData hint_data = {SH_COLORED, /* hint_number = */ 0, /* hint_color = */ 1};
		for (auto hint_coords : ffc) (*shot_hints)[hint_coords] = hint_data;
	}
	
	return ASR_OK;
}

// When bot is completely killed, all margin cells are marked appropriately + all harmed cels are re-marked as 'killed'
void MarginedField::mark_killed_boat()
{
	CoordsSeq boat(_harmed_boat_head, _harmed_boat_tail);

	FieldCoords c1;
	FieldCoords c2;
	boat.get_margin(c1, c2); // get corners of the margin

	// mark margin area on the field
	for (unsigned int x = c1._x; x <= c2._x; ++x) {
		for (unsigned int y = c1._y; y <= c2._y; ++y) {
			_field_m._info[x][y] = FPIM_MARGIN;
		}
	}

	// mark killed boat itself on the field
	boat.mark_a_boat(_field_m, FPIM_KILLED);
}

AlgoStepRes MarginedField::update_harmed_boat(const FieldCoords& coords)
{
	if (coords.is_hv_adjacent(_harmed_boat_head)) {
		_harmed_boat_head = coords;
	} else if (coords.is_hv_adjacent(_harmed_boat_tail)) {
		_harmed_boat_tail = coords;
	} else {
		return ASR_INTERNAL_ERROR;
	}
	return ASR_OK;
}

AlgoStepRes MarginedField::apply_shot_result(const FieldCoords& coords, ShotResult res)
{
	// Assume good behaviour for algorithm:
	// 1. No shooting the same point twice (MISSES/HARMED/KILLED)
	// 2. No shooting at prohibited locations (MARGIN)
	assert(_field_m._info[coords._x][coords._y] == FPIM_UNKNOWN);

	AlgoStepRes ret = ASR_INTERNAL_ERROR;
	if (res == SR_MISSED) {
		_field_m._info[coords._x][coords._y] = FPIM_MISSED;
		return ASR_OK;
	}

	// either SR_HARMED or SR_KILLED
	if (_harm_counter == 0) {
		_harmed_boat_head = _harmed_boat_tail = coords;	
	} else {
		// 2nd or 3rd harm of the big boat - update coords of tail/head
		if ((ret = update_harmed_boat(coords)) != ASR_OK) {
			return ret;
		}
	}
	switch (res) {
		case SR_HARMED:
			_harm_counter++;
			_field_m._info[coords._x][coords._y] = FPIM_HARMED;
			return ASR_OK;
		case SR_KILLED:
			_harm_counter = 0; // reset harming flow for case it was harmed before
			mark_killed_boat();
			return ASR_OK;
		default:
			break;
	}
	return ret;
}

