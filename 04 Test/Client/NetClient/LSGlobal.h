#ifndef	_LS_GLOBAL_H_
#define _LS_GLOBAL_H_

// ��������ͨѶ���ݽṹ
#pragma pack(push)      // �������״̬
#pragma pack(1)         // �趨Ϊ1�ֽڶ���

// ͷ���ݽṹ
typedef struct __PackHead
{
    unsigned int cmdCode;
    unsigned int length;
}PACKHEAD, *PPACKHEAD;

#pragma pack(pop)       // �ָ�����״̬

const int netClientSuccess      = 0;
const int netClientErrParam     = -1;
const int netClientErrTimeOut   = -2;
const int netClientErrDisconnect= -3;
const int netClientErrSocket    = -4;
const int netClientErrLength    = -5;

const int packHeadSize = sizeof(PACKHEAD);

typedef struct __RecvParam
{
    int     fd;
    char*   pbuf;
    int     buflen;
    int     recvlen;
    int     error;

    // ��ʼ��
    __RecvParam()
    {
        fd = 0;

        pbuf = nullptr;
        buflen = 0;

        recvlen = 0;

        error = netClientErrTimeOut;
    }

}RecvParam, *PRecvParam;


// recv data until break  or over
int readdata(int fd,char* pbuf,const int len);
int readdata(PRecvParam pRecvParam);

// send data until break  or over
int writedata(int fd,const char* pbuf,const int len);

#endif

