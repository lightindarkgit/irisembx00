OUTDIR = ./obj

EXECUTABLE := ./bin/myshell

CFLAGS = -std=c++11 -O3 -pthread -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE
CXXFLAGS = $(CFLAGS)

VPATH = .

Q = @

ifneq ($(wildcard $(DJDIR)/bin/rm),)
RM-F := rm -f
else
RM-F := del
endif

SOURCE := $(notdir $(wildcard *.cpp))
OBJS := $(patsubst %.cpp, $(OUTDIR)/%.o, $(SOURCE))
DEPS := $(patsubst %.o, %.d, $(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)), $(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst $(OUTDIR)/%.d, %.cpp, $(MISSING_DEPS)))
CXXFLAGS += -MMD

.PHONY: prepare everything deps objs clean veryclean rebuild execlean relink

everything: prepare $(EXECUTABLE)

deps: $(DEPS)

$(OUTDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC $(addprefix -I, $(INCLUDE)) -c -o $@ $<

prepare:
	$(Q)if [ ! -d obj ]; then mkdir -p obj ; fi;
	$(Q)if [ ! -d bin ]; then mkdir -p bin ; fi;

objs: $(OBJS)

clean:
	$(RM-F) $(OUTDIR)/*.o
	$(RM-F) $(OUTDIR)/*.d

execlean:
	$(RM-F) $(EXECUTABLE)

veryclean: clean execlean
	$(RM-F) -rf bin
	$(RM-F) -rf obj

rebuild: veryclean
	$(MAKE) everything
relink: execlean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS):
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(addprefix -L, $(LIBPATH)) $(addprefix -l, $(LIBS))

##=== makefile end ===

