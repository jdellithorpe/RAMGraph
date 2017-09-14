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
  
  while (!done) {
    done = true;
    for (int i = 0; i < stages.size(); i++) {
      done = stages.at(i)->advance(done) && done;
    }
  }
}

} // namespace RAMGraph
