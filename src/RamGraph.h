#ifndef RAMGRAPH_RAMGRAPH_H                                                     
#define RAMGRAPH_RAMGRAPH_H

#include <string>

#include "RamCloud.h"
#include "Transaction.h"

using namespace std;
using namespace RAMCloud;

namespace RAMGraph {

  class RamGraph;
  class Vertex;
  class Edge;

  class RamGraph {
    private:
      string coordinatorLocator;
      string graphName;
      RamCloud client;

    public:
      Transaction tx;
      uint64_t vertexTableId;
      uint64_t edgeListTableId;

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
//      string label;
//      uint64_t id;
      uint64_t upper;
      uint64_t lower;

    public:
//      VertexId(string label, uint64_t id) 
//          : label(label)
//          , id(id) {
//      }
      
      VertexId(uint64_t upper, uint64_t lower) 
          : upper(upper) 
          , lower(lower) {
      }

//      bool operator==(const VertexId& rhs) const {
//        return label == rhs.label && id == rhs.id;
//      }

      bool operator==(const VertexId& rhs) const {
        return lower == rhs.lower && upper == rhs.upper;
      }
  };

//  struct VertexIdHash {
//    std::size_t operator()(VertexId const& vId) const {
//      size_t h1 = hash<string>{}(vId.label);
//      size_t h2 = hash<uint64_t>{}(vId.id);
//      return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion)
//    }
//  };

  struct VertexIdHash {
    std::size_t operator()(VertexId const& vId) const {
      size_t h1 = hash<uint64_t>{}(vId.lower);
      size_t h2 = hash<uint64_t>{}(vId.upper);
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

//      Vertex(string label, uint64_t id) 
//          : id(label, id) {
//      }

      Vertex(uint64_t upper, uint64_t lower) 
          : id(upper, lower) {
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
