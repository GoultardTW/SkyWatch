#include <hiredis/hiredis.h>
#include <iostream>
#include <postgresql/libpq-fe.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "control_center.hpp"
#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
