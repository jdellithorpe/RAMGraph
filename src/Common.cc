#include <arpa/inet.h>

#include "Common.h"

namespace RAMGraph {

uint64_t htonll(uint64_t hostlonglong) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return ntohll(hostlonglong);
#else
  return hostlonglong;
#endif
}

uint64_t ntohll(uint64_t netlonglong) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  union u64_as_u32 {
    uint64_t val;
    uint32_t dat[2];
  } net, host;
  
  net.val = netlonglong;
  host.dat[0] = ntohl(net.dat[1]);
  host.dat[1] = ntohl(net.dat[0]);
  return host.val;
#else
  return netlonglong;  
#endif
}

} // namespace RAMGraph


