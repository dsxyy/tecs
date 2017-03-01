/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_manager_methods.h
* 文件标识：见配置管理计划书
* 内容摘要：版本管理XMl配置接口申明
* 当前版本：1.0
* 作    者：李灿伟
* 完成日期：2011年10月13日
*
* 修改记录1：
*     修改日期：2011/10/13
*     版 本 号：V1.0
*     修 改 人：李灿伟
*     修改内容：创建
******************************************************************************/
#ifndef TC_VERSION_MANAGER_INFO_H
#define TC_VERSION_MANAGER_INFO_H

#include "sky.h"
#include "mid.h"
#include "event.h"

namespace zte_tecs
{

/*****************************************************************************/
/* 版本信息类 */
/*****************************************************************************/
class VersionInfo : public Serializable
{
public:
    VersionInfo() {};
    ~VersionInfo() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(VersionInfo);
        WRITE_STRING(_destination);
        WRITE_DIGIT(_grade);
        WRITE_STRING(_filename);
        WRITE_DIGIT(_flag);
        WRITE_STRING(_version);
         SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(VersionInfo);
        READ_STRING(_destination);
        READ_DIGIT(_grade);
        READ_STRING(_filename);
        READ_DIGIT(_flag);
        READ_STRING(_version);
        DESERIALIZE_END();    
    };
	//目的标示，标示版本最终存放或运行的主机，用application名来定义
	string _destination;
       int _grade;
	//加载文件名称
	string _filename;
	//设置load命令后是否执行安装操作,0表示不进行版本安装，1表示进行版本安装
	int _flag;       // 版本加载标记
       string _version;                 // 版本号
};

/*****************************************************************************/
/* 版本信息类 */
/*****************************************************************************/
class VersionInfoReq : public Serializable
{
public:
    VersionInfoReq() {};
    VersionInfoReq(const string& destination,int32 grade,const string& filename,const string& host,const string& user,const string& password)
	{
           _destination = destination;
           _grade = grade;
           _filename = filename;
           _host_address = host;
           _username = user;
           _password = password;
	};	
    ~VersionInfoReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(VersionInfo);
        WRITE_STRING(_destination);
        WRITE_DIGIT(_grade);
        WRITE_STRING(_filename);
        WRITE_STRING(_host_address);
        WRITE_STRING(_username);
        WRITE_STRING(_password);		
         SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(VersionInfo);
        READ_STRING(_destination);
        READ_DIGIT(_grade);
        READ_STRING(_filename);
        READ_STRING(_host_address);
        READ_STRING(_username);		
        READ_STRING(_password);
        DESERIALIZE_END();    
    };
	//目的标示，标示版本最终存放或运行的主机，用application名来定义
	string _destination;
       int32 _grade;
	//加载文件名称
	string _filename;
       string   _host_address;
       string   _username;
       string  _password;       
};

/*****************************************************************************/
//用来向用户返回查询到的版本信息
/*****************************************************************************/
class VersionInfoAck : public Serializable
{
public:
    VersionInfoAck(){};	
    VersionInfoAck(int32 num) 
    {
        _num = num;
    }

    ~VersionInfoAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(VersionInfoAck);
        WRITE_DIGIT(_result);		
        WRITE_DIGIT(_num);
        WRITE_OBJECT_VECTOR(_version_resource);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(VersionInfoAck);
        READ_DIGIT(_result);		
        READ_DIGIT(_num);
        READ_OBJECT_VECTOR(_version_resource);
        DESERIALIZE_END();    
    };
    int _result;    
    int  _num;
    vector <VersionInfo> _version_resource;

};

} //end namespace zte_tecs

#endif //end TC_VERSION_MANAGER_INFO_H
