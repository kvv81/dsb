#include <cassert>
#include <cstring>

#include "field_bitmap.h"

FieldBitmap::FieldBitmap(bool is_init /* = true */)
{
	if (is_init) {
		memset(&_data[0], 0, sizeof(_data));
	}
}

void FieldBitmap::set_border()
{
	// calcualte top and bottom border value 0011111111111100
	constexpr FieldRow t_and_b = ((1ULL << (FIELD_SIZE + 2)) - 1) << (BORDER_EXTRA-1);
	_data[-1-FBC_MIN] = _data[FIELD_SIZE-FBC_MIN] = t_and_b;

	// set left and right borders
	const FieldRow msk = get_1point_mask(-1) | get_1point_mask(FIELD_SIZE);
	for (int y=0; y<FIELD_SIZE; y++) {
#if 1
		// Set two bits at once
		set_row_mask(y, msk);
#else
		// Equal operation but via two operations
		set(-1, y);
		set(FIELD_SIZE, y);
#endif
	}
}

bool FieldBitmap::is_intersected(signed int y, FieldRow mask) const
{
	assert(y >= FBC_MIN && y <= FBC_MAX);

	return ((_data[y - FBC_MIN] & mask) != 0);
}

#ifdef __GNUC__	// For gcc compiler we use builtin __builtin_popcountll function
static_assert(sizeof(unsigned long long)==8, "unsigned long long data type is not 64-bit integral");
#define POPCOUNT64 __builtin_popcountll
#else
constexpr uint64_t m1  = 0x5555555555555555; // binary: 0101...
constexpr uint64_t m2  = 0x3333333333333333; // binary: 00110011..
constexpr uint64_t m4  = 0x0f0f0f0f0f0f0f0f; // binary:  4 zeros,  4 ones ...

inline int popcount64b(uint64_t x)
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
    x += x >>  8;  //put count of each 16 bits into their lowest 8 bits
    x += x >> 16;  //put count of each 32 bits into their lowest 8 bits
    x += x >> 32;  //put count of each 64 bits into their lowest 8 bits
    return x & 0x7f;
}
#define POPCOUNT64 popcount64b
#endif

int FieldBitmap::popcount3(signed int y, FieldRow mask) const
{
	assert(y > FBC_MIN && y < FBC_MAX);

	FieldRow v1 = (_data[y - FBC_MIN - 1] & mask);
	FieldRow v2 = (_data[y - FBC_MIN    ] & mask);
	FieldRow v3 = (_data[y - FBC_MIN + 1] & mask);

	// try to merge all three rows into single data type and get a popcount in one call
	// (if FIELD_SIZE is too large and three FieldRow values don't fit into single 64-bit value
	// then we should call popcount function few times instead)
#if (FIELD_SIZE + BORDER_EXTRA*2)*3 <= 64
	constexpr unsigned int FIELD_ROW_SIZE = (FIELD_SIZE + BORDER_EXTRA*2);
	unsigned long long v = v1;
	v |= (((unsigned long long)v2) << FIELD_ROW_SIZE);
	v |= (((unsigned long long)v3) << (FIELD_ROW_SIZE*2));
	return POPCOUNT64(v);
#elif FIELD_SIZE + BORDER_EXTRA*2 <= 64
	int res = POPCOUNT64((unsigned long long)v1);
	res += POPCOUNT64((unsigned long long)v2);
	res += POPCOUNT64((unsigned long long)v3);
	return res;
#else
#	error oops - Single FieldRow field doesnt fit in 64 bits, need to split on few parts
#endif
}

void FieldBitmap::set(signed int x, signed int y, bool is_set /* = true */)
{
	assert(x >= FBC_MIN && x <= FBC_MAX);
	assert(y >= FBC_MIN && y <= FBC_MAX);

	if (is_set) {
		// Set bit to 1
		_data[y - FBC_MIN] |= (1ULL << (x-FBC_MIN));
	} else {
		// Reset bit to 0
		_data[y - FBC_MIN] &= (~(1ULL << (x-FBC_MIN)));
	}
}

// Variant of set() function when the mask is specified by caller (several bist can be set)
void FieldBitmap::set_row_mask(signed int y, FieldRow mask, bool is_set /* = true */)
{
	assert(y >= FBC_MIN && y <= FBC_MAX);

	if (is_set) {
		// Set bits of mask to 1
		_data[y - FBC_MIN] |= mask;
	} else {
		// Reset bits of mask to 0
		_data[y - FBC_MIN] &= (~mask);
	}
}

int	FieldBitmap::get(signed int x, signed int y) const
{
	assert(x >= FBC_MIN && x <= FBC_MAX);
	assert(y >= FBC_MIN && y <= FBC_MAX);

	return (_data[y - FBC_MIN] & (1ULL << (x-FBC_MIN))) != 0;
}

void FieldBitmap::get_transponated(const FieldBitmap& src)
{
	// Using normalized coords y=0..MAX -> yn=0-FBC_MIN..MAX-FBC_MIN
	for (int yn = 0; yn < FIELD_SIZE+BORDER_EXTRA*2; yn++) {
		FieldRow val = 0;
		FieldRow msk = 1ULL << yn;
		for (int xn = 0; xn < FIELD_SIZE+BORDER_EXTRA*2; xn++) {
			if (src._data[xn] & msk) {
				val |= (1ULL << xn);
			}
		}

		_data[yn] = val;
	}
}

// Get representation mask of the boat of given size (if placed on the right-most position)
FieldRow FieldBitmap::get_boat_initial_mask(unsigned int size)
{
	assert(size > 0 && size <= 4);
	return ((1ULL << size) - 1) << BORDER_EXTRA;
}

FieldRow FieldBitmap::get_margin_initial_mask(unsigned int size)
{
	assert(size > 0 && size <= 4);
	return ((1ULL << (size+2)) - 1) << (BORDER_EXTRA-1);
}

FieldRow FieldBitmap::get_1point_mask(signed int x)
{
	assert(x >= FBC_MIN && x <= FBC_MAX);
	return (1ULL << (x-FBC_MIN));
}
