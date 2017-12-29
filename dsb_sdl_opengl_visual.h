#ifndef _DSB_SDL_OPENGL_VISUAL_H__
#define _DSB_SDL_OPENGL_VISUAL_H__

// SDL-OpenGL visal routines
bool dsb_sdl_opengl_visual_init(const std::string &placement_name, const std::string &algo_name);
bool dsb_sdl_opengl_visual_process_events();

void dsb_sdl_opengl_visual_show_next_shot(const PlacementInfo& field,
	const DSBAlgoGenricData& gdata, bool is_combined,
	const ShotHints* shot_hints, const FieldCoords* coords = NULL, ShotResult sres = SR_MISSED);

bool dsb_sdl_opengl_visual_pause();
void dsb_sdl_opengl_visual_cleanup();

#endif // _DSB_SDL_OPENGL_VISUAL_H__
