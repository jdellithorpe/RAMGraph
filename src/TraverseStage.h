#ifndef RAMGRAPH_TRAVERSESTAGE_H
#define RAMGRAPH_TRAVERSESTAGE_H

#include <vector>
#include <unordered_map>

#include "Stage.h"
#include "EdgeList.h"

using namespace std;

namespace RAMGraph {

  class TraverseStage : public Stage {
    private:
      string eLabel;
      EdgeDirection dir;
      string nLabel;
      vector<EdgeList> eLists;
      vector<Vertex>* inputBuffer;
      vector<Vertex> outputBuffer;
      unordered_map<Vertex, vector<Vertex>, VertexHash> travMap;

    public:
      TraverseStage(string eLabel, EdgeDirection dir, string nLabel) 
          : eLabel(eLabel)
          , dir(dir)
          , nLabel(nLabel)
          , eLists()
          , inputBuffer(NULL)
          , outputBuffer()
          , travMap() {

      }

      void setInputBuffer(vector<Vertex>* inputBuffer) {
        this->inputBuffer = inputBuffer; 
      }

      vector<Vertex>* getOutputBuffer() {
        return &outputBuffer;
      }

      void clearOutputBuffer() {
        outputBuffer.clear();
      }

      bool hasOutput() {
        return !outputBuffer.empty();
      }

      /* Advance this stage. Returns true if the stage is finished advancing,
       * false otherwise.
       */
      bool advance() {
        bool edgeListsDone = true;
        for (EdgeList list : eLists) {
          edgeListsDone &= list.advance();
          vector<Vertex>* lOutBuf = list.getOutputBuffer();
          for (Vertex v : *lOutBuf) {
            outputBuffer.push_back(v);
            travMap[list.getHomeVertex()].push_back(v);
          }
          list.clearOutputBuffer();
        }
        
        if (inputBuffer->size() > 0)
          edgeListsDone = false;

        for (Vertex v : *inputBuffer) {
          eLists.emplace_back(v, eLabel, dir, nLabel);
        }

        inputBuffer->clear();

        return edgeListsDone;
      }
  }; // class TraverseStage
} // namespace RAMGraph

#endif // RAMGRAPH_TRAVERSESTAGE_H
