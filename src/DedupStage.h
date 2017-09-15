#ifndef RAMGRAPH_DEDUPSTAGE_H
#define RAMGRAPH_DEDUPSTAGE_H

#include <unordered_map>
#include <vector>
#include <unordered_set>

#include "Common.h"
#include "RamGraph.h"
#include "Stage.h"

using namespace std;

namespace RAMGraph {

  class DedupStage : public Stage {
    private:
      RamGraph* graph;
      vector<Vertex>* inputBuffer;
      vector<Vertex> outputBuffer;
      unordered_set<Vertex, VertexHash> filter;

    public:
      DedupStage();

      bool advance(bool prevDone);

      void setGraph(RamGraph* graph);

      void setInputBuffer(vector<Vertex>* inputBuffer);

      vector<Vertex>* getOutputBuffer();

      void clearOutputBuffer();
  }; // class TraverseStage
} // namespace RAMGraph



#endif // RAMGRAPH_DEDUPSTAGE_H

