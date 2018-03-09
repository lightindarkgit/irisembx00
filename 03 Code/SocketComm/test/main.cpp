#include "../socketAPI.h"
#include "../socketInterfaceAPI.h"
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class TestFun
{
public:
	TestFun(){};
	~TestFun(){};
public:
	void Mytest(char *buf,int len)
	{
		int i = 1;
		printf ("this is test");
	}
};

int main()
{
	ClientData cd;
	strcpy(cd.Addr,"10.2.1.107");
	TestFun t;
	
	ISocketAPI *p = ISocketAPI::CreateInstance(std::bind(&TestFun::Mytest,&t,std::placeholders::_1,std::placeholders::_2),cd);
	if (NULL == p)
	{
		printf ("test is error");
	}

	char buf[10] = "is test";
	int iRet = 1;
	iRet = p->Start();
	printf("return value is %d\n",iRet);
	if (iRet > 0)
	{
		printf("start send....");
		int sendnum = p->SendData(buf,10);
		printf("sleep 5000...%d\n",sendnum);
		sleep(3);
	}
	else
	{
		printf("ERROR");
	}

return 0;

//       while (1){};
}
