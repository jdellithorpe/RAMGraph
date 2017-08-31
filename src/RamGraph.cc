#include "RamGraph.h"

#include "RamCloud.h"

#include <string>
#include <map>
#include <vector>

namespace RAMGraph {

  /****************************************************************************
   * RamGraph Class
   ***************************************************************************/
  RamGraph::RamGraph(string coordinatorLocator, string graphName) 
      : coordinatorLocator(coordinatorLocator)
      , graphName(graphName) {

  }

} // namespace RAMGraph
