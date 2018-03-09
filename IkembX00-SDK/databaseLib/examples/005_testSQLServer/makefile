## 编译连接SQLServer数据库的示例程序
##

# The following variable is specific to ODBC
# values might depend on your environment - feel free to set it accordingly.

ODBCINCLUDEDIR = -I/usr/include
ODBCLIBDIR = -L/usr/lib
ODBCLIBS = -lodbc -lodbcinst

# The rest of the Makefile is independent of the target environment.

COMPILER = g++
CXXFLAGS = -Wall -pedantic -Wno-long-long
INCLUDEDIRS = -I/usr/local/include/soci ${ODBCINCLUDEDIR}
LIBDIRS = -L.. -L../../../core ${ODBCLIBDIR}
LIBS = -lsoci_core -lsoci_odbc -ldl ${ODBCLIBS}  


UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	SHARED_LINK_FLAGS = -dynamiclib -flat_namespace -undefined suppress
else
	SHARED_LINK_FLAGS = -shared
endif

OBJECTS = main.o

SHARED_OBJECTS = main-s.o

testSQLServer : main.cpp
	${COMPILER} -o $@ $? ${CXXFLAGS} ${INCLUDEDIRS} ${LIBDIRS} ${LIBS}

clean :
	rm -f main testSQLServer
