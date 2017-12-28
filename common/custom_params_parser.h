#ifndef __CUSTOM_PARAMS_PARSER_H__
#define __CUSTOM_PARAMS_PARSER_H__

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

class CustomParamsParser {
private:
	const std::string _params;

public:
	CustomParamsParser(const std::string& params)
		: _params(params)
	{ }

	template<typename T>
	void fix_istream(std::istringstream &ss, T& value)
	{
	}

	template<typename T>
	bool parse_var(const std::string& var_name, T& value)
	{
		std::string pattern(var_name);
		pattern += '=';

		size_t spos = 0;
		while (1) {
			size_t pos = _params.find(pattern, spos);
			if (pos == std::string::npos) {
				return false;
			}
			// Prevent partial match by variable suffix, for example "my_var=1" fill match to 'var' 
			if (pos > 0 && _params[pos-1] != ':') {
				continue;
			}
			spos = pos + pattern.length();
			size_t pos2 = _params.find(':', spos);
			std::string value_str;
			if (pos2 == std::string::npos) {
				value_str = _params.substr(spos);
			} else {
				value_str = _params.substr(spos, pos2-spos);
			}

			std::istringstream ss(value_str);
			fix_istream(ss, value);
			//ss >> value;
			//if (ss.fail()) {
			if (!(ss >> value)) {
				std::cout << "Parsing error: " << var_name << "=" << value_str << " cannot be parsed!" << std::endl;
				return false;
			}
			return true;
			// return !ss.fail();
		}
		// return false;
	}
};

#endif // __CUSTOM_PARAMS_PARSER_H__
