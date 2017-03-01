/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarmPoll类实现文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月27日
*
* 修改记录1：
*     修改日期：2011/7/27
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/

#include "snmp_info_method.h"
#include "snmp_onlinecfg.h"
#include "snmp_interface.h"

extern int SetTrapVersion(int TrapVersion);

namespace zte_tecs
{

using namespace xmlrpc_c;
SnmpConfigurationPoolInfoMethod::SnmpConfigurationPoolInfoMethod(UserPool *upool=0)
{
    _method_name = "SnmpConfigurationMethod";
    _user_id = -1;
    _upool = upool;
    _snmp_conf_pool = SnmpConfigurationPool::GetInstance();
}

/******************************************************************************/

void SnmpConfigurationPoolInfoMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int64 start_index = 0;
    int64 query_count = 0;
    int64 next_index = 0;
    int64 max_count = 0;
    vector<xmlrpc_c::value> arraySnmpConf;
    vector<xmlrpc_c::value> arraySnmpConf1;
    ostringstream       oss;
    string              str;
    string              str1;
    ostringstream       os;
    int ret_code  = 0;

    start_index = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count = xmlrpc_c::value_i8(paramList.getI8(2));

    uid = get_userid();
    oss <<" 1=1 " <<\
          " order by oid limit " << query_count <<\
          " offset  "<< start_index    ;
    str = oss.str();
    str1 = "1=1";

    if (_snmp_conf_pool == NULL)
    {
        ret_code = SNMP_CONFIGURATION_NOT_EXIST;
        xRET2(xINT(ret_code), xSTR("SNMP configuration not exist. \n"));
        return;
    }
    else
    {
        if (_snmp_conf_pool->Dump(arraySnmpConf1,str1) < 0 ) //首先查全表，获取max_count
        {
            ret_code = SNMP_CONFIGURATION_FAIL_QUERY;
            xRET2(xINT(ret_code), xSTR("Fail to query SNMP configuration. \n"));
            return;
        }
        else
        {
            max_count = arraySnmpConf1.size();
            if (_snmp_conf_pool->Dump(arraySnmpConf,str) < 0) //分包查询
            {
                ret_code = SNMP_CONFIGURATION_FAIL_QUERY;
                xRET2(xINT(ret_code), xSTR("Fail to query SNMP configuration. \n"));
                return;
            }
            else
            {
                if(arraySnmpConf.size() < (unsigned int)(query_count) )
                {
                    next_index = -1;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(arraySnmpConf.size()), xARRAY(arraySnmpConf));
                }
                else
                {
                  /*  map<string, xmlrpc_c::value> p = value_struct(arraySnmpConf[arraySnmpConf.size()-1]);
                    map<string, xmlrpc_c::value>::iterator it;

                    if(p.end() != (it = p.find("oid")))
                    {
                        next_index = xmlrpc_c::value_int(it->second);
                    } */
                    next_index =  start_index + query_count;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(query_count), xARRAY(arraySnmpConf));
                }
            }

        }
    }
	/* 返回  */
}

SnmpConfigurationPoolInfoSetMethod::SnmpConfigurationPoolInfoSetMethod(UserPool *upool=0)
{
    _method_name = "SnmpConfigurationSetMethod";
    _user_id = -1;
    _upool = upool;
    _snmp_conf_pool = SnmpConfigurationPool::GetInstance();
}

void SnmpConfigurationPoolInfoSetMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int ret_code  = 0;
    SnmpConfiguration *sc = NULL;
    string error_str;
    int64 oid = 0;
    int rc = 0;
    ostringstream       oss;

    string community = xmlrpc_c::value_string(paramList.getString(1));
    string trapcommunity = xmlrpc_c::value_string(paramList.getString(2));
    int trapversion = xmlrpc_c::value_int(paramList.getInt(3));

    if (CheckCommunityLength((char *)community.c_str()) != 0 ||
        CheckCommunityLength((char *)trapcommunity.c_str()) != 0 ||
        CheckTrapVersion(trapversion) != 0)
    {
        oss << "SNMP configuration with wrong value. \n";
        ret_code = SNMP_CONFIGURATION_WRONG_VALUE;
    }
    else
    {

        uid = get_userid();

        if (_snmp_conf_pool == NULL)
        {
            oss << "SNMP configuration not exist. \n";
            ret_code = SNMP_CONFIGURATION_NOT_EXIST;
        }
        else
        {
            sc = _snmp_conf_pool->Get(1,false);

            if(sc)
            {
                _snmp_conf_pool->Put(sc,true);

                T_SnmpConfiguration snmp_conf(community,trapcommunity,trapversion);

                sc->set_current_conf(snmp_conf);

                rc = _snmp_conf_pool->Update(sc);

                if (rc != 0)
                {
                    oss << "SNMP configuration update failed. \n";
                    ret_code = SNMP_CONFIGURATION_UPDATE_FAILED;
                }
            }
            else
            {
                T_SnmpConfiguration snmp_conf(community,trapcommunity,trapversion);
                if (_snmp_conf_pool->Allocate(&oid, snmp_conf, error_str) < 0 )
                {
                    OutPut(SYS_ERROR,"fail to operate snmp_conf table,because of %s\n",error_str.c_str());
                    oss << "fail to operate snmp_conf table, because of " << error_str.c_str();
                    ret_code = SNMP_CONFIGURATION_ADD_FAIL;
                }
                else
                {
                	OutPut(SYS_DEBUG,"add a new snmp_conf table\n");
                }
            }

            if (ret_code == 0)
            {
                /*修改内存中配置*/
                SetSnmpCommunity((char *)community.c_str());
                SetSNMPTrapCommunity((char *)trapcommunity.c_str(),trapcommunity.length());
                SetTrapVersion(trapversion);
                oss << "";
            }

        }
    }

    /* 返回  */
    xRET2(xINT(ret_code), xSTR(oss.str()));
}


/******************************************************************************/

TrapAddressPoolInfoMethod::TrapAddressPoolInfoMethod (UserPool *upool=0)
{
    _method_name = "TrapAddressMethod";
    _user_id = -1;
    _upool = upool;
    _trap_address_pool = TrapAddressPool::GetInstance();

}

void TrapAddressPoolInfoMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int64 start_index = 0;
    int64 query_count = 0;
    int64 next_index = 0;
    int64 max_count = 0;
    vector<xmlrpc_c::value>          arrayTrapAddress;
    vector<xmlrpc_c::value>          arrayTrapAddress1;
    ostringstream       oss;
    string              str;
    string              str1;
    ostringstream       os;
    int ret_code  = 0;

    start_index = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count = xmlrpc_c::value_i8(paramList.getI8(2));

    uid = get_userid();
    oss <<" 1=1 " <<\
          " order by oid limit " << query_count <<\
          " offset  "<< start_index  ;
    str = oss.str();
    str1 = "1=1";

    if (_trap_address_pool == NULL)
    {
        ret_code = TRAP_ADDRESS_NOT_EXIST;
        xRET2(xINT(ret_code), xSTR("Trap address not exist. \n"));
        return;
    }
    else
    {
        if (_trap_address_pool->Dump(arrayTrapAddress1,str1) < 0 )
        {
            ret_code = TRAP_ADDRESS_FAIL_QUERY;
            xRET2(xINT(ret_code),xSTR("Fail to query trap address. \n"));
            return;
        }
        else
        {
            max_count = arrayTrapAddress1.size();
            if (_trap_address_pool->Dump(arrayTrapAddress,str) < 0) //分包查询
            {
                ret_code = TRAP_ADDRESS_FAIL_QUERY;
                xRET2(xINT(ret_code), xSTR("Fail to query trap address. \n"));
                return;
            }
            else
            {
                if(arrayTrapAddress.size() < (unsigned int)(query_count) )
                {
                    next_index = -1;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(arrayTrapAddress.size()), xARRAY(arrayTrapAddress));
                }
                else
                {
                    /* map<string, xmlrpc_c::value> p = value_struct(arrayTrapAddress[arrayTrapAddress.size()-1]);
                    map<string, xmlrpc_c::value>::iterator it;

                    if(p.end() != (it = p.find("oid")))
                    {
                        next_index = xmlrpc_c::value_int(it->second);
                    } */

                    next_index =  start_index + query_count;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(query_count), xARRAY(arrayTrapAddress));
                }
            }
        }
    }

    /* 返回  */
}

/******************************************************************************/

TrapAddressPoolInfoSetMethod::TrapAddressPoolInfoSetMethod (UserPool *upool=0)
{
    _method_name = "TrapAddressSetMethod";
    _user_id = -1;
    _upool = upool;
    _trap_address_pool = TrapAddressPool::GetInstance();

}

void TrapAddressPoolInfoSetMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int ret_code  = 0;
    TrapAddress *ta = NULL;
    string error_str;
    int64 oid = 0;
//    int rc = 0;
    ostringstream   oss;
    string address_key;


    string ipaddress = xmlrpc_c::value_string(paramList.getString(1));
    int port = xmlrpc_c::value_int(paramList.getInt(2));
    oss << ipaddress << ":" << port ;
    address_key = oss.str();

    if ((port < 0) || !IsValidIp(ipaddress))
    {
        oss << "Trap address wrong value. \n";
        ret_code = TRAP_ADDRESS_WRONG_VALUE;
    }
    else
    {
        uid = get_userid();

        if (_trap_address_pool == NULL)
        {
            ret_code = TRAP_ADDRESS_NOT_EXIST;
            xRET2(xINT(ret_code), xSTR("Trap address not exist. \n"));
            return;
        }
        else
        {
            ta = _trap_address_pool->Get(address_key,false);

            if(ta)
            {
/*
                _trap_address_pool->Put(ta,true);

                T_TrapAddress trap_address(ipaddress,port,address_key);

                ta->set_current_address(trap_address);

                rc = _trap_address_pool->Update(ta);

                if (rc != 0)
                {
                    ret_code = TRAP_ADDRESS_UPDATE_FAILED;
                }
*/
                ret_code = TRAP_ADDRESS_ALREADY_EXIST;
                oss << "Trap address already exist. \n";
            }
            else
            {
                T_TrapAddress trap_address(ipaddress,port);
                if (_trap_address_pool->Allocate(&oid, trap_address, error_str) < 0 )
                {
                    OutPut(SYS_ERROR,"fail to operate trap_address table,because of %s\n",error_str.c_str());
                    oss << "fail to operate trap_address table,because of " << error_str.c_str();
                    ret_code = TRAP_ADDRESS_ADD_FAIL;
                }
                else
                {
                    OutPut(SYS_DEBUG,"add a new trap_address table\n");
                    oss << "add a new trap_address table\n";
                }
            }

            if (ret_code == 0)
            {
                /*修改内存中配置*/
                SetTrapAddress((char *)address_key.c_str());
                oss << "";
            }

        }
    }

    /* 返回  */
    xRET2(xINT(ret_code), xSTR(oss.str()));
    return;
}

/******************************************************************************/

TrapAddressPoolInfoDeleteMethod::TrapAddressPoolInfoDeleteMethod (UserPool *upool=0)
{
    _method_name = "TrapAddressDeleteMethod";
    _user_id = -1;
    _upool = upool;
    _trap_address_pool = TrapAddressPool::GetInstance();

}

void TrapAddressPoolInfoDeleteMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int ret_code  = 0;
    TrapAddress *ta = NULL;
    string error_str;
//    int rc = 0;
    ostringstream   oss;
    string address_key;


    string ipaddress = xmlrpc_c::value_string(paramList.getString(1));
    int port = xmlrpc_c::value_int(paramList.getInt(2));
    oss << ipaddress << ":" << port ;
    address_key = oss.str();

    if ((port < 0) || !IsValidIp(ipaddress))
    {
        oss << "Trap address wrong value. \n";
        ret_code = TRAP_ADDRESS_WRONG_VALUE;
    }
    else
    {
        uid = get_userid();

        if (_trap_address_pool == NULL)
        {
            ret_code = TRAP_ADDRESS_NOT_EXIST;
            xRET2(xINT(ret_code), xSTR("Trap address not exist. \n"));
            return;
        }
        else
        {
            ta = _trap_address_pool->Get(address_key,false);

            if(ta)
            {
                if(_trap_address_pool->Drop(ta) < 0 )
                {
                    OutPut(SYS_ERROR,"fail to operate trap_address table,because of %s\n",error_str.c_str());
                    oss << "fail to operate trap_address table,because of " << error_str.c_str();
                    ret_code = TRAP_ADDRESS_REC_DELETE_ERROR;
                }
            }
            else
            {
                oss << "Trap address record not exist. \n";
                ret_code = TRAP_ADDRESS_REC_NOT_EXIST;
            }

            if (ret_code == 0)
            {
                /*修改内存中配置*/
                int ip = inet_network(ipaddress.c_str());
                if (DeleteTrapAddress(ip,port) != 0)
                    OutPut(SYS_ERROR,"fail to delete trap address");
                oss << "";
            }

        }
    }

    /* 返回  */
    xRET2(xINT(ret_code), xSTR(oss.str()));
    return;
}

SnmpXmlRpcUserPoolGetMethod::SnmpXmlRpcUserPoolGetMethod(UserPool *upool=0)
{
    _method_name = "SnmpXmlRpcUserGetMethod";
    _user_id = -1;
    _upool = upool;
    _snmp_user_pool = SnmpXmlRpcUserPool::GetInstance();
}

/******************************************************************************/

void SnmpXmlRpcUserPoolGetMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int64 start_index = 0;
    int64 query_count = 0;
    int64 next_index = 0;
    int64 max_count = 0;
    vector<xmlrpc_c::value> arraySnmpUser;
    vector<xmlrpc_c::value> arraySnmpUser1;
    ostringstream       oss;
    string              str;
    string              str1;
    ostringstream       os;
    int ret_code  = 0;

    start_index = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count = xmlrpc_c::value_i8(paramList.getI8(2));

    uid = get_userid();
    oss <<" 1=1 " <<\
          " order by oid limit " << query_count <<\
          " offset  "<< start_index    ;
    str = oss.str();
    str1 = "1=1";

    if (_snmp_user_pool == NULL)
    {
        ret_code = SNMP_USER_NOT_EXIST;
        xRET2(xINT(ret_code), xSTR("SNMP user not exist. \n"));
        return;
    }
    else
    {
        if (_snmp_user_pool->Dump(arraySnmpUser1,str1) < 0 ) //首先查全表，获取max_count
        {
            ret_code = SNMP_USER_FAIL_QUERY;
            xRET2(xINT(ret_code), xSTR("Fail to query SNMP user. \n"));
            return;
        }
        else
        {
            max_count = arraySnmpUser1.size();
            if (_snmp_user_pool->Dump(arraySnmpUser,str1) < 0) //分包查询
            {
                ret_code = SNMP_USER_FAIL_QUERY;
                xRET2(xINT(ret_code), xSTR("Fail to query SNMP user. \n"));
                return;
            }
            else
            {
                if(arraySnmpUser.size() < (unsigned int)(query_count) )
                {
                    next_index = -1;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(arraySnmpUser.size()), xARRAY(arraySnmpUser));
                }
                else
                {
                   /* map<string, xmlrpc_c::value> p = value_struct(arraySnmpUser[arraySnmpUser.size()-1]);
                    map<string, xmlrpc_c::value>::iterator it;

                    if(p.end() != (it = p.find("oid")))
                    {
                        next_index = xmlrpc_c::value_int(it->second);
                    } */

                    next_index =  start_index + query_count;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(query_count), xARRAY(arraySnmpUser));
                }
            }

        }
    }
	/* 返回  */
}

SnmpXmlRpcUserPoolSetMethod::SnmpXmlRpcUserPoolSetMethod(UserPool *upool=0)
{
    _method_name = "SnmpXmlRpcUserSetMethod";
    _user_id = -1;
    _upool = upool;
    _snmp_user_pool = SnmpXmlRpcUserPool::GetInstance();
}

void SnmpXmlRpcUserPoolSetMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int ret_code  = 0;
    SnmpXmlRpcUser *sc = new SnmpXmlRpcUser;
    string error_str;
//    int64 oid = 0;
    int rc = 0;
    ostringstream       oss;

    string user = xmlrpc_c::value_string(paramList.getString(1));

    uid = get_userid();

    if (_snmp_user_pool == NULL)
    {
        oss << "SNMP user not exist. \n";
        ret_code = SNMP_USER_NOT_EXIST;
    }
    else
    {
        if(_snmp_user_pool->GetUser(sc) == 0)
        {
            T_SnmpXmlRpcUser snmp_user(user);

            sc->set_current_user(snmp_user);

            rc = _snmp_user_pool->SetUser(sc);

            if (rc != 0)
            {
                oss << "Fail to update SNMP user. \n";
                ret_code = SNMP_USER_UPDATE_FAILED;
            }
            else
            {
                /*首先判断是否可以插入，如果可以插入，则删除整表并重新插入*/
                _snmp_user_pool->DeleteTable();
                _snmp_user_pool->SetUser(sc);
            }
        }
        else
        {
            T_SnmpXmlRpcUser snmp_user(user);

            sc->set_current_user(snmp_user);

            if (_snmp_user_pool->SetUser(sc) < 0 )
            {
                OutPut(SYS_ERROR,"fail to add SNMP user.\n");
                oss << "fail to add SNMP user.";
                ret_code = SNMP_USER_ADD_FAIL;
            }
            else
            {
                OutPut(SYS_DEBUG,"add a new snmp_user table\n");
            }
        }
        if (ret_code == 0)
        {
            if (!SnmpLoginAuth(user))
            {
                OutPut(SYS_ERROR,"fail to get session string\n");
                oss << "Fail to get session string \n";
                ret_code = SNMP_USER_SESSION_FAIL;
            }
        }

    }

    /* 返回  */
    xRET2(xINT(ret_code), xSTR(oss.str()));
    delete sc;
}

} // namespace zte_tecs


