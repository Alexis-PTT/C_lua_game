//
// Created by pat on 21/05/2026.
//

#include "lua_executer.h"

#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void run_lua() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Run a simple Lua script
    if (luaL_dostring(L, "print('Hello from Lua!')") != LUA_OK) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
}

