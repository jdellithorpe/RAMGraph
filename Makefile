MAJOR_VERSION := 0
MINOR_VERSION := 0

RAMCLOUD_DIR := $(HOME)/projects/RAMCloud
RAMCLOUD_OBJ_DIR := $(RAMCLOUD_DIR)/obj.master

LIBRAMGRAPH := libramgraph.so
LIBRAMGRAPH_MAJOR := $(LIBRAMGRAPH).$(MAJOR_VERSION)
LIBRAMGRAPH_MAJOR_MINOR := $(LIBRAMGRAPH_MAJOR).$(MINOR_VERSION)

TARGETS := $(LIBRAMGRAPH_MAJOR_MINOR) \
					 $(LIBRAMGRAPH_MAJOR) \
					 $(LIBRAMGRAPH) \
           ExampleClient
						
all: $(TARGETS)

$(LIBRAMGRAPH_MAJOR_MINOR): src/*.cc src/*.h
	g++ -fPIC -shared -o $@ $^ -Wl,-soname,$(LIBRAMGRAPH_MAJOR) -g -std=c++0x -I./src -I$(RAMCLOUD_DIR)/src -I$(RAMCLOUD_OBJ_DIR) -L$(RAMCLOUD_OBJ_DIR) 

$(LIBRAMGRAPH_MAJOR): $(LIBRAMGRAPH_MAJOR_MINOR)
	ln -s $(LIBRAMGRAPH_MAJOR_MINOR) $(LIBRAMGRAPH_MAJOR)

$(LIBRAMGRAPH): $(LIBRAMGRAPH_MAJOR)
	ln -s $(LIBRAMGRAPH_MAJOR) $(LIBRAMGRAPH)

%: examples/%.cc
	g++ -o $@ $^ -g -std=c++0x -I./src -I. -L. -lramgraph

install: uninstall
	ln -s $(realpath $(LIBRAMGRAPH_MAJOR_MINOR)) $(HOME)/local/lib/$(LIBRAMGRAPH_MAJOR)

uninstall:
	rm $(HOME)/local/lib/$(LIBRAMGRAPH_MAJOR)

clean:
	rm -f $(TARGETS)
