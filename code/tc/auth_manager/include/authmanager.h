/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�authmanager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��authmanager��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ� �����
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ������
*     �޸����ݣ�����
*******************************************************************************/
#ifndef AM_AUTHMANAGER_H
#define AM_AUTHMANAGER_H
#include "mid.h"
#include "message.h"
#include "message_unit.h"
#include "authrequest.h"
#include "drivermanager.h"
#include "auth_config.h"
#include "user_pool.h"
#include "usergroup_pool.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>


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
@brief ��������: ��Ȩ����Ȩ��������࣬��Ҫ�����Ȩ����Ȩ����Ĵ���\n
@li @b ����˵��: ��
*/
class AuthManager
{
public:
    static AuthManager* GetInstance()
    {
        if(!_instance)
        {
            _instance = new AuthManager();
        }
        return _instance;
    }

    UserPool* get_upool() 
    { 
        return _upool;
    }

    UsergroupPool* get_gpool()
    {
        return _gpool; 
    }
    
    /**************************************************************************/
    /**
    @brief ��������: �ж��Ƿ񱾵ؼ�Ȩ
    */
    bool is_local() 
    { 
        if(_dm == NULL) 
        {
            return false; 
        }
        else 
        {
            return true;
        }
    }    

    /**************************************************************************/
    /**
    @brief ��������:���ر��ؼ�Ȩ����
    @li @b ����б�
    @verbatim
        ��
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    STATUS LoadMads(UserPool* upool);
    
    /**************************************************************************/
    /**
    @brief ��������:�����Ȩ����
    @li @b ����б�
    @verbatim
        ar ��Ȩ�������
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    bool Authenticate(AuthRequest * ar);
    
    /**************************************************************************/
    /**
    @brief ��������:������Ȩ����
    @li @b ����б�
    @verbatim
        ar ��Ȩ�������
    @endverbatim
    
    @li @b �����б�
    @verbatim
        �� 
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    bool Authorize(AuthRequest * ar);
   
private:
    static AuthManager *_instance;
    AuthManager();    
    ~AuthManager();
        
    DISALLOW_COPY_AND_ASSIGN(AuthManager);

    /**
    *   ��������
    */
    void Lock();
    
    /**
    *   ��������:��������
    */
    void Unlock();     


    /**
     *  ��Ȩ����  
     */
    DriverManager*   _dm; 
    
    /**
     *  ��Ϣ��Ԫ
     */
    MessageUnit*     _mu;

    /**
     *  �û���
     */
    UserPool*        _upool;

    /**
     *  �û����
     */
    UsergroupPool*   _gpool;

    /**
     *  Mutex to access the auth_requests
     */
    pthread_mutex_t      _mutex; 
  
};

}
#endif 

