#include "UniqueTraverseStage.h"

using namespace std;

namespace RAMGraph {

UniqueTraverseStage::UniqueTraverseStage(string eLabel, EdgeDirection dir, 
    string nLabel, unordered_set<Vertex, VertexHash>* seenSet) 
    : eLabel(eLabel)
    , dir(dir)
    , nLabel(nLabel)
    , eLists()
    , inputBuffer(NULL)
    , outputBuffer()
    , seenSet(seenSet) {

}

void 
UniqueTraverseStage::setInputBuffer(vector<Vertex>* inputBuffer) {
  this->inputBuffer = inputBuffer; 
}

void
UniqueTraverseStage::setGraph(RamGraph* graph) {
  this->graph = graph;
}

vector<Vertex>* 
UniqueTraverseStage::getOutputBuffer() {
  return &outputBuffer;
}

void 
UniqueTraverseStage::clearOutputBuffer() {
  outputBuffer.clear();
}

bool 
UniqueTraverseStage::hasOutput() {
  return !outputBuffer.empty();
}

/* Advance this stage. Returns true if the stage is finished advancing,
 * false otherwise.
 */
bool 
UniqueTraverseStage::advance(bool prevDone) {

  if (prevDone && inputBuffer->empty()) {
    bool edgeListsDone = true;
    for (int i = 0; i < eLists.size(); i++) {
      edgeListsDone = eLists.at(i).advance() && edgeListsDone;
      vector<Vertex>* lOutBuf = eLists.at(i).getOutputBuffer();
      for (int j = 0; j < lOutBuf->size(); j++) {
        if (seenSet->count(lOutBuf->at(j)) == 0) {
          outputBuffer.push_back(lOutBuf->at(j));
        }
      }

      eLists.at(i).clearOutputBuffer();
    }

    return edgeListsDone;
  } else {
    for (int i = 0; i < eLists.size(); i++) {
      eLists.at(i).advance();
    }
  }
  
  for (int i = 0; i < inputBuffer->size(); i++) {
    if (seenSet->count(inputBuffer->at(i)) == 0) {
      seenSet->insert(inputBuffer->at(i));
      eLists.emplace_back(graph, inputBuffer->at(i), eLabel, dir, nLabel);
    }
  }

  inputBuffer->clear();

  return false;
}

} // namespace RAMGraph
