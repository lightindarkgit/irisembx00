# The following variable is specific to this backend and its correct
# values might depend on your environment - feel free to set it accordingly.

PGSQLINCLUDEDIR = -I/usr/include -I/usr/include/postgresql
PGSQLLIBDIR = -L/usr/lib/postgresql
PGSQLLIBS = -lpq 
SQLITE3LIBS = -lsqlite3

# The rest of the Makefile is indepentent of the target environment.

COMPILER = g++
CXXFLAGS = -Wall -std=c++11 -pedantic -Wno-long-long
SHARED_CXXFLAGS = ${CXXFLAGS} -fPIC
INCLUDEDIRS = ${PGSQLINCLUDEDIR} -I/usr/local/include/soci -Iinclude

SHARED_LIBDIRS = ${PGSQLLIBDIR} -L/usr/local/lib
SHARED_LIBS = ${PGSQLLIBS} ${SQLITE3LIBS}

UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	SHARED_LINK_FLAGS = -dynamiclib -flat_namespace -undefined suppress
else
	SHARED_LINK_FLAGS = -shared
endif

OBJECTS = IKDatabaseLib.o IKDatabaseSession.o IKPsqlSession.o IKSqliteSession.o

SHARED_OBJECTS = IKDatabaseLib-s.o IKDatabaseSession-s.o IKPsqlSession-s.o IKSqliteSession-s.o


libdatabase.a : ${OBJECTS}
	ar rv $@ $?
	rm *.o


IKDatabaseLib.o : src/IKDatabaseLib.cpp
	${COMPILER} -c $? ${CXXFLAGS} ${INCLUDEDIRS}

IKDatabaseSession.o : src/IKDatabaseSession.cpp
	${COMPILER} -c $? ${CXXFLAGS} ${INCLUDEDIRS}

IKPsqlSession.o : src/IKPsqlSession.cpp
	${COMPILER} -c $? ${CXXFLAGS} ${INCLUDEDIRS}

IKSqliteSession.o : src/IKSqliteSession.cpp
	${COMPILER} -c $? ${CXXFLAGS} ${INCLUDEDIRS}


shared : ${SHARED_OBJECTS}
	${COMPILER} ${SHARED_LINK_FLAGS} -o libdatabase.so \
		${SHARED_OBJECTS} ${SHARED_LIBDIRS} ${SHARED_LIBS}
	rm *.o

IKDatabaseLib-s.o : src/IKDatabaseLib.cpp
	${COMPILER} -c -o $@ $? ${SHARED_CXXFLAGS} ${INCLUDEDIRS}

IKDatabaseSession-s.o : src/IKDatabaseSession.cpp
	${COMPILER} -c -o $@ $? ${SHARED_CXXFLAGS} ${INCLUDEDIRS}

IKPsqlSession-s.o : src/IKPsqlSession.cpp
	${COMPILER} -c -o $@ $? ${SHARED_CXXFLAGS} ${INCLUDEDIRS}

IKSqliteSession-s.o : src/IKSqliteSession.cpp
	${COMPILER} -c -o $@ $? ${SHARED_CXXFLAGS} ${INCLUDEDIRS}

clean :
	rm -f libdatabase.a libdatabase.so *.o
