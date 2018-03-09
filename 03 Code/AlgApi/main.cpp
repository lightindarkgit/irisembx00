#include <QtCore/QCoreApplication>
#include "SRC/IK_IR_DLL.h"
#include "SRC/AlgApi.h"
#include <QtGui>
#include "CameraDemo.h"


#include "SRC/BRCCommon.h"
const int g_constIrisImgHeaderLen = 15;
const int g_constBmpLen = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256+
        g_constIKImgWidth*g_constIKImgHeight;


int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
//    CAlgApi *pTestAlgApi = new CAlgApi();
//    pTestAlgApi->Init();

//    CCameraApi *pTestCameraApi = new CCameraApi();

//    return a.exec();

    QApplication app(argc,argv);
    CameraDemo cameraDemo;
    cameraDemo.resize(640,480);
    cameraDemo.show();

    return app.exec();
}
