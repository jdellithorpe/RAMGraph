#include "TraverseStage.h"
#include "Parameters.h"

using namespace std;

namespace RAMGraph {

TraverseStage::TraverseStage(string eLabel, EdgeDirection dir, string nLabel) 
    : graph(NULL)
    , eLabel(eLabel)
    , dir(dir)
    , nLabel(nLabel)
    , eLists()
    , inputBuffer(NULL)
    , outputBuffer() {

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

//  cout << endl;
//  cout << "Advancing " << eLists.size() << " edge lists ";
//  uint64_t start = Cycles::rdtsc();
  for (int i = 0; i < eLists.size(); i++) {
    edgeListsDone = eLists.at(i).advance() && edgeListsDone;
    vector<Vertex>* lOutBuf = eLists.at(i).getOutputBuffer();
    for (int j = 0; j < lOutBuf->size(); j++) {
      outputBuffer.push_back(lOutBuf->at(j));
    }

    eLists.at(i).clearOutputBuffer();
  }
//  uint64_t end = Cycles::rdtsc();
//  cout << "took " << Cycles::toMicroseconds(end - start) << "us" << endl;

  eLists.erase(remove_if(eLists.begin(), eLists.end(), 
      [](EdgeList& l) { return l.state == EdgeList::State::DONE; }), 
      eLists.end());

  if (inputBuffer->size() > 0)
    edgeListsDone = false;

//  cout << endl;
//  cout << "Emplacing " << inputBuffer->size() << " edge lists ";
//  start = Cycles::rdtsc();
  for (int i = 0; i < inputBuffer->size(); i++) {
    eLists.emplace_back(graph, inputBuffer->at(i), eLabel, dir, nLabel);

    if ((i + 1) % READOP_BATCH_TRIGGER_THRESHOLD == 0) {
      eLists.back().advance();
    }
  }
//  end = Cycles::rdtsc();
//  cout << "took " << Cycles::toMicroseconds(end - start) << "us" << endl;
 
  if (!inputBuffer->empty()) {
    eLists.back().advance();
    inputBuffer->clear();
  } 

  return edgeListsDone;
}

} // namespace RAMGraph
