#ifndef PP_CONFIG_H
#define PP_CONFIG_H

#include "repast_hpc/SharedDiscreteSpace.h"

template<typename T>
using Grid = repast::SharedDiscreteSpace<T, repast::StrictBorders, repast::SimpleAdder<T>>;

#endif
