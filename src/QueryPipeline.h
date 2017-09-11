#ifndef RAMGRAPH_QUERYPIPELINE_H
#define RAMGRAPH_QUERYPIPELINE_H

#include <vector>

#include "Stage.h"

using namespace std;

namespace RAMGraph {

  class QueryPipeline {
    private:
      vector<Stage*> stages;
      vector<Vertex> starts;

    public:
      QueryPipeline()
          : stages()
          , starts() {

      }

      void addStart(Vertex v) {
        starts.push_back(v);
      }

      void addStage(Stage* stage) {
        if (!stages.empty()) {
          Stage* tailStage = stages.back();
          stage->setInputBuffer(tailStage->getOutputBuffer());
        } else {
          stage->setInputBuffer(&starts);
        }

        stages.push_back(stage);
      }

      void execute() {
        bool done = false;
        
        while (!done) {
          done = true;
          for (Stage* stage : stages) {
            done &= stage->advance();
          }
        }
      }
  }; // class QueryPipeline
} // namespace RAMGraph

#endif // RAMGRAPH_QUERYPIPELINE_H
