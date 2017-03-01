/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�drivermanager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��drivermanager��Ķ����ļ�
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
#ifndef AM_DRIVERMANAGER_H
#define AM_DRIVERMANAGER_H
#include "message.h"
#include "message_unit.h"
#include "auth_config.h"
#include "user_pool.h"
#include "mid.h"
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/registry.hpp>

namespace zte_tecs
{

/**
@brief ��������: ����������\n
@li @b ����˵��: ��
*/
class DriverManager: public MessageHandler
{
public:
    static DriverManager* GetInstance()
    {
        if(!_instance)
        {
            _instance = new DriverManager();
        }
        return _instance;
    }

    STATUS Init()
    {
        _upool = UserPool::GetInstance();
        return StartMu(DRIVERMANAGER);
    }
    
    /**************************************************************************/
    /**
    @brief ��������: ��������
    */
    virtual ~DriverManager();
   
    /**************************************************************************/
    /**
    @brief ��������: ����userpool
    */  
    void set_userpool(UserPool* up)
    {
        _upool = up;
    }
    
    /**************************************************************************/
    /**
    @brief ��������: ��ȡuserpool
    */  
    UserPool* get_userpool(void)
    {
        return _upool;
    }
    
    /**
     *  ��Ϣ������
     */    
    void MessageEntry(const MessageFrame& frame);
    
private:
    DriverManager();
    DISALLOW_COPY_AND_ASSIGN(DriverManager);
    
    STATUS StartMu(const string& name);
    void HandleAuthenticateEvent(const MessageFrame& frame);
    void HandleAuthorizeEvent(const MessageFrame& frame);
    void HandleSessionEvent(const MessageFrame& frame);    
    
    MessageUnit* _mu;
    UserPool* _upool;
    static DriverManager* _instance;
  
} ;

}
#endif 


