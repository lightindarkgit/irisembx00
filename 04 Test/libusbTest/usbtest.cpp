#include "usbtest.h"

void  ImageCallBack(int                type,        //回调类型
  unsigned  char		*buf, 		// 图像数据
  int                length)
{
    EXPECT_EQ(0,type);
    bool isBuf = (buf[0]!=0);
    EXPECT_TRUE(isBuf);
    EXPECT_EQ(TransBufferSize,length);

}
CUSBAPITest usbTest;
CUSBAPITest::CUSBAPITest()
{
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(SyncUSBAPI, TestOpen)
{

     EXPECT_EQ(0,usbTest.t_syncUSBAPI.Open());

//    EXPECT_EQ(0,usbTest.t_syncUSBAPI.Open());
//    usbTest.t_syncUSBAPI.Close();

//    EXPECT_EQ(0,usbTest.t_syncUSBAPI.Open());
    //usbTest.t_syncUSBAPI.Close();

//    EXPECT_EQ(0,t_syncUSBAPI.Open());
   // EXPECT_ANY_THROW(usbTest.t_syncUSBAPI);
    //usbTest.t_syncUSBAPI.Close();
    //t_usbAPI._bufLength =12;
    //EXPECT_EQ(12,t_usbAPI._bufLength);

}
/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(SyncUSBAPI, TestGetImage)
{
    unsigned char * buf = new unsigned char[TransBufferSize];
    unsigned int   len ;
    usbTest.t_syncUSBAPI.GetImage(buf,len);

    bool isBuf = (buf[0]!=0);
    EXPECT_TRUE(isBuf);
    EXPECT_EQ(TransBufferSize,len);

    delete[] buf;
}
/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(SyncUSBAPI, TestClose)
{
    usbTest.t_syncUSBAPI.Close();
//    EXPECT_EQ();
}
/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(AsyncUSBAPI, TestOpen)
{
    EXPECT_EQ(0,usbTest.t_asyncUSBAPI.Open(ImageCallBack));
   //usbTest.t_asyncUSBAPI.Close();
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(AsyncUSBAPI, TestClose)
{
    usbTest.t_asyncUSBAPI.Close();
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBAPI, TestInit)
{
    EXPECT_EQ(0,usbTest.t_usbAPI.Init());

    usbTest.t_usbAPI.Close();
//    usbTest.t_usbAPI.GetImage()

}


/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTOpen)
{
    EXPECT_EQ(0,usbTest.t_usbDevice.Open());
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTDeviceCount)
{
    EXPECT_EQ(5,usbTest.t_usbDevice.DeviceCount());
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTGetBulkEndPoint)
{
    IkBulkEndPoint *bulk = usbTest.t_usbDevice.GetBulkEndPoint(DirFromDevice);
//    bulk.
    EXPECT_TRUE(bulk!=NULL);

}


/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTGetDeviceDescriptor)
{
    IKDeviceDescriptor descriptor = usbTest.t_usbDevice.GetDeviceDescriptor();
    EXPECT_TRUE(descriptor.Length == 0);
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTGetUSBConfig)
{
    IkUSBConfig ikUsbConfig;
    EXPECT_EQ(0,usbTest.t_usbDevice.GetUSBConfig(0,ikUsbConfig));

}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTIsOpen)
{
    EXPECT_TRUE(usbTest.t_usbDevice.IsOpen());
    usbTest.t_usbDevice.Close();
    EXPECT_FALSE(usbTest.t_usbDevice.IsOpen());
    usbTest.t_usbDevice.Open();


}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTReset)
{
    EXPECT_TRUE(usbTest.t_usbDevice.Reset());
}

/*****************************************************************************
*                         USBAPI单元测试
*  函 数 名：
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
TEST(IkUSBDevice, TESTClose)
{
    usbTest.t_usbDevice.Close();
    EXPECT_FALSE(usbTest.t_usbDevice.IsOpen());
}
