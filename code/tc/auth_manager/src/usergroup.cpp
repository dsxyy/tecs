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

#include "usergroup.h"

namespace zte_tecs
{

/* -------------------------------------------------------------------------- */    

string& UserGroup::to_str(string& str) const
{
    ostringstream os;

    string enable_str = _enabled ? "True" : "False";

    os << 
       "ID = "        << _oid        << endl <<
       "GROUPNAME = " << _groupname << endl <<
       "GROUPTYP = "  << _grouptype << endl <<
       "ENABLE = "     << enable_str << endl <<
       "CREATE_TIME = "  << _create_time << endl <<
       "DESCRIPTION = " << _description ;

    str = os.str();

    return str;
}

/* -------------------------------------------------------------------------- */

string& UserGroup ::to_xml(string& str) const
{
    ostringstream os;

    int enable_int = _enabled ? 1 : 0;

    os << 
      "<USERGROUP>" 
          "<ID>"        << _oid         << "</ID>"           <<
          "<GROUPNAME>" << _groupname  << "</GROUPNAME>"    << 
          "<GROUPTYPE>" << _grouptype  << "</GROUPTYPE>"    <<
          "<ENABLE>"    << enable_int << "</ENABLE>"       <<
          "<CREATE_TIME>" << _create_time << "<CREATE_TIME>" <<
          "<DESCRIPTION>" << _description << "</DESCRIPTION>" <<
      "</USERGROUP>";

    str = os.str();

    return str;
}

/* -------------------------------------------------------------------------- */

}
