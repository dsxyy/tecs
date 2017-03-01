/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_xmlrpc_portgroup_method.h
* �ļ���ʶ��
* ����ժҪ��VNetPortGroupXMLRPC��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2013��3��3��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�
*     �޸����ݣ�����
******************************************************************************/

#ifndef VNET_XMLRPC_PORTGROUP_METHOD_H
#define VNET_XMLRPC_PORTGROUP_METHOD_H

#include "sky.h"
#include "vnet_api.h"
#include "tecs_rpc_method.h"
#include "vnet_libnet.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"

#include "vnet_xmlrpc_netplane_method.h"

using namespace std;
namespace zte_tecs {
extern  int32   vnet_print_xmlrpc;
#define VNETDBG_XMLRPC(name, no, var)    {if(vnet_print_xmlrpc) (cout << "-->" << name << ":" << no << " :: " << var << endl);}
/**
@brief ��������: PortGroup Data������Ϣ
@li @b ����˵������
*/
class VNetConfigPortGroupDataXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
{
/*******************************************************************************
* 1. public section
********************************************************************************/
public:
    VNetConfigPortGroupDataXMLRPC(UserPool *upool=0)
    {
        _method_name = "VNetConfigPortGroupDataXMLRPC";
        _user_id = -1;
        _upool = upool;
    }
    ~VNetConfigPortGroupDataXMLRPC(){};

    void MethodEntry( xmlrpc_c::paramList const& paramList,
        xmlrpc_c::value *   const  retval);

public:
    int32 m_iAction;

private:

    DISALLOW_COPY_AND_ASSIGN(VNetConfigPortGroupDataXMLRPC);
};


class VNetConfigPortGroupVlanXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
{
/*******************************************************************************
*1. public section
*******************************************************************************/
public:
    VNetConfigPortGroupVlanXMLRPC(UserPool *upool=0)
    {
        _method_name = "VNetConfigPortGroupVlanXMLRPC";
        _user_id = -1;
        _upool = upool;
    }
   ~VNetConfigPortGroupVlanXMLRPC(){};

    void MethodEntry( xmlrpc_c::paramList const& paramList,
        xmlrpc_c::value *   const  retval);

public:
    int32 m_iAction;

private:

    DISALLOW_COPY_AND_ASSIGN(VNetConfigPortGroupVlanXMLRPC);
};

class VNetPortGroupQueryXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
{
/*******************************************************************************
*1. public section
*******************************************************************************/
public:
    VNetPortGroupQueryXMLRPC(UserPool *upool=0)
    {
        _method_name = "VNetPortGroupQueryXMLRPC";
        _user_id = -1;
        _upool = upool;
    }
   ~VNetPortGroupQueryXMLRPC(){};

    void MethodEntry( xmlrpc_c::paramList const& paramList,
        xmlrpc_c::value *   const  retval);

public:
    int32 m_iAction;

private:

    DISALLOW_COPY_AND_ASSIGN(VNetPortGroupQueryXMLRPC);
};

}  /* end namespace zte_tecs */


#endif /* end TC_VNET_CONFIG_H */


