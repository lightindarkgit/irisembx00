/******************************************************************************************
** �ļ���:   CameraBase.h
���� ��Ҫ��:   CCameraBase
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2014-01-15
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ��������ͷ����ʵ�֣����ⲻ�ɼ�
 *���� ��Ҫģ��˵��: CameraBase.h
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <mutex>
#include <string.h>

#include "AlgApi.h"
#include "BRCCommon.h"

//��������ͷ����
class CCameraBase
{
public:
    ~CCameraBase();
    //��ȡ��ʵ��ָ��
    static CCameraBase* GetInstance();

    //��ʼ���ɼ�����ͼ������ͷ
    int		Init(OUT char *strFunRts);
    //�ɼ�����ͼ��
    int		CapImg(OUT unsigned char *pFaceImg, OUT char *strFunRts);
    //�ͷŲɼ�����ͼ������ͷ�����Դ
    void	Release();

private:
    CCameraBase();
    CCameraBase(const CCameraBase&);//��ֹ������ֵ
    CCameraBase& operator=(const CCameraBase&);

    int OpenDevice();       //���豸
    int CloseDevice();      //�ر��豸
    int InitDevice();       //��ʼ���豸
    int StartCapturing();   //��ʼ�ɼ�
    int StopCapturing();    //ֹͣ�ɼ�

    int UninitDevice();     //����ʼ���豸
    int GetFrame(void **, size_t*);//���������
    int UngetFrame();       //�������������
    int InitMmap();

    int ConvertYuv2RgbPixel(int y, int u, int v);//����ת����rgb��ʽ
    int ConvertYuv2RgbBuffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);//���ɼ�����yuvͼ��ת����rgb��ʽͼ��

private:
//    static shared_ptr<CAPIBase> s_pSingletonCAPIBase;
    static CCameraBase *s_pSingletonCCameraBase;                //��ʵ������ָ��
    static std::mutex s_instanceCameraMutex;                   //��ȡʵ��mutex

    struct buffer
    {
        void * start;
        size_t length;
    };

    int _fd;                    //�豸�ļ���������
    buffer* _buffers;
    unsigned int _nBuffers;     //buffer����
    int _index;                 //v4l2_buffer������
    int _result;                //����ֵ
    v4l2_buffer _queueBuf;      //���ڽ���ͼ���buffer
};
