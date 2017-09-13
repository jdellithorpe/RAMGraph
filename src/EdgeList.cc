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
    , values()
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

  values.push_back(new Buffer);
  readOps.push_back(new ReadOp(&graph->tx, graph->edgeListTableId,
      key, keyLen, values.back(), true));
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
      if (readOps.at(0)->isReady()) {
        readOps.at(0)->wait();
        
        uint32_t cursor = 0;
        uint32_t numTailSegs =
            ntohl(*(values.at(0)->getOffset<uint32_t>(cursor)));
        cursor += sizeof(uint32_t);

        while (cursor < values.at(0)->size()) {
          uint64_t upper = ntohll(*(values.at(0)->getOffset<uint64_t>(cursor)));
          cursor += sizeof(uint64_t);
          uint64_t lower = ntohll(*(values.at(0)->getOffset<uint64_t>(cursor)));
          cursor += sizeof(uint64_t);
          uint16_t propLen = ntohs(*(values.at(0)->getOffset<uint16_t>(cursor)));
          cursor += sizeof(uint16_t);
          // For now we'll skip over the edge properties
          cursor += propLen;

          outputBuffer.emplace_back(upper, lower);
        }

        delete values.at(0);
        delete readOps.at(0);
        values.erase(values.begin());
        readOps.erase(readOps.begin());

        if (numTailSegs > 0) {
          values.reserve(numTailSegs);
          readOps.reserve(numTailSegs);
          for (uint32_t i = numTailSegs; i > 0; i--) {
            char* key = rcKey.data();
            *(key + keyLen - sizeof(uint32_t)) = htonl(i);
            values.push_back(new Buffer);
            readOps.push_back(new ReadOp(&graph->tx, graph->edgeListTableId,
                  key, keyLen, values.back(), true));
          }

          state = TAIL_PHASE;
          return false;
        } else {
          state = DONE;
          return true;
        }
      } else {
        state = HEAD_PHASE;
        return false;
      }
      break;
    case TAIL_PHASE:
      for (int i = 0; i < readOps.size(); i++) {
        if (readOps[i]->isReady()) {
          readOps[i]->wait();

          uint32_t cursor = 0;
          while (cursor < values[i]->size()) {
            uint64_t upper = ntohll(*(values[i]->getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint64_t lower = ntohll(*(values[i]->getOffset<uint64_t>(cursor)));
            cursor += sizeof(uint64_t);
            uint16_t propLen = ntohs(*(values[i]->getOffset<uint16_t>(cursor)));
            cursor += sizeof(uint16_t);
            // For now we'll skip over the edge properties
            cursor += propLen;

            outputBuffer.emplace_back(upper, lower);
          }

          delete values[i];
          delete readOps[i];
          values[i] = NULL;
          readOps[i] = NULL;
        }
      }

      values.erase(remove_if(values.begin(), values.end(), 
            [](Buffer* b) { return b == NULL; }), values.end());
      readOps.erase(remove_if(readOps.begin(), readOps.end(), 
            [](ReadOp* r) { return r == NULL; }), readOps.end());

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
