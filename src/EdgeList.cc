#include "EdgeList.h"

using namespace std;
using namespace RAMCloud;
typedef Transaction::ReadOp ReadOp;

namespace RAMGraph {

EdgeList::EdgeList(RamGraph* graph, Vertex v, string eLabel, 
    EdgeDirection dir, string nLabel)
    : graph(graph)
    , homeVertex(v)
    , eLabel(eLabel)
    , dir(dir)
    , nLabel(nLabel)
    , outputBuffer()
//    , headSegBuffer()
//    , headSegReadOp(&graph->tx, graph->edgeListTableId, "test", 4, 
//      &headSegBuffer, true)
    , state(HEAD_PHASE) {
}

Vertex 
EdgeList::getHomeVertex() {
  return homeVertex;
}

vector<Vertex>* 
EdgeList::getOutputBuffer() {
  return &outputBuffer;
}

void 
EdgeList::clearOutputBuffer() {
  outputBuffer.clear();
}

bool 
EdgeList::advance() {
  switch (state) {
    case HEAD_PHASE:
//      if (readOps[0].isReady()) {
//        readOps[0].wait();
//        uint32_t numTailSegs = *(values[0].getStart<uint32_t>());
//
//      }
      break;
    case TAIL_PHASE:
      break;
    default:
      break;
  }

  return true;
}

}; // namespace RAMGraph
