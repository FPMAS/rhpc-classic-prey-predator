#ifndef CONFIG_H
#include "repast_hpc/SharedDiscreteSpace.h"

template<typename T>
using Grid = repast::SharedDiscreteSpace<T, repast::StrictBorders, repast::SimpleAdder<T>>;

#endif
