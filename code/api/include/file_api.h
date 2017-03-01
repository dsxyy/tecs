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

#ifndef TECS_FILE_API_H
#define TECS_FILE_API_H
#include "rpcable.h"
#include "api_error.h"



/**
@brief 功能描述: 文件查询接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiCoredumpUrlInfo : public Rpcable
{
public:
    ApiCoredumpUrlInfo(){};
    ~ApiCoredumpUrlInfo(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(access_type);
        TO_VALUE(url);
        TO_VALUE(path);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(access_type);
        FROM_VALUE(url);
        FROM_VALUE(path);
        FROM_RPC_END();
    };

    string            access_type;
    string            url;  
    string            path;
};


#endif

