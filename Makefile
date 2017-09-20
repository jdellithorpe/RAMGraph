MAJOR_VERSION := 0
MINOR_VERSION := 0

RAMCLOUD_DIR := $(HOME)/projects/RAMCloud
RAMCLOUD_OBJ_DIR := $(RAMCLOUD_DIR)/obj.master

LIBRAMGRAPH := libramgraph.so
LIBRAMGRAPH_MAJOR := $(LIBRAMGRAPH).$(MAJOR_VERSION)
LIBRAMGRAPH_MAJOR_MINOR := $(LIBRAMGRAPH_MAJOR).$(MINOR_VERSION)

TARGETS := $(LIBRAMGRAPH) \
					 TestClient \
           ExampleClient \
					 Query10

LIBSRCFILES := $(wildcard src/*.cc)
LIBOBJFILES := $(patsubst src/%.cc, build/%.o, $(LIBSRCFILES))

all: $(TARGETS)

build/%.o: src/%.cc
	g++ $(FLAGS) -fPIC -shared -c -o $@ $^ -g -std=c++11 -I./src -I$(RAMCLOUD_DIR)/src -I$(RAMCLOUD_OBJ_DIR)

$(LIBRAMGRAPH_MAJOR_MINOR): $(LIBOBJFILES)
	g++ $(FLAGS) -fPIC -shared -o $@ $^ -Wl,-soname,$(LIBRAMGRAPH_MAJOR) -g -std=c++11 -I./src -I$(RAMCLOUD_DIR)/src -I$(RAMCLOUD_OBJ_DIR) -L$(RAMCLOUD_OBJ_DIR) -lramcloud

$(LIBRAMGRAPH_MAJOR): $(LIBRAMGRAPH_MAJOR_MINOR)
	ln -s -f $(LIBRAMGRAPH_MAJOR_MINOR) $(LIBRAMGRAPH_MAJOR)

$(LIBRAMGRAPH): $(LIBRAMGRAPH_MAJOR)
	ln -s -f $(LIBRAMGRAPH_MAJOR) $(LIBRAMGRAPH)

%: examples/%.cc
	g++ -c -o $@.o $^ -g -std=c++11 -I./src -I$(RAMCLOUD_DIR)/src -I$(RAMCLOUD_OBJ_DIR)
	g++ -o $@ $@.o -L. -L$(RAMCLOUD_OBJ_DIR) -lramgraph -lramcloud

install: uninstall
	ln -s -f $(realpath $(LIBRAMGRAPH_MAJOR_MINOR)) $(HOME)/local/lib/$(LIBRAMGRAPH_MAJOR)

uninstall:
	rm -f $(HOME)/local/lib/$(LIBRAMGRAPH_MAJOR)

clean:
	rm -f $(TARGETS) *.o build/* $(LIBRAMGRAPH_MAJOR) $(LIBRAMGRAPH_MAJOR_MINOR)
