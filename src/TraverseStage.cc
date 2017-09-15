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
//    , travMap() 
{

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
TraverseStage::advance(bool prevDone) {
  bool edgeListsDone = true;
  for (int i = 0; i < eLists.size(); i++) {
    edgeListsDone = eLists.at(i).advance() && edgeListsDone;
    vector<Vertex>* lOutBuf = eLists.at(i).getOutputBuffer();
    for (int j = 0; j < lOutBuf->size(); j++) {
      outputBuffer.push_back(lOutBuf->at(j));
//      travMap[eLists.at(i).getHomeVertex()].push_back(lOutBuf->at(j));
    }

    eLists.at(i).clearOutputBuffer();
  }
  
  if (inputBuffer->size() > 0)
    edgeListsDone = false;

  for (int i = 0; i < inputBuffer->size(); i++) {
    eLists.emplace_back(graph, inputBuffer->at(i), eLabel, dir, nLabel);
  }

  inputBuffer->clear();

  return edgeListsDone;
}

} // namespace RAMGraph
