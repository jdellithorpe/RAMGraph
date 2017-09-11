#ifndef RAMGRAPH_EDGELIST_H
#define RAMGRAPH_EDGELIST_H

#include <vector>

#include "RamGraph.h"

using namespace std;

namespace RAMGraph {

  class EdgeList {
    private:
      Vertex homeVertex;
      string eLabel;
      EdgeDirection dir;
      string nLabel;
      vector<Vertex> outputBuffer;

    public:
      EdgeList(Vertex v, string eLabel, EdgeDirection dir, string nLabel)
          : homeVertex(v)
          , eLabel(eLabel)
          , dir(dir)
          , nLabel(nLabel)
          , outputBuffer() {
            
      }

      Vertex getHomeVertex() {
        return homeVertex;
      }

      vector<Vertex>* getOutputBuffer() {
        return &outputBuffer;
      }

      void clearOutputBuffer() {
        outputBuffer.clear();
      }

      bool advance() {
        return true;
      }
  }; // class EdgeList


}; // namespace RAMGraph

#endif // RAMGRAPH_EDGELIST_H
