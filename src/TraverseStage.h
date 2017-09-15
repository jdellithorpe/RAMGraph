#ifndef RAMGRAPH_TRAVERSESTAGE_H
#define RAMGRAPH_TRAVERSESTAGE_H

#include <vector>
#include <unordered_map>

#include "RamGraph.h"
#include "Stage.h"
#include "EdgeList.h"

using namespace std;

namespace RAMGraph {

  class TraverseStage : public Stage {
    protected:
      RamGraph* graph;
      string eLabel;
      EdgeDirection dir;
      string nLabel;
      vector<EdgeList> eLists;
      vector<Vertex>* inputBuffer;
      vector<Vertex> outputBuffer;
//      unordered_map<Vertex, vector<Vertex>, VertexHash> travMap;

    public:
      TraverseStage(string eLabel, EdgeDirection dir, string nLabel);

      void setInputBuffer(vector<Vertex>* inputBuffer);

      void setGraph(RamGraph* graph);

      vector<Vertex>* getOutputBuffer();

      void clearOutputBuffer();

      bool hasOutput();

      /* Advance this stage. Returns true if the stage is finished advancing,
       * false otherwise.
       */
      bool advance(bool prevDone);

  }; // class TraverseStage
} // namespace RAMGraph

#endif // RAMGRAPH_TRAVERSESTAGE_H
