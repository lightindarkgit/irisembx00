#ifndef _EXEC_TIME_H_
#define _EXEC_TIME_H_

#include <string>
#include <sys/time.h>

using namespace std;

#define		PRINT_USE_TIME	0.1

class Exectime
{
public:	
	Exectime(const char* pfuncname = NULL);
	virtual ~Exectime();

private:
	string _functionname;
	struct timeval  _tmvb;
	struct timeval  _tmve;
};

#endif

