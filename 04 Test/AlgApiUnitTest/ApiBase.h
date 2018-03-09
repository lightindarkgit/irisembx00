/******************************************************************************************
** �ļ���:   AlgApi_Private.h
���� ��Ҫ��:   CAPIBase,APILocker
**  
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2013-11-11
** �޸���:   
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ�㷨�߼�API�ڲ�ʹ�õ���Դ����ض��壬���ⲻ�ɼ�
 *���� ��Ҫģ��˵��: AlgApi_Private.h                        
**
** ��  ��:   1.0.0
** ��  ע:   
**
*****************************************************************************************/
#pragma once

#include "IK_IR_DLL.h"
#include "BRCCommon.h"
#include "AlgApi.h"
#include "ApiGetFlag.h"

#include "syncusbapi.h"

#include <sys/time.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unistd.h>

//using namespace std;
//using std::tr1::shared_ptr;


const int g_constMaxENRTime         =	20;		//Ŀǰʵ�ַ�ʽ�ĵ�λ���룬ע����̳�����ʱ��ᱨע�ᳬʱ��AlgApiCtrlParam��MaxENRTime��Ĭ��ֵ
const int g_constImgNumToSelect =       1;//3;      //�Ӽ���ͼ����ѡ��һ���ϸ�ĺ�Ĥͼ�������㷨
const int g_constImgNumToInform =       3;      //ÿ���õ�����ͼ����ۺ��⼸��ͼ���λ����Ϣ��ͼ���Ƿ�������Ϣ������֪ͨ�ϲ�APP
const int g_constImgNumOfClear =        1;      //����ͼ�����
const int g_constIdenFailedTolerantNum =3;      //ʶ��ʧ�����������������ֵ��ʶ��ʧ��



class CAPIBase
{
public:    
    ~CAPIBase();

    //��ȡ��ʵ��ָ��
    static CAPIBase* GetInstance();
//    static shared_ptr<CAPIBase> GetInstance();
    //��ȡ�豸��õĺ�Ĥͼ����Ϣ�ĳߴ���Ϣ
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo, OUT char *strFunRts);
    //��ʼ���㷨�߼�APIʹ�õ������Դ
    int		Init(OUT char *strFunRts);
    //�ͷ��㷨�߼�APIʹ�õ������Դ
//    int 	Release(OUT char *strFunRts);

    //ʹ�㷨�߼�API��������״̬�����ɼ���Ĥͼ�񣬵����ͷ������Դ
    int		Sleep(OUT char *strFunRts);
    //ʹ�㷨�߼�API������״̬���ѣ��ָ���������״̬
    int		Wake(OUT char *strFunRts);
    //��ȡ�㷨�߼�API���Ʋ���
    int		GetParam(OUT AlgApiCtrlParam *paramStruct, OUT char *strFunRts);
    //�����㷨�߼�API���Ʋ���
    int		SetParam(IN AlgApiCtrlParam *paramStruct, OUT char *strFunRts);

    //ͬ����ʽ�ɼ���Ĥͼ��
    int		SyncCapIrisImg(OUT unsigned char *pIrisImg, OUT LRIrisClearFlag &lrIrisClearFlag, OUT IrisPositionFlag &irisPositionFlag, OUT char *strFunRts);
    //�첽��ʽ�ɼ���Ĥͼ��
    int		AsyncCapIrisImg(IN CBAlgApiCapIrisImg appCapIrisImg, OUT char *strFunRts);

    //ͬ����ʽ��Ĥע��
    int		SyncStartEnroll(OUT APIIrisInfo *enrIrisL, IN int numEnrL, OUT APIIrisInfo *enrIrisR,  IN int numEnrR, OUT LRSucFailFlag &lrFlag, OUT char *strFunRts);
    //�첽��ʽ��Ĥע��
    int		AsyncStartEnroll(IN CBAlgApiEnroll appEnroll, IN int numEnrL, IN int numEnrR, OUT char *strFunRts);

    //ֹͣ��Ĥע��
    int		StopEnroll(OUT char *strFunRts);

    //ͬ����ʽ��Ĥʶ��
    int		SyncStartIden(IN unsigned char *codeListL, IN int codeNumL,  IN unsigned char *codeListR, IN int codeNumR,
                      OUT APIIrisInfo *recIrisL, OUT APIIrisInfo *recIrisR, OUT LRSucFailFlag &lrFlag,
                      OUT APIMatchOutput *pLeftMatchOutputParam, OUT APIMatchOutput *pRightMatchOutputParam, OUT char *strFunRts);
    //�첽��ʽ��Ĥʶ��
    int		AsyncStartIden(IN CBAlgApiIden appIden, IN unsigned char *codeListL, IN int codeNumL, IN unsigned char *codeListR, IN int codeNumR, OUT char *strFunRts);

    //ֹͣ��Ĥʶ��
    int		StopIden(OUT char *strFunRts);
    //��Ĥʶ������ģ��ȶ�
    int		Match(IN unsigned char *recCode, IN int recNum, IN unsigned char *codeList, IN int codeNum, OUT APIMatchOutput *pMatchOutputParam, OUT char *strFunRts);

private:
    CAPIBase();
    CAPIBase(const CAPIBase&);//��ֹ������ֵ
    CAPIBase& operator=(const CAPIBase&);

    void    InitCapCBSrc(void);                        //��ʼ���첽��ͼ���������Դ
    void    InitEnrCBSrc(void);                        //��ʼ���첽ע�ắ�������Դ
    void    InitIdenCBSrc(void);                       //��ʼ���첽ʶ���������Դ

    static void* ThreadMain(IN CAPIBase* pParam);               //���߳�
    static void* ThreadAsyncCaptureImg(IN CAPIBase* pParam);    //��ͼ�̣߳��첽��ͼʱʹ��
    static void* ThreadAsyncEnroll(IN CAPIBase* pParam);    //ע���̣߳��첽ע��ʱʹ��
    static void* ThreadAsyncIden(IN CAPIBase* pParam);      //ʶ���̣߳��첽ʶ��ʱʹ��

private:
//    static shared_ptr<CAPIBase> s_pSingletonCAPIBase;
    static CAPIBase *s_pSingletonCAPIBase;
//    static APILocker s_instanceLocker;              //��ȡʵ����
    static std::mutex s_instanceMutex;                   //��ȡʵ��mutex

    std::mutex _xferCurrentImgMutex;                     //������ǰ�ɼ�ͼ��mutex
    std::condition_variable _xferCurrentImgCV;           //������ǰ�ɼ�ͼ��condition_variable

    std::mutex _xferBestImgMutex;                        //�����ɼ������ͼ��mutex
    std::condition_variable _xferBestImgCV;              //�����ɼ������ͼ��condition_variable

    std::mutex _wakeMainThreadMutex;                     //�������߳�mutex
    std::condition_variable _wakeMainThreadCV;           //�������߳�condition_variable

    std::mutex _setEnrRecMutex;                          //����ע�ᡢʶ���־λmutex

    std::mutex _asyncCapImgMutex;                        //�첽��ͼ�߳�mutex
    std::condition_variable _asyncCapImgCV;              //�첽��ͼ�߳�condition_variable
    bool _asyncCapImgFlag;

    std::mutex _asyncEnrMutex;                           //�첽ע���߳�mutex
    std::condition_variable _asyncEnrCV;                 //�첽ע���߳�condition_variable
    bool _asyncEnrFlag;                                  //�첽ע���߳�flag

    std::mutex _asyncIdenMutex;                          //�첽ʶ���߳�mutex
    std::condition_variable _asyncIdenCV;                //�첽ʶ���߳�condition_variable
    bool _asyncIdenFlag;                                 //�첽ʶ���߳�flag

private:
    //�ͻص���ز���
    CBAlgApiCapIrisImg _cbCapIrisImg;               //��ͼ�ص�����
//    unsigned char *_cbIrisImg;                      //���ڽ����첽��ͼ������������ͼ��ָ��
//    LRIrisClearFlag *_cbLRIrisClearFlag;            //���ڽ����첽��ͼ������������ͼ���Ƿ�������־
//    IrisPositionFlag *_cbIrisPositionFlag;          //���ڽ����첽��ͼ������������ͼ��λ����Ϣ��־
    unsigned char _cbIrisImg[g_constIrisImgSize];   //�����첽��ͼ����ͼ��
    LRIrisClearFlag _cbLRIrisClearFlag;             //�����첽��ͼ��ͼ���Ƿ�������־
    IrisPositionFlag _cbIrisPositionFlag;           //�����첽��ͼ��ͼ��λ����Ϣ��־

    CBAlgApiEnroll _cbEnroll;                       //ע��ص�����
//    APIIrisInfo *_cbEnrIrisL;                       //���ڽ����첽ע�ắ��������������ע���Ĥ��Ϣ�ṹ��ָ��
    APIIrisInfo _cbEnrIrisL[g_constMaxEnrollImNum]; //�����첽ע�ắ��������ע���Ĥ��Ϣ�ṹ��
    int _cbNumEnrL;                                 //���ڽ����첽ע�ắ�������������ۺ�Ĥ����
//    APIIrisInfo *_cbEnrIrisR;                       //���ڽ����첽ע�ắ��������������ע���Ĥ��Ϣ�ṹ��ָ��
    APIIrisInfo _cbEnrIrisR[g_constMaxEnrollImNum]; //�����첽ע�ắ��������ע���Ĥ��Ϣ�ṹ��
    int _cbNumEnrR;                                 //���ڽ����첽ע�ắ�������������ۺ�Ĥ����
//    LRSucFailFlag *_cbEnrLRFlag;                    //���ڽ����첽ע�ắ���������ĳɹ�ʧ�ܱ�־
    LRSucFailFlag _cbEnrLRFlag;                     //�����첽ע�ắ���ĳɹ�ʧ�ܱ�־

    CBAlgApiIden _cbIden;                           //ʶ��ص�����
    unsigned char *_cbCodeListL;                    //���ڽ����첽ʶ����������������ע������ģ��
    int _cbCodeNumL;                                //���ڽ����첽ʶ����������������ע������ģ������
    unsigned char *_cbCodeListR;                    //���ڽ����첽ʶ����������������ע������ģ��
    int _cbCodeNumR;                                //���ڽ����첽ʶ����������������ע������ģ������
//    LRSucFailFlag *_cbIdenLRFlag;                   //���ڽ����첽ʶ�����������ĳɹ�ʧ�ܱ�־
    LRSucFailFlag _cbIdenLRFlag;                    //�����첽ʶ�����ĳɹ�ʧ�ܱ�־

//    APIIrisInfo *_cbRecIrisL;                       //���ڽ����첽ʶ����������������ʶ���Ĥ��Ϣ�ṹ��ָ��
//    APIIrisInfo *_cbRecIrisR;                       //���ڽ����첽ʶ����������������ʶ���Ĥ��Ϣ�ṹ��ָ��
//    APIMatchOutput *_cbLeftMatchOutputParam;        //���ڽ����첽ʶ��������������������ȶ���Ϣ
//    APIMatchOutput *_cbRightMatchOutputParam;       //���ڽ����첽ʶ��������������������ȶ���Ϣ
    APIIrisInfo _cbRecIrisL;                       //�����첽ʶ����������ʶ���Ĥ��Ϣ�ṹ��ָ��
    APIIrisInfo _cbRecIrisR;                       //�����첽ʶ����������ʶ���Ĥ��Ϣ�ṹ��ָ��
    APIMatchOutput _cbLeftMatchOutputParam;        //�����첽ʶ��������������ȶ���Ϣ
    APIMatchOutput _cbRightMatchOutputParam;       //�����첽ʶ��������������ȶ���Ϣ

private:
    SyncUSBAPI *_syncUsbApi;                        //USBͬ���ɼ�ͼ����

private:
    HANDLE _algHandle;                              //�㷨ʹ�õľ��
    AlgApiCtrlParam _algApiCtrlParam;               //�㷨�߼����Ʋ���
    int _qualityMode;                               //ϵͳʹ��ʱ��ͼ��������Ҫ�󣬿������Ϊԭ����ע��ģʽ����ʶ��ģʽ
    int _instanceNum;                               //ʵ������

    unsigned char _pCapImg[g_constIrisImgSize];     //Ϊ�ɼ�����ͼ�����ռ�
    unsigned char *_pSwapImg;                       //�������û���ͼ��ָ��
    EyeDetectionInfo _eyeDetectRet;                 //Ϊ�㷨������۾����ṹ��
    LRIrisClearFlag _lrIrisClearFlag;               //������ͼ���Ƿ�ϸ��־λ
    IrisPositionFlag _irisPositionFlag;             //��Ĥλ����Ϣ��־λ

    bool _keepIdenFlag;                             //����ʶ��״̬
    bool _outIdenFlag;                              //ʶ�����˳���־
    bool _keepEnrFlag;                              //����ע��״̬
    bool _outEnrFlag;                               //ע�ắ���˳���־

    bool _mainThreadFlag;                           //��ʾ���߳��Ƿ��ڹ���״̬
    bool _isWake;                                   //��ʾ�Ƿ���wake״̬

    bool _swapNewImgFlag;                           //��ʾ�Ƿ񽻻�ͼ��
    bool _swapNewGoodImgFlag;                       //��ʾ�Ƿ񽻻��ϸ�ͼ��
    bool _getNewImgFlag;                            //��ʾ�Ƿ��ȡ��һ���µ�ͼ��
    bool _getLeftNewGoodImgFlag;                    //��ʾ�����Ƿ����µĺϸ�ͼ��
    bool _getRightNewGoodImgFlag;                   //��ʾ�����Ƿ����µĺϸ�ͼ��


    IrisImageInfo _leftCurImg;                      //���²ɼ�������ͼ��
    IrisImageInfo _leftBestImg;                     //������ͼ��������ѡȡ����õ�ͼ�񣨵ȴ��������㷨����ʶ���ע�ᣩ
    IrisImageInfo _leftEnrollImg[g_constIREnrollTimeNum];//����ע��ʱѡȡ���������ͼ��
    IrisImageInfo _rightCurImg;                     //���²ɼ�������ͼ��
    IrisImageInfo _rightBestImg;                    //������ͼ��������ѡȡ����õ�ͼ�񣨵ȴ��������㷨����ʶ���ע�ᣩ
    IrisImageInfo _rightEnrollImg[g_constIREnrollTimeNum];//����ע��ʱѡȡ���������ͼ��

    IKIREnrMatchImInfo _leftEnrMatchImgInfo[g_constIREnrollImgNum+1];//����ע��ȶԺ�Ĥͼ��
    IKIREnrMatchImInfo _rightEnrMatchImgInfo[g_constIREnrollImgNum+1];//����ע��ȶԺ�Ĥͼ��
    IKIREnrMatchImInfo _curBestEnrMatchImgInfo;     //��ǰע��ȶԺ�Ĥͼ��


    std::thread _tThreadMain;                            //���߳�
    std::thread _tAsyncCapImg;                           //�첽��ͼ�߳�
    std::thread _tAsyncEnroll;                           //�첽ע���߳�
    std::thread _tAsyncIden;                             //�첽ʶ���߳�

};
