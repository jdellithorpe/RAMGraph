#ifndef RAMGRAPH_STAGE_H                                                     
#define RAMGRAPH_STAGE_H

#include <vector>

#include "RamGraph.h"

using namespace std;

namespace RAMGraph {

  /**
   * An interface for stages in a query pipeline.
   *
   * Implementations all can be advanced in a non-blocking fashion.
   */
  class Stage {
    public:
      virtual bool advance() = 0;
      virtual void setInputBuffer(vector<Vertex>* inputBuffer) = 0;
      virtual vector<Vertex>* getOutputBuffer() = 0;
  };
} // namespace RAMGraph

#endif // RAMGRAPH_STAGE_H 
