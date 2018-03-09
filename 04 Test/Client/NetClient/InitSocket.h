#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")	           // 链接到WS2_32.lib//说明：CE及PPC用前者，WS2_32.lib用于PC

//网络通信出始化类
class CInitSocket
{
public:
    CInitSocket(BYTE minorVer = 2, BYTE majorVer = 2)
    {
        // 初始化WS2_32.dll
        WSADATA wsaData;
        WORD sockVersion = MAKEWORD(minorVer, majorVer);
        if (::WSAStartup(sockVersion, &wsaData) != 0)
        {
            exit(0);
        }
    }
    ~CInitSocket()
    {
        ::WSACleanup();
    }
};
