#ifndef RAMGRAPH_RAMGRAPH_H                                                     
#define RAMGRAPH_RAMGRAPH_H

#include <string>

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
    public:
      string label;
      long id;

    public:
      VertexId(string label, long id) 
          : label(label)
          , id(id) {
      }

      bool operator==(const VertexId& rhs) const {
        return label == rhs.label && id == rhs.id;
      }
  };

  struct VertexIdHash {
    std::size_t operator()(VertexId const& vId) const {
      size_t h1 = hash<string>{}(vId.label);
      size_t h2 = hash<long>{}(vId.id);
      return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion)
    }
  };

  class Vertex {
    public:
      VertexId id;

    public:
      Vertex(VertexId id) 
          : id(id) {
      }

      Vertex(string label, long id) 
          : id(label, id) {
      }

      bool operator==(const Vertex& rhs) const {
        return id == rhs.id;
      }
  };

  struct VertexHash {
    std::size_t operator()(Vertex const& v) const {
      return VertexIdHash{}(v.id);
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
