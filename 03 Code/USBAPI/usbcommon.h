#ifndef USBCOMMON_H
#define USBCOMMON_H
#include "../common.h"

#define	E_PERM		 1	/* 操作不允许*/
#define	E_NOENT		 2	/* 没有这样的文件或目录*/
#define	E_SRCH		 3	/* 没有这样的过程*/
#define	E_INTR		 4	/* 中断系统调用*/
#define	E_IO		 5	/* I/O 错误*/
#define	E_NXIO		 6	/* 没有这样的设备或地址**/
#define	E_2BIG		 7	/* 参数列表太长*/
#define	E_NOEXEC	 8	/* 格式错误*/
#define	E_BADF		 9	/* 坏文件号*/
#define	E_CHILD		10	/* 没有子进程*/
#define	E_AGAIN		11	/* 再试 */
#define	E_NOMEM		12	/* 内存越界*/
#define	E_ACCES		13	/* 没有权限*/
#define	E_FAULT		14	/* 坏地址*/
#define	E_NOTBLK	15	/* 块设备要求*/
#define	E_BUSY		16	/* 设备或资源忙*/
#define	E_EXIST		17	/* 文件存在*/
#define	E_XDEV		18	/* 跨平台连接*/
#define	E_NODEV		19	/* 没有这样的设备*/
#define	E_NOTDIR	20	/* 不是一个目录*/
#define	E_ISDIR		21	/* 是一个目录*/
#define	E_INVAL		22	/* 无效参数*/
#define	E_NFILE		23	/* 文件表溢出*/
#define	E_MFILE		24	/* 打开的文件太多 */
#define	E_NOTTY		25	/* 不是打字机*/
#define	E_TXTBSY	26	/* 文本文件忙*/
#define	E_FBIG		27	/* 文件太大*/
#define	E_NOSPC		28	/* 设备上没有剩余空间*/
#define	E_SPIPE		29	/* 非法寻求*/
#define	E_ROFS		30	/* 只读文件系统*/
#define	E_MLINK		31	/* 太多链接*/
#define	E_PIPE		32	/* 破管*/
#define	E_DOM		33	/* 数学参数从域的函数*/
#define	E_RANGE		34	/* 不能表示的数学结果*/
#define	E_ERROR		35	/* */

//自定义错误描述
#define E_DEVICE_NO_OPEN   20000  /*设备未打开*/
#define E_NO_ENDPOINT      20001  /*端点不存在*/
#define E_NO_DEVICE_HANDLE 20002  /*设备句柄不存在*/
#define E_NO_TRANSFER      20003  /*传输句柄不存在*/
#define E_SUBMIT           20004  /*传输提交错误*/
#define E_NO_CONTEXT       20005  /*设备上下文为获取错误*/
#define E_CLAIM_INTERFACE  20006  /*设置端点接口错误*/


//各种数据的大小
typedef enum {
    MaxEndPoints=32,
    MaxInterfaces=8,
    UsbStringMaxlen=256,
    BufsizeUport=2048,
    ImageWidth = 2048,
    ImageHeight= 704,
    TransBufferSize  =ImageWidth*ImageHeight,
    ProcessBufferSize= TransBufferSize*2,
    GrayBufferSize   =(ImageWidth-1)*(ImageHeight-1),
    PPMHeaderLength =18,
    DataOffset=8
} TypeSize;

//设备PID和VID的值
static const unsigned short VID = 0x0547;
static const unsigned short PID = 0x1002;

////传输状态
enum TransferStatus {
     TransferCompleted,
     TransferError,
     TransferTimedOut,
     TransferCancelled,
     TransferStall,
     TransferNoDevice,
     TransferOverflow
};

#endif // USBCOMMON_H
