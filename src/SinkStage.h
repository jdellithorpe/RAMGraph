#ifndef RAMGRAPH_SINKSTAGE_H
#define RAMGRAPH_SINKSTAGE_H

#include <unordered_map>
#include <vector>
#include <unordered_set>

#include "Common.h"
#include "RamGraph.h"
#include "Stage.h"

using namespace std;

namespace RAMGraph {

  class SinkStage : public Stage {
    private:
      RamGraph* graph;
      vector<Vertex>* inputBuffer;
      vector<Vertex> outputBuffer;

    public:
      SinkStage();

      bool advance(bool prevDone);

      void setGraph(RamGraph* graph);

      void setInputBuffer(vector<Vertex>* inputBuffer);

      vector<Vertex>* getOutputBuffer();

      void clearOutputBuffer();
  }; // class SinkStage
} // namespace RAMGraph



#endif // RAMGRAPH_SINKSTAGE_H


