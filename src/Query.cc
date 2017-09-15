#include "Query.h"

using namespace std;

namespace RAMGraph {

Query::Query(RamGraph* graph)
    : graph(graph)
    , stages()
    , starts() {
}

void 
Query::addStart(Vertex v) {
  starts.push_back(v);
}

void 
Query::addStage(Stage* stage) {
  if (!stages.empty()) {
    Stage* tailStage = stages.back();
    stage->setInputBuffer(tailStage->getOutputBuffer());
  } else {
    stage->setInputBuffer(&starts);
  }
  
  stage->setGraph(graph);

  stages.push_back(stage);
}

void 
Query::execute() {
  bool done = false;
 
  uint64_t timeArray[stages.size()];
  memset(timeArray, 0, stages.size()*sizeof(uint64_t)); 
  
  uint64_t outputCounts[stages.size()];
  memset(outputCounts, 0, stages.size()*sizeof(uint64_t));

  while (!done) {
    done = true;
    for (int i = 0; i < stages.size(); i++) {
      uint64_t start = Cycles::rdtsc();
      done = stages.at(i)->advance(done) && done;
      timeArray[i] += Cycles::rdtsc() - start;
      outputCounts[i] += stages.at(i)->getOutputBuffer()->size();
    }
  }

  cout << "Stage Times" << endl;
  for (int i = 0; i < stages.size(); i++) {
    cout << "  ";
    cout << "Stage " << i << ": " << Cycles::toMicroseconds(timeArray[i]) <<
        "us" << endl;
  }

  cout << "Stage Output Counts" << endl;
  for (int i = 0; i < stages.size(); i++) {
    cout << "  ";
    cout << "Stage " << i << ": " << outputCounts[i] << " vertices" << endl;
  }
}

} // namespace RAMGraph
