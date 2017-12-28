template<class Element>
FieldBitmap::FieldBitmap(const FieldInfoTpl<Element>& field, bool is_transponate /* = false */)
{
	// fill top and bottom lines by zeroes
	for (int i=0; i<BORDER_EXTRA; i++) {
		_data[i] = _data[FIELD_SIZE+BORDER_EXTRA+i] = 0;
	}

	for (int y=0; y<FIELD_SIZE; y++) {
		FieldRow val = 0;
		for (int x=0; x<FIELD_SIZE; x++) {
			FieldCoords coord(x, y);
			if (is_transponate) {
				coord.transponate();
			}
			if (is_boat_cell(field.get(coord))) {
				val |= (1ULL << x);
			}
		}
		_data[y - FBC_MIN] = (val << BORDER_EXTRA);
	}
}

template<class Element>
void FieldBitmap::add_eclipse(const FieldInfoTpl<Element>& field, bool is_set_missed)
{
	for (signed int y=0; y<FIELD_SIZE; y++) {
		for (signed int x=0; x<FIELD_SIZE; x++) {
			Element e = field.get(x,y);
			if (is_boat_cell(e)) {
#if 1
				// Optimized flow to set three bits at once
				FieldRow one_point_mask = get_1point_mask(x);
				FieldRow threee_points_mask = one_point_mask | (one_point_mask>>1) | (one_point_mask<<1);
				set_row_mask(y-1, threee_points_mask);
				set_row_mask(y,   threee_points_mask);
				set_row_mask(y+1, threee_points_mask);
#else
				set(x-1, y-1);
				set(x-1, y);
				set(x-1, y+1);
				set(x, y-1);
				set(x, y);
				set(x, y+1);
				set(x+1, y-1);
				set(x+1, y);
				set(x+1, y+1);
#endif
			} else if (is_set_missed && is_missed_cell(e)) {
				set(x, y);
			}
		}
	}
}
