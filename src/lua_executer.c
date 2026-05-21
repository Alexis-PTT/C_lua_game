//
// Created by pat on 21/05/2026.
//

#include "lua_executer.h"

#include <stdio.h>
#include "screen.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
static I2C_screen c;

int lua_test_screen(lua_State *L) {
    testProg(&c);
    return 0;
}


void run_lua() {
    printf("init screen \n");
    initI2C(&c);
    initLCD(&c);
    lua_State *L = luaL_newstate();
    lua_register(L, "test_screen", lua_test_screen);
    luaL_openlibs(L);
    // if (luaL_dofile(L, "./src/game.lua") != LUA_OK) {
    //     // print the error message from lua
    //     printf("Lua error: %s\n", lua_tostring(L, -1));
    //     lua_pop(L, 1);  // remove error from stack
    // }
    const char *script =
    "print('hello from lua')\n"
    "test_screen()\n";

    if (luaL_dostring(L, script) != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_close(L);
}


