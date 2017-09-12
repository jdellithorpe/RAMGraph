#ifndef RAMGRAPH_QUERY_H
#define RAMGRAPH_QUERY_H

#include <vector>

#include "RamGraph.h"
#include "Stage.h"

using namespace std;

namespace RAMGraph {

  class Query {
    private:
      RamGraph* graph;
      vector<Stage*> stages;
      vector<Vertex> starts;

    public:
      Query(RamGraph* graph);

      void addStart(Vertex v);

      void addStage(Stage* stage);

      void execute();

  }; // class Query
} // namespace RAMGraph

#endif // RAMGRAPH_QUERY_H
