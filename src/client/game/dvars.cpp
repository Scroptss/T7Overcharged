#include <std_include.hpp>

#define GET_DVAR(ptr) ((game::dvar_t*)((*(INT64*)(((uintptr_t)GetModuleHandle(NULL) + ptr)))))

namespace dvars
{
	game::dvar_t* ui_error_report = GET_DVAR(0x1686FF30); //updated
	game::dvar_t* ui_error_report_delay = GET_DVAR(0x1686FF38); //updated
	game::dvar_t* ui_error_callstack_ship = GET_DVAR(0x1686ED20); //updated

	game::dvar_t* sv_mapname = GET_DVAR(0x17736848); //updated
	game::dvar_t* ui_mapname = GET_DVAR(0x17960BC0); //updated
	game::dvar_t* ui_gametype = GET_DVAR(0x17960BD0); //updated

	game::dvar_t* com_maxclients = GET_DVAR(0x1686E8D0); //updated
}