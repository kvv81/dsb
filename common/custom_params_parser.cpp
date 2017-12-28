#include "custom_params_parser.h"

template<>
void CustomParamsParser::fix_istream(std::istringstream &ss, bool& value)
{
	ss >> std::boolalpha;
}

