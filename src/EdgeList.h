#ifndef RAMGRAPH_EDGELIST_H
#define RAMGRAPH_EDGELIST_H

#include <vector>
#include <string>

#include "RamGraph.h"
#include "RamCloud.h"
#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

//typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

  class EdgeList {
    private:
      enum State {
        HEAD_PHASE,
        TAIL_PHASE
      };

      RamGraph* graph;
      Vertex homeVertex;
      string eLabel;
      EdgeDirection dir;
      string nLabel;
      vector<Vertex> outputBuffer;
//      Buffer headSegBuffer;
//      ReadOp headSegReadOp;
      State state;

    public:
      EdgeList(RamGraph* graph, Vertex v, string eLabel, EdgeDirection dir, 
          string nLabel);

      Vertex getHomeVertex();

      vector<Vertex>* getOutputBuffer();

      void clearOutputBuffer();

      bool advance();
  }; // class EdgeList


}; // namespace RAMGraph

#endif // RAMGRAPH_EDGELIST_H
