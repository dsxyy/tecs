/******************************************************************************
* Copyright (c) 2011������������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�usergroup.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���û��������Ϣ�־û���
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��21��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/21
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
******************************************************************************/

#ifndef AUTH_USERGROUP_H
#define AUTH_USERGROUP_H

#include <string>
#include "serialization.h"
#include "datetime.h"

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: �û��������Ϣ�־û���\n
@li @b ����˵��: ��
*/
class UserGroup
{
    friend class UsergroupPool;
    
public:
    enum Role
    {
        CLOUD_ADMINISTRATOR    = 1,
        ACCOUNT_ADMINISTRATOR  = 2,
        ACCOUNT_USER           = 3 
    };    

    UserGroup()
    {
        _oid = 0;
        _grouptype = 0; 
        _enabled = false;
    }
  
    /**************************************************************************/
    /**
    @brief ��������:print the User object into a string in plain text
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
    string& to_str(string& str) const;
    
    /**************************************************************************/
    /**
    @brief ��������:print the User object into a string in XML format
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
    string& to_xml(string& str) const;

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û���Ψһid
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
    int64   get_gid()
    {
        return _oid;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û����Ƿ�����
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
    bool    is_enabled()
    {
        return _enabled;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û�����
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
    string& get_groupname()
    {
        return _groupname;
    }

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�û�������
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
    int     get_grouptype()
    {
        return _grouptype;
    }

    const string & get_description() const
    {
        return _description;
    }

    string  get_create_time()
    {
        return _create_time.tostr(true);
    }

    /**************************************************************************/
    /**
    @brief ��������:���ø��û���
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
    void    enable()
    {
        _enabled = true;
    }

    /**************************************************************************/
    /**
    @brief ��������:���ø��û���
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
    void    disable()
    {
        _enabled = false;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û�������
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
    void    set_groupname(string name)
    {
        _groupname = name;
    }

    /**************************************************************************/
    /**
    @brief ��������:�����û�������
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
    void set_grouptype(int type)
    {
        _grouptype = type;
    }


    void set_description(const string & description)
    {
        _description = description;
    }    
    
private:  
    int64   _oid;
    string  _groupname;
    int     _grouptype; 
    bool    _enabled;    
    string  _description;
    Datetime  _create_time;
};

}
#endif
