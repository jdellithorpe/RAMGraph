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

  cout << "iter ";
  for (int i = 0; i < stages.size(); i++) {
    int width = 0;
    if (i == 0) {
      width = 14;
    } else {
      width = 17;
    }

    cout << setw(width) << "Stage" << i;
  }
  cout << setw(20) << "Total";
  cout << endl;

  uint32_t iteration = 0;
  while (!done) {
    done = true;
    uint64_t iterTime = 0;
    cout << setw(3) << iteration << ": ";
    for (int i = 0; i < stages.size(); i++) {
      uint64_t start = Cycles::rdtsc();
      done = stages.at(i)->advance(done) && done;
      uint64_t end = Cycles::rdtsc();
      timeArray[i] += end - start;
      iterTime += end - start;
      outputCounts[i] += stages.at(i)->getOutputBuffer()->size();

      int width = 0;
      if (i == 0) {
        width = 10;
      } else {
        width = 7;
      }

      cout << setw(width) << stages.at(i)->getOutputBuffer()->size();
      cout << setw(9) << Cycles::toMicroseconds(end - start) << "us";
    }

    cout << setw(11) << Cycles::toMicroseconds(iterTime) << "us"; 
    cout << endl;

    iteration++;
  }

  cout << endl;
  cout << "Tot: ";
  for (int i = 0; i < stages.size(); i++) {
      int width = 0;
      if (i == 0) {
        width = 10;
      } else {
        width = 7;
      }

      cout << setw(width) << outputCounts[i];
      cout << setw(9) << Cycles::toMicroseconds(timeArray[i])/1000l << "ms";
  }
  cout << endl;
}

} // namespace RAMGraph
