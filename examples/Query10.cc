#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <ctime> 
#include <iomanip>

#include "RamGraph.h"
#include "TraverseStage.h"
#include "UniqueTraverseStage.h"
#include "FilterStage.h"
#include "DedupStage.h"
#include "SinkStage.h"
#include "Query.h"
#include "Cycles.h"

using namespace std;
using namespace RAMGraph;

int main() {
  RamGraph graph("basic+udp:host=192.168.1.179,port=12246", "ldbc_snb_sf0001");

  for (int i = 0; i < 10; i++) {
    Query query(&graph);

    query.addStart(Vertex{4, 4398046514187});

    unordered_set<Vertex, VertexHash> seenSet;

    UniqueTraverseStage ts1("knows", OUT, "Person", &seenSet);
    UniqueTraverseStage ts2("knows", OUT, "Person", &seenSet);
    DedupStage ds1;
    FilterStage fs1([](unordered_map<string, vector<string>>& p) {
        uint64_t bday_ms;
        istringstream iss(p["birthday"][0]);
        iss >> bday_ms;
        time_t bday_tt = static_cast<time_t>(bday_ms/1000);
        tm* bday_tm = gmtime(&bday_tt);
        int bmonth = bday_tm->tm_mon; // [0,11]
        int bday = bday_tm->tm_mday; // [1,31]
        int month = 8 - 1;
        if ((bmonth == month && bday >= 21) || 
            (bmonth == ((month + 1) % 12) && bday < 22)) {                
          return true;                                                  
        }                                                               
        return false;
      });
    TraverseStage ts3("hasCreator", IN, "Post");
    TraverseStage ts4("hasTag", OUT, "Tag");
    SinkStage ss1;

    query.addStage(&ts1);
    query.addStage(&ts2);
    query.addStage(&ds1);
    query.addStage(&fs1);
    query.addStage(&ts3);
    query.addStage(&ts4);
    query.addStage(&ss1);

    uint64_t start = Cycles::rdtsc();
    graph.beginTx();
    query.execute();
    graph.abortTx();
    uint64_t end = Cycles::rdtsc();

    uint64_t millis = Cycles::toMicroseconds(end - start);
    cout << "Time: " << millis/1000 << "ms" << endl;

//    vector<Vertex>* out = fs1.getOutputBuffer();
//
//    for (Vertex& v : *out) {
//      cout << v << endl;
//    }
  }

  return 0;
}
