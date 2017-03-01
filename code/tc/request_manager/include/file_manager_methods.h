/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�file_manager_methods.h
* �ļ���ʶ��
* ����ժҪ��file��XML-RPC ������
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2013��3��4��
*
*******************************************************************************/

#ifndef FILE_MANAGER_METHODS_H        
#define FILE_MANAGER_METHODS_H

#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "api_const.h"
#include "tecs_errcode.h"
#include "image_url.h"


// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: �ļ���ѯ��ʵ����\n
@li @b ����˵��: ��
*/
class FileGetCoredumpUrlMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    FileGetCoredumpUrlMethod (UserPool *upool=0)
    {
        _method_name = "FileGetCoredumpUrlMethod";
        _user_id = -1;
        _upool = upool;
    };
    ~FileGetCoredumpUrlMethod (){};
    
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP_END
    }

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                         xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

   DISALLOW_COPY_AND_ASSIGN(FileGetCoredumpUrlMethod); 

};

}
#endif /* IMAGE_MANAGER_METHODS_H */

