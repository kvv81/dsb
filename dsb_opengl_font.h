#ifndef __DSB_OPENGL_FONT_H__
#define __DSB_OPENGL_FONT_H__

#include <string>

#define DSB_OENGL_FONT_WIDTH	8
#define DSB_OENGL_FONT_HEIGHT	11

void dsb_opengl_init_raster_font();
void dsb_opengl_print_string(const std::string& s);


#endif // __DSB_OPENGL_FONT_H__
