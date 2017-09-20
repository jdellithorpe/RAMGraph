#include <arpa/inet.h>
#include <iostream>

#include "Common.h"
#include "FilterStage.h"
#include "MemoryPools.h"
#include "Parameters.h"

#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

FilterStage::FilterStage(const function <bool (unordered_map<string, vector<string>>&)> &pred) 
    : graph(NULL)
    , inputBuffer(NULL)
    , processBuffer()
    , outputBuffer()
    , readOps()
    , pred(pred) {

}

bool
FilterStage::advance(bool prevDone) {
  graph->client.poll();

  bool done = true;

  for (int i = 0; i < readOps.size(); i++) {
    if (readOps.at(i) != NULL) {
      ReadOp* readOp = &readOps.at(i)->op;
      if (readOp->isReady()) {
        readOp->wait();

        Buffer* value = &readOps.at(i)->val;
        
        unordered_map<string, vector<string>> properties;

        uint32_t cursor = 0;
        while (cursor < value->size()) {
          uint32_t propLen = ntohl(*(value->getOffset<uint32_t>(cursor)));
          cursor += sizeof(uint32_t);
          if (propLen > 0) {
            uint32_t propStartPos = cursor;
            
            uint16_t keyLen = ntohs(*(value->getOffset<uint16_t>(cursor)));
            cursor += sizeof(uint16_t);
            
            char key[keyLen + 1];
            value->copy(cursor, keyLen, key);
            key[keyLen] = '\0';
            cursor += keyLen;
            
            vector<string> propVals;
            while (cursor - propStartPos < propLen) {
              uint16_t valLen = ntohs(*(value->getOffset<uint16_t>(cursor)));
              cursor += sizeof(uint16_t);

              char val[valLen + 1];
              value->copy(cursor, valLen, val);
              val[valLen] = '\0';
              cursor += valLen;

              propVals.emplace_back((const char*)val);
            }

            properties.emplace((const char*)key, propVals);
          }
        }

        if (pred(properties)) {
          outputBuffer.push_back(processBuffer.at(i));
        }

        MemoryPools::readOpAndBufPool.destroy(readOps.at(i));
        readOps.at(i) = NULL;
      } else {
        // This ReadOp is outstanding
        done = false;
      }
    }
  }

  if (!inputBuffer->empty()) {
    for (int i = 0; i < inputBuffer->size(); i++) {
      processBuffer.push_back(inputBuffer->at(i));
    }

    inputBuffer->clear();

    int count = 0;
    for (int i = readOps.size(); i < processBuffer.size(); i++) {
      Vertex* v = &(processBuffer.at(i));

      // make key
      uint32_t keyLen = 2*sizeof(uint64_t) + sizeof(uint8_t);
      char key[keyLen];

      uint32_t cursor = 0;
      *(uint64_t*)(key + cursor) = htonll(v->id.upper);
      cursor += sizeof(uint64_t);
      *(uint64_t*)(key + cursor) = htonll(v->id.lower);
      cursor += sizeof(uint64_t);
      *(uint8_t*)(key + cursor) = (uint8_t)1; // properties

      readOps.push_back(MemoryPools::readOpAndBufPool.construct(graph->tx.get(), 
            graph->vertexTableId, (char*)key, keyLen, true));

      count++;

      if (count % READOP_BATCH_TRIGGER_THRESHOLD == 0) {
        readOps.back()->op.isReady();
      }
    }

    if (count > 0) {
      readOps.back()->op.isReady();
    }

    done = false;
  }

  if (done) {
    processBuffer.clear();
    readOps.clear();
  }
  
  return done;
}

void
FilterStage::setGraph(RamGraph* graph) {
  this->graph = graph;
}

void
FilterStage::setInputBuffer(vector<Vertex>* inputBuffer) {
  this->inputBuffer = inputBuffer;
}

vector<Vertex>* 
FilterStage::getOutputBuffer() {
  return &outputBuffer;
}

void
FilterStage::clearOutputBuffer() {
  outputBuffer.clear();
}

}; // namespace RAMGraph
