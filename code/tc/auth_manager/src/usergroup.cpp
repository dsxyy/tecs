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
