#include "RamGraph.h"
#include "TraverseStage.h"
#include "Query.h"

using namespace RAMGraph;

int main() {
  RamGraph graph("basic+udp:host=192.168.1.179,port=12246", "ldbcsnbval01");

  Query query(&graph);

  query.addStart(Vertex{4, 123});

  TraverseStage ts1("knows", UN, "person");
  TraverseStage ts2("knows", UN, "person");
  TraverseStage ts3("knows", UN, "person");

  query.addStage(&ts1);
  query.addStage(&ts2);
  query.addStage(&ts3);

  query.execute();

  return 0;
}
