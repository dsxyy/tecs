/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_api.h
* 文件标识：
* 内容摘要：映像模块对外接口
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年10月19日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef TECS_IMAGE_API_H
#define TECS_IMAGE_API_H
#include "rpcable.h"
#include "api_error.h"
#include "api_const.h"



class  ImageParaCheck
{
public:
    static bool Type(const string &str)
    {
        if ((IMAGE_TYPE_KERNEL!= str)
            &&(IMAGE_TYPE_RAMDISK!= str)
            &&(IMAGE_TYPE_MACHINE!= str)
            &&(IMAGE_TYPE_CDROM != str)
            &&(IMAGE_TYPE_DATABLOCK!= str) )
        {
            return false;
        }        
        return true;
    };

    static bool Arch(const string &str)
    {
        if (IMAGE_ARCH_I386 != str && IMAGE_ARCH_X86_64 != str)
        {
            return false;
        }        
        return true;
    };

    static bool Bus(const string &str)
    {
        if (DISK_BUS_IDE != str
            && DISK_BUS_SCSI != str 
            && DISK_BUS_XVD != str)
        {
            return false;
        }        
        return true;
    };
};


/**
@brief 功能描述: 映像注册接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiImageReg: public Rpcable
{
public:

    ApiImageReg():id(-1){ };
    
    ~ApiImageReg() {  };
    
    bool Validate(string &err_str)
    {
        ostringstream   oss;

        // 映像类型
        if (! ImageParaCheck::Type(type))
        {
            oss << "Invalide image type ( " <<type <<" )";
            err_str = oss.str();
            return false;
        }

        // 映像类型
        if(!ImageParaCheck::Arch(arch))
        {
            oss << "Invalide image arch ( " <<arch <<" )";
            err_str = oss.str();
            return false;
        }
        
        if(0 >= id)
        {
           oss << "Invalide id ( " <<id <<")";
            err_str = oss.str();
            return false;
        }
        return true;
    }
           

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(name);
        TO_VALUE(type);
        TO_VALUE(arch);
        TO_VALUE(is_public);        
        TO_VALUE(is_available);
        TO_VALUE(description);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(name);
        FROM_VALUE(type);
        FROM_VALUE(arch);
        FROM_VALUE(is_public);        
        FROM_VALUE(is_available);
        FROM_VALUE(description);
        FROM_RPC_END();
    };
     
    long long  id;
    string     name;
    string     type;
    string     arch;    
    bool       is_public;  
    bool       is_available; 
    string     description;
};

/**
@brief 功能描述: 映像查询接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiImageInfo : public Rpcable
{
public:
    ApiImageInfo():id(-1){};
    ~ApiImageInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(user_name);
        TO_VALUE(name);
        TO_VALUE(file);
        TO_VALUE(md5sum);
        TO_VALUE(type);
        TO_VALUE(is_public);
        TO_VALUE(is_enabled); 
        TO_VALUE(register_time);
        TO_I8(size);
        TO_VALUE(running_vms);
	TO_VALUE(ref_count_by_private_vt);
	TO_VALUE(ref_count_by_public_vt);
        TO_VALUE(arch);   
        TO_VALUE(bus); 
        TO_VALUE(description);  
        TO_VALUE(os_type);
        TO_I8(disk_size);
        TO_VALUE(disk_format);
        TO_VALUE(container_format);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(user_name);
        FROM_VALUE(name);
        FROM_VALUE(file);
        FROM_VALUE(md5sum);
        FROM_VALUE(type);
        FROM_VALUE(is_public);
        FROM_VALUE(is_enabled); 
        FROM_VALUE(register_time);
        FROM_I8(size);
        FROM_VALUE(running_vms);
	FROM_VALUE(ref_count_by_private_vt);
	FROM_VALUE(ref_count_by_public_vt);
        FROM_VALUE(arch); 
        FROM_VALUE(bus); 
        FROM_VALUE(description); 
        FROM_VALUE(os_type);
        FROM_I8(disk_size);
        FROM_VALUE(disk_format);
        FROM_VALUE(container_format);
        FROM_RPC_END();
    };
    
    long long         id;  //映像文件ID
    string            user_name;
    string            name; //映像名称
    string            file; //实际对应的文件名称
    string            md5sum;
    string            type;
    bool              is_public;
    bool              is_enabled;
    string            register_time;
    long long         size;
    int               running_vms;
    int               ref_count_by_private_vt;
    int               ref_count_by_public_vt;
    string            arch;
    string            bus;
    string            description;
    string            os_type;
    long long         disk_size;
    string            disk_format;
    string            container_format;
};

class ApiImageFtpSvrInfo : public Rpcable
{
public:
    ApiImageFtpSvrInfo(int port,string ip):ftp_svr_port(port),ftp_svr_ip(ip)
    {
    };

    ~ApiImageFtpSvrInfo(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(ftp_svr_port);
        TO_VALUE(ftp_svr_ip); 
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(ftp_svr_port);
        FROM_VALUE(ftp_svr_ip);
        FROM_RPC_END();
    };
    
    int      ftp_svr_port;
    string   ftp_svr_ip;
};


#endif

