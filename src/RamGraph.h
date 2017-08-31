#ifndef RAMGRAPH_RAMGRAPH_H                                                     
#define RAMGRAPH_RAMGRAPH_H

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace RAMGraph {

  class RamGraph;
  class Vertex;
  class Edge;

  class RamGraph {
    private:
      string coordinatorLocator;
      string graphName;

    public:
      RamGraph(string coordinatorLocator, string graphName);
  };

  enum EdgeType {
    DIRECTED,
    UNDIRECTED
  };

  enum EdgeDirection {
    IN,
    OUT,
    UN
  };

  class VertexId {
    private:
      string label;
      long id;

    public:
      VertexId(string label, long id) 
          : label(label)
          , id(id) {
      }
  };

  class Vertex {
    private:
      VertexId id;

    public:
      Vertex(VertexId id) 
          : id(id) {
      }
  };

  class Edge {
    private:
      VertexId vId1;
      VertexId vId2;
      EdgeType type;
      string eLabel;

    public:
      Edge(VertexId vId1, VertexId vId2, EdgeType type, string eLabel) 
          : vId1(vId1)
          , vId2(vId2)
          , type(type)
          , eLabel(eLabel) {

      }
  };

} // namespace RAMGraph

#endif // RAMGRAPH_RAMGRAPH_H
