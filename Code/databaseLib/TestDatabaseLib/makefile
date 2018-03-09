## 编译连接本地postgresql数据库的示例程序
##

# The following variable is specific to postgresql

PGSQLINCLUDEDIR = -I/usr/include -I/usr/include/postgresql
PGSQLLIBDIR     = -L/usr/lib/postgresql
PGSQLLIBS       = -lpq

SOCIINCLUDER    =  -I/usr/local/include/soci
SOCILIBDIR      = -L/usr/local/lib
SOCILIBS        = -lsoci_core -ldl -lsoci_postgresql -lsoci_sqlite3

SQLITE3LIBS     = -lsqlite3

IKINCLUDER      = -I../IKDatabaseLib/include -Iinclude
IKLIBDIR	= -L../../lib
IKLIBS          = -ldatabase

# The rest of the Makefile is indepentent of the target environment.

COMPILER = g++
CXXFLAGS = -Wall -g -std=c++11 -pthread

SHARED_CXXFLAGS = ${CXXFLAGS} -fPIC

INCLUDEDIRS     = ${PGSQLINCLUDEDIR} ${SOCIINCLUDER} ${IKINCLUDER}

SHARED_LIBDIRS  = ${PGSQLLIBDIR} ${SOCILIBDIR} ${IKLIBDIR}
SHARED_LIBS     = ${PGSQLLIBS} ${SOCILIBS} ${SQLITE3LIBS} ${IKLIBS}

LIBDIRS         = ${PGSQLLIBDIR} ${SOCILIBDIR} ${IKLIBDIR}
LIBS            = ${PGSQLLIBS} ${SOCILIBS} ${SQLITE3LIBS} ${IKLIBS} -luuid

UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	SHARED_LINK_FLAGS = -dynamiclib -flat_namespace -undefined suppress
else
	SHARED_LINK_FLAGS = -shared
endif

#OBJECTS = main-linux.o

#SHARED_OBJECTS = main-linux-s.o

#testdblib : ${OBJECTS}
#	${COMPILER} -o testdblib ${OBJECTS}

testdb : src/main-linux.cpp
	${COMPILER} -o $@ $? ${CXXFLAGS} ${INCLUDEDIRS} ${LIBDIRS} ${LIBS}

clean :
	rm -f ${OBJECTS} testdb
