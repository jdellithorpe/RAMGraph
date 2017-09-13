#ifndef RAMGRAPH_COMMON_H                                                       
#define RAMGRAPH_COMMON_H 

#include <stdint.h>

namespace RAMGraph {

uint64_t htonll(uint64_t hostlonglong);
uint64_t ntohll(uint64_t hostlonglong);

} // namespace RAMGraph

#endif // RAMGRAPH_COMMON_H
