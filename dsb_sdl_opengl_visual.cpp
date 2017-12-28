// C++ Headers
#include <string>
#include <iostream>

// OpenGL / glew Headers
#define GL3_PROTOTYPES 1
#include <GL/glew.h>

// SDL2 Headers
#include <SDL2/SDL.h>

#include "common/all.h"
#include "algo/api/dsb_algo_api.h"
#include "dsb_opengl_font.h"

// Our SDL_Window (just like with SDL2 wihout OpenGL)
static SDL_Window *g_mainWindow;

// Our opengl context handle
static SDL_GLContext g_mainContext;

static bool g_is_paused = false;

static void check_SDL_error(int line = -1)
{
	std::string error = SDL_GetError();

	if (error != "") {
		std::cout << "SLD Error : " << error << std::endl;

		if (line != -1) {
			std::cout << "\nLine : " << line << std::endl;
		}

		SDL_ClearError();
	}
}

static bool set_opengl_attributes()
{
	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	return true;
}

void print_SDL_GL_attributes()
{
	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION : " << value << std::endl;

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;
}

// Icon - created by GIMP (RGBA, 32-bit color 32x32) and exported to .C
static char g_icon[32*32*4+1] = {
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\243\243\377j\243\243\377\312\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\312\243\243\377j\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\7\0\0\0""5"
	"\0\0\0x\0\0\0W\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\243"
	"\243\377a\243\243\377\336\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\240\240"
	"\373\336\211\211\326l\0\0\0*\0\0\0U\0\0\0\263\0\0\0\363\0\0\0\311\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\243\243\377\22\243\243\377\253\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\241\241\374\377\230\230\356\377\204\204\317"
	"\377]]\222\377--G\340\2\2\3\327\0\0\0\274\0\0\0\330\0\0\0\203\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\243\243\3772\243\243\377\331\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\236\236\370\377\205\205\321\377]]\222\377>>a\377((@\377"
	"??d\377VV\206\351\16\16\27\272\0\0\0\241\0\0\0""2\0\0\0\0\0\0\0\0\0\0\0\0"
	"\243\243\3775\243\243\377\353\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\236\236\370\377\212"
	"\212\330\377ll\252\377@@e\377((@\377AAf\377qq\261\377\207\207\324\377nn\255"
	"\377&&<\372##7\213\0\0\0\21\0\0\0\0\0\0\0\0\243\243\377\33\243\243\377\337"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\236\236\370\377\212\212\330\377ll\252\377>>a\377\34\34,\377>>b\377"
	"qq\261\377\213\213\332\377\235\235\367\377\213\213\332\377AAf\377>>a\377"
	"\177\177\307\345\222\222\345\36\0\0\0\0\0\0\0\0\243\243\377\267\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\241\241\374\377\225"
	"\225\352\377ss\265\377>>b\377\34\34,\377==`\377ll\251\377\211\211\327\377"
	"\235\235\367\377\242\242\376\377\236\236\370\377rr\263\377((@\377]]\222\377"
	"\230\230\356\377\243\243\377\267\0\0\0\0\243\243\377q\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\236\236\370\377\204\204\320"
	"\377XX\213\377++D\377??d\377ll\251\377\211\211\327\377\235\235\367\377\242"
	"\242\376\377\243\243\377\377\243\243\377\377\212\212\330\377>>b\377??d\377"
	"\204\204\320\377\241\241\374\377\243\243\377\377\243\243\377\16\243\243\377"
	"\360\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\241\241\374\377\224\224\350\377mm\254"
	"\377@@e\377++D\377XX\213\377\204\204\320\377\236\236\370\377\242\242\376"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\236\236\370\377ll\252"
	"\377\34\34,\377ll\252\377\236\236\370\377\243\243\377\377\243\243\377\377"
	"\243\243\377~\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\242\242\376\377\242\242\376\377\242\242\376\377\242"
	"\242\376\377\242\242\376\377\242\242\376\377\237\237\371\377\204\204\320"
	"\377UU\205\377\35\35.\377>>b\377ss\265\377\225\225\352\377\240\240\373\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\212\212\330\377>>a\377==`\377\211\211\327\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\337\243\243\377\377\243\243\377"
	"\377\241\241\374\377\230\230\356\377\212\212\330\377~~\306\377||\303\377"
	"||\303\377||\303\377||\303\377{{\301\377ll\252\377AAf\377--G\377ff\240\377"
	"\210\210\326\377\235\235\367\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\236\236\370\377"
	"ll\252\377\34\34,\377ll\252\377\236\236\370\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\237\237\371\377\205"
	"\205\321\377]]\222\377==`\377&&<\377$$9\377$$9\377$$9\377##8\377\35\35.\377"
	"\32\32)\377::[\377uu\270\377\233\233\364\377\242\242\376\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\241\241\374\377\204\204\320\377@@e\377>>b\377\211\211\327\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\237\237\371\377zz\300\377::\\\377''>\377>>a\377WW\211\377]]\222"
	"\377]]\222\377ZZ\216\377HHq\377!!5\377\21\21\34\377[[\217\377\206\206\323"
	"\377\226\226\353\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\225\225\352"
	"\377XX\213\377++D\377ss\265\377\236\236\370\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\237\237\371\377}}\305\377>>b\377''>"
	"\377ee\237\377\211\211\327\377\225\225\352\377\230\230\356\377\224\224\350"
	"\377vv\272\377AAg\377>>b\377cc\234\377zz\300\377__\225\377kk\250\377\225"
	"\225\352\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\236\236\370\377ss\265\377++D\377XX\212\377\224\224\351"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\205\205\321\377AAg\377CCi\377qq\261\377\232\232\361\377\242"
	"\242\376\377\243\243\377\377\237\237\371\377}}\305\377EEl\377EEl\377~~\306"
	"\377\235\235\366\377\204\204\317\377OO|\377AAg\377pp\260\377\236\236\370"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\241\241\374\377\204"
	"\204\320\377AAf\377AAf\377\204\204\320\377\241\241\374\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377]]\222\377"
	"((@\377rr\263\377\235\235\366\377\243\243\377\377\243\243\377\377\237\237"
	"\371\377~~\306\377CCj\377==`\377qq\261\377\230\230\357\377\243\243\377\377"
	"\237\237\372\377\177\177\307\377KKv\377CCj\377\177\177\310\377\237\237\372"
	"\377\243\243\377\377\243\243\377\377\223\223\347\377UU\205\377--G\377tt\267"
	"\377\236\236\370\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377;;]\377??d\377\213\213\332\377"
	"\243\243\377\377\243\243\377\377\237\237\371\377~~\306\377EEm\377==a\377"
	"EEl\377IIs\377ll\252\377\224\224\350\377\243\243\377\377\235\235\367\377"
	"zz\300\377@@e\377HHq\377\216\216\337\377\242\242\376\377\231\231\360\377"
	"]]\223\377\33\33+\377ee\237\377\233\233\363\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\35\35.\377[[\217\377\227\227\355\377\243\243\377\377\237\237"
	"\371\377~~\306\377FFn\377EEm\377pp\260\377>>b\377//K\377BBh\377XX\213\377"
	"xx\274\377\231\231\360\377\232\232\362\377nn\255\37733P\377ZZ\215\377\213"
	"\213\332\377\\\\\221\377\27\27%\377))A\377\203\203\315\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377UU\206\377..I\377nn\255\377\227\227"
	"\355\377}}\305\377FFn\377EEm\377~~\306\377\225\225\353\377JJt\377AAf\377"
	"]]\222\377OO|\377<<^\377XX\213\377\177\177\307\377\206\206\322\377UU\205"
	"\377RR\201\377aa\231\377\35\35-\377\14\14\23\377DDk\377\223\223\347\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\216\216\337\377ZZ\216"
	"\37711N\377ZZ\216\377??c\377EEl\377~~\306\377\236\236\370\377\242\242\376"
	"\377\\\\\220\377HHq\377YY\214\377EEm\377++D\37777W\377RR\201\377xx\275\377"
	"yy\277\377XX\211\377==`\377//I\377\32\32)\377XX\213\377\230\230\356\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\241\241\374\377\222"
	"\222\345\377]]\222\377\30\30'\377\37\37""2\377qq\262\377\235\235\367\377"
	"\243\243\377\377\243\243\377\377ll\252\377;;]\37755S\377CCi\377XX\212\377"
	"aa\231\377SS\203\377__\225\377__\226\377++E\377AAg\377__\226\377##7\377]"
	"]\222\377\230\230\356\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\241\241\374\377\216\216\337\377HHr\377\20\20\32\37733P\377\212\212"
	"\331\377\243\243\377\377\243\243\377\377\243\243\377\377~~\306\377;;]\377"
	"<<^\377pp\260\377\235\235\367\377\236\236\370\377vv\272\37799Z\377\34\34"
	"-\377TT\204\377\203\203\316\377xx\274\377$$9\377]]\222\377\230\230\356\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\214\214\333\343"
	"YY\214\377..H\377\\\\\221\377\205\205\321\377\237\237\371\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\212\212\331\377LLw\377JJu\377\201\201"
	"\312\377\233\233\364\377uu\270\377EEl\377CCi\377nn\255\377\224\224\350\377"
	"\237\237\372\377{{\301\377\"\"6\377``\227\377\232\232\361\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\37722O\301))A\377ll\251\377\232"
	"\232\362\377\242\242\376\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\227\227\355\377rr\263\377mm\254\377\202\202\314\377"
	"hh\243\377--G\377BBh\377~~\306\377\236\236\370\377\243\243\377\377\240\240"
	"\373\377qq\261\377\30\30&\377qq\261\377\240\240\373\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\1\1\2\333..I\372xx\274\377\237\237\371"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\240\240\373\377\220\220\342\377ww\273\377QQ\200\377\36\36""0"
	"\377TT\204\377\204\204\317\377\235\235\367\377\243\243\377\377\243\243\377"
	"\377\230\230\356\377ZZ\216\377%%:\377||\303\377\242\242\376\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\0\0\0\\**;\307QPc\377\211\210"
	"\311\377\241\241\373\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\236\236\370\377}}\305\377AAf\377,,F\377hh\243\377\223\223"
	"\347\377\240\240\373\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\210\210\325\377;;]\377??d\377\213\213\333\377\243\243\377\377\243\243\377"
	"\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\256\251*\33\223\217#~\223\217"
	"9\365ywt\377\225\225\342\377\243\243\377\377\243\243\377\377\237\237\371"
	"\377\207\207\324\377ff\240\377CCi\377EEl\377uu\270\377\233\233\364\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\233\233\364"
	"\377ff\241\377\35\35.\377mm\254\377\236\236\370\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\267\370\361<\214\365\356;\327"
	"\347\341:\347\211\2053\374sr\223\377\231\231\357\377\241\241\374\377\204"
	"\204\317\37733P\377\23\23\36\377..H\377xx\274\377\236\236\370\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\236\236\370\377"
	"uu\270\377--G\377UU\205\377\223\223\347\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\337\243\243\377\33\370\361<\370\370"
	"\361<\372\366\360;\366\351\342<\357kh3\374ll\242\377\216\216\337\377UU\205"
	"\377\27\27%\377\37\37""2\377\40\40""3\377mm\254\377\236\236\370\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\232\232\362\377qq\261\377CCi\377"
	"BBh\377\204\204\320\377\241\241\374\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377"
	"\377\243\243\377\353\243\243\3775\0\0\0\0\370\361<\376\370\361<\376\370\361"
	"<\370\365\357;\361\225\2214\300..=\364``\227\37722O\377dd\236\377oo\256\377"
	">>a\377CCi\377tt\266\377\224\224\350\377\232\232\361\377\232\232\361\377"
	"hh\244\377&&=\377>>b\377~~\306\377\237\237\371\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377"
	"\243\243\377\377\243\243\377\331\243\243\3772\0\0\0\0\0\0\0\0\370\361<\376"
	"\370\361<\377\370\361<\367\366\357;\300\213\207!W\10\10\13\234<<^\330xx\275"
	"\377\233\233\364\377\236\236\370\377}}\305\377BBh\377++D\377SS\202\377__"
	"\225\377``\227\377++D\377::\\\377zz\277\377\236\236\370\377\243\243\377\377"
	"\243\243\377\377\243\243\377\377\243\243\377\377\243\243\377\377\243\243"
	"\377\377\243\243\377\377\243\243\377\253\243\243\377\22\0\0\0\0\0\0\0\0\0"
	"\0\0\0\370\361<\375\370\361<\374\370\361<\355\370\361<g\205\201\40\15\0\0"
	"\0\"\0\0\0!\227\227\355e\243\243\377\336\243\243\377\377\235\235\367\377"
	"\204\204\317\377WW\211\377((?\377##7\377\33\33+\377TT\204\377\204\204\317"
	"\377\236\236\370\377\243\243\377\377\243\243\377\377\243\243\377\377\243"
	"\243\377\377\243\243\377\377\243\243\377\377\243\243\377\336\243\243\377"
	"a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
};

constexpr unsigned int g_default_window_width = 768;
constexpr unsigned int g_default_window_height = 768;

int g_cur_win_width = g_default_window_width;
int g_cur_win_height = g_default_window_height;

void print_number_centered(float x, float y, unsigned int num)
{
	glColor3f(0.7, 1.0, 0.7);

	std::string s = std::to_string(num);

	float x_center_offset = 2.0f * (DSB_OENGL_FONT_WIDTH/2.0f) * s.length() / g_cur_win_width;
	float y_center_offset = 2.0f * (DSB_OENGL_FONT_HEIGHT/2.0f) / g_cur_win_height;
	glRasterPos2f(x-x_center_offset, y-y_center_offset);

	dsb_opengl_print_string(s);
}

bool dsb_sdl_opengl_visual_init(const std::string &placement_name, const std::string &algo_name)
{

	// Initialize SDL's Video subsystem
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "Failed to init SDL\n";
		return false;
	}

	std::string title("Deadly Sea Battle - AI competition: ");
	title += placement_name + " placement, ";
	title += algo_name + " algo";

	// Create resizable centered window with pre-defined resolution
	g_mainWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		g_default_window_width, g_default_window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Check that everything worked out okay
	if (!g_mainWindow) {
		std::cout << "Unable to create window\n";
		check_SDL_error(__LINE__);
		return false;
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(g_icon, 32, 32, 32, 32*4, 0xff, 0xff00, 0xff0000, 0xff000000);
	if (surface != NULL) {
		// The icon is attached to the window pointer
		SDL_SetWindowIcon(g_mainWindow, surface);

		// std::cout << "Icon set!\n";

		// ...and the surface containing the icon pixel data is no longer required.
		SDL_FreeSurface(surface);
	}


	// Create our opengl context and attach it to our window
	g_mainContext = SDL_GL_CreateContext(g_mainWindow);
	
	set_opengl_attributes();

	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	// Init GLEW
	// Apparently, this is needed for Apple. Thanks to Ross Vander for letting me know
#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

#if 0
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 0, 10, 10, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
#endif

#if 0
	int count = SDL_GetNumAudioDevices(0);
	for (int i = 0; i < count; ++i) {
	    std::cout << "Audio device " << i << ": " << SDL_GetAudioDeviceName(i, 0) << std::endl;
	}
#endif

	dsb_opengl_init_raster_font();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(g_mainWindow);

	print_SDL_GL_attributes();
	return true;
}


bool dsb_sdl_opengl_visual_process_events()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				std::cout << "Asked to QUIT..." << std::endl;
				return false;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						std::cout << "Pressed Esc, QUITING..." << std::endl;
						return false;
					case SDLK_SPACE: {
						std::cout << "Paused, press SPACE again to continue..." << std::endl;

						bool is_paused = true;
						while (is_paused) {
							while (SDL_PollEvent(&event)) {
								if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
									is_paused = false;
								}
							}
							SDL_Delay(10);
						}
						break;
					}
					default:
						break;
				}
				break;
			case SDL_WINDOWEVENT: // update OpenGL after SDL window resize
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && event.window.windowID == SDL_GetWindowID(g_mainWindow)) {
					glViewport(0, 0, event.window.data1, event.window.data2);
					g_cur_win_width = event.window.data1;
					g_cur_win_height = event.window.data2;
				}
				break;
		}
	}
	return true;
}

// We assume that OpenGL viewport is -1.0f .. 1.0f for both GL x and y coords; size of field in OpenGL coords is 2.0f
#define HFSF (FIELD_SIZE/2.0f)	// Half Field Size Float
#define RHFSF (2.0f/FIELD_SIZE) // Reversed HFSF

struct GLCoords {
	float x;
	float y;
};

inline GLCoords get_gl_coords_center(unsigned int x, unsigned int y)
{
	GLCoords res;
	res.x = (x-HFSF+0.5f)*RHFSF;
	res.y = (HFSF-y-0.5f)*RHFSF;
	return res;
}

inline GLCoords get_gl_coords_top_left_corner(unsigned int x, unsigned int y)
{
	GLCoords res;
	res.x = (x-HFSF)*RHFSF;
	res.y = (HFSF-y)*RHFSF;
	return res;
}

static void draw_bombed_square(unsigned int x, unsigned int y, int hint_color = -1)
{

	if (hint_color == -1) {
		glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.8f, 0.8f);
	} else if (hint_color >= 1 && hint_color <= 3) {
		glBegin(GL_LINE_LOOP);
		float c = (3-hint_color) * 0.25f + 0.3f;
		glColor3f(c, c, 0.0f);
	}
 
	//const float radius = 0.1 * 0.5;
	const float radius = (1.0f / FIELD_SIZE) * 0.7f;
	GLCoords center = get_gl_coords_center(x, y);

	const int parts = 20;
	for (int i=0; i<parts; i++) {
		float r = (i*M_PI*2) / parts;
		glVertex2f(cosf(r)*radius + center.x, sinf(r)*radius + center.y);
	}

	glEnd();
}

enum SquareType {ST_HEALTH, ST_KILLED, ST_HARMED};
static void draw_boat_square(unsigned int x, unsigned int y, SquareType st)
{

	switch (st) {
		case ST_KILLED:
			glColor3f(1.0f, 0.4f, 0.4f);
		break;
		case ST_HARMED:
			glColor3f(1.0f, 0.7f, 0.5f);
		break;
		case ST_HEALTH:
			glColor3f(0.1f, 0.25f, 0.1f);
		break;
	}

	const float sz = RHFSF;
	GLCoords corner = get_gl_coords_top_left_corner(x, y);

	glBegin(GL_QUADS);
		glVertex2f(corner.x, corner.y);
		glVertex2f(corner.x, corner.y-sz);
		glVertex2f(corner.x+sz, corner.y-sz);
		glVertex2f(corner.x+sz, corner.y);
	glEnd();
}

static void draw_miss_square(unsigned int x, unsigned int y, bool is_prohibited)
{
	GLCoords center = get_gl_coords_center(x, y);

	if (is_prohibited) {
		const float d = (1.0f / FIELD_SIZE) * 0.3f;
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);
			glVertex2f(center.x-d, center.y-d);
			glVertex2f(center.x+d, center.y+d);
			glVertex2f(center.x-d, center.y+d);
			glVertex2f(center.x+d, center.y-d);
		glEnd();
	} else {
		glPointSize(6.0f);
		glColor3f(1.0f, 0.4f, 0.4f);

		glBegin(GL_POINTS);
			glVertex2f(center.x, center.y);
		glEnd();
	}
}

// Scans the field and check if harmed boat is only partially harmed or killed
static bool
check_if_boat_killed(const DSBAlgoGenricData& gdata, unsigned int x, unsigned int y)
{
	if (gdata._field.get(x, y) == FPI_KILLED) {
		return true;
	}

	signed int sx = x;
	signed int sy = y;

#define CHECK_FOR_PART()							\
	FieldPosInfo pi = gdata._field.get(sx, sy);		\
	if (pi == FPI_KILLED) {							\
		return true;								\
	}												\
	if (pi == FPI_MISSED || pi == FPI_UNKNOWN) {	\
		break;										\
	}

	while (--sx >= 0) {
		CHECK_FOR_PART()
	}

	sx = x;

	while (++sx < FIELD_SIZE) {
		CHECK_FOR_PART()
	}

	sx = x;


	while (--sy >= 0) {
		CHECK_FOR_PART()
	}

	sy = y;

	while (++sy < FIELD_SIZE) {
		CHECK_FOR_PART()
	}

	return false;
}

static bool
check_if_near_killed_boat(const DSBAlgoGenricData& gdata, signed int x, signed int y)
{
	FieldPosInfo v;

#define TEST_IF_KILLED_BOAT_CELL(X, Y)									\
	v = gdata._field.get_or_bail(X, Y, FPI_UNKNOWN);					\
	if (v == FPI_KILLED) {												\
		return true;													\
	} else if (v == FPI_HARMED && check_if_boat_killed(gdata, X, Y)) {	\
		return true;													\
	}

	TEST_IF_KILLED_BOAT_CELL(x-1, y-1)
	TEST_IF_KILLED_BOAT_CELL(x-1, y)
	TEST_IF_KILLED_BOAT_CELL(x-1, y+1)
	TEST_IF_KILLED_BOAT_CELL(x, y-1)
	TEST_IF_KILLED_BOAT_CELL(x, y+1)
	TEST_IF_KILLED_BOAT_CELL(x+1, y-1)
	TEST_IF_KILLED_BOAT_CELL(x+1, y)
	TEST_IF_KILLED_BOAT_CELL(x+1, y+1)

	return false;
}

void dsb_sdl_opengl_visual_show_next_shot(const PlacementInfo& field,
	const DSBAlgoGenricData& gdata, bool is_combined, const ShotHints* shot_hints,
	const FieldCoords* coords = NULL, ShotResult sres = SR_MISSED)
{
	if (sres == SR_HARMED) {
		glClearColor((random()*0.4f)/RAND_MAX, 0.0f, 0.0f, 1.0f);
	} else if (sres == SR_KILLED) {
		glClearColor(0.6f, 0.0f, 0.0f, 1.0f);
	} else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw a field FIELD_SIZE x FIELD_SIZE
	glLineWidth(2.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
		for (signed int i=0;i <= FIELD_SIZE; i++) {
			float f = (i-HFSF)*RHFSF;
			glVertex2f(f, -1.0f);
			glVertex2f(f, 1.0f);
			glVertex2f(-1.0f, f);
			glVertex2f(1.0f, f);
		}
	glEnd();

	// Fill cells
	for (signed int x=0; x<FIELD_SIZE; ++x) {
		for (signed int y=0; y<FIELD_SIZE; ++y) {
			bool is_ignore_hint = false;
			if (coords != NULL && coords->_x == x && coords->_y == y) {
				if (is_combined && field.get(x,y)) {
					draw_boat_square(x, y, ST_HEALTH);
					is_ignore_hint = true;
				}
				draw_bombed_square(x, y);
			} else if (gdata._field.get(x, y) == FPI_UNKNOWN) {
				if (field.get(x,y)) {
					if (is_combined) {
						draw_boat_square(x, y, ST_HEALTH);
					}
				} else {
					if (check_if_near_killed_boat(gdata, x,y)) {
						draw_miss_square(x, y, /* is_prohibited = */ true);
						is_ignore_hint = true;
					}
				}
			} else if (gdata._field.get(x, y) == FPI_MISSED) {
				draw_miss_square(x, y, /* is_prohibited = */ false);
				is_ignore_hint = true;
			} else {
				draw_boat_square(x, y,
					check_if_boat_killed(gdata, x,y) ? ST_KILLED : ST_HARMED
				);
				is_ignore_hint = true;
			}

			if (!is_ignore_hint && shot_hints != NULL) {
				FieldCoords coord(x, y);
				auto hint_it = shot_hints->find(coord);
				if (hint_it != shot_hints->end()) {
					if (hint_it->second.hint_flags & SH_COLORED) {
						draw_bombed_square(x, y, hint_it->second.hint_color);
					}

					if (hint_it->second.hint_flags & SH_NUMBERED) {
						GLCoords center = get_gl_coords_center(x, y);
						print_number_centered(center.x, center.y, hint_it->second.hint_number);
					}
				}
			}
		}
	}
	// print_number_centered(-0.9f, -0.9f, 12345678);

	SDL_GL_SwapWindow(g_mainWindow);
}

void dsb_sdl_opengl_visual_cleanup()
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(g_mainContext);

	// Destroy our window
	SDL_DestroyWindow(g_mainWindow);

	// Shutdown SDL 2
	SDL_Quit();
}

