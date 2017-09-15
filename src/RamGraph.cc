#include <string>

#include "RamGraph.h"

namespace RAMGraph {

/****************************************************************************
 * RamGraph Class
 ***************************************************************************/
RamGraph::RamGraph(string coordinatorLocator, string graphName) 
    : coordinatorLocator(coordinatorLocator)
    , graphName(graphName)
    , client(coordinatorLocator.c_str())
    , tx() {
  string vertexTableName = graphName + "_vertexTable";
  string edgeListTableName = graphName + "_edgeListTable";
  vertexTableId = client.createTable(vertexTableName.c_str());
  edgeListTableId = client.createTable(edgeListTableName.c_str());
}

void
RamGraph::beginTx() {
  tx.construct(&client);
}

bool
RamGraph::commitTx() {
  bool success = tx.get()->commitAndSync();
  tx.destroy();
  return success;
}

void
RamGraph::abortTx() {
  tx.destroy();
  return;
}

} // namespace RAMGraph
