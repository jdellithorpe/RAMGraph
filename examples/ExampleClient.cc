#include "RamGraph.h"
#include "TraverseStage.h"
#include "QueryPipeline.h"

using namespace RAMGraph;

int main() {
  RamGraph graph("basic+udp:host=192.168.1.179,port=12246", "ldbcsnbval01");

  QueryPipeline qp;

  TraverseStage ts1("knows", UN, "person");
  TraverseStage ts2("knows", UN, "person");
  TraverseStage ts3("knows", UN, "person");

  qp.addStart(Vertex{"person", 123});

  qp.addStage(&ts1);
  qp.addStage(&ts2);
  qp.addStage(&ts3);

  qp.execute();

  return 0;
}
