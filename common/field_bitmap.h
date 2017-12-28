#ifndef __FIELD_BITMAP_H__
#define __FIELD_BITMAP_H__

#include <cstdint>

#include "coords.h" // for FIELD_SIZE
#include "field.h"	// for FieldInfoTpl, is_boat_cell

#define BORDER_EXTRA 3	// we use extra cells near the border to allow bit-wise operations in generic way
						// if we have 4-size boat as the larges then we need extra 3 cells beyond the border to allow masking
#define FBC_MIN (-1*BORDER_EXTRA) // Field bitmap min and max coords (both x and y)
#define FBC_MAX (FIELD_SIZE-1+BORDER_EXTRA)

#if FIELD_SIZE + BORDER_EXTRA*2 <= 16
typedef uint16_t FieldRow;	// 16 bits is enough to store 10 bits of field's row and 3 extra cells on both borders
#elif FIELD_SIZE + BORDER_EXTRA*2 <= 32
typedef uint32_t FieldRow;	// 32 bits is ok
#elif FIELD_SIZE + BORDER_EXTRA*2 <= 64
typedef uint64_t FieldRow;	// 64 bits as the exteme possibility
#else
#error Oops too large FIELD_SIZE cannot be handled by single integral FieldRow variable
#endif


class FieldBitmap {
public:
	FieldBitmap(bool is_init = true);

	template<class Element>
	FieldBitmap(const FieldInfoTpl<Element>& field, bool is_transponate = false);

	template<class Element>
	void add_eclipse(const FieldInfoTpl<Element>& field, bool is_set_missed = false);

	int			get(signed int x, signed int y) const;
	void		set(signed int x, signed int y, bool is_set = true);
	void		set_row_mask(signed int y, FieldRow mask, bool is_set = true);

	void		set_border();


	bool		is_intersected(signed int y, FieldRow mask) const;
	int			popcount3(signed int y, FieldRow mask) const;
	void		get_transponated(const FieldBitmap& src);

	static FieldRow	get_1point_mask(signed int x);
	static FieldRow get_boat_initial_mask(unsigned int size);
	static FieldRow get_margin_initial_mask(unsigned int size);
	
private:
	FieldRow _data[FIELD_SIZE+BORDER_EXTRA*2];
};

#include "field_bitmap.hpp"

#endif // __FIELD_BITMAP_H__
