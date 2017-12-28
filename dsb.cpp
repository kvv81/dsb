#include <SDL2/SDL.h>	// fo SDL_Delay

#include <memory>		// for std::unique_ptr
#include <thread>		// for std::thread
#include <atomic>		// for std::atomic
#include <vector>		// for std::vector
#include <string>		// for std::string
#include <iostream>		// for std:cout

#include <time.h>		// for time()
#include <cstdlib>		// for atoi()

#include "placement/random_placement/random_placement.h"
#include "placement/eclipsed_placement/eclipsed_placement.h"
#include "algo/random_algo/random_algo.h"
#include "algo/field_mask_algo/field_mask_algo.h"
#include "algo/eclipsed_algo/eclipsed_algo.h"
#include "algo/dummy_algo/dummy_algo.h"
#include "algo/mixed_algo/mixed_algo.h"
#include "dsb_sdl_opengl_visual.h"
#include "dsb_console_visual.h"

enum VisualEngine {VE_NONE=0, VE_CONSOLE_FULL, VE_CONSOLE_SHORT, VE_SDL_OPENGL};
enum VisualType {VT_RAW, VT_COMBINED/*, VT_SPLIT */};

static VisualEngine g_visual	= VE_SDL_OPENGL;
static VisualType	g_vtype		= VT_COMBINED;
static unsigned int g_seed		= time(NULL);
static unsigned int g_delay		= 100;
static unsigned int g_num		= 10000;

static RandomPlacement			g_rp;
static EclipsedPlacement		g_ep;
static std::string				g_placement(g_ep.get_placement_name());
static DSBPlacementApi* const	g_placement_repo[] = {&g_rp, &g_ep};

static const DSBAlgoGenricData	g_dummy_gdata; // Needed to construct Algos
static RandomAlgo				g_ra(g_dummy_gdata);
static FieldMaskAlgo			g_fma(g_dummy_gdata);
static EclipsedAlgo				g_ea(g_dummy_gdata);
static DummyAlgo				g_da(g_dummy_gdata);
static MixedAlgo				g_ma(g_dummy_gdata);
static std::string				g_algo(g_ma.get_algo_name());
static DSBAlgoApi* const		g_algo_repo[] = {&g_ra, &g_fma, &g_ea, &g_da, &g_ma};

static std::string				g_custom_params;

static int print_usage(char* argv0)
{
	std::cout << "Usage: " << argv0 << " [<options>]\n";
	std::cout << "\t--algo|-a <algo_name>         : use specified algorithm for shooting (default=" << g_algo << ")\n";
	std::cout << "\t--placement|-p <placement>    : use specified method of boats placement (default=" << g_placement << ")\n";
	std::cout << "\t--custom-params|-c <params>   : apply specified custom params to algo/placement (no params by default)\n";
	std::cout << "\t--visual|-v <visual_name>     : use specified visualization of the game progress (default=sdl_opengl)\n";
	std::cout << "\t--visual-type|-t <visual_type>: apply specified type of visualization (default=combined)\n";
	std::cout << "\t--num|-n <games_count>        : use specified amount of games to calculate statistics, "
		"0 for running till manual interruption (default=" << g_num << ")\n";
	std::cout << "\t--delay|-d <delay_ms>         : use specified delay between steps, in milliseconds. "
		"Not applicable for none/console_short visualization. (default=" << g_delay << ")\n";
	std::cout << "\t--seed|-s <seed>              : apply specified seed for random() algorithm before start\n";
	std::cout << "\n";

	std::cout << "Avaliable algo_names: ";
	for (size_t i = sizeof(g_algo_repo)/sizeof(g_algo_repo[0]); i>0; ) {
		std::cout << g_algo_repo[--i]->get_algo_name();
		std::cout << ( (i>0) ? ", " : "\n");
	}
	std::cout << "Avaliable placements: ";
	for (size_t i = sizeof(g_placement_repo)/sizeof(g_placement_repo[0]); i>0; ) {
		std::cout << g_placement_repo[--i]->get_placement_name();
		std::cout << ( (i>0) ? ", " : "\n");
	}
	std::cout << "Available visual_names: none, console_full, console_short, sdl_opengl\n";
	std::cout << "Available visual_types: combined, split, raw\n";

	// ----------------------------------------------------------------------------------
	std::cout << "Custom params for algos:\n";
	for (size_t i = sizeof(g_algo_repo)/sizeof(g_algo_repo[0]); i>0; ) {
		const DSBAlgoApi* algo = g_algo_repo[--i];
		std::string usage = algo->get_custom_params_usage();
		if (!usage.empty()) {
			std::cout << '\t' << algo->get_algo_name() << ": " << usage << std::endl;
		}
	}
	std::cout << "Custom params for placements:\n";
	for (size_t i = sizeof(g_placement_repo)/sizeof(g_placement_repo[0]); i>0; ) {
		const DSBPlacementApi* placemnt = g_placement_repo[--i];
		std::string usage = placemnt->get_custom_params_usage();
		if (!usage.empty()) {
			std::cout << '\t' << placemnt->get_placement_name() << ": " << usage << std::endl;
		}
	}

	return 1;
}

#define NEED_2ND_PARAM(OPT)											\
	if (i == argc-1) {												\
		std::cout << OPT " option needs aditional parameter!\n";	\
		return false;												\
	}
		
static bool parse_args(int argc, char* argv[])
{
	for (int i=1; i<argc; ++i) {
		std::string arg(argv[i]);
		if (arg == "--help" || arg == "-?" || arg == "-h" || arg == "/?") {
			std::cout << "Welcome to Deadly Sea Battle - AI competition!\n";
			return false;
		}

		if (arg == "--algo" || arg == "-a") {
			NEED_2ND_PARAM("--algo")
			g_algo = argv[++i];

			bool is_found = false;
			for (auto p_algo : g_algo_repo) {
				if (p_algo->get_algo_name() == g_algo) {
					is_found = true;
					break;
				}
			}
			if (!is_found) {
				std::cout << "Unsupported algo name '" << g_algo << "'\n";
				return false;
			}
		} else if (arg == "--placement" || arg == "-p") {
			NEED_2ND_PARAM("--placement")
			g_placement = argv[++i];

			bool is_found = false;
			for (auto p_placement : g_placement_repo) {
				if (p_placement->get_placement_name() == g_placement) {
					is_found = true;
					break;
				}
			}
			if (!is_found) {
				std::cout << "Unsupported placement name '" << g_placement << "'\n";
				return false;
			}
		} else if (arg == "--custom-params" || arg == "-c") {
			NEED_2ND_PARAM("--custom-params")
			if (!g_custom_params.empty()) {
				g_custom_params += ':';
			}
			g_custom_params += argv[++i];
		} else if (arg == "--visual" || arg == "-v") {
			NEED_2ND_PARAM("--visual")
			std::string v(argv[++i]);
			if ( v == "none") {
				g_visual = VE_NONE;
			} else if (v == "console_full") {
				g_visual = VE_CONSOLE_FULL;
			} else if (v == "console_short") {
				g_visual = VE_CONSOLE_SHORT;
			} else if (v == "sdl_opengl") {
				g_visual = VE_SDL_OPENGL;
			} else {
				std::cout << "Unsupported visual: " << v << '\n';
				return false;
			}
		} else if (arg == "--visual-type" || arg == "-t") {
			NEED_2ND_PARAM("--visual-type")
			std::string v(argv[++i]);
			if (v == "raw") {
				g_vtype = VT_RAW;
			} else if (v == "combined") {
				g_vtype = VT_COMBINED;
			} else {
				std::cout << "Unsupported visual-type: " << v << '\n';
				return false;
			}
		} else if (arg == "--delay" || arg == "-d") {
			NEED_2ND_PARAM("--delay")
			int delay = atoi(argv[++i]);
			if (delay >= 0) {
				g_delay = static_cast<unsigned int>(delay);
			}
		} else if (arg == "--num" || arg == "-n") {
			NEED_2ND_PARAM("--num")
			g_num = atoi(argv[++i]);
		} else if (arg == "--seed" || arg == "-s") {
			NEED_2ND_PARAM("--seed")
			g_seed = atoi(argv[++i]);
		} else {
			std::cout << "Unrecognized parameter: " << arg << "\n\n";
			return false;
		}
	}
	return true;
}

static ShotResult get_shot_res(const PlacementInfo& field, const FieldCoords& coords, DSBAlgoGenricData& gdata)
{
	// If no boat part at this cell then you missed...
	if (!field.get(coords)) {
		gdata._field._info[coords._x][coords._y] = FPI_MISSED;
		return SR_MISSED;
	}

// shortcut macro to write less code
#define RETURN_IF_ALIVE_BOARD_PART(X,Y)							\
	if (!field.get(X, Y)) break;								\
	if (gdata._field.get(X, Y) == FPI_UNKNOWN) {				\
		gdata._field._info[coords._x][coords._y] = FPI_HARMED;	\
		return SR_HARMED;							\
	}

	unsigned int harmed_cells = 0;
	// Try to search alive (non-harmed) boat part adjicent to harmed boat
	// Trace all 4 directions until end-of-boat or until unknown cell is found
	for (signed int x=((signed int) coords._x) - 1; x>((signed int)coords._x) - 4 && x>=0; --x) {
		unsigned int x2 = (unsigned int) x;
		RETURN_IF_ALIVE_BOARD_PART(x2, coords._y);
		harmed_cells++;
	}
	for (signed int y=((signed int) coords._y) - 1; y>((signed int)coords._y) - 4 && y>=0; --y) {
		unsigned int y2 = (unsigned int) y;
		RETURN_IF_ALIVE_BOARD_PART(coords._x, y2);
		harmed_cells++;
	}
	for (unsigned int x=coords._x + 1; x<coords._x+4 && x<FIELD_SIZE; ++x) {
		RETURN_IF_ALIVE_BOARD_PART(x, coords._y);
		harmed_cells++;
	}
	for (unsigned int y=coords._y + 1; y<coords._y+4 && y<FIELD_SIZE; ++y) {
		RETURN_IF_ALIVE_BOARD_PART(coords._x, y);
		harmed_cells++;
	}

	// if no alive cell is found then this boat is completely dead...
	gdata._field._info[coords._x][coords._y] = FPI_KILLED;
	gdata._killed_boats++;

	// Note - harmed_cells == size - 1 because focal cell (x,y) is not counted
	gdata._killed_boats_of_size[harmed_cells]++;

	int boats_count = 4 - harmed_cells;
	if (gdata._killed_boats_of_size[harmed_cells] > boats_count) {
		for (;;) ;
	}
	assert(gdata._killed_boats_of_size[harmed_cells] <= boats_count);

	return SR_KILLED;
}

static void show_next_shot(const PlacementInfo& field, const DSBAlgoGenricData& gdata,
	const ShotHints* shot_hints, const FieldCoords* coords = NULL, ShotResult sres = SR_MISSED)
{
	bool is_combined = (g_vtype == VT_COMBINED);

	if (g_visual == VE_CONSOLE_FULL) {
		dsb_console_visual_show_next_shot(field, gdata, is_combined, coords);
	} else if (g_visual == VE_SDL_OPENGL) {
		dsb_sdl_opengl_visual_show_next_shot(field, gdata, is_combined, shot_hints, coords, sres);
	}
}

static void show_field(const PlacementInfo& field, const DSBAlgoGenricData& gdata, const ShotHints* shot_hints, ShotResult sres = SR_MISSED)
{
	show_next_shot(field, gdata, shot_hints, NULL, sres);
}

constexpr unsigned int max_shots_per_game = FIELD_SIZE*FIELD_SIZE + 1;

// Single game process
static signed int play_one_game(const DSBAlgoApi* algo, const DSBPlacementApi* placement)
{
	PlacementInfo field;

	std::unique_ptr<DSBPlacementApi> p(placement->clone());
	if (!p->get_placement(field)) {
		std::cout << "Placement failure!" << std::endl;
		return -1;
	}

	DSBAlgoGenricData gdata;
	std::unique_ptr<DSBAlgoApi> a(algo->clone(gdata));

	AlgoStepRes res;
	
	// the worst game is to try each unknown cell
	// if algorithm shoots twice per cell, it is considered ill-crafted
	do {

		FieldCoords coords;
		std::unique_ptr<ShotHints> shot_hints;
		if (g_visual == VE_SDL_OPENGL) {
			shot_hints.reset(new ShotHints);
		}
		res = a->get_next_shot(coords, shot_hints.get());
		if (res != ASR_OK) {
			std::cout << "algo:" << algo->get_algo_name() << ": get_next_shot() returned err=" << (int) res << "\n";
			return -2;
		}

		if (g_visual != VE_NONE && g_visual != VE_CONSOLE_SHORT) {
			std::cout << "(" << coords._x << "," << coords._y << ")";
			if (g_visual == VE_CONSOLE_FULL) {
				std::cout << std::endl;
			} else {
				std::cout << "\t-\t" << std::flush;
			}
		}

		/* Visualization stage #1: show the field with hints from the algo, no fire position yet */
		show_field(field, gdata, shot_hints.get());
		if (g_visual != VE_NONE && g_visual != VE_CONSOLE_SHORT) {
			SDL_Delay(g_delay);
		}

		if (g_visual == VE_SDL_OPENGL) {
			if (!dsb_sdl_opengl_visual_process_events()) {
				return -1;
			}
		}

		/* Visualization stage #2: show the filed with hints and with current chosen fire position */
		show_next_shot(field, gdata, shot_hints.get(), &coords);

		if (g_visual != VE_NONE && g_visual != VE_CONSOLE_SHORT) {
			SDL_Delay(g_delay*2);
		}

		ShotResult sres = get_shot_res(field, coords, gdata);
		res = a->apply_shot_result(coords, sres);

		if (g_visual != VE_NONE && g_visual != VE_CONSOLE_SHORT) {
			switch (sres) {
				case SR_MISSED: std::cout << "missed...\n"; break;
				case SR_HARMED: std::cout << "Harmed!\n"; break;
				case SR_KILLED: std::cout << "KILLED!!!\n"; break;
			}
		}

		/* Visualization stage #3: show result of fire, hints are not drawn as not vaild any more (obsolete) */
		show_field(field, gdata, /*shot_hints.get()*/ NULL, sres);

		if (res != ASR_WON) {
			gdata._step_number++;
		}

		if (g_visual != VE_NONE && g_visual != VE_CONSOLE_SHORT) {
			unsigned int delay_factor = (sres == SR_MISSED) ? 1 : 3;
			SDL_Delay(g_delay*delay_factor);
		}
	} while (res != ASR_WON && gdata._step_number < max_shots_per_game);

	if (res == ASR_WON) {
		if (g_visual == VE_CONSOLE_SHORT) {
			dsb_console_visual_show_next_shot(field, gdata, NULL);
		}
		if (g_visual != VE_NONE) {
			std::cout << "Won with " << gdata._step_number << " shots!\n";

			const unsigned int game_over_factor = 20;
			SDL_Delay(g_delay*game_over_factor);
		}

		return gdata._step_number;
	}
	return -3;
}

// Wrapper to terminate and return/exit in one line
static int term(int res, bool do_exit = false)
{				
	if (g_visual == VE_SDL_OPENGL) {
		dsb_sdl_opengl_visual_cleanup();
	}
	if (do_exit) {
		exit(res);
	}
	return res;
}

// Game statistics to be updated by parallel threads (in sharing mode)
struct GameStats {
	std::atomic<unsigned long long> games_count;
	std::atomic<unsigned long long> total_shots;
	std::atomic<unsigned long long> shots_count[max_shots_per_game];

	GameStats()
		: games_count(0)
		, total_shots(0)
	{
		for (int i=0; i<max_shots_per_game; ++i) shots_count[i]=0;
	}
};

static void
run_games_func(const DSBAlgoApi* algo, const DSBPlacementApi* placement, GameStats& stats, unsigned int thread_games_count)
{
	unsigned int my_thread_games_count = 0;
	do {
		signed int shots = play_one_game(algo, placement);
		if (shots <= 0) {
			// We are in multi-threaded process, exit from one thread must terminate all the rest threads prematurely
			(void) term(shots, /* do_exit = */ true);
		}
		stats.total_shots += (unsigned int) shots;

		assert(shots <= max_shots_per_game);
		stats.shots_count[shots]++;

		stats.games_count++;
		my_thread_games_count++;
	} while (my_thread_games_count < thread_games_count || thread_games_count == 0);
	std::cout << "Thread has finished the work." << std::endl;
}

inline void print_summary(const GameStats& stats)
{
	std::cout << "*** Played " << stats.games_count << " games with total shots_count=" << stats.total_shots <<
		", \taverage=" << ((double)stats.total_shots)/stats.games_count << '\n';
}

// entry point
int main(int argc, char* argv[])
{
	// Statistics containter

	if (!parse_args(argc, argv)) {
		return print_usage(argv[0]);
	}

	//---------------------------------------------------------------------------------------
	DSBAlgoApi*			algo		= NULL; // those pointers will refer to global objects, don't need to be released
	DSBPlacementApi*	placement	= NULL;

	for (auto p_algo: g_algo_repo) {
		if (p_algo->get_algo_name() == g_algo) {
			algo = p_algo;
			break;
		}
	}
	assert(algo != NULL);	// validated by parse_args via g_algo_repo 

	for (auto p_placement : g_placement_repo) {
		if (p_placement->get_placement_name() == g_placement) {
			placement = p_placement;
			break;
		}
	}
	assert(placement != NULL);	// validated by parse_args via g_placement_repo

	if (g_visual == VE_NONE) {
		std::cout << "Using placement=" << placement->get_placement_name() << " and algo=" << algo->get_algo_name() << std::endl;
	}

	// In SDL_OpenGL mode placement and algo are printed in the title of the window;
	// in the silent mode we might not see used algo/placemnt if default values are used...
	// So print them explicitly
	if (!algo->process_custom_params(g_custom_params)) {
		std::cout << "Algo has failed to process custom params!" << std::endl;
		return -1;
	}

	if (!placement->process_custom_params(g_custom_params)) {
		std::cout << "Placement has failed to process custom params!" << std::endl;
		return -1;
	}

	//---------------------------------------------------------------------------------------
	srandom(g_seed);
	if (g_visual == VE_SDL_OPENGL) {
		if (!dsb_sdl_opengl_visual_init(placement->get_placement_name(), algo->get_algo_name())) {
			return -1;
		}
	} else if (g_visual == VE_NONE) {
		std::cout << "Playing " << g_num << " games in silent mode..." << std::endl;
	}
	//---------------------------------------------------------------------------------------
	// Main flow

	GameStats stats;
	if (g_visual == VE_NONE && g_num != 0) {
		// Try to use multy-threading
		unsigned int threads_num = std::thread::hardware_concurrency();
		if (threads_num == 0) {
			std::cerr << "Cannot detect number of CPUs, running in single-thread mode...!" << std::endl;
			threads_num = 1;
		} else {
			if (threads_num > 1) {
				std::cout << "Using multi-threading for silent mode, detected " << threads_num << " CPUs" << std::endl;
				if (g_num < threads_num) {
					// We have more CPUs than games requested, no need to load all CPUs ;-)
					threads_num = g_num;
				}
			}
		}
		std::vector<std::thread> threads;
		unsigned int run_games = 0;
		for (unsigned int i = threads_num; i--; ) {
			// The last thread can run more games than others (games count might be not the multiple of CPUs count)
			unsigned int this_thread_games_count = (i==0) ? (g_num - run_games) : (g_num / threads_num);
			// std::cout << "Running thread for " << this_thread_games_count << " games..." << std::endl;
			threads.push_back(std::thread(run_games_func, algo, placement, std::ref(stats), this_thread_games_count));
			run_games += this_thread_games_count;
		}
		assert(run_games == g_num); // Just to be on the safe side - we asked to run all games we wanted

		for (auto& th : threads) th.join(); // Wait for all threads to finish
	} else {
		// Do not use multithreading, run everything in current thread directly
		run_games_func(algo, placement, stats, g_num);
	}
	
	//---------------------------------------------------------------------------------------
	// Dump statistics diagram
		
	print_summary(stats);
	for (int i=0; i < max_shots_per_game; i++) {
		if (stats.shots_count[i] > 0) {
			std::cout << i << ' ';
			float percent = (stats.shots_count[i] * 100.0f) / stats.games_count;
			for (int j=0; j<(int)(10*percent); j++) {
				std::cout << '=';
			}
			std::cout << "> " << percent << "% (" << stats.shots_count[i] << ")\n";
		}
	}
	print_summary(stats);

	return term(0);
}
