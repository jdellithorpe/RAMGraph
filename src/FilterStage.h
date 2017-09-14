#ifndef RAMGRAPH_FILTERSTAGE_H
#define RAMGRAPH_FILTERSTAGE_H

#include <unordered_map>
#include <vector>

#include "Common.h"
#include "RamGraph.h"
#include "Stage.h"

using namespace std;

namespace RAMGraph {

  class FilterStage : public Stage {
    private:
      RamGraph* graph;
      vector<Vertex>* inputBuffer;
      vector<Vertex> outputBuffer;
      vector<ReadOpAndBuf*> readOps;
      const function <bool (unordered_map<string, vector<string>>&)> &pred;

    public:
      FilterStage(const function <bool (unordered_map<string, vector<string>>&)> &pred);

      bool advance(bool prevDone);

      void setGraph(RamGraph* graph);

      void setInputBuffer(vector<Vertex>* inputBuffer);

      vector<Vertex>* getOutputBuffer();

      void clearOutputBuffer();
  }; // class TraverseStage
} // namespace RAMGraph



#endif // RAMGRAPH_FILTERSTAGE_H
