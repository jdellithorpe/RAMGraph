#include <arpa/inet.h>
#include <iostream>

#include "SinkStage.h"
#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

SinkStage::SinkStage() 
    : graph(NULL)
    , inputBuffer(NULL)
    , outputBuffer() {

}

bool
SinkStage::advance(bool prevDone) {
  inputBuffer->clear();
  return true;
}

void
SinkStage::setGraph(RamGraph* graph) {
  this->graph = graph;
}

void
SinkStage::setInputBuffer(vector<Vertex>* inputBuffer) {
  this->inputBuffer = inputBuffer;
}

vector<Vertex>* 
SinkStage::getOutputBuffer() {
  return &outputBuffer;
}

void
SinkStage::clearOutputBuffer() {
  outputBuffer.clear();
}

}; // namespace RAMGraph


