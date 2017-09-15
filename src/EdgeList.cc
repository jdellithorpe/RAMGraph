#include <arpa/inet.h>

#include <algorithm>

#include "Common.h"
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

  readOps.push_back(new ReadOpAndBuf(&graph->tx, graph->edgeListTableId, key, 
        keyLen, true));
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
      if (readOps.at(0)->op.isReady()) {
        bool objectExists;
        readOps.at(0)->op.wait(&objectExists);

        if (objectExists) {
          uint32_t cursor = 0;
          uint32_t numTailSegs =
              ntohl(*(readOps.at(0)->val.getOffset<uint32_t>(cursor)));
          cursor += sizeof(uint32_t);

          while (cursor < readOps.at(0)->val.size()) {
            uint64_t upper = ntohll(*(readOps.at(0)->val.getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint64_t lower = ntohll(*(readOps.at(0)->val.getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint16_t propLen = ntohs(*(readOps.at(0)->val.getOffset<uint16_t>(cursor)));
            cursor += sizeof(uint16_t);
            // For now we'll skip over the edge properties
            cursor += propLen;

            outputBuffer.emplace_back(upper, lower);
          }

          delete readOps.at(0);
          readOps.erase(readOps.begin());

          if (numTailSegs > 0) {
            readOps.reserve(numTailSegs);
            for (uint32_t i = numTailSegs; i > 0; i--) {
              char* key = rcKey.data();
              *(key + keyLen - sizeof(uint32_t)) = htonl(i);
              readOps.push_back(new ReadOpAndBuf(&graph->tx, 
                    graph->edgeListTableId, key, keyLen, true));
            }

            state = TAIL_PHASE;
            return false;
          } else {
            state = DONE;
            return true;
          } 
        } else { // object does not exist
          delete readOps.at(0);
          readOps.erase(readOps.begin());
         
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

          delete readOps.at(i);
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
