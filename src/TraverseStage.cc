#include "TraverseStage.h"

using namespace std;

namespace RAMGraph {

TraverseStage::TraverseStage(string eLabel, EdgeDirection dir, string nLabel) 
    : eLabel(eLabel)
    , dir(dir)
    , nLabel(nLabel)
    , eLists()
    , inputBuffer(NULL)
    , outputBuffer()
    , travMap() {

}

void 
TraverseStage::setInputBuffer(vector<Vertex>* inputBuffer) {
  this->inputBuffer = inputBuffer; 
}

void
TraverseStage::setGraph(RamGraph* graph) {
  this->graph = graph;
}

vector<Vertex>* 
TraverseStage::getOutputBuffer() {
  return &outputBuffer;
}

void 
TraverseStage::clearOutputBuffer() {
  outputBuffer.clear();
}

bool 
TraverseStage::hasOutput() {
  return !outputBuffer.empty();
}

/* Advance this stage. Returns true if the stage is finished advancing,
 * false otherwise.
 */
bool 
TraverseStage::advance() {
  bool edgeListsDone = true;
  for (EdgeList list : eLists) {
    edgeListsDone &= list.advance();
    vector<Vertex>* lOutBuf = list.getOutputBuffer();
    for (Vertex v : *lOutBuf) {
      outputBuffer.push_back(v);
      travMap[list.getHomeVertex()].push_back(v);
    }
    list.clearOutputBuffer();
  }
  
  if (inputBuffer->size() > 0)
    edgeListsDone = false;

  for (Vertex v : *inputBuffer) {
    eLists.emplace_back(graph, v, eLabel, dir, nLabel);
  }

  inputBuffer->clear();

  return edgeListsDone;
}

} // namespace RAMGraph
