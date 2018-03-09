STATIC=1
VPATH=./lib
LIBSO=libsocketcomm.so
LIBA=libsocketcomm.a 
LIBPATH=lib
OBJPATH=obj
CPP=g++-4.8
CFLAGS=-Wall -g -pthread  -std=c++11
LDFLAGS= -fPIC -shared
AR=ar
ifeq ($(STATIC),0)
LIB:=$(LIBA)
#LIB=libsocketcomm.a
else
#LIB=libsocketcomm.so
LIB:=$(LIBSO)
endif

LIBSRC=$(wildcard *.cpp)  $(wildcard Client/*.cpp) $(wildcard Server/*.cpp)
LIBOBJ:=$(patsubst %.cpp,%.o,$(LIBSRC))
#DEPEND=$(LIBSRC:.cpp=.d)
all:$(LIB)
#%.d:%.cpp
#	@echo $(CPP)
#	@echo $(CFLAGS)
#	$(gen_dep)
$(LIBOBJ):%.o:%.cpp
	$(CPP) -c $(CFLAGS) $< -o $@ 
	@echo compileing obj file
#del warnning
#-include $(DEPEND)
#define gen_dep
#set -e; rm -f $@; \
#$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
#sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
#rm -f $@.$$$$
#endef 

ifeq ($(STATIC),0)
$(LIB):$(LIBOBJ)
	$(AR) -r $(LIB) $^
	@echo "building static lib success"
	@mv *.a $(LIBPATH)
	$(call endwork)
else
$(LIB):$(LIBOBJ)
	$(CPP)  $(LDFLAGS) -o $(LIB) $^
	@echo building dy lib success
#	@mv *.so $(LIBPATH)
	$(call endwork)
endif 
define endwork
	-@mv *.so lib
@mkdir -p obj
@mv $(LIBOBJ) $(OBJPATH)
	@echo makefile is end
endef
#delete obj file
.PHONY:clean
clean:
	rm -f $(LIBPATH)/* $(OBJPATH)/* $(DEPEND) 
