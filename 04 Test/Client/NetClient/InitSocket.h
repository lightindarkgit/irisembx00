#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")	           // ���ӵ�WS2_32.lib//˵����CE��PPC��ǰ�ߣ�WS2_32.lib����PC

//����ͨ�ų�ʼ����
class CInitSocket
{
public:
    CInitSocket(BYTE minorVer = 2, BYTE majorVer = 2)
    {
        // ��ʼ��WS2_32.dll
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
