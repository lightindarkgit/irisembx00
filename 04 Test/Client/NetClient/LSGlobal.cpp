#include "LSGlobal.h"
#include "LSHeaders.h"

DWORD WINAPI ReceiveWorkerThread(LPVOID lpParam);

int readdata(PRecvParam pRecvParam)
{
    assert(pRecvParam != NULL);
    assert(pRecvParam->pbuf != NULL);

    // 等待10毫秒
    DWORD waitMilliSeconds = 60 * 1000;

    // Start a thread that will perform the main task of the service
    HANDLE hThread = CreateThread(NULL, 0, ReceiveWorkerThread, (LPVOID)pRecvParam, 0, NULL);

    WaitForSingleObject(hThread, waitMilliSeconds);

    return pRecvParam->error;
}

DWORD WINAPI ReceiveWorkerThread(LPVOID lpParam)
{
    PRecvParam pParam = (PRecvParam)lpParam;

    int haveRecvedLen = 0;
    int nret = 0;
    int needrecvlen = 0;

    int fd = pParam->fd;
    char* pbuf = pParam->pbuf;
    const int len = pParam->buflen;

    //  Periodically check if the service has been requested to stop
    while (1)
    {
        // 接收数据
        nret = recv(fd, pbuf + haveRecvedLen, len - haveRecvedLen, 0);

        // 如果nret==0，对方关闭
        if (nret == 0)
        {
            // break by  peer                                                         
            pParam->error = netClientErrDisconnect;
            return ERROR_SUCCESS;
        }
        else if (nret < 0)
        {
            // 小于0，可能出错
            if (errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            pParam->error = netClientErrSocket;

            return ERROR_SUCCESS;
        }

        haveRecvedLen += nret;

        // 解出头部
        if (haveRecvedLen >= 8)
        {
            PPACKHEAD phd = (PPACKHEAD)pbuf;
            needrecvlen = phd->length + packHeadSize;
            if ((needrecvlen) > len)
            {
                // 需要接受的长度大于预先给出的buffer的长度
                // 返回错误，长度不够，返回的长度表示实际需要分配的长度
                pParam->recvlen = needrecvlen;
                pParam->error = netClientErrLength;

                return ERROR_SUCCESS;
            }
        }

        if ((needrecvlen != 0)
            && (needrecvlen <= haveRecvedLen))
        {
            // 接收完毕
            // break;
            pParam->recvlen = needrecvlen;
            pParam->error = netClientSuccess;
            return ERROR_SUCCESS;

        }
    }

    return ERROR_SUCCESS;
}

int writedata(int fd,const char* pbuf,const int len)
{
	int needsendlen = 0;                                                     
	int nret = 0;
	while(needsendlen < len)
	{
		nret = send(fd,pbuf + needsendlen,len - needsendlen,0);
		if(nret < 0)                                                         
		{                                                                    
			if(errno == EINTR || errno == EAGAIN)                            
			{                                                                
				continue;                                                    
			}                                                                
			return nret;                                                     
		}                                                                    

		needsendlen += nret;                                                 
	}                                                                        

	return needsendlen;
}

