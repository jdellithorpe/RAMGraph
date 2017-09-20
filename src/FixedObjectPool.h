#ifndef RAMGRAPH_FIXEDOBJECTPOOL_H
#define RAMGRAPH_FIXEDOBJECTPOOL_H

#include <vector>
#include <cstddef>
#include <cstdlib> 
#include <cstdio>
#include <cstring>
#include <iostream>

#include "Common.h"

using namespace std;

namespace RAMGraph {

  template <typename T>
  class FixedObjectPool
  {
    private:
      void* basePointer;
      uint32_t count;
      vector<void*> pool;

    public:
      FixedObjectPool(uint32_t count)
          : basePointer(NULL)
          , count(count)
          , pool() {
        size_t len = sizeof(T) * count;
        basePointer = (void*)malloc(len);
        if (basePointer == NULL) {
          fprintf(stderr, "FixedObjectPool(): malloc(%lu) failed for "
              "FixedObjectPool construction\n", len);
          exit(1);
        }

        cout << "setting memory to zero\n";
        memset(basePointer, 0, len);

        pool.reserve(count);

        for (uint32_t i = 0; i < count; i++) {
          pool.push_back((void*)((char*)basePointer + i * sizeof(T)));
        }
      }

      ~FixedObjectPool() {
        if (pool.size() != count) {
          fprintf(stderr, "~FixedObjectPool(): There were %d objects that "
              "were not returned to the free list\n", count - pool.size());
          exit(1);
        }

        free(basePointer);
      }

      template<typename... Args>
      T*
      construct(Args&&... args) {
        if (pool.size() == 0) {
          fprintf(stderr, "FixedObjectPool::construct(): Pool is out of "
              "memory!\n");
          exit(1);
        }

        void* backing = pool.back();
        pool.pop_back();

        T* object = NULL;
        try {
          object = new(backing) T(static_cast<Args&&>(args)...);
        } catch (...) {
          pool.push_back(backing);
          throw;
        }

        return object;
      }

      void
      destroy(T* object)
      {
        object->~T();
        pool.push_back(static_cast<void*>(object));
      }
  };

} // namespace RAMGraph

#endif  // RAMGRAPH_FIXEDOBJECTPOOL_H
