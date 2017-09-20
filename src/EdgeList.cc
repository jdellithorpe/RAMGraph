#include <arpa/inet.h>

#include <algorithm>

#include "Common.h"
#include "MemoryPools.h"
#include "EdgeList.h"

using namespace std;
using namespace RAMCloud;
typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

EdgeList::EdgeList(RamGraph* graph, Vertex v, string eLabel, 
    EdgeDirection dir, string nLabel)
    : graph(graph)
    , homeVertex(v)
    , eLabel(eLabel)
    , dir(dir)
    , nLabel(nLabel)
    , keyLen(2*sizeof(uint64_t) + sizeof(uint16_t) + eLabel.length() +
      sizeof(uint8_t) + sizeof(uint16_t) + nLabel.length() + sizeof(uint32_t))
    , rcKey()
    , outputBuffer()
    , headSegReadOp(NULL)
    , readOps()
    , state(HEAD_PHASE) {

  rcKey.reserve(keyLen);

  char* key = rcKey.data();

  uint32_t cursor = 0;
  *(uint64_t*)(key + cursor) = htonll(v.id.upper);
  cursor += sizeof(uint64_t);
  *(uint64_t*)(key + cursor) = htonll(v.id.lower);
  cursor += sizeof(uint64_t);
  *(uint16_t*)(key + cursor) = htons((uint16_t)eLabel.length());
  cursor += sizeof(uint16_t);
  memcpy((void*)(key + cursor), eLabel.c_str(), eLabel.length());
  cursor += eLabel.length();
  *(uint8_t*)(key + cursor) = (uint8_t)dir;
  cursor += sizeof(uint8_t);
  *(uint16_t*)(key + cursor) = htons((uint16_t)nLabel.length());
  cursor += sizeof(uint16_t);
  memcpy((void*)(key + cursor), nLabel.c_str(), nLabel.length());
  cursor += nLabel.length();
  *(uint32_t*)(key + cursor) = 0;

//  printf("%s: ", eLabel.c_str());
//  for (int i = 0; i < keyLen; i++) {
//    printf("%02hhX ", key[i]);
//  }
//  printf("\n");

//  uint64_t start = Cycles::rdtsc();
  headSegReadOp = MemoryPools::readOpAndBufPool.construct(graph->tx.get(), 
        graph->edgeListTableId, key, keyLen, true);
//  uint64_t end = Cycles::rdtsc();
//  cout << endl << "push_back time: " << Cycles::toNanoseconds(end - start) <<
//      "ns" << endl;
}

Vertex 
EdgeList::getHomeVertex() {
  return homeVertex;
}

vector<Vertex>* 
EdgeList::getOutputBuffer() {
  return &outputBuffer;
}

void 
EdgeList::clearOutputBuffer() {
  outputBuffer.clear();
}

bool 
EdgeList::advance() {
  graph->client.poll();

  switch (state) {
    case HEAD_PHASE:
      if (headSegReadOp->op.isReady()) {
        bool objectExists;
        headSegReadOp->op.wait(&objectExists);

        if (objectExists) {
          uint32_t cursor = 0;
          uint32_t numTailSegs =
              ntohl(*(headSegReadOp->val.getOffset<uint32_t>(cursor)));
          cursor += sizeof(uint32_t);

          while (cursor < headSegReadOp->val.size()) {
            uint64_t upper = ntohll(*(headSegReadOp->val.getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint64_t lower = ntohll(*(headSegReadOp->val.getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint16_t propLen = ntohs(*(headSegReadOp->val.getOffset<uint16_t>(cursor)));
            cursor += sizeof(uint16_t);
            // For now we'll skip over the edge properties
            cursor += propLen;

            outputBuffer.emplace_back(upper, lower);
          }

          MemoryPools::readOpAndBufPool.destroy(headSegReadOp);

          if (numTailSegs > 0) {
            readOps.reserve(numTailSegs);
            for (uint32_t i = numTailSegs; i > 0; i--) {
              char* key = rcKey.data();
              *(key + keyLen - sizeof(uint32_t)) = htonl(i);
              readOps.push_back(MemoryPools::readOpAndBufPool.construct(
                    graph->tx.get(), graph->edgeListTableId, key, keyLen, 
                    true));
            }

            state = TAIL_PHASE;
            return false;
          } else {
            state = DONE;
            return true;
          } 
        } else { // object does not exist
          MemoryPools::readOpAndBufPool.destroy(headSegReadOp);
         
          state = DONE;
          return false; 
        }
      } else {
        state = HEAD_PHASE;
        return false;
      }
      break;
    case TAIL_PHASE:
      for (int i = 0; i < readOps.size(); i++) {
        if (readOps.at(i)->op.isReady()) {
          readOps.at(i)->op.wait();

          uint32_t cursor = 0;
          while (cursor < readOps.at(i)->val.size()) {
            uint64_t upper = ntohll(*(readOps.at(i)->val.getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint64_t lower = ntohll(*(readOps.at(i)->val.getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint16_t propLen = ntohs(*(readOps.at(i)->val.getOffset<uint16_t>(cursor)));
            cursor += sizeof(uint16_t);
            // For now we'll skip over the edge properties
            cursor += propLen;

            outputBuffer.emplace_back(upper, lower);
          }

          MemoryPools::readOpAndBufPool.destroy(readOps.at(i));
          readOps.at(i) = NULL;
        }
      }

      readOps.erase(remove_if(readOps.begin(), readOps.end(), 
            [](ReadOpAndBuf* r) { return r == NULL; }), readOps.end());

      if (readOps.empty()) {
        state = DONE;
        return true;
      } else {
        state = TAIL_PHASE;
        return false;
      }
      break;
    case DONE:
      return true;
      break;
    default:
      break;
  }

  return false;
}

}; // namespace RAMGraph
