/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�current_alarm_info_method.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CurrentAlarm�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��27��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/27
*     �� �� �ţ�V1.0
*     �� �� �ˣ�����
*     �޸����ݣ�����
*******************************************************************************/


#ifndef SNMP_INFO_METHOD_H
#define SNMP_INFO_METHOD_H

#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "snmp_database_pool.h"

#include <sstream>
#include <ctime>


// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;

#define SNMP_USER_PASSWORD_MAX_LENGTH  20

#define SNMP_CONFIGURATION_NOT_EXIST TECS_ERR_SNMP_BASE
#define SNMP_CONFIGURATION_FAIL_QUERY TECS_ERR_SNMP_BASE+1
#define SNMP_CONFIGURATION_ADD_FAIL   TECS_ERR_SNMP_BASE+2
#define SNMP_CONFIGURATION_UPDATE_FAILED TECS_ERR_SNMP_BASE+3
#define SNMP_CONFIGURATION_WRONG_VALUE TECS_ERR_SNMP_BASE+4

#define TRAP_ADDRESS_NOT_EXIST TECS_ERR_SNMP_BASE
#define TRAP_ADDRESS_FAIL_QUERY TECS_ERR_SNMP_BASE+1
#define TRAP_ADDRESS_ADD_FAIL   TECS_ERR_SNMP_BASE+2
#define TRAP_ADDRESS_UPDATE_FAILED TECS_ERR_SNMP_BASE+3
#define TRAP_ADDRESS_WRONG_VALUE TECS_ERR_SNMP_BASE+4
#define TRAP_ADDRESS_ALREADY_EXIST TECS_ERR_SNMP_BASE+20
#define TRAP_ADDRESS_REC_NOT_EXIST TECS_ERR_SNMP_BASE+21
#define TRAP_ADDRESS_REC_DELETE_ERROR TECS_ERR_SNMP_BASE+22

#define SNMP_USER_NOT_EXIST TECS_ERR_SNMP_BASE
#define SNMP_USER_FAIL_QUERY TECS_ERR_SNMP_BASE+1
#define SNMP_USER_ADD_FAIL   TECS_ERR_SNMP_BASE+2
#define SNMP_USER_UPDATE_FAILED TECS_ERR_SNMP_BASE+3
#define SNMP_USER_WRONG_VALUE TECS_ERR_SNMP_BASE+4
#define SNMP_USER_SESSION_FAIL TECS_ERR_SNMP_BASE+10
#define SNMP_USER_PASSWORD_EXCEED_LENGTH TECS_ERR_SNMP_BASE+11


namespace zte_tecs
{

class SnmpConfigurationPoolInfoMethod : public TecsRpcMethod
{

public:
    SnmpConfigurationPoolInfoMethod(UserPool *upool);
    ~SnmpConfigurationPoolInfoMethod(){};
	virtual void MethodEntry(xmlrpc_c::paramList const& paramList,
                             xmlrpc_c::value *   const  retval);

   
private:
    DISALLOW_COPY_AND_ASSIGN(SnmpConfigurationPoolInfoMethod); 
    SnmpConfigurationPool* _snmp_conf_pool;
};

class SnmpConfigurationPoolInfoSetMethod : public TecsRpcMethod
{

public:
    SnmpConfigurationPoolInfoSetMethod(UserPool *upool);
    ~SnmpConfigurationPoolInfoSetMethod(){};
	virtual void MethodEntry(xmlrpc_c::paramList const& paramList,
                             xmlrpc_c::value *   const  retval);

   
private:
    DISALLOW_COPY_AND_ASSIGN(SnmpConfigurationPoolInfoSetMethod); 
    SnmpConfigurationPool* _snmp_conf_pool;
};


class TrapAddressPoolInfoMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    TrapAddressPoolInfoMethod (UserPool *upool);
	
    ~TrapAddressPoolInfoMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                            xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(TrapAddressPoolInfoMethod); 
   TrapAddressPool* _trap_address_pool;

};

class TrapAddressPoolInfoSetMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    TrapAddressPoolInfoSetMethod (UserPool *upool);
	
    ~TrapAddressPoolInfoSetMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                            xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(TrapAddressPoolInfoSetMethod); 
   TrapAddressPool* _trap_address_pool;

};

class TrapAddressPoolInfoDeleteMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    TrapAddressPoolInfoDeleteMethod (UserPool *upool);
	
    ~TrapAddressPoolInfoDeleteMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                            xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(TrapAddressPoolInfoDeleteMethod); 
   TrapAddressPool* _trap_address_pool;

};

class SnmpXmlRpcUserPoolGetMethod : public TecsRpcMethod
{

public:
    SnmpXmlRpcUserPoolGetMethod(UserPool *upool);
    ~SnmpXmlRpcUserPoolGetMethod(){};
	virtual void MethodEntry(xmlrpc_c::paramList const& paramList,
                             xmlrpc_c::value *   const  retval);

   
private:
    DISALLOW_COPY_AND_ASSIGN(SnmpXmlRpcUserPoolGetMethod); 
    SnmpXmlRpcUserPool* _snmp_user_pool;
};

class SnmpXmlRpcUserPoolSetMethod : public TecsRpcMethod
{

public:
    SnmpXmlRpcUserPoolSetMethod(UserPool *upool);
    ~SnmpXmlRpcUserPoolSetMethod(){};
	virtual void MethodEntry(xmlrpc_c::paramList const& paramList,
                             xmlrpc_c::value *   const  retval);

   
private:
    DISALLOW_COPY_AND_ASSIGN(SnmpXmlRpcUserPoolSetMethod); 
    SnmpXmlRpcUserPool* _snmp_user_pool;
};

}
#endif

