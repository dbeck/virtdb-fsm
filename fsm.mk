BUILD_ROOT           := $(shell pwd)

# FIXME on Windows
FIX_CXX_11_BUG =
LINUX_LDFLAGS =
ifeq ($(shell uname), Linux)
FIX_CXX_11_BUG  = -Wl,--no-as-needed
LINUX_LDFLAGS   = -pthread
SODIUM_LDFLAGS  += -lrt
endif

BASIC_FSM_INCLUDES := -I$(BUILD_ROOT)/src/. 

ifeq ($(RELEASE), 1)
$(info DOING RELEASE BUILD)
CXXFLAGS += -Wall -O3 -DNDEBUG -DRELEASE -std=c++11 -fPIC $(FIX_CXX_11_BUG) $(LINUX_LDFLAGS) $(BASIC_FSM_INCLUDES)
LDFLAGS += $(FIX_CXX_11_BUG) $(LINUX_LDFLAGS) 
CFLAGS += -Wall -O3 -DNDEBUG -DRELEASE -fPIC
else
$(info DOING DEBUG BUILD)
CXXFLAGS += -Wall -g3 -std=c++11 -fPIC $(FIX_CXX_11_BUG) $(LINUX_LDFLAGS) $(BASIC_FSM_INCLUDES)
LDFLAGS += -g3 $(FIX_CXX_11_BUG) $(LINUX_LDFLAGS) 
CFLAGS += -Wall -g3 -fPIC
endif

$(info $$CFLAGS is [${CFLAGS}])
$(info $$CXXFLAGS is [${CXXFLAGS}])
$(info $$LDFLAGS is [${LDFLAGS}])

FSM_SRCS           := $(wildcard src/fsm/*.cc)
FSM_OBJECTS        := $(patsubst %.cc,%.o,$(FSM_SRCS))

FSM_LIB := libfsm.a

all: $(FSM_LIB)

$(FSM_LIB): $(FSM_OBJECTS)
	ar rcsv $(FSM_LIB) $(FSM_OBJECTS)

clean: 
	rm -f $(FSM_LIB) $(FSM_OBJECTS) 
	rm -f *.a *.o 
	@echo "checking for suspicious files"
	@find . -type f -name "*.so"
	@find . -type f -name "*.a"
	@find . -type f -name "*.o"
