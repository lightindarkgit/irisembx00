## 编译连接本地postgresql数据库的示例程序
##

# The following variable is specific to postgresql

PGSQLINCLUDEDIR = -I/usr/include -I/usr/include/postgresql -I/usr/local/include/soci
PGSQLLIBDIR = -L/usr/lib/postgresql
PGSQLLIBS = -lpq

# The rest of the Makefile is indepentent of the target environment.

COMPILER = g++
CXXFLAGS = -Wall -g -pthread
SHARED_CXXFLAGS = ${CXXFLAGS} -fPIC
INCLUDEDIRS = ${PGSQLINCLUDEDIR}

SHARED_LIBDIRS = ${PGSQLLIBDIR}
SHARED_LIBS = ${PGSQLLIBS} ../../core/libsoci_core.a

LIBDIRS = ${PGSQLLIBDIR}
LIBS = -lsoci_core -lsoci_postgresql -ldl ${PGSQLLIBS}

UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	SHARED_LINK_FLAGS = -dynamiclib -flat_namespace -undefined suppress
else
	SHARED_LINK_FLAGS = -shared
endif

OBJECTS = testConnPsql.o

SHARED_OBJECTS = testConnPsql-s.o

testConnPsql : testConnPsql.cpp
	${COMPILER} -o $@ $? ${CXXFLAGS} ${INCLUDEDIRS} ${LIBDIRS} ${LIBS}

clean :
	rm -f test-postgresql
