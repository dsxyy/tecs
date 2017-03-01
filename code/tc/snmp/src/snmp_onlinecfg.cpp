#include "snmp_onlinecfg.h"
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "mib_module_includes.h"
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include "snmpUDPDomain.h"
#include "crypt.h"

extern char g_community[VACMSTRINGLEN];

extern oid snmpNotifyTable_variables_oid[9];

/**********************************************************************
* SetSNMPTrapCommunity
* 功能描述：设置TRAP pdu的共同体名字段值
* 访问的表：无
* 修改的表：无
* 输入参数：
      TrapCommunity - 将要设置的新的共同体名
* 输出参数：无
* 返 回 值：
          0 - 成功；1 - 失败
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2010/06/05    V1.0    曹亮        创建
************************************************************************/
int SetSNMPTrapCommunity (char *TrapCommunity, size_t Community_len)
{
    struct targetAddrTable_struct *ptr;
    struct targetParamTable_struct *ptr2;

    if (NULL == TrapCommunity)
        return 1;

    for (ptr = get_addrTable(); ptr != 0; ptr = ptr->next)
    {
        memset (ptr->sess->community, 0, ptr->sess->community_len);
        memcpy (ptr->sess->community, TrapCommunity, Community_len);
        ptr->sess->community_len = Community_len;
    }

    for (ptr2 = get_aPTable(); ptr2 != 0; ptr2 = ptr2->next)
    {
        memset (ptr2->secName, 0, strlen (ptr2->secName));
        memcpy (ptr2->secName, TrapCommunity, Community_len);
    }

    snmpd_parse_config_trapcommunity ("trapcommunity", TrapCommunity);

    return 0;
}

void SetSnmpCommunity(char *community)
{
    memset(g_community,VACMSTRINGLEN,0);
    if(strlen(community) < VACMSTRINGLEN)
        memcpy(g_community,community,strlen(community));
}

int CheckCommunityLength(char *community)
{
    if(strlen(community) >= VACMSTRINGLEN)
        return 1;
    else
        return 0;
}

int CheckTrapVersion(int trapversion)
{
    if(trapversion != SNMP_VERSION_2c && trapversion != SNMP_VERSION_1)
        return 1;
    else
        return 0;
}

void SetTrapAddress(char *cptr)
{
    if (GetTrapVersion() == SNMP_VERSION_1)
    {
        snmpd_parse_config_trapsink("trapsink",cptr);
    }
    else
    {
        snmpd_parse_config_trap2sink("trap2sink",cptr);
    }
}

int DeleteTrapAddress(int address,int port)
{
    struct targetAddrTable_struct *ptr = NULL;
    struct targetParamTable_struct *parm_ptr = NULL;
    netsnmp_session *sess;
    char buf[100] = {0};
    ostringstream   oss;
    oid notify_oid[50] = {0};
    int oid_len = 0;
    u_char value[100] = {0};
    long lvalue = 0;
    
    ptr = get_addrForAddress(address,port);
    
    if (ptr == NULL)
        return 1;
    else
    {
        memcpy(buf,ptr->tagList,strlen(ptr->tagList));
        sess = get_target_sessions (buf, NULL, NULL);
        
        if(sess == NULL)
            return 1;
        else
        {            
            snmpTargetAddrTable_remove(ptr);
            
            parm_ptr = get_paraForName(buf);
            
            if(parm_ptr)
                snmpTargetParamTable_remove(parm_ptr);
                
            memcpy(notify_oid,snmpNotifyTable_variables_oid,sizeof(snmpNotifyTable_variables_oid)); 
            
            oid_len = sizeof(snmpNotifyTable_variables_oid)/sizeof(oid);
            
            notify_oid[oid_len] = 1;
            notify_oid[oid_len+1] = 5;
            
            oid_len += 2;
            
            for(int i=0;i<(int)strlen(buf);i++)
            {
                notify_oid[oid_len+i] = (oid)buf[i];
            }
            
            oid_len += strlen(buf);
            
            lvalue = RS_DESTROY;
            memcpy(value,&lvalue,sizeof(long));
            write_snmpNotifyRowStatus(ACTION,
                          value,
                          ASN_INTEGER,
                          sizeof(long),
                          NULL, notify_oid, oid_len);
            
            write_snmpNotifyRowStatus(COMMIT,
                          value,
                          ASN_INTEGER,
                          sizeof(long),
                          NULL, notify_oid, oid_len);              
                          
        }
    }
    return 0;
}

