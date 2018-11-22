/*
*/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ltablib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

int main()
{
	lua_State *L = lua_open();
	if (!L)
		return -1;
	luaL_openlibs(L);

	const char *fname = "test.lua";
	if (luaL_loadfile(L, fname) != 0) lua_error(L);
	lua_call(L, 0, LUA_MULTRET);
	return 0;
}
