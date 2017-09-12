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
    , tx(&client) {
  string vertexTableName = graphName + "_vertexTable";
  string edgeListTableName = graphName + "_edgeListTable";
  vertexTableId = client.createTable(vertexTableName.c_str());
  edgeListTableId = client.createTable(edgeListTableName.c_str());
}

} // namespace RAMGraph
