#ifndef __DSB_ALGO_API_H__
#define __DSB_ALGO_API_H__

#include <string>		// for std::string
#include <map>			// for std::map
#include <common/all.h> // for FieldInfo, FieldCoords

// amount of all boats on the filed (1 x size4 + 2 x size3 + 3 x size2 + 4 x size1)
#define ALL_BOATS_COUNT 10 

// Generic data of any algorithm, will be maintained by engine and avaliable as const object
struct DSBAlgoGenricData {
	unsigned int _step_number;	// turn number, starting from 1

	// Note - standard field does not mark margin of already killed ships (they will be still 'UNKNOWN')
	FieldInfo _field;

	unsigned int _killed_boats;				// amount of killed boats of all sizes
	unsigned int _killed_boats_of_size[4];	// amount of killed boats per each size

	DSBAlgoGenricData()
		: _step_number(1)
		, _killed_boats(0)
	{
		memset(&_killed_boats_of_size[0], 0, sizeof(_killed_boats_of_size));
	}
};

// Algo processing result:
// 'FAILURE' should be returned by algo in case of engine's unexpected behaviour, for example:
// -- if harmed ship has no anjacent 'UNKNOWN' cells and it is expected to be already killed;
// -- algo cannot find unknown cells any more (ships has no margin in between and algo doesn't expect any ships there);
// -- if all ships are already killed but algo is still called. 
// All such types of errors are engine's issues.
// 'INTERNAL ERROR' means algo internal error (algo can't continue) and the current game is terminated.
// 'ASR_NO_GUESS' is used by primitive stage of algo to report that this algorithm cannot guess any location of the boat
enum AlgoStepRes { ASR_OK, ASR_FAILURE, ASR_INTERNAL_ERROR, ASR_NO_GUESS, ASR_WON };

// Short result to apply by algo after each try
enum ShotResult { SR_MISSED, SR_HARMED, SR_KILLED };

enum ShotHint { SH_COLORED = 1, SH_NUMBERED = 2, SH_COLORED_AND_NUMBERED = 3 };
struct ShotHintData {
	int hint_flags; /* filled by ShotHint values */
	int hint_number;
	int hint_color;
};

typedef std::map<FieldCoords,ShotHintData> ShotHints;

// Interface of sea battle algorithm
class DSBAlgoApi {
public:
	DSBAlgoApi(const DSBAlgoGenricData& gdata)
		: _gdata(gdata)
	{ }

	virtual ~DSBAlgoApi() { }

	virtual std::string get_algo_name() const = 0;

	virtual bool process_custom_params(const std::string& params)
	{
		return true; // Ignore custom params by default; derived algos classes can override this if params are needed
	}
	virtual std::string get_custom_params_usage() const
	{
		return std::string(); // empty string means "no usage"
	}

	virtual DSBAlgoApi* clone(const DSBAlgoGenricData& gdata) const = 0;

	virtual AlgoStepRes get_next_shot(FieldCoords& coords, ShotHints* shot_hints) = 0;
	virtual AlgoStepRes get_next_shot_or_bail(FieldCoords& coords, ShotHints* shot_hints) = 0;

	virtual AlgoStepRes apply_shot_result(const FieldCoords& coords, ShotResult res) = 0;

protected:
	const DSBAlgoGenricData& _gdata;
};

#endif // __DSB_ALGO_API_H__
