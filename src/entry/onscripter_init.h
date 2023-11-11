#ifndef __ONSCRIPER_INIT_H__
#define __ONSCRIPER_INIT_H__

#ifdef __APPLE__
#include "onscripter_osx.h"
#else
void onscripter_init(ONScripter *ons, int argc, char *argv[]) {}
#endif

#endif
