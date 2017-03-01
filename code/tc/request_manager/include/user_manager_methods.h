/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�user_manager_method.h
* �ļ���ʶ��
* ����ժҪ��user manager��XML-RPCע�᷽����
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��04��27��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012��04��27��
*    �� �� �ţ�V1.0
*    �� �� �ˣ����Ľ�
*    �޸����ݣ�����
*******************************************************************************/
#ifndef USER_MANAGER_METHODS_H        
#define USER_MANAGER_METHODS_H
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_common.h"
#include "api_error.h"
#include "tecs_errcode.h"

namespace zte_tecs
{
/**
@brief ��������: �û�������Ϣ��ѯ\n
@li @b ����˵��: һ�δ������в���
*/
class UserFullQueryMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,                 TECS_ERR_TIMEOUT) 
        ERR_MAP(ERR_AUTHORIZE_FAILED,           TECS_ERR_AUTHORIZE_FAILED) 
        ERR_MAP(ERROR_INVALID_ARGUMENT,         TECS_ERR_PARA_INVALID) 
        ERR_MAP_END
    }

    enum UserCfgFullQueryType
    {
        ALL_USER        = -1, /** < �����û���, ��admin�û����� */
        CURRENT_USER    = 0, /** < ��ǰ�û��ģ����û��Լ�����  */
        APPOINTED_USER  = 1, /** < ָ���û���, ��admin�û����� */
    };
    
    UserFullQueryMethod (UserPool *upool=0)
    {
        _method_name = "UserFullQueryMethod";
        _user_id = -1;
        _upool = upool;
    };
	
    ~UserFullQueryMethod(){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                                 xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(UserFullQueryMethod); 
};

}
#endif

