#ifndef RAMGRAPH_EDGELIST_H
#define RAMGRAPH_EDGELIST_H

#include <vector>
#include <string>

#include "Common.h"
#include "RamGraph.h"
#include "RamCloud.h"
#include "Transaction.h"
#include "Tub.h"

using namespace std;
using namespace RAMCloud;

typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

  class EdgeList {
    private:
      enum State {
        HEAD_PHASE,
        TAIL_PHASE,
        DONE
      };

      RamGraph* graph;
      Vertex homeVertex;
      string eLabel;
      EdgeDirection dir;
      string nLabel;
      uint32_t keyLen;
      vector<char> rcKey;
      vector<Vertex> outputBuffer;
      vector<ReadOpAndBuf*> readOps;

    public:
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
