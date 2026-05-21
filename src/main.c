

#include <stdio.h>
#include "screen.h"
#include "lua_executer.h"

void app_main(void) {

    printf("Hello, World!\n");
    I2C_screen c;
    initI2C(&c);
    initLCD(&c);
    testProg(&c);
    run_lua();
}

