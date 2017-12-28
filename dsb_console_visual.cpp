#include <iostream> // for std:cout

#include "common/all.h"
#include "algo/api/dsb_algo_api.h"

void dsb_console_visual_show_next_shot(const PlacementInfo& field, const DSBAlgoGenricData& gdata,
	bool is_combined, const FieldCoords* coords = NULL)
{

	for (int i=0; i<FIELD_SIZE*2+2; ++i) {
		std::cout << "=";
	}
	std::cout << '\n';
	for (unsigned int y=0; y<FIELD_SIZE; ++y) {
		std::cout << "|";
		for (unsigned int x=0; x<FIELD_SIZE; ++x) {
			if (coords != NULL && coords->_x == x && coords->_y == y) {
				std::cout << "??";
			} else if (gdata._field.get(x, y) == FPI_UNKNOWN) {
				if (is_combined && field.get(x, y)) {
					std::cout << "**";
				} else {
					std::cout << "  ";
				}
			} else if (gdata._field.get(x, y) == FPI_MISSED) {
				std::cout << "..";
			} else if (gdata._field.get(x, y) == FPI_HARMED) {
				std::cout << "xx";
			} else {
				std::cout << "XX";
			}
		}
		std::cout << "|\n";
	}
	for (int i=0; i<FIELD_SIZE*2+2; ++i) {
		std::cout << "=";
	}
	std::cout << '\n';
	std::cout.flush();
}

