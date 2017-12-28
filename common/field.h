#ifndef __FIELD_H__
#define __FIELD_H__
#include <iostream>
#include <cstring>	// for memset()
#include <cassert>	// for assert()

#include "coords.h" // for FIELD_SIZE

enum FieldPosInfo { FPI_UNKNOWN = 0, FPI_MISSED, FPI_HARMED, FPI_KILLED };

// Here we use template because custom algorithms can use similar concept but modified element type
template<typename Element>
struct FieldInfoTpl {
	typedef Element value_type;

	FieldInfoTpl(Element init_value = (Element) 0)
	{
		memset(&_info[0][0], (int) init_value, sizeof(_info));
	}

	Element get(unsigned int x, unsigned int y) const
	{
		assert(x<FIELD_SIZE);
		assert(y<FIELD_SIZE);

		return _info[x][y];
	}

	Element get_or_bail(signed int x, signed int y, Element default_value = 0) const
	{
		if (x >= 0 && x < FIELD_SIZE && y >= 0 && y < FIELD_SIZE) {
			return _info[x][y];
		}
		return default_value;
	}

	Element get(const FieldCoords& coords) const
	{
		return get(coords._x, coords._y);
	}

	bool in_range(signed int x, signed int y) const
	{
		bool is_outsize = (x<0 || y<0 || x>=FIELD_SIZE || y>=FIELD_SIZE);
		return !is_outsize;
	}

	void sum(const FieldInfoTpl<Element>& add_source, signed int factor, Element& mix_value, Element& max_value, bool is_source_transponated = false)
	{
		// TODO: optimize the sum of single row using SSE
		if (is_source_transponated) {
			for (int y=0; y<FIELD_SIZE; y++) {
				for (int x=0;x<FIELD_SIZE; x++) {
					Element& d = _info[x][y];
					d += add_source._info[y][x] * factor;
					if (mix_value > d) mix_value = d;
					if (max_value < d) max_value = d;
				}
			}
		} else {
			for (int y=0; y<FIELD_SIZE; y++) {
				for (int x=0;x<FIELD_SIZE; x++) {
					Element& d = _info[y][x];
					d += add_source._info[y][x] * factor;
					if (mix_value > d) mix_value = d;
					if (max_value < d) max_value = d;
				}
			}
		}
	}

	void dump()
	{
		//std::cout << "\n";
		for (int y=0; y<FIELD_SIZE; y++) {
			for (int x=0;x<FIELD_SIZE; x++) {
				std::cout << _info[x][y] << "\t";
			}
			std::cout << "\n";
		}
	}
	
	Element _info[FIELD_SIZE][FIELD_SIZE];
};

typedef FieldInfoTpl<FieldPosInfo> FieldInfo;
typedef FieldInfoTpl<bool> PlacementInfo;
typedef FieldInfoTpl<signed short> PositionScore;
	
inline bool is_boat_cell(FieldPosInfo fpi_cell)
{
	return ( (fpi_cell == FPI_HARMED) || ( fpi_cell == FPI_KILLED) );
}

inline bool is_boat_cell(bool placement_cell)
{
	return placement_cell;
}

inline bool is_missed_cell(bool)
{
	return false;
}

inline bool is_missed_cell(FieldPosInfo fpi_cell)
{
	return (fpi_cell == FPI_MISSED);
}

#endif // __FIELD_H__
