#include "Exectime.h"
#include "Logger.h"

#include <stdio.h>
#include <stdlib.h>


Exectime::Exectime(const char* pfuncname)
{
	if(pfuncname != NULL)
	{
		_functionname = string(pfuncname);
        //LOG_DEBUG("Enter Func -------%s-------",_functionname.c_str());
	}
	else
	{
		_functionname = "";
	}

	gettimeofday(&_tmvb,NULL);
}

Exectime::~Exectime()
{
	gettimeofday(&_tmve,NULL);

	int64_t etms = (_tmve.tv_sec - _tmvb.tv_sec);
	int64_t etmus = (_tmve.tv_usec - _tmvb.tv_usec);

	double fetm = (double)(etms * 1000000 + etmus) / 1000000;

	if(fetm > PRINT_USE_TIME)
	{
        LOG_WARN("Exec Func[%s] Use Time: [%f s]",_functionname.c_str() ,fetm);
	}
	else
	{
        //LOG_DEBUG("Exec Func[%s] Use Time: [%f s]",_functionname.c_str() ,fetm);
	}

	if(_functionname.length() > 0)
	{
        //LOG_DEBUG("Leave Func -------%s-------",_functionname.c_str());
	}
}

