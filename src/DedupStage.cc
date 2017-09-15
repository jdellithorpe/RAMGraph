#include <arpa/inet.h>
#include <iostream>

#include "DedupStage.h"
#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

DedupStage::DedupStage() 
    : graph(NULL)
    , inputBuffer(NULL)
    , outputBuffer()
    , filter() {

}

bool
DedupStage::advance(bool prevDone) {
  if (!inputBuffer->empty()) {
    for (int i = 0; i < inputBuffer->size(); i++) {
      if (filter.count(inputBuffer->at(i)) == 0) {
        outputBuffer.push_back(inputBuffer->at(i));
        filter.insert(inputBuffer->at(i));
      }
    }

    inputBuffer->clear();
  } 

  if (prevDone && inputBuffer->empty()) {
    return true;
  } else {
    return false;
  }
}

void
DedupStage::setGraph(RamGraph* graph) {
  this->graph = graph;
}

void
DedupStage::setInputBuffer(vector<Vertex>* inputBuffer) {
  this->inputBuffer = inputBuffer;
}

vector<Vertex>* 
DedupStage::getOutputBuffer() {
  return &outputBuffer;
}

void
DedupStage::clearOutputBuffer() {
  outputBuffer.clear();
}

}; // namespace RAMGraph

