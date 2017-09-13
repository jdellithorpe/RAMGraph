#include <iostream>

#include "RamGraph.h"
#include "TraverseStage.h"
#include "Query.h"

using namespace RAMGraph;

int main() {
  RamGraph graph("basic+udp:host=192.168.1.179,port=12246", "ldbc_snb_sf0001");

  Query query(&graph);

  query.addStart(Vertex{4, 933});

  TraverseStage ts1("knows", OUT, "Person");
  TraverseStage ts2("knows", OUT, "Person");
//  TraverseStage ts3("knows", OUT, "Person");

  query.addStage(&ts1);
  query.addStage(&ts2);
//  query.addStage(&ts3);

  query.execute();

  vector<Vertex>* out = ts2.getOutputBuffer();

  for (Vertex v : *out) {
    cout << v << endl;
  }

  return 0;
}
