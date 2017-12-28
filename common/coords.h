#ifndef __COORDS_H__
#define __COORDS_H__

#include <cassert>	// for assert()

// Now we can handle filed size up to 58
#define FIELD_SIZE 10

// Coords are defined in range [0..FIELD_SIZE-1]
struct FieldCoords {
	unsigned int _x;
	unsigned int _y;
	
	FieldCoords()
		: _x(0)
		, _y(0)
	{ }

	FieldCoords(unsigned int x, unsigned int y)
		: _x(x)
		, _y(y)
	{
		assert(_x<FIELD_SIZE);
		assert(_y<FIELD_SIZE);
	}

	// Detects if other coords are either horizontally or vertically adjacent to current coords
	// (4 possible positions froming a cross)
	// The same coord as current doesn match
	bool is_hv_adjacent(const FieldCoords& c2) const
	{
		if (_x == c2._x && _y == c2._y+1) return true;
		if (_x == c2._x && _y == c2._y-1) return true;
		if (_x == c2._x+1 && _y == c2._y) return true;
		if (_x == c2._x-1 && _y == c2._y) return true;

		return false;
	}

	void transponate()
	{
		unsigned int tmp = _x;
		_x = _y;
		_y = tmp;
	}

	bool operator<(const FieldCoords& b) const {
		if (_y < b._y) return true;
		if (_y > b._y) return false;
		return (_x < b._x);
	}
};

// Coords sequence can be either vertical or horizontal
// Initialization makes the head as upper/left point and tail as bottom/right point.
class CoordsSeq {
private:
	FieldCoords		_cur;
	bool			_is_x_seq;
	unsigned int	_last_coord; // last x or last y coord of the sequence (depends on _is_x_seq)
public:
	CoordsSeq(const FieldCoords& a, const FieldCoords& b)
	{
		if (a._x == b._x) {	// Vertical sequence?
			_is_x_seq = false;
			if (a._y < b._y) {
				_cur = a;
				_last_coord = b._y;
			} else {
				_cur = b;
				_last_coord = a._y;
			}
		} else if (a._y == b._y) { // Horizontal sequence
			_is_x_seq = true;
			if (a._x < b._x) {
				_cur = a;
				_last_coord = b._x;
			} else {
				_cur = b;
				_last_coord = a._x;
			}
		} else { // OOps?
			assert ((a._x == b._x) || (a._y == b._y));
		}
	}

	CoordsSeq(const FieldCoords& a, bool is_x_seq, unsigned int size)
		: _cur(a)
		, _is_x_seq(is_x_seq)
		, _last_coord((is_x_seq ? a._x : a._y) + size - 1)
	{ }

#if 0
	CoordsSeq(unsigned int x, unsigned int y, unsigned int size)
		: _cur(x, y)
		, _is_x_seq(true)
		, _last_coord(x+size-1)
	{ }
#endif

	const FieldCoords& cur() const
	{
		return _cur;
	}


	FieldCoords tail() const
	{
		if (_is_x_seq) {
			return FieldCoords(_last_coord, _cur._y);
		}
		return FieldCoords(_cur._x, _last_coord);
	}

	const FieldCoords& next() 
	{
		if (_is_x_seq) {
			++_cur._x;
		} else {
			++_cur._y;
		}
		return _cur;
	}

	// Complement to next() method to verify the end of sequence
	bool is_outside() const
	{
		if (_is_x_seq) {
			return _cur._x > _last_coord;
		}
		return _cur._y > _last_coord;
	}

	bool is_x_seq() const
	{
		return _is_x_seq;
	}

	// Get coords of the margin
	// If boat is near the border, margin is adjusted to be inside the field
	void get_margin(FieldCoords& a, FieldCoords& b) const
	{
		a = cur();
		b = tail();

		if (a._x>0) --a._x;
		if (a._y>0) --a._y;
		if (b._x<FIELD_SIZE-1) ++b._x;
		if (b._y<FIELD_SIZE-1) ++b._y;
	}

	template<class SomePlacementInfo, typename SomePlacementInfoValue>
	void mark_a_boat(SomePlacementInfo& pi, SomePlacementInfoValue val)
	{
		FieldCoords coord = cur();
		do {
			pi._info[coord._x][coord._y] = val;
			coord = next();
		} while (!is_outside());
	}
};


#endif // __COORDS_H__
