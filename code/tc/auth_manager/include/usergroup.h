/******************************************************************************
* Copyright (c) 2011，深圳市中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：usergroup.h
* 文件标识：见配置管理计划书
* 内容摘要：用户组对象信息持久化类
* 当前版本：1.0
* 作    者：王明辉
* 完成日期：2011年7月21日
*
* 修改记录1：
*    修改日期：2011/7/21
*    版 本 号：V1.0
*    修 改 人：王明辉
*    修改内容：创建
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
@brief 功能描述: 用户组对象信息持久化类\n
@li @b 其它说明: 无
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
    @brief 功能描述:print the User object into a string in plain text
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    string& to_str(string& str) const;
    
    /**************************************************************************/
    /**
    @brief 功能描述:print the User object into a string in XML format
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    string& to_xml(string& str) const;

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户组唯一id
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    int64   get_gid()
    {
        return _oid;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户组是否启用
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    bool    is_enabled()
    {
        return _enabled;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户组名
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    string& get_groupname()
    {
        return _groupname;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取用户组类型
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
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
    @brief 功能描述:启用该用户组
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    void    enable()
    {
        _enabled = true;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:禁用该用户组
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    void    disable()
    {
        _enabled = false;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户组名称
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    void    set_groupname(string name)
    {
        _groupname = name;
    }

    /**************************************************************************/
    /**
    @brief 功能描述:设置用户组类型
    @li @b 入参列表：
    @verbatim
        无
    @endverbatim
    
    @li @b 出参列表：
    @verbatim
        无 
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
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
