#ifndef RAMGRAPH_UNIQUETRAVERSESTAGE_H
#define RAMGRAPH_UNIQUETRAVERSESTAGE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "RamGraph.h"
#include "Stage.h"
#include "EdgeList.h"

using namespace std;

namespace RAMGraph {

  class UniqueTraverseStage : public Stage {
    protected:
      RamGraph* graph;
      string eLabel;
      EdgeDirection dir;
      string nLabel;
      vector<EdgeList> eLists;
      vector<Vertex>* inputBuffer;
      vector<Vertex> outputBuffer;
      unordered_set<Vertex, VertexHash>* seenSet;

    public:
      UniqueTraverseStage(string eLabel, EdgeDirection dir, string nLabel,
          unordered_set<Vertex, VertexHash>* seenSet);

      void setInputBuffer(vector<Vertex>* inputBuffer);

      void setGraph(RamGraph* graph);

      vector<Vertex>* getOutputBuffer();

      void clearOutputBuffer();

      bool hasOutput();

      /* Advance this stage. Returns true if the stage is finished advancing,
       * false otherwise.
       */
      bool advance(bool prevDone);

  }; // class UniqueTraverseStage
} // namespace RAMGraph

#endif // RAMGRAPH_UNIQUETRAVERSESTAGE_H
