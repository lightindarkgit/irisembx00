#include "Global.h"
#include "../Common/Exectime.h"


/* ********************************************************/
/**
 * @Name: exec_cmd	执行命令行
 *
 * @Param: cmd		命令行
 * @Param: output	输出结果
 *
 * @Returns:	0 成功 -1 失败
 */
/* ********************************************************/
int exec_cmd(const char* cmd,char* output)
{
	
    FILE* pipe = popen(cmd,"r");
	if(pipe == NULL)
	{
		return -1;
	}

	char buffer[129] = {0};
	while(!feof(pipe))
	{
		if(fgets(buffer,128,pipe))
		{
			strcat(output,buffer);
		}
	}

	pclose(pipe);
	return 0;
}

