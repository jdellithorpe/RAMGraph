#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "RamGraph.h"
#include "TraverseStage.h"
#include "UniqueTraverseStage.h"
#include "FilterStage.h"
#include "Query.h"

using namespace RAMGraph;

int main() {
  RamGraph graph("basic+udp:host=192.168.1.179,port=12246", "ldbc_snb_sf0001");

  Query query(&graph);

  query.addStart(Vertex{4, 933});

  unordered_set<Vertex, VertexHash> seenSet;

  UniqueTraverseStage ts1("knows", OUT, "Person", &seenSet);
  UniqueTraverseStage ts2("knows", OUT, "Person", &seenSet);
  UniqueTraverseStage ts3("knows", OUT, "Person", &seenSet);
//  FilterStage fs1([](unordered_map<string, vector<string>>& p) {
//      if (p["firstName"][0] == "Petros") {
//        return true;
//      } else {
//        return false;
//      }
//    });
//  TraverseStage ts2("knows", OUT, "Person");
//  TraverseStage ts3("knows", OUT, "Person");

  query.addStage(&ts1);
//  query.addStage(&ts2);
//  query.addStage(&ts3);

  graph.beginTx();
  query.execute();
  graph.abortTx();

  vector<Vertex>* out = ts1.getOutputBuffer();

  for (Vertex& v : *out) {
    cout << v << endl;
  }

  return 0;
}
