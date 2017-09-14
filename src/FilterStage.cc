#include <arpa/inet.h>
#include <iostream>

#include "FilterStage.h"
#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

FilterStage::FilterStage(const function <bool (unordered_map<string, vector<string>>&)> &pred) 
    : graph(NULL)
    , inputBuffer(NULL)
    , outputBuffer()
    , readOps()
    , pred(pred) {

}

bool
FilterStage::advance() {
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
          outputBuffer.push_back(inputBuffer->at(i));
        }

        delete readOps.at(i);
        readOps.at(i) = NULL;
      } else {
        // This ReadOp is outstanding
        done = false;
      }
    }
  }

  // Process new input, enqueueing new ReadOps
  if (inputBuffer->size() > readOps.size()) {
    for (int i = readOps.size(); i < inputBuffer->size(); i++) {
      Vertex* v = &(inputBuffer->at(i));

      // make key
      uint32_t keyLen = 2*sizeof(uint64_t) + sizeof(uint8_t);
      char key[keyLen];

      uint32_t cursor = 0;
      *(uint64_t*)(key + cursor) = htonll(v->id.upper);
      cursor += sizeof(uint64_t);
      *(uint64_t*)(key + cursor) = htonll(v->id.lower);
      cursor += sizeof(uint64_t);
      *(uint8_t*)(key + cursor) = (uint8_t)1; // properties

      readOps.push_back(new ReadOpAndBuf(&graph->tx, graph->vertexTableId, 
            key, keyLen, true));
    }

    done = false;
  } 

  if (done) {
    inputBuffer->clear();
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
