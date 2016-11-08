#pragma once
#include "tch_add/TchClientExport.h"
extern int cef_argc;
extern char* cef_argv[16];
typedef int(*TchMain)(int argc,char* argv[]);
