/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�authrequest.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��authrequest��Ķ����ļ�
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
#ifndef AM_AUTHREQUEST_H
#define AM_AUTHREQUEST_H
#include "sky.h"
#include "auth_config.h"
#include "log_agent.h"
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
@brief ��������: ��Ȩ�����࣬���ڶ����Ȩ����Ȩ�������������Ϣ\n
@li @b ����˵��: ��
*/  
class AuthRequest
{
    friend class AuthManager;
public:

    /**
     *  ��Ȩ��������
     */
    enum Operation
    {
        CREATE,  /** Authorization to create an object (host, vm, net, image)*/
        DELETE,  /** Authorization to delete an object */
        USE,     /** Authorization to use an object */
        MANAGE,  /** Authorization to manage an object */
        INFO     /** Authorization to view an object */
    };

    /**
     *  ��Ȩ��������
     */
    enum Object
    {
        VM,
        HOST,
        NET,
        IMAGE,
        USER,
        USERGROUP,
        CLUSTER,
        PROJECT,
        VMTEMPLATE,
        VMCONFIG,
        VERSION,
        SA,
        FILE,
        AFFINITY_REGION
    };
    
    /**************************************************************************/
    /**
    @brief ��������: ���캯������ʼ������

    @li @b ����б�
    @verbatim
        uid �û�id
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
    explicit AuthRequest(int64 uid);
    
    /**************************************************************************/
    /**
    @brief ��������: �����������ͷ�����ڴ�

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
    ~AuthRequest();
    
    /**************************************************************************/
    /**
    @brief ��������: ���Ӽ�Ȩ
    @li @b ����б�
    @verbatim
        username �û���
        password  ����
        session     �Ự��Ϣ
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
    void AddAuthenticate(const string &username,
                          const string &password,
                          const string &secret);
                          
    /**************************************************************************/
    /**
    @brief ��������: ������Ȩ
    @li @b ����б�
    @verbatim
        ob        ��������
        ob_id    ��������id
        op        ��������
        owner   ���������ߵ�uid    
        pub      �Ƿ񹫿�
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
    void AddAuthorize(Object        ob,
                       const string& ob_id,
                       Operation     op,
                       int64         owner,
                       bool          pub);

    void AddAuthorize(Object        ob,
                       int64        ob_id,
                       Operation     op,
                       int64         owner,
                       bool          pub);
                       
    /**************************************************************************/
    /**
     *
     */
    
    void Send(const Serializable& msg, MessageOption& option)
    {
        _mu->Send(msg, option);
    }
    
    /**************************************************************************/
    /**
    @brief ��������: �ȴ����������
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
    void Wait(DriverMessage& msg);
    
    /**************************************************************************/
    /**
    @brief ��������: ���������֪ͨ
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
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    void Notify();
    
    /**************************************************************************/
    /**
    @brief ��������:ƽ��Ȩ����
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
      1-�ɹ���0ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    bool PlainAuthorize();
    
    /**************************************************************************/
    /**
    @brief ��������:ƽ��Ȩ����
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
        1-�ɹ���0ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    bool PlainAuthenticate();
    
    /**************************************************************************/
    /**
    @brief ��������: ��ȡ�������
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
        �õ���������ַ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    const AuthorizeMessage& get_auth_req();
 
    /**************************************************************************/
    /**
    @brief ��������:Զ�̼�Ȩ���
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
    bool set_result(const bool result);
    
    /**************************************************************************/
    /**
    @brief ��������:�õ�Զ�̼�Ȩ���
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
    bool get_result();      
    
    /**************************************************************************/
    /**
    @brief ��������:���ü�Ȩ����ַ���
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
    bool set_message(const string& message);
    
    /**************************************************************************/
    /**
    @brief ��������:�õ���Ȩ����ַ���
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
    string get_message();

    
private:

    DISALLOW_COPY_AND_ASSIGN(AuthRequest);
    
    /**
    *  ���ַ�������base64����
    */
    static string * Base64Encode(const string& in);
    
    /**
     *  �û�id
     */
    int64 _uid;
    
    /**
     * ��Ȩ�û����û���
     */
    string _username;

    /**
     *  ��Ȩ�û�������
     */
    string _password;

    /**
     *  ��Ȩ�û���session��Ϣ
     */
    string _secret;
    
    /**
     * ��Ȩ�������
     */
    AuthorizeMessage _auth_req;

    /**
     *  �Ƿ�Ϊƽ��Ȩ
     */
    bool _self_authorize;
    
    /**
     *  ��Ϣ��Ԫ
     */ 
    MessageUnit *_mu;
   
    /**
     *  ��Ȩ���
     */
    bool  _result;

    /**
     *  Զ�̼�Ȩ����Ϣ
     */
    string _message;

}; 

}
#endif 
