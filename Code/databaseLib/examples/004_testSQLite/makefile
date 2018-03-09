## 编译连接SQLite数据库的示例程序
##

# The following variable is specific to ODBC
# values might depend on your environment - feel free to set it accordingly.

SQLITE3INCLUDEDIR = -I/usr/include -I/usr/local/include/soci
SQLITE3LIBDIR = 
SQLITE3LIBS = -lsqlite3

# The rest of the Makefile is independent of the target environment.

COMPILER = g++
CXXFLAGS = -Wall -pedantic -Wno-long-long
INCLUDEDIRS = -I/usr/local/include/soci ${SQLITE3INCLUDEDIR}
LIBDIRS = ${SQLITE3LIBDIR}
LIBS = -lsoci_core -lsoci_sqlite3 -ldl ${SQLITE3LIBS}  


UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	SHARED_LINK_FLAGS = -dynamiclib -flat_namespace -undefined suppress
else
	SHARED_LINK_FLAGS = -shared
endif

OBJECTS = main.o

SHARED_OBJECTS = main-s.o

testSQLite : main.cpp
	${COMPILER} -o $@ $? ${CXXFLAGS} ${INCLUDEDIRS} ${LIBDIRS} ${LIBS}

clean :
	rm -f main testSQLite
