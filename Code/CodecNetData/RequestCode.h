/*
 * RequestCode.h
 * 请求码及回应码定义
 *
 *  Created on: 2014年3月18日
 *      Author: yqhe
 */

#pragma once

///////////////////////////////////////////////////////////
// 请求请求码
const unsigned short  REQ_KEEP_ALIVE                = 0x0001;           // 心跳

// deleted at 20140429 by yqhe
// 去掉部门相关请求

////////////////////////////
// added at 20140429 by yqhe
// 增加数据操作令牌相关请求
const unsigned short  REQ_SYNC_TOKEN				= 0x0101;			// 同步操作令牌

const unsigned short  REQ_DEV_CONNECT				= 0x0121;			// 设备基本连接参数
///////////////////////////

// 普通人员相关
const unsigned short  REQ_ADD_PERSON_INFO           = 0x0201;           // 增加人员信息
const unsigned short  REQ_UPDATE_PERSON_INFO        = 0x0202;           // 修改人员信息
const unsigned short  REQ_DELETE_PERSON_INFO        = 0x0203;           // 删除人员信息
const unsigned short  REQ_ADD_PERSON_PHOTO          = 0x0204;           // 增加人员照片
const unsigned short  REQ_UPDATE_PERSON_PHOTO       = 0x0205;           // 更新人员照片
const unsigned short  REQ_DELETE_PERSON_PHOTO       = 0x0206;           // 删除人员照片

////////////////////////////
// added at 20140429 by yqhe
// 增加获得人员列表请求
const unsigned short  REQ_GET_PERSON_LIST			= 0x0211;			// 获得人员列表
///////////////////////////

// 管理员相关
const unsigned short  REQ_ADD_ADMIN_INFO            = 0x0221;           // 增加操作员信息
const unsigned short  REQ_UPDATE_ADMIN_INFO         = 0x0222;           // 修改操作员信息
const unsigned short  REQ_DELETE_ADMIN_INFO         = 0x0223;           // 删除操作员信息
const unsigned short  REQ_ADD_ADMIN_PHOTO           = 0x0224;           // 增加操作员照片
const unsigned short  REQ_UPDATE_ADMIN_PHOTO        = 0x0225;           // 更新操作员照片
const unsigned short  REQ_DELETE_ADMIN_PHOTO        = 0x0226;           // 删除操作员照片

///////////////////////////////
// modified at 20140429 by yqhe
// 服务器向设备更新虹膜特征相关
const unsigned short  REQ_ADD_PERSON_IRIS    		= 0x0301;           // 增加虹膜特征从服务器到客户端
const unsigned short  REQ_DELETE_PERSON_IRIS 		= 0x0302;           // 删除虹膜特征从服务器到客户端
const unsigned short  REQ_ADD_ADMIN_IRIS    		= 0x0311;           // 增加管理员虹膜特征从服务器到客户端
const unsigned short  REQ_DELETE_ADMIN_IRIS 		= 0x0312;           // 删除管理员虹膜特征从服务器到客户端

// 客户端上传虹膜注册数据
const unsigned short  REQ_UPLOAD_PERSON_IRIS        = 0x0321;           // 上传虹膜注册数据从客户端到服务器
///////////////////////////////

// 识别记录相关
const unsigned short  REQ_UPLOAD_RECOG_RECORD       = 0x0401;           // 上传识别记录从客户端到服务器
const unsigned short  REQ_SEND_RECOG_RECORD         = 0x0421;           // 实时发送识别记录从客户端到服务器

const unsigned short  REQ_GET_RECOG_RECORD			= 0x0441;			// 获得指定的识别记录

// deleted at 20140429 by yqhe
// 切换工作模式相关

// 服务器控制设备相关
const unsigned short  REQ_SRVCTRL_SHUTDOWN          = 0x0661;           // 关机
const unsigned short  REQ_SRVCTRL_RESET             = 0x0662;           // 重启
const unsigned short  REQ_SRVCTRL_ADJUST_TIME       = 0x0663;           // 调整客户端时间
////////////////////////////////
// added at 20140429 by yqhe
// 调整或增加设备与X00服务之间的交互内容
const unsigned short  REQ_SRVCTRL_ADJUST_DEVIP      = 0x0664;           // 修改设备IP地址
const unsigned short  REQ_SRVCTRL_ADJUST_SRVIP      = 0x0665;           // 修改设备对应的服务器IP地址

// 查询设备信息
const unsigned short  REQ_GET_DEV_INFO			    = 0x0671;			// 查询设备信息

const unsigned short  REQ_GET_SRV_TIME				= 0x0681;			// 获得服务器时间
////////////////////////////////

// 集群识别
const unsigned short  REQ_CLUSTER_IMAGE_IDENTIFY    = 0x0701;           // 图像方式集群识别

// added at 20140320 by yqhe
// 由于在网络数据编解码过程中，发现原来把控制和图像放在一起，不方便编解码
// 所以把集群识别的控制参数和图像分开，作为两个请求
const unsigned short  REQ_CLUSTER_IMAGE_IDEN_CONFIG = 0x0702;			// 图像方式集群识别配置识别参数

const unsigned short  REQ_CLUSTER_FEATURE_IDENTIFY  = 0x0711;           // 特征方式集群识别

// added at 20140320 yqhe
const unsigned short  REQ_CLUSTER_FEATURE_IDEN_CONFIG=0x0712;           // 特征方式集群识别配置识别参数

// end of 请求码
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//回应请求码
const unsigned short  ACK_KEEP_ALIVE                = 0x4001;           // 心跳

// deleted at 20140429 by yqhe
// 去掉部门相关请求

////////////////////////////
// added at 20140429 by yqhe
// 增加数据操作令牌相关请求
const unsigned short  ACK_SYNC_TOKEN				= 0x4101;			// 同步令牌

const unsigned short  ACK_DEV_CONNECT				= 0x4121;			// 设备基本连接参数
////////////////////////////

// 普通人员相关
const unsigned short  ACK_ADD_PERSON_INFO           = 0x4201;           // 增加人员信息
const unsigned short  ACK_UPDATE_PERSON_INFO        = 0x4202;           // 修改人员信息
const unsigned short  ACK_DELETE_PERSON_INFO        = 0x4203;           // 删除人员信息
const unsigned short  ACK_ADD_PERSON_PHOTO          = 0x4204;           // 增加人员照片
const unsigned short  ACK_UPDATE_PERSON_PHOTO       = 0x4205;           // 更新人员照片
const unsigned short  ACK_DELETE_PERSON_PHOTO       = 0x4206;           // 删除人员照片

////////////////////////////
// added at 20140429 by yqhe
// 增加获得人员列表请求
const unsigned short  ACK_GET_PERSON_LIST			= 0x4211;			// 获得人员列表
///////////////////////////

// 管理员相关
const unsigned short  ACK_ADD_ADMIN_INFO            = 0x4221;           // 增加操作员信息
const unsigned short  ACK_UPDATE_ADMIN_INFO         = 0x4222;           // 修改操作员信息
const unsigned short  ACK_DELETE_ADMIN_INFO         = 0x4223;           // 删除操作员信息
const unsigned short  ACK_ADD_ADMIN_PHOTO           = 0x4224;           // 增加操作员照片
const unsigned short  ACK_UPDATE_ADMIN_PHOTO        = 0x4225;           // 更新操作员照片
const unsigned short  ACK_DELETE_ADMIN_PHOTO        = 0x4226;           // 删除操作员照片

///////////////////////////////
// modified at 20140429 by yqhe
// 服务器向设备更新虹膜特征相关
const unsigned short  ACK_ADD_PERSON_IRIS    		= 0x4301;           // 增加虹膜特征从服务器到客户端
const unsigned short  ACK_DELETE_PERSON_IRIS 		= 0x4302;           // 删除虹膜特征从服务器到客户端
const unsigned short  ACK_ADD_ADMIN_IRIS    		= 0x4311;           // 增加管理员虹膜特征从服务器到客户端
const unsigned short  ACK_DELETE_ADMIN_IRIS 		= 0x4312;           // 删除管理员虹膜特征从服务器到客户端

// 客户端上传虹膜注册数据
const unsigned short  ACK_UPLOAD_PERSON_IRIS        = 0x4321;           // 上传虹膜注册数据从客户端到服务器
///////////////////////////////

// 识别记录相关
const unsigned short  ACK_UPLOAD_RECOG_RECORD       = 0x4401;           // 上传识别记录从客户端到服务器
const unsigned short  ACK_SEND_RECOG_RECORD         = 0x4421;           // 实时发送识别记录从客户端到服务器

const unsigned short  ACK_GET_RECOG_RECORD			= 0x4441;			// 获得指定的识别记录

// deleted at 20140429 by yqhe
// 切换工作模式相关

// 服务器控制设备相关
const unsigned short  ACK_SRVCTRL_SHUTDOWN          = 0x4661;           // 关机
const unsigned short  ACK_SRVCTRL_RESET             = 0x4662;           // 重启
const unsigned short  ACK_SRVCTRL_ADJUST_TIME       = 0x4663;           // 调整客户端时间
////////////////////////////////
// added at 20140429 by yqhe
// 调整或增加设备与X00服务之间的交互内容
const unsigned short  ACK_SRVCTRL_ADJUST_DEVIP      = 0x4664;           // 修改设备IP地址
const unsigned short  ACK_SRVCTRL_ADJUST_SRVIP      = 0x4665;           // 修改设备对应的服务器IP地址

// 查询设备信息
const unsigned short  ACK_GET_DEV_INFO			    = 0x4671;			// 查询设备信息

const unsigned short  ACK_GET_SRV_TIME				= 0x4681;			// 获得服务器时间
/////////////////////////////////

// 集群识别
const unsigned short  ACK_CLUSTER_IMAGE_IDENTIFY    = 0x4701;           // 图像方式集群识别，发送识别图像
const unsigned short  ACK_CLUSTER_FEATURE_IDENTIFY  = 0x4711;           // 特征方式集群识别，发送识别特征

// added at 20140320 by yqhe
// 配置识别控制参数
const unsigned short  ACK_CLUSTER_IMAGE_IDEN_CONFIG	= 0x4702;           // 图像方式集群识别配置识别参数
const unsigned short  ACK_CLUSTER_FEATURE_IDEN_CONFIG=0x4712;           // 特征方式集群识别配置识别参数

// end of 回应码
///////////////////////////////////////////////////////////




