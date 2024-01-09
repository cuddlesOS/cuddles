#ifndef HALT_H
#define HALT_H

#include "def.h"

[[noreturn]] void freeze();
[[noreturn]] void panic(str msg);

#endif
