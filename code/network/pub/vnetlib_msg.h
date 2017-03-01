/*********************************************************************
* 版权所有 (C)2009, 中兴通讯股份有限公司。
*
* 文件名称： vnetlib_msg.h
* 文件标识：
* 其它说明： 本文件定义了vnetlib库对外事件接口
* 当前版本： V1.0
* 作    者： chen.zhiwei
* 完成日期：
*
* 修改记录1：
*    修改日期：2013年01月31日
*    版 本 号：V1.0
*    修 改 人：陈志伟
*    修改内容：创建
**********************************************************************/

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
#ifndef    _PUB_VNET_MSG_H_
#define    _PUB_VNET_MSG_H_

/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "../../sky/include/sky.h"
#include "../../sky/include/workflow.h"
#include "../../common/include/vm_messages.h"
#include "vnetlib_common.h"

//VSI相关定义
#define STR_VSIID_INIT_VALUE "00000000000000000000000000000000"
#define STR_MGRID_INIT_VALUE "00000000000000000000000000000001"
#define STR_VSITYPEID_INIT_VALUE "000001"

#define VSIFORMAT_PRINTF "%8.8x%4.4x%4.4x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x"
#define VSIFORMAT_SCANF "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x"
#define MGRIDFORMAT_SCANF "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x"
#define VSITYPEIDFORMAT_SCANF "%2x%2x%2x"


#define VNETLIB_MIN_VLAN_ID      (2)
#define VNETLIB_MAX_VLAN_ID      (4094)

#define VNETLIB_MIN_SEGMENT_ID   (1)
#define VNETLIB_MAX_SEGMENT_ID   (16000000)


#define VNET_XMLRPC_SUCCESS 0
#define VNET_XMLRPC_ERROR   1
#define VNET_XMLRPC_TIMEOUT 2

#define VNET_SET_ACKMSG_REQUEST_INFO(MSG, ACKMSG) \
        ACKMSG.m_mid = MSG.m_mid;

typedef enum tagENLoopPortOper
{
    EN_INVALID_LOOPPORT_OPER = 0,
    EN_ADD_LOOPPORT,
    EN_SET_LOOPPORT,
    EN_UNSET_LOOPPORT,
    EN_DEL_LOOPPORT,
    EN_LOOPPORT_OPER_ALL,
}EN_LOOPPORT_OPER;

typedef enum tagIsolateTech
{
    ISOLATE_VLAN = 0,
    ISOLATE_VXLAN,
    ISOLATE_VLAN_VXLAN,   
}IsolateTech;

class CLoopPortMsg:public VNetConfigBaseMsg
{
    public:
        explicit CLoopPortMsg() {m_dwOper = EN_INVALID_LOOPPORT_OPER;}
        virtual ~CLoopPortMsg() {}

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CLoopPortMsg);
            WRITE_VALUE(m_strvnaUUID);
            WRITE_VALUE(m_strportUUID);
            WRITE_VALUE(m_strName);      
            WRITE_VALUE(m_dwOper);
            SERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CLoopPortMsg);
            READ_VALUE(m_strvnaUUID);
            READ_VALUE(m_strportUUID);            
            READ_VALUE(m_strName);       
            READ_VALUE(m_dwOper);
            DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        int32 IsValidOper(void) const
        {
            if((m_dwOper > EN_INVALID_LOOPPORT_OPER) && (m_dwOper < EN_LOOPPORT_OPER_ALL))
            {
                return 1;
            }
            return 0;
        }

    public:

        void Print(void)
        {
            cout << "LoopPort operation code: " << m_dwOper << endl;
            cout << "VNA UUID: " << m_strvnaUUID << endl;
            cout << "Por UUID: " << m_strportUUID << endl;
            cout << "Por Name: " << m_strName << endl;
            return ;
        }
        
    public:
        string m_strvnaUUID;
        string m_strportUUID;
        string m_strName;
        uint32 m_dwOper;
};

//kernalport operation
typedef enum tagENKernalPortOper
{
    EN_INVALID_KERNALPORT_OPER = 0,
    EN_ADD_KERNALPORT,
    EN_DEL_KERNALPORT,
    EN_MOD_KERNALPORT,
    EN_KERNALPORT_OPER_ALL,        
}EN_KERNALPORT_OPER;

// KernalPort Message;
class CKerNalPortMsg : public VNetConfigBaseMsg
{
    public:
        explicit CKerNalPortMsg() {m_dwOper = EN_INVALID_KERNALPORT_OPER;}
        virtual ~CKerNalPortMsg() {}

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CKerNalPortMsg);
            WRITE_VALUE(m_strVNAUUID);
            WRITE_VALUE(m_strSwitchName);
            WRITE_VALUE(m_strip);
            WRITE_VALUE(m_strmask);
            WRITE_VALUE(m_strName);
            WRITE_VALUE(m_nType);
            WRITE_VALUE(m_strPGUUID);
            WRITE_VALUE(m_isdhcp);
            WRITE_VALUE(m_strUUID);
            WRITE_VALUE(m_strDescription);            
            WRITE_VALUE(m_dwOper);
            SERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CKerNalPortMsg);
            READ_VALUE(m_strVNAUUID);
            READ_VALUE(m_strSwitchName);
            READ_VALUE(m_strip);
            READ_VALUE(m_strmask);
            READ_VALUE(m_strName);
            READ_VALUE(m_nType);
            READ_VALUE(m_strPGUUID);
            READ_VALUE(m_isdhcp);
            READ_VALUE(m_strUUID);
            READ_VALUE(m_strDescription);            
            READ_VALUE(m_dwOper);
            DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        int32 IsValidOper(void) const
        {
            if((m_dwOper > EN_INVALID_KERNALPORT_OPER) && (m_dwOper < EN_KERNALPORT_OPER_ALL))
            {
                return 1;
            }
            return 0;
        }

    public:

        void Print(void)
        {
            cout << "KernalPort operation code: " << m_dwOper << endl;
            cout << "OBJ UUID: " << m_strUUID << endl;
            cout << "name    : " << m_strName <<endl;
            cout << "DESCR: "  << m_strDescription<< endl;
            cout << " is dhcp     :"<<m_isdhcp<<endl;
            cout << " ip          :"<<m_strip<<endl;
            cout << " m_strmask   :"<<m_strmask<<endl;
            cout << " Port Type   :"<<m_nType<<endl;
            cout << " switch name: " <<m_strSwitchName<<endl;
 
            return ;
        }
        
    public:
        string m_strVNAUUID;
        string m_strSwitchName;
        string m_strip;
        string m_strmask;
        string m_strName;
        int32  m_nType;
        string m_strPGUUID;
        int32  m_isdhcp;
        string m_strUUID;
        string m_strDescription;
        uint32 m_dwOper;
};

// Netplane operation
typedef enum tagENNegplaneOper
{
    EN_INVALID_NETPLANE_OPER = 0,
    EN_ADD_NETPLANE,
    EN_DEL_NETPLANE,
    EN_MOD_NETPLANE,
    EN_NETPLANE_OPER_ALL,        
}EN_NETPLANE_OPER;

// Netplane Message;
class CNetplaneMsg : public VNetConfigBaseMsg
{
    public:
        explicit CNetplaneMsg() {m_dwOper = EN_INVALID_NETPLANE_OPER;}
        virtual ~CNetplaneMsg() {}

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CNetplaneMsg);  
            WRITE_STRING(m_strName);
            WRITE_STRING(m_strUUID);
            WRITE_STRING(m_strDescription);            
            WRITE_DIGIT(m_dwMTU); 
            WRITE_DIGIT(m_dwOper);
            SERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CNetplaneMsg);
            READ_STRING(m_strName);
            READ_STRING(m_strUUID);
            READ_STRING(m_strDescription);            
            READ_DIGIT(m_dwMTU);
            READ_DIGIT(m_dwOper);
            DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        int32 IsValidOper(void) const
        {
            if((m_dwOper > EN_INVALID_NETPLANE_OPER) && (m_dwOper < EN_NETPLANE_OPER_ALL))
            {
                return 1;
            }
            return 0;
        }

    public:

        void Print(void)
        {
            cout << "NetPlane operation code: " << m_dwOper << "." << endl;
            cout << "OBJ UUID: " << m_strUUID << "." << endl;
            cout << "Netplane: " << m_strName 
                 << ", DESCR: "  << m_strDescription
                 << endl;
            cout << "Netplane MTU: " << m_dwMTU << "." << endl;                
            return ;
        }
        
    public:
        string m_strName;
        string m_strUUID;
        string m_strDescription;
        uint32 m_dwMTU;  
        uint32 m_dwOper;
};

// Logic Network operation code
typedef enum tagENLogicNetworkOper
{
    EN_INVALID_LOGIC_NETWORK_OPER = 0,
    EN_ADD_LOGIC_NETWORK,
    EN_DEL_LOGIC_NETWORK,
    EN_MOD_LOGIC_NETWORK,
    EN_LOGIC_NETWORK_OPER_ALL,        
}EN_LOGIC_NETWORK_OPER;

typedef enum tagENIPBootProtoMode
{
    EN_IP_BP_MODE_INVALID = -1,
    EN_IP_BP_MODE_STATIC = 0,
    EN_IP_BP_MODE_DYNAMIC, 
    EN_IP_BP_MODE_NONE, 
    EN_IP_BP_MODE_ALL
}EN_IP_BOOT_PROTO_MODE;

class CLogicNetworkMsg : public VNetConfigBaseMsg
{
    public:
        explicit CLogicNetworkMsg() 
        {
            m_dwOper = EN_INVALID_LOGIC_NETWORK_OPER; 
            m_nIPBootProtoMode = EN_IP_BP_MODE_INVALID;
        }
        
        virtual ~CLogicNetworkMsg() {}

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CLogicNetworkMsg);                 
            WRITE_STRING(m_strPortGroupUUID)
            WRITE_STRING(m_strName);
            WRITE_STRING(m_strUUID);
            WRITE_STRING(m_strDescription); 
            WRITE_DIGIT(m_nIPBootProtoMode);
            WRITE_DIGIT(m_dwOper);
            SERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CLogicNetworkMsg);
            READ_STRING(m_strPortGroupUUID)
            READ_STRING(m_strName);
            READ_STRING(m_strUUID);
            READ_STRING(m_strDescription);  
            READ_DIGIT(m_nIPBootProtoMode);
            READ_DIGIT(m_dwOper);
            DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
        }

        int32 IsValidOper(void) const
        {
            if((m_dwOper > EN_INVALID_LOGIC_NETWORK_OPER) && (m_dwOper < EN_LOGIC_NETWORK_OPER_ALL))
            {
                return 1;
            }
            return 0;
        }

        int32 IsValidIPBootProtoMode(void) const
        {
            if((EN_IP_BP_MODE_STATIC == m_nIPBootProtoMode) || 
               (EN_IP_BP_MODE_DYNAMIC == m_nIPBootProtoMode) ||
               (EN_IP_BP_MODE_NONE == m_nIPBootProtoMode))
            {
                return 1;
            }
            return 0;
        }

    public:

        void Print(void)
        {
            cout << "Logic network operation code: " << m_dwOper << "." << endl;                
            cout << "Port group UUID: " << m_strPortGroupUUID << "." << endl;                
            cout << "Logic network UUID: " << m_strUUID << "." << endl;
            cout << "Logic network name: " << m_strName 
                 << ", IP alloc mode: "  << m_nIPBootProtoMode
                 << ", Description: "  << m_strDescription
                 << endl;
            return ;
        }
        
    public:
        string m_strPortGroupUUID;
        string m_strName;
        string m_strUUID;
        string m_strDescription;
        int32  m_nIPBootProtoMode;
        uint32 m_dwOper;
};
// 资源池;
// IPv4 address range;
typedef enum tagENIPv4RangeOper
{
    EN_INVALID_IPV4_RANGE_OPER = 0,
    EN_ADD_IPV4_RANGE,
    EN_DEL_IPV4_RANGE,
    EN_IPV4_RANGE_OPER_ALL,        
}EN_IPV4_RANGE_OPER;

class CNetplaneIPv4RangeMsg : public VNetConfigBaseMsg
{
public:
    explicit CNetplaneIPv4RangeMsg() {m_dwOper = EN_INVALID_IPV4_RANGE_OPER;}
    virtual ~CNetplaneIPv4RangeMsg() {}

public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CNetplaneIPv4RangeMsg);                 
        WRITE_STRING(m_strIPv4Start);
        WRITE_STRING(m_strIPv4StartMask);
        WRITE_STRING(m_strIPv4End);
        WRITE_STRING(m_strIPv4EndMask);
        WRITE_STRING(m_strNetplaneUUID);
        WRITE_DIGIT(m_dwOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CNetplaneIPv4RangeMsg);
        READ_STRING(m_strIPv4Start);
        READ_STRING(m_strIPv4StartMask);
        READ_STRING(m_strIPv4End);
        READ_STRING(m_strIPv4EndMask);
        READ_STRING(m_strNetplaneUUID);
        READ_DIGIT(m_dwOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    int32 IsValidOper(void) const
    {
        if((m_dwOper > EN_INVALID_IPV4_RANGE_OPER) && (m_dwOper < EN_IPV4_RANGE_OPER_ALL))
        {
            return 1;
        }
        return 0;
    }

    int32 IsValidRange(void) const
    {
        if(m_strIPv4Start.empty()    ||
           m_strIPv4End.empty()      ||
           m_strIPv4StartMask.empty()||
           m_strIPv4EndMask.empty()  ||
           m_strNetplaneUUID.empty())
        {
            return 0;
        }
        return 1;
    }

public:

    void Print(void)
    {
        cout << "IP Range operation code: " << m_dwOper << "." << endl;                
        cout << "Range: {" << m_strIPv4Start << "-" << m_strIPv4StartMask 
             << ", " << m_strIPv4End << "-" << m_strIPv4EndMask << "}." << endl;
        return ;
    }
    
public:
    string m_strIPv4Start;
    string m_strIPv4StartMask;
    string m_strIPv4End;            
    string m_strIPv4EndMask;
    string m_strNetplaneUUID;
    uint32 m_dwOper;
};

class CLogicNetworkIPv4RangeMsg : public VNetConfigBaseMsg
{
public:
    explicit CLogicNetworkIPv4RangeMsg() {m_dwOper = EN_INVALID_IPV4_RANGE_OPER;}
    virtual ~CLogicNetworkIPv4RangeMsg() {}

public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CLogicNetworkIPv4RangeMsg);                 
        WRITE_STRING(m_strIPv4Start);
        WRITE_STRING(m_strIPv4StartMask);
        WRITE_STRING(m_strIPv4End);
        WRITE_STRING(m_strIPv4EndMask);
        WRITE_STRING(m_strLogicNetworkUUID);
        WRITE_DIGIT(m_dwOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CLogicNetworkIPv4RangeMsg);
        READ_STRING(m_strIPv4Start);
        READ_STRING(m_strIPv4StartMask);
        READ_STRING(m_strIPv4End);
        READ_STRING(m_strIPv4EndMask);
        READ_STRING(m_strLogicNetworkUUID);
        READ_DIGIT(m_dwOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    int32 IsValidOper(void) const
    {
        if((m_dwOper > EN_INVALID_IPV4_RANGE_OPER) && (m_dwOper < EN_IPV4_RANGE_OPER_ALL))
        {
            return 1;
        }
        return 0;
    }

    int32 IsValidRange(void) const
    {
        if(m_strIPv4Start.empty()    ||
           m_strIPv4End.empty()      ||
           m_strIPv4StartMask.empty()||
           m_strIPv4EndMask.empty()  ||
           m_strLogicNetworkUUID.empty())
        {
            return 0;
        }
        
        return 1;
    }

public:

    void Print(void)
    {
        cout << "IP Range operation code: " << m_dwOper << "." << endl;                
        cout << "Range: {" << m_strIPv4Start << "-" << m_strIPv4StartMask 
             << ", " << m_strIPv4End << "-" << m_strIPv4EndMask << "}." << endl;
        return ;
    }
    
public:
    string m_strIPv4Start;
    string m_strIPv4StartMask;
    string m_strIPv4End;            
    string m_strIPv4EndMask;
    string m_strLogicNetworkUUID;
    uint32 m_dwOper;
};

// MAC range;
typedef enum tagENMACRangeOper
{
    EN_INVALID_MAC_RANGE_OPER = 0,
    EN_ADD_MAC_RANGE,
    EN_DEL_MAC_RANGE,
    EN_MAC_RANGE_OPER_ALL,        
}EN_MAC_RANGE_OPER;

class CMACRangeMsg : public VNetConfigBaseMsg
{
public:
    explicit CMACRangeMsg() {m_dwOper = EN_INVALID_MAC_RANGE_OPER;}
    virtual ~CMACRangeMsg() {}

public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CMACRangeMsg);                 
        WRITE_STRING(m_strMACStart);
        WRITE_STRING(m_strMACEnd);
        WRITE_STRING(m_strNetplaneUUID);
        WRITE_DIGIT(m_dwOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CMACRangeMsg);
        READ_STRING(m_strMACStart);
        READ_STRING(m_strMACEnd);
        READ_STRING(m_strNetplaneUUID);
        READ_DIGIT(m_dwOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    int32 IsValidOper(void) const
    {
        if((m_dwOper > EN_INVALID_MAC_RANGE_OPER) && (m_dwOper < EN_MAC_RANGE_OPER_ALL))
        {
            return 1;
        }
        return 0;
    }

    int32 IsValidRange(void) const
    {
        if(m_strMACStart.empty() || m_strMACEnd.empty())
        {
            return 0;
        }
        
        return 1;
    }

public:

    void Print(void)
    {
        cout << "Mac Range operation code: " << m_dwOper << "." << endl;                
        cout << "Range: {" << m_strMACStart << ", " << m_strMACEnd << "}." << endl;
        return ;
    }
    
public:
    string m_strMACStart;
    string m_strMACEnd;
    string m_strNetplaneUUID;
    uint32 m_dwOper;
};

// Vlan range;
typedef enum tagENVlanRangeOper
{
    EN_INVALID_VLAN_RANGE_OPER = 0,
    EN_ADD_VLAN_RANGE,
    EN_DEL_VLAN_RANGE,
    EN_VLAN_RANGE_OPER_ALL,        
}EN_VLAN_RANGE_OPER;

class CVlanRangeMsg : public VNetConfigBaseMsg
{
public:
    explicit CVlanRangeMsg() {m_dwOper = EN_INVALID_VLAN_RANGE_OPER;}
    virtual ~CVlanRangeMsg() {}

public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVlanRangeMsg);                 
        WRITE_DIGIT(m_nVlanStart);
        WRITE_DIGIT(m_nVlanEnd);
        WRITE_STRING(m_strNetplaneUUID);
        WRITE_DIGIT(m_dwOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVlanRangeMsg);
        READ_DIGIT(m_nVlanStart);
        READ_DIGIT(m_nVlanEnd);
        READ_STRING(m_strNetplaneUUID);
        READ_DIGIT(m_dwOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    int32 IsValidOper(void) const
    {
        if((m_dwOper > EN_INVALID_VLAN_RANGE_OPER) && (m_dwOper < EN_VLAN_RANGE_OPER_ALL))
        {
            return 1;
        }
        return 0;
    }

    int32 IsValidRange(void) const
    {
        if((m_nVlanStart > m_nVlanEnd)       ||
           (m_nVlanStart < VNETLIB_MIN_VLAN_ID) || 
           (m_nVlanEnd > VNETLIB_MAX_VLAN_ID))
        {
            return 0;
        }
        
        return 1;
    }

public:

    void Print(void)
    {
        cout << "Vlan Range operation code: " << m_dwOper << "." << endl;                
        cout << "Range: {" << m_nVlanStart << ", " << m_nVlanEnd << "}." << endl;
        return ;
    }
    
public:
    int32 m_nVlanStart;
    int32 m_nVlanEnd;
    string m_strNetplaneUUID;
    uint32 m_dwOper;
};

// Segment range;
typedef enum tagENSegmentRangeOper
{
    EN_INVALID_SEGMENT_RANGE_OPER = 0,
    EN_ADD_SEGMENT_RANGE,
    EN_DEL_SEGMENT_RANGE,
    EN_SEGMENT_RANGE_OPER_ALL,        
}EN_SEGMENT_RANGE_OPER;


class CSegmentRangeMsg : public VNetConfigBaseMsg
{
public:
    explicit CSegmentRangeMsg() {m_dwOper = EN_INVALID_SEGMENT_RANGE_OPER;}
    virtual ~CSegmentRangeMsg() {}

public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CSegmentRangeMsg);                 
        WRITE_DIGIT(m_nSegmentStart);
        WRITE_DIGIT(m_nSegmentEnd);
        WRITE_STRING(m_strNetplaneUUID);
        WRITE_DIGIT(m_dwOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CSegmentRangeMsg);
        READ_DIGIT(m_nSegmentStart);
        READ_DIGIT(m_nSegmentEnd);
        READ_STRING(m_strNetplaneUUID);
        READ_DIGIT(m_dwOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    }

    int32 IsValidOper(void) const
    {
        if((m_dwOper > EN_INVALID_SEGMENT_RANGE_OPER) && (m_dwOper < EN_SEGMENT_RANGE_OPER_ALL))
        {
            return 1;
        }
        return 0;
    }

    int32 IsValidRange(void) const
    {
        if((m_nSegmentStart > m_nSegmentEnd)       ||
           (m_nSegmentStart < VNETLIB_MIN_SEGMENT_ID) || 
           (m_nSegmentEnd > VNETLIB_MAX_SEGMENT_ID))
        {
            return 0;
        }
        
        return 1;
    }

public:

    void Print(void)
    {
        cout << "Segment Range operation code: " << m_dwOper << "." << endl;                
        cout << "Range: {" << m_nSegmentStart << ", " << m_nSegmentEnd << "}." << endl;
        return ;
    }
    
public:
    int32 m_nSegmentStart;
    int32 m_nSegmentEnd;
    string m_strNetplaneUUID;
    uint32 m_dwOper;
};


//网卡VSI的具体信息
class CVNetVSIInfo :public Serializable
{
    public:
       string    m_strvsiid;
       string    m_strip; 
       string    m_strnetmask;
       string    m_strgateway;    
       string    m_strmac;
       string    m_strpci_order;
       string    m_strbridge;
       uint32    m_unvlan;
       
   public:  
      explicit CVNetVSIInfo() {}
      virtual ~CVNetVSIInfo() {}

   public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVSIInfo);    
            WRITE_STRING(m_strvsiid);
            WRITE_STRING(m_strip);
            WRITE_STRING(m_strnetmask);
            WRITE_STRING(m_strgateway);
            WRITE_STRING(m_strmac);            
            WRITE_STRING(m_strpci_order);
            WRITE_STRING(m_strbridge);
            WRITE_DIGIT(m_unvlan);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVSIInfo);
            READ_STRING(m_strvsiid);
            READ_STRING(m_strip);
            READ_STRING(m_strnetmask)
            READ_STRING(m_strgateway);
            READ_STRING(m_strmac);
            READ_STRING(m_strpci_order);            
            READ_STRING(m_strbridge);
            READ_DIGIT(m_unvlan);
            DESERIALIZE_END();
        }   

        void Print()
        {
            cout << "CVNetVSIInfo:" <<endl;
            cout << "   m_strvsiid       : " << m_strvsiid << endl;
            cout << "   m_strip          : " << m_strip << endl;
            cout << "   m_strnetmask     : " << m_strnetmask << endl;
            cout << "   m_strgateway     : " << m_strgateway << endl;
            cout << "   m_strmac         : " << m_strmac << endl;
            cout << "   m_strpci_order   : " << m_strpci_order << endl;
            cout << "   m_strbridge      : " << m_strbridge << endl;
            cout << "   m_unvlan         : " << m_unvlan << endl;       
        };
};


//TC/CC/HC和NETWORK间VNIC结构体定义
class CVNetVnicConfig: public Serializable
{
public:
    int32   m_nNicIndex;                      //NicIndex,输入参数
    int32   m_nIsSriov;                        //IsSriov,输入参数
    int32   m_nIsLoop;                         //IsLoop,输入参数
    string  m_strLogicNetworkID;        //LogicNetwork UUID,输入参数
    string  m_strAdapterModel;           //网卡型号,输入参数
    string  m_strIP;                       //用户指定的IP
    string  m_strNetmask;                  //用户指定的掩码
    string  m_strGateway;                  //用户指定的网关地址
    string  m_strMac;                      //用户指定的Mac
    string  m_strVSIProfileID;             //VSIProfile UUID,输出参数


public:
    CVNetVnicConfig()    
    {
         m_nNicIndex    = 0;
         m_nIsSriov    = 0;
         m_nIsLoop= 0;
         m_strLogicNetworkID   = STR_VSIID_INIT_VALUE;
         m_strAdapterModel   = "";
         m_strVSIProfileID   = STR_VSIID_INIT_VALUE;
         m_strIP = "";
         m_strNetmask = "";
         m_strGateway = "";
         m_strMac = "";
    };
    CVNetVnicConfig(const int32   nic_index,
                    const int32   sriov,
                    const int32   loop,
                    const string  logic_network_id,
                    const string  model)    
    {
         m_nNicIndex    = nic_index;
         m_nIsSriov    = sriov;
         m_nIsLoop= loop;
         m_strLogicNetworkID   = logic_network_id;
         m_strAdapterModel   = model;
    };

    CVNetVnicConfig(const int32   nic_index,
                    const int32   sriov,
                    const int32   loop,
                    const string  &logic_network_id,
                    const string  &model,
                    const string  &vsi_profile_id)    
    {
         m_nNicIndex= nic_index;
         m_nIsSriov = sriov;
         m_nIsLoop  = loop;
         m_strLogicNetworkID = logic_network_id;
         m_strAdapterModel   = model;
         m_strVSIProfileID     = vsi_profile_id;
    };

    CVNetVnicConfig(const int32   nic_index,
                    const int32   sriov,
                    const int32   loop,
                    const string  logic_network_id,                    
                    const string  model,
                    const string  assip,
                    const string  assmask,
                    const string  assgateway,
                    const string  assmac)    
    {
         m_nNicIndex    = nic_index;
         m_nIsSriov    = sriov;
         m_nIsLoop= loop;
         m_strLogicNetworkID   = logic_network_id;
         m_strIP = assip;
         m_strNetmask = assmask;
         m_strGateway = assgateway;
         m_strMac = assmac;
         m_strAdapterModel   = model;
    };

    CVNetVnicConfig(const int32   nic_index,
                    const int32   sriov,
                    const int32   loop,
                    const string  &logic_network_id,                    
                    const string  &model,                    
                    const string  &vsi_profile_id,
                    const string  &assip,
                    const string  &assmask,
                    const string  &assgateway,
                    const string  &assmac)    
    {
         m_nNicIndex= nic_index;
         m_nIsSriov = sriov;
         m_nIsLoop  = loop;
         m_strLogicNetworkID = logic_network_id;
         m_strIP = assip;
         m_strNetmask = assmask;
         m_strGateway = assgateway;
         m_strMac = assmac;
         m_strAdapterModel   = model;
         m_strVSIProfileID     = vsi_profile_id;
    };

    CVNetVnicConfig(const VmNicConfig &nic) : m_strLogicNetworkID(nic._logic_network_id), m_strAdapterModel(nic._model), m_strVSIProfileID(nic._vsi_profile_id)
    {
         m_nNicIndex= nic._nic_index;
         m_nIsSriov = nic._sriov;
         m_nIsLoop  = nic._loop;
         m_strIP    = nic._ip;
         m_strNetmask = nic._netmask;
         m_strGateway = nic._gateway;
         m_strMac   = nic._mac;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetVnicConfig);
        WRITE_DIGIT(m_nNicIndex);
        WRITE_DIGIT(m_nIsSriov);
        WRITE_DIGIT(m_nIsLoop);
        WRITE_STRING(m_strLogicNetworkID);
        WRITE_STRING(m_strAdapterModel);
        WRITE_STRING(m_strIP);
        WRITE_STRING(m_strNetmask);
        WRITE_STRING(m_strGateway);
        WRITE_STRING(m_strMac);
        WRITE_STRING(m_strVSIProfileID);         
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetVnicConfig);
        READ_DIGIT(m_nNicIndex);
        READ_DIGIT(m_nIsSriov);
        READ_DIGIT(m_nIsLoop);
        READ_STRING(m_strLogicNetworkID);
        READ_STRING(m_strAdapterModel);
        READ_STRING(m_strIP);
        READ_STRING(m_strNetmask);
        READ_STRING(m_strGateway);
        READ_STRING(m_strMac);
        READ_STRING(m_strVSIProfileID);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "Vnic Config:" << endl;
        cout << "   Vnic Index           :" << m_nNicIndex << endl;		
        cout << "   IsSriov              :" << m_nIsSriov << endl;
        cout << "   IsLoop               :" << m_nIsLoop << endl;
        cout << "   LogicNetwork ID      :" << m_strLogicNetworkID << endl;
        cout << "   Adapter Model        :" << m_strAdapterModel << endl;
        cout << "   Input IP             :" << m_strIP << endl;
        cout << "   Input Netmask        :" << m_strNetmask << endl;
        cout << "   Input Gateway        :" << m_strGateway << endl;
        cout << "   Input Mac            :" << m_strMac << endl;
        cout << "   VSI UUID             :" << m_strVSIProfileID << endl;
    };
};

class CWatchDogInfo: public Serializable
{
public:
   bool     m_nIsWDGEnable;
   int32    m_nTimeOut;
  
public:  
    explicit CWatchDogInfo() 
    {
        m_nIsWDGEnable = FALSE;
        m_nTimeOut = 0;
    };
    
    CWatchDogInfo(bool is_enable, int32 period) 
    {
        m_nIsWDGEnable = is_enable;
        m_nTimeOut     = period;
    };

    virtual ~CWatchDogInfo() {};

public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWatchDogInfo);    
        WRITE_DIGIT(m_nIsWDGEnable);
        WRITE_DIGIT(m_nTimeOut);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWatchDogInfo);
        READ_DIGIT(m_nIsWDGEnable);
        READ_DIGIT(m_nTimeOut);
        DESERIALIZE_END();
    }   

    void Print()
    {
        cout << "CWatchDogInfo:" <<endl;
        cout << "   m_nWDGEnable       : " << m_nIsWDGEnable << endl;
        cout << "   m_nTimeOut         : " << m_nTimeOut << endl;
    };
};

//TC调度到满足网络条件的CC消息
class CVNetSelectCCListMsg: public WorkReq
{
    public:
        MID    m_mid;         //计算不填写 网络使用
        int64  m_vm_id;       //由计算填写，虚拟机id
        int64  m_project_id;  //由计算填写，工程id
        string m_appointed_cc;
        vector <CVNetVnicConfig> m_VmNicList;//由计算填写
        CWatchDogInfo m_WatchDogInfo; //由计算填写，WDG信息
        
    public:   
        CVNetSelectCCListMsg(){};

        /* 该构造函数由计算使用 */
        CVNetSelectCCListMsg(const string &id_of_action,
                              const int64      vm_id,
                              const int64      project_id,
                              const string &appointed_cc,
                              const vector <CVNetVnicConfig> &vm_nic_list,
                              const CWatchDogInfo &watch_dog_Info):WorkReq(id_of_action)
        {
           m_vm_id = vm_id;
           m_project_id = project_id;
           m_appointed_cc = appointed_cc;
           m_VmNicList  = vm_nic_list;
           m_WatchDogInfo = watch_dog_Info;
        };

        virtual ~CVNetSelectCCListMsg() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetSelectCCListMsg);
            WRITE_STRING(m_appointed_cc);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            WRITE_DIGIT(m_project_id);
            WRITE_OBJECT_VECTOR(m_VmNicList);
            WRITE_OBJECT(m_WatchDogInfo);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetSelectCCListMsg);
            READ_STRING(m_appointed_cc);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            READ_DIGIT(m_project_id);
            READ_OBJECT_VECTOR(m_VmNicList);
            READ_OBJECT(m_WatchDogInfo);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetSelectCCListMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_appointed_cc      : " << m_appointed_cc  << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicList.begin();
            for ( ; itrVnic != m_VmNicList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//TC调度到满足条件的CC消息的应答
class CVNetSelectCCListMsgAck:public WorkAck
{
    public:
        int64  m_vm_id;       //由计算填写，虚拟机id
        MID    m_mid;          //由VNM填写，从请求消息中拷贝出来即可
        vector <string> m_select_cc_list; //由VNM填写，调度到的CC列表

    public:    
        /* 默认构造函数 LIB库使用 */
        CVNetSelectCCListMsgAck()
        {
            action_id ="";
            state = ERROR;
            progress = 0;
            detail = "";
        }

        /* 该构造函数由网络VNM使用 */
        CVNetSelectCCListMsgAck(const string &_action_id,
                                int  _state,
                                int  _progress,
                                const string &_detail,
                                const vector<string> _select_cc_list)
        {
            action_id = _action_id;
            state     = _state;
            progress  = _progress;
            detail    = _detail;
            m_select_cc_list = _select_cc_list;
        };
        virtual ~CVNetSelectCCListMsgAck(){}

        void Print()
        {  
            cout << "CVNetSelectCCListMsgAck:" <<endl;
            cout << "   application      :"<<m_mid._application<<endl;
            cout << "   process          :"<<m_mid._process<<endl;
            cout << "   unit             :"<<m_mid._unit<<endl;
            cout << "   vm id            :"<<m_vm_id<<endl; 
            
            vector <string>::iterator itrselectcc = m_select_cc_list.begin();
            for( ; itrselectcc != m_select_cc_list.end(); ++itrselectcc)
            {
                cout<<"select cc :"<<*itrselectcc<<endl;
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        }
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetSelectCCListMsgAck);
            WRITE_VALUE(m_vm_id);
            WRITE_OBJECT(m_mid);
            WRITE_STRING_VECTOR(m_select_cc_list);
            SERIALIZE_PARENT_END(WorkAck);
        }    

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetSelectCCListMsgAck);
            READ_VALUE(m_vm_id);
            READ_OBJECT(m_mid);
            READ_STRING_VECTOR(m_select_cc_list);
            DESERIALIZE_PARENT_END(WorkAck);
        }
};

//CC调度到HC消息
class CVNetSelectHCListMsg :public WorkReq
{
    public:
       MID    m_mid;                         //网络填写
       vector <CVNetVnicConfig> m_VmNicList;  //由计算填写，虚拟网卡
       CWatchDogInfo m_WatchDogInfo;
       string m_cc_application;              //由计算填写，指定的cc的application
       string m_appointed_hc;
       int64  m_vm_id;                       //由计算填写, 虚拟机id
       int64  m_project_id;                  //由计算填写，工程id

    public:
       CVNetSelectHCListMsg(){}

       /* 该构造函数由计算使用 */
       CVNetSelectHCListMsg(vector <CVNetVnicConfig> m_VmNicList,
                            CWatchDogInfo m_WatchDogInfo,
                            string m_cc_application,
                            string m_appointed_hc,
                            int64  m_vm_id,
                            int64  m_project_id)
       {
        
       }
     
       virtual ~CVNetSelectHCListMsg(){}
        
       SERIALIZE
       {
           SERIALIZE_BEGIN(CVNetSelectHCListMsg);           
           WRITE_OBJECT(m_mid);
           WRITE_OBJECT_VECTOR(m_VmNicList);
           WRITE_OBJECT(m_WatchDogInfo);
           WRITE_STRING(m_cc_application);
           WRITE_STRING(m_appointed_hc);
           WRITE_DIGIT(m_vm_id);
           WRITE_DIGIT(m_project_id);
           SERIALIZE_PARENT_END(WorkReq);
       }   

       DESERIALIZE
       {
           DESERIALIZE_BEGIN(CVNetSelectHCListMsg);           
           READ_OBJECT(m_mid);
           READ_OBJECT_VECTOR(m_VmNicList);
           READ_OBJECT(m_WatchDogInfo);
           READ_STRING(m_cc_application);
           READ_STRING(m_appointed_hc);
           READ_DIGIT(m_vm_id);
           READ_DIGIT(m_project_id);
           DESERIALIZE_PARENT_END(WorkReq);
       }

       void Print()
        {
            cout << "CVNetSelectHCListMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_cc_application : " << m_cc_application << endl;
            cout << "   m_appointed_hc      : " << m_appointed_hc << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicList.begin();
            for ( ; itrVnic != m_VmNicList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//CC调度到HC消息的应答
class CVNetSelectHCListMsgAck :public WorkAck
{
    public:
        int64  m_vm_id;       //由计算填写，虚拟机id
        MID    m_mid;                             //由VNM填写，从请求消息中拷贝出来即可
        vector <string> m_select_hc_list;  //由VNM填写,调度到的hc列表

    public:    

        /* 该构造函数网络使用 */
        CVNetSelectHCListMsgAck()
        {
           state = ERROR;
           progress = 0;
           action_id = "";
        }
        
        virtual ~CVNetSelectHCListMsgAck(){}

        void Print()
        {  
            cout << "CVNetSelectHCListMsgAck:" <<endl;
            cout << "   application      :"<<m_mid._application<<endl;
            cout << "   process          :"<<m_mid._process<<endl;
            cout << "   unit             :"<<m_mid._unit<<endl;
            cout << "   vmid             :"<<m_vm_id<<endl;
            
           
            vector <string>::iterator itrselecthc = m_select_hc_list.begin();
            for( ; itrselecthc != m_select_hc_list.end(); ++itrselecthc)
            {
                cout<<"select hc         :"<<*itrselecthc<<endl;
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetSelectHCListMsgAck);
            WRITE_VALUE(m_vm_id);
            WRITE_OBJECT(m_mid);
            WRITE_STRING_VECTOR(m_select_hc_list);
            SERIALIZE_PARENT_END(WorkAck);
        }     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetSelectHCListMsgAck);
            READ_VALUE(m_vm_id);
            READ_OBJECT(m_mid);
            READ_STRING_VECTOR(m_select_hc_list);
            DESERIALIZE_PARENT_END(WorkAck);
        }
};

//指定CC分配网络资源的消息
class CVNetGetResourceCCMsg :public WorkReq
{
    public:
        MID             m_mid;               //待讨论由谁填
        string          m_cc_application;    //由计算填写，指定CC的application
        vector<CVNetVnicConfig> m_VmNicCfgList;      //由计算填写，配置的虚拟网卡列表
        CWatchDogInfo   m_WatchDogInfo;
        int64           m_vm_id;             //由计算填写，虚拟机id
        int64           m_project_id;        //由计算填写, 工程id

    public:    
        CVNetGetResourceCCMsg(){};

        /* 该构造函数计算使用 */
        CVNetGetResourceCCMsg(const string   &id_of_action,
                               const string   &cc_application,
                               const vector <CVNetVnicConfig> &vm_nic_cfg_list,
                               const CWatchDogInfo &watchdog_info,
                               const int64      vm_id,
                               const int64      project_id):WorkReq(id_of_action)
        {
           m_cc_application = cc_application;
           m_VmNicCfgList  = vm_nic_cfg_list;
           m_WatchDogInfo = watchdog_info;
           m_vm_id = vm_id;
           m_project_id = project_id;            
        };
        
        virtual ~CVNetGetResourceCCMsg(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetGetResourceCCMsg);
            WRITE_OBJECT(m_mid);
            WRITE_STRING(m_cc_application);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            WRITE_OBJECT(m_WatchDogInfo);
            WRITE_DIGIT(m_vm_id);
            WRITE_DIGIT(m_project_id);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetGetResourceCCMsg);
            READ_OBJECT(m_mid);
            READ_STRING(m_cc_application);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            READ_OBJECT(m_WatchDogInfo);
            READ_DIGIT(m_vm_id);
            READ_DIGIT(m_project_id);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetGetResourceCCMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_cc_application : " << m_cc_application << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//指定CC分配网络资源的应答
class CVNetGetResourceCCMsgAck :public WorkAck
{
    public:
        int64           m_vm_id;             //由计算填写，虚拟机id
        MID    m_mid;                             //由VNM填写，从请求消息中拷贝出来即可
        vector<CVNetVnicConfig> m_VmNicCfgList;      //由VNM填写，分配的VLAN,MAC,IP等信息

    public:    
        CVNetGetResourceCCMsgAck(){}

        /*该构造函数由网络使用 */
        CVNetGetResourceCCMsgAck(const string &_action_id,
                                 int    _state,
                                 int    _progress,
                                 const string &_detail,
                                 const vector<CVNetVnicConfig> &vec_nic)
        {
            action_id = _action_id;
            state = _state;
            progress = _progress;
            detail = _detail;
            m_VmNicCfgList = vec_nic;
        };
        virtual ~CVNetGetResourceCCMsgAck(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetGetResourceCCMsgAck);
            WRITE_VALUE(m_vm_id);
            WRITE_OBJECT(m_mid);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetGetResourceCCMsgAck);
            READ_VALUE(m_vm_id);
            READ_OBJECT(m_mid);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetGetResourceCCMsgAck:" <<endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//释放给CC分配的网络资源
class CVNetFreeResourceCCMsg :public WorkReq
{
    public:
        MID             m_mid;
        int64           m_project_id;
        int64           m_vm_id;
        string          m_cc_application;
        vector<CVNetVnicConfig> m_VmNicCfgList; 
        CWatchDogInfo m_WatchDogInfo;

    public:    
        /* 计算使用 */
        CVNetFreeResourceCCMsg(const string  &id_of_action,
                                           int64  project_id, 
                                           int64  vid,
                                           const string  &cc,
                                           const vector<CVNetVnicConfig>  &nic_list,
                                           const CWatchDogInfo &watchdog_info):WorkReq(id_of_action)
        {
            m_project_id = project_id;
            m_vm_id = vid;
            m_cc_application = cc;
            m_VmNicCfgList = nic_list;
            m_WatchDogInfo = watchdog_info;
        };
        
        CVNetFreeResourceCCMsg(){}
        virtual ~CVNetFreeResourceCCMsg(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetFreeResourceCCMsg);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_project_id);
            WRITE_DIGIT(m_vm_id);
            WRITE_STRING(m_cc_application);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            WRITE_OBJECT(m_WatchDogInfo);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetFreeResourceCCMsg);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_project_id);
            READ_DIGIT(m_vm_id);
            READ_STRING(m_cc_application);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            READ_OBJECT(m_WatchDogInfo);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetFreeResourceCCMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_cc_application : " << m_cc_application << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//释放给CC分配的网络资源的应答
class CVNetFreeResourceCCMsgAck :public WorkAck
{
    public:
        int64           m_vm_id;             //由计算填写，虚拟机id
        MID     m_mid;


    public:    
        CVNetFreeResourceCCMsgAck(){}
        virtual ~CVNetFreeResourceCCMsgAck(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetFreeResourceCCMsgAck);
            WRITE_VALUE(m_vm_id);
            WRITE_OBJECT(m_mid);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetFreeResourceCCMsgAck);
            READ_VALUE(m_vm_id);
            READ_OBJECT(m_mid);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetFreeResourceCCMsgAck:" <<endl;
            cout <<"action_id  :"<<action_id<<endl;
            cout <<"state      :"<<state<<endl;
            cout <<"process    :"<<progress<<endl;
            cout <<"detail     :"<<detail<<endl;
        };
};

//指定HC分配网络资源
class CVNetGetResourceHCMsg :public WorkReq
{
    public:
        MID     m_mid;
        string  m_cc_application;
        string  m_hc_application;
        int64   m_project_id;
        int64   m_vm_id;
        vector<CVNetVnicConfig> m_VmNicCfgList; 
        CWatchDogInfo m_WatchDogInfo;

    public:    
        CVNetGetResourceHCMsg(){}
        virtual ~CVNetGetResourceHCMsg(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetGetResourceHCMsg);
            WRITE_OBJECT(m_mid);
            WRITE_STRING(m_cc_application);
            WRITE_STRING(m_hc_application);
            WRITE_DIGIT(m_project_id);
            WRITE_DIGIT(m_vm_id);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            WRITE_OBJECT(m_WatchDogInfo);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetGetResourceHCMsg);
            READ_OBJECT(m_mid);
            READ_STRING(m_cc_application);
            READ_STRING(m_hc_application);
            READ_DIGIT(m_project_id);
            READ_DIGIT(m_vm_id);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            READ_OBJECT(m_WatchDogInfo);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetGetResourceHCMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_cc_application : " << m_cc_application << endl;
            cout << "   m_hc_application : " << m_hc_application << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//指定HC分配网络资源的应答
class CVNetGetResourceHCMsgAck :public WorkAck
{
    public:
        MID     m_mid;
        int64   m_vm_id;

    public:    
        CVNetGetResourceHCMsgAck(){}
        virtual ~CVNetGetResourceHCMsgAck(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetGetResourceHCMsgAck);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetGetResourceHCMsgAck);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetGetResourceHCMsgAck:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "action_id  :"<<action_id<<endl;
            cout << "state      :"<<state<<endl;
            cout << "process    :"<<progress<<endl;
            cout << "detail     :"<<detail<<endl;
            cout << "==============================="<<endl;
        };
};

//释放给HC分配的网络资源
class CVNetFreeResourceHCMsg :public WorkReq
{
    public:
        MID     m_mid;
        int64   m_vm_id;
        int64   m_project_id;
        string  m_cc_application;
        string  m_hc_application;
        vector<CVNetVnicConfig> m_VmNicCfgList;
        CWatchDogInfo m_WatchDogInfo;

    public:    
        CVNetFreeResourceHCMsg(){}
        virtual ~CVNetFreeResourceHCMsg(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetFreeResourceHCMsg);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            WRITE_DIGIT(m_project_id);
            WRITE_STRING(m_cc_application);
            WRITE_STRING(m_hc_application);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            WRITE_OBJECT(m_WatchDogInfo);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetFreeResourceHCMsg);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            READ_DIGIT(m_project_id);
            READ_STRING(m_cc_application);
            READ_STRING(m_hc_application);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            READ_OBJECT(m_WatchDogInfo);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetFreeResourceHCMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_cc_application : " << m_cc_application << endl;
            cout << "   m_hc_application : " << m_hc_application << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//释放给HC分配的网络资源的应答
class CVNetFreeResourceHCMsgAck :public WorkAck
{
    public:
        MID     m_mid;
        int64   m_vm_id;

    public:    
        CVNetFreeResourceHCMsgAck(){}
        virtual ~CVNetFreeResourceHCMsgAck(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetFreeResourceHCMsgAck);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetFreeResourceHCMsgAck);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetFreeResourceHCMsgAck:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "action_id  :"<<action_id<<endl;
            cout << "state      :"<<state<<endl;
            cout << "process    :"<<progress<<endl;
            cout << "detail     :"<<detail<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
                
};

//NETWORK返回给HC的VNIC，供HC构造VM的XML文件用
class CVnetVnicXML: public Serializable
{
public:
    explicit CVnetVnicXML() {
    m_strPCIOrder = "";
    m_strBridgeName = "";
    m_strIP = "";
    m_strNetmask = "";
    m_strGateway = "";
    m_strMac = "";
    m_nCvlan = 0;
}
    virtual ~CVnetVnicXML() {}

public:
    CVNetVnicConfig m_cVnicConfig;

    string  m_strPCIOrder;                  //SRIOVPCI,不相关
    string  m_strBridgeName;
    string  m_strIP;
    string  m_strNetmask ;
    string  m_strGateway;
    string  m_strMac;
   uint32  m_nCvlan;
   
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVnetVnicXML);
        WRITE_OBJECT(m_cVnicConfig);
        WRITE_STRING(m_strPCIOrder);
        WRITE_STRING(m_strBridgeName);
        WRITE_STRING(m_strIP);
        WRITE_STRING(m_strNetmask);
        WRITE_STRING(m_strGateway);
        WRITE_STRING(m_strMac);
        WRITE_DIGIT(m_nCvlan);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVnetVnicXML);
        READ_OBJECT(m_cVnicConfig);
        READ_STRING(m_strPCIOrder);
        READ_STRING(m_strBridgeName);
        READ_STRING(m_strIP);
        READ_STRING(m_strNetmask);
        READ_STRING(m_strGateway);
        READ_STRING(m_strMac);
        READ_DIGIT(m_nCvlan);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "CVnetVnicXML:" <<endl;
        m_cVnicConfig.Print();
        cout << "   m_strPCIOrder        : " << m_strPCIOrder << endl;
        cout << "   m_strBridgeName      : " << m_strBridgeName << endl;
        cout << "   m_strIP              : " << m_strIP << endl;
        cout << "   m_strNetmask         : " << m_strNetmask << endl;
        cout << "   m_strGateway         : " << m_strGateway << endl;
        cout << "   m_strMac             : " << m_strMac << endl;
        cout << "   m_nCvlan             : " << m_nCvlan << endl;
    };

};

//startnetwork检验PCI/BRIDGE消息
class CVNetStartNetworkMsg : public WorkReq
{
    public:
        MID    m_mid;
        int64  m_vm_id;
        int32  m_option;
        vector<CVNetVnicConfig> m_VmNicCfgList;
        CWatchDogInfo m_WatchDogInfo;
        
    public:   
        CVNetStartNetworkMsg(){}
        
        /* 该构造函数计算使用*/
        CVNetStartNetworkMsg(MID _mid, 
                             int64 _vmid, 
                             int32 _option,
                             CWatchDogInfo _wdginfo,
                             vector<CVNetVnicConfig> _vecVnicConfig)
        {
            m_mid = _mid;
            m_vm_id = _vmid;
            m_option = _option;
            m_WatchDogInfo = _wdginfo;
            m_VmNicCfgList = _vecVnicConfig;
             
        }
        virtual ~CVNetStartNetworkMsg() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetStartNetworkMsg);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            WRITE_DIGIT(m_option);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            WRITE_OBJECT(m_WatchDogInfo);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetStartNetworkMsg);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            READ_DIGIT(m_option);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            READ_OBJECT(m_WatchDogInfo);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetStartNetworkMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_option         : " << m_option << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }

            m_WatchDogInfo.Print();
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//startnetwork检验PCI/BRIDGE消息的应答
class CVNetStartNetworkMsgAck : public WorkAck
{
    public:
        MID    m_mid;
        int64  m_vmid;
        vector<CVnetVnicXML> m_vsiinfo;
        CVnetVnicXML    m_wdg_info;
        
    public:   
        CVNetStartNetworkMsgAck(){}
        virtual ~CVNetStartNetworkMsgAck() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetStartNetworkMsgAck);
            WRITE_OBJECT(m_mid);
            WRITE_OBJECT_VECTOR(m_vsiinfo);
            WRITE_OBJECT(m_wdg_info);
            WRITE_VALUE(m_vmid);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetStartNetworkMsgAck);
            READ_OBJECT(m_mid);
            READ_OBJECT_VECTOR(m_vsiinfo);
            READ_OBJECT(m_wdg_info);
            READ_VALUE(m_vmid);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetStartNetworkMsgAck:" <<endl;
            cout << "vm id :"<<m_vmid<<endl;
            cout << "action_id  :"<<action_id<<endl;
            cout << "state      :"<<state<<endl;
            cout << "process    :"<<progress<<endl;
            cout << "detail     :"<<detail<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;

            vector<CVnetVnicXML>::iterator itrVnic = m_vsiinfo.begin();
            for ( ; itrVnic != m_vsiinfo.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "watchdog nic info:"<<endl;
            m_wdg_info.Print();
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//stopnetwork消息
class CVNetStopNetworkMsg : public WorkReq
{
    public:
        MID    m_mid;
        int64  m_vm_id;
        int32  m_option;
        vector<CVNetVnicConfig> m_VmNicCfgList;
        CWatchDogInfo m_WatchDogInfo;
        
    public:   
        CVNetStopNetworkMsg(){}

        /* 该构造函数计算使用 */
        CVNetStopNetworkMsg(MID _mid,
                            int64 _vmid,
                            int32 _option,
                            vector<CVNetVnicConfig> _VmNicCfgList,
                            CWatchDogInfo _WatchDogInfo)
        {
            m_mid = _mid;
            m_vm_id = _vmid;
            m_option = _option;
            m_VmNicCfgList = _VmNicCfgList;
            m_WatchDogInfo = _WatchDogInfo;
        }
        
        virtual ~CVNetStopNetworkMsg() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetStopNetworkMsg);
            WRITE_DIGIT(m_option);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            WRITE_OBJECT(m_WatchDogInfo);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetStopNetworkMsg);
            READ_DIGIT(m_option);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            READ_OBJECT(m_WatchDogInfo);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetStopNetworkMsg:" <<endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            m_WatchDogInfo.Print();
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//stopnetworkack消息
class CVNetStopNetworkMsgAck : public WorkAck
{
    public:
        MID    m_mid;
        int64  m_vmid;
        
    public:   
        CVNetStopNetworkMsgAck(){}
        virtual ~CVNetStopNetworkMsgAck() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetStopNetworkMsgAck);
            WRITE_OBJECT(m_mid);
            WRITE_VALUE(m_vmid);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetStopNetworkMsgAck);
            READ_OBJECT(m_mid);
            READ_VALUE(m_vmid);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetStopNetworkMsgAck:" <<endl;
            cout << "action_id  :"<<action_id<<endl;
            cout << "state      :"<<state<<endl;
            cout << "process    :"<<progress<<endl;
            cout << "detail     :"<<detail<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//通知VNA DOMID消息
class CVNetNotifyDomIDMsg : public WorkReq
{
    public:
        MID    m_mid;
        int  m_dom_id;
        int64  m_vm_id;
        int64  m_project_id;
        vector<CVNetVnicConfig> m_VmNicCfgList;
       
    public:   
        CVNetNotifyDomIDMsg(){}
        virtual ~CVNetNotifyDomIDMsg() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetNotifyDomIDMsg);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_dom_id);
            WRITE_DIGIT(m_vm_id);
            WRITE_DIGIT(m_project_id);
            WRITE_OBJECT_VECTOR(m_VmNicCfgList);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetNotifyDomIDMsg);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_dom_id);
            READ_DIGIT(m_vm_id);
            READ_DIGIT(m_project_id);
            READ_OBJECT_VECTOR(m_VmNicCfgList);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetNotifyDomIDMsg:" <<endl;
            cout << "   m_dom_id         : " << m_dom_id << endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_project_id     : " << m_project_id << endl;

            vector<CVNetVnicConfig>::iterator itrVnic = m_VmNicCfgList.begin();
            for ( ; itrVnic != m_VmNicCfgList.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//通知VNA DOMID消息应答
class CVNetNotifyDomIDMsgAck : public WorkAck
{
    public:
        MID    m_mid;
        int64  m_vmid;
        
    public:   
        CVNetNotifyDomIDMsgAck(){}
        virtual ~CVNetNotifyDomIDMsgAck() {}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetNotifyDomIDMsgAck);
            WRITE_OBJECT(m_mid);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetNotifyDomIDMsgAck);
            READ_OBJECT(m_mid);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetNotifyDomIDMsgAck:" <<endl;
            cout << "vm  id                :" <<m_vmid<<endl;
            cout << "action_id             :"<<action_id<<endl;
            cout << "state                 :"<<state<<endl;
            cout << "process               :"<<progress<<endl;
            cout << "detail                :"<<detail<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

class CVNetVMVSIMsg:public Serializable
{
    public:
        CVNetVMVSIMsg(){}
         virtual ~CVNetVMVSIMsg() {}
         
    public:     
        int64           m_project_id;
        int64           m_vm_id;
        int32           m_domain_id;
        vector<CVNetVnicConfig> m_vnics;
        CWatchDogInfo m_WatchDogInfo;

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVMVSIMsg);
            WRITE_DIGIT(m_project_id);
            WRITE_DIGIT(m_vm_id);
            WRITE_DIGIT(m_domain_id);
            WRITE_OBJECT_VECTOR(m_vnics);
            SERIALIZE_END();
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVMVSIMsg);
            READ_DIGIT(m_project_id);
            READ_DIGIT(m_vm_id);
            READ_DIGIT(m_domain_id);
            READ_OBJECT_VECTOR(m_vnics);
            DESERIALIZE_END();
        };

        void Print()
        {
            cout << "CVNetVMVSIMsg:" <<endl;
            cout << "   m_project_id     : " << m_project_id << endl;
            cout << "   m_vm_id          : " << m_vm_id << endl;
            cout << "   m_domain_id      : " << m_domain_id << endl;
            vector<CVNetVnicConfig>::iterator itrVnic = m_vnics.begin();
            for ( ; itrVnic != m_vnics.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

class CVNetAllVMVSIMsg:public Serializable
{
    public:
        CVNetAllVMVSIMsg(){}
         virtual ~CVNetAllVMVSIMsg() {}
         
        void DeleteVSInfo(int64 vmid)
        {
            vector<CVNetVMVSIMsg>::iterator itor = m_allvnics.begin();
            for(;itor != m_allvnics.end();)
            {
                if(itor->m_vm_id == vmid)
                {
                    cout <<"remove vsiinfo vmid is :"<<vmid<<endl;
                    itor=m_allvnics.erase(itor);
                    //break;
                }
                else
                {
                    itor++;
                }
            }
        };

    public:     
        vector<CVNetVMVSIMsg> m_allvnics;

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetAllVMVSIMsg);
            WRITE_OBJECT_VECTOR(m_allvnics);
            SERIALIZE_END();
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetAllVMVSIMsg);
            READ_OBJECT_VECTOR(m_allvnics);
            DESERIALIZE_END();
        };

        void Print()
        {
            cout << "CVNetAllVMVSIMsg:" <<endl;
            vector<CVNetVMVSIMsg>::iterator itrVnic = m_allvnics.begin();
            for ( ; itrVnic != m_allvnics.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

class CVNetAllVMVSIMsgAck:public Serializable
{
    public:
        CVNetAllVMVSIMsgAck(){}
         virtual ~CVNetAllVMVSIMsgAck() {}
         
    public:     
        int32 m_result;
        
    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetAllVMVSIMsgAck);
            WRITE_DIGIT(m_result);
            SERIALIZE_END();
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetAllVMVSIMsgAck);
            READ_DIGIT(m_result);
            DESERIALIZE_END();
        };

        void Print()
        {
            cout << "CVNetAllVMVSIMsgAck:" <<endl;
            cout << "   m_result:" <<m_result<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};


//LIB和VNA的心跳消息
class CVNetVNAHeartBeatMsg : public Serializable
{
    public:
        CVNetVNAHeartBeatMsg(){};
        
        explicit CVNetVNAHeartBeatMsg(const string module_app, const int32 module_role)
        {
            m_module_app  = module_app;
            m_module_role  = module_role;
        }
        virtual ~CVNetVNAHeartBeatMsg(){}
        
        string& get_module_app(void){return m_module_app;}
        int32 get_module_role(void){return m_module_role;}
        
        void set_module_app(const string &module_app){m_module_app = module_app;}
        void set_module_role(const int32 &module_role){m_module_role = module_role;}
        
        void print()
        {
            cout << "CVNetVNAHeartBeatMsg:" <<endl;
            cout << "   m_module_app     : " << m_module_app << endl;
            cout << "   m_module_role     : " << m_module_role << endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        }

     public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVNAHeartBeatMsg);
            WRITE_STRING(m_module_app);
            WRITE_DIGIT(m_module_role);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVNAHeartBeatMsg);
            READ_STRING(m_module_app);
            READ_DIGIT(m_module_role);
            DESERIALIZE_END();
        };
    public:
        string  m_module_app;
        int32 m_module_role;
};

//VNA回应LIB的心跳消息
class CVNetVNAHeartBeatAckMsg : public Serializable
{
    public:
        CVNetVNAHeartBeatAckMsg(){}
        
        explicit CVNetVNAHeartBeatAckMsg(const string module_app)
        {
            m_module_app  = module_app;
        }
        virtual ~CVNetVNAHeartBeatAckMsg(){}
        
        string& get_module_app(void){return m_module_app;}
        
        void set_module_app(const string &module_app){m_module_app = module_app;}
        
        void print()
        {
            cout << "CVNetVNAHeartBeatAckMsg:" <<endl;
            cout << "   m_ret_code       : " << m_ret_code << endl;
            cout << "   m_module_app     : " << m_module_app << endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        }

     public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVNAHeartBeatAckMsg);
            WRITE_DIGIT(m_ret_code);
            WRITE_STRING(m_module_app);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVNAHeartBeatAckMsg);
            READ_DIGIT(m_ret_code);
            READ_STRING(m_module_app);
            DESERIALIZE_END();
        }
    public:
        int     m_ret_code;
        string  m_module_app;
};

//LIB通知VNA主机集群改变消息
class CvnetVNAClusterMsg : public Serializable
{
    public:
        CvnetVNAClusterMsg(){}
        
        explicit CvnetVNAClusterMsg(const string module_app, 
                                   const  string cluster_app)
        {
            m_module_app  = module_app;
            m_cluster_app = cluster_app;
        }
        virtual ~CvnetVNAClusterMsg(){}
        
        string& get_module_app(void){return m_module_app;}
        string& get_cluster_app(void){return m_cluster_app;}
        
        void set_module_app(const string &module_app){m_module_app = module_app;}
        void set_cluster_app(const string &cluster_app){m_cluster_app = cluster_app;}
        
        void print()
        {
            cout << "CvnetVNAClusterMsg:" <<endl;
            cout << "   m_module_app     : " << m_module_app << endl;
            cout << "   m_cluster_app    : " << m_cluster_app << endl; 
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        }

     public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CvnetVNAClusterMsg);
            WRITE_STRING(m_module_app);
            WRITE_STRING(m_cluster_app);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CvnetVNAClusterMsg);
            READ_STRING(m_module_app);
            READ_STRING(m_cluster_app);
            DESERIALIZE_END();
        }
    public:
        string  m_module_app;
        string  m_cluster_app;
};

//LIB和VNA注册消息
class CVNetModuleRegMsg : public Serializable
{
    public:
        CVNetModuleRegMsg(){};
        
        explicit CVNetModuleRegMsg(const string module_app, 
                                   const int32    module_role,
                                   const string cluster_app)
        {
            m_module_app  = module_app;
            m_module_role = module_role;
            m_module_ext_info = cluster_app;
        }
        virtual ~CVNetModuleRegMsg(){}
        
        string& get_module_app(void){return m_module_app;}
        int32& get_module_role(void){return m_module_role;}
        string& get_module_clussterapp(void){return m_module_ext_info;}
        
        void set_module_app(const string &module_app){m_module_app = module_app;}
        void set_module_role(const int32 &module_role){m_module_role = module_role;}
        void set_module_cluster(const string &module_cluster){m_module_ext_info = module_cluster;}
        
        void Print()
        {
            cout << "CVNetModuleRegMsg:" <<endl;
            cout << "   m_module_app     : " << m_module_app << endl;
            cout << "   m_module_role    : " << m_module_role << endl; 
            cout << "   m_module_ext_info: " << m_module_ext_info << endl; 
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        }

     public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetModuleRegMsg);
            WRITE_STRING(m_module_app);
            WRITE_DIGIT(m_module_role);
            WRITE_STRING(m_module_ext_info);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetModuleRegMsg);
            READ_STRING(m_module_app);
            READ_DIGIT(m_module_role);
            READ_STRING(m_module_ext_info);
            DESERIALIZE_END();
        }
    public:
        string  m_module_app;
        int32   m_module_role;
        string  m_module_ext_info;
};

//VNA回应LIB注册消息
class CVNetModuleACKRegMsg : public Serializable
{
    public:
        CVNetModuleACKRegMsg(){};
        explicit CVNetModuleACKRegMsg(const string module_app, 
                                   const int32  module_role)
        {
            m_module_app  = module_app;
            m_module_role = module_role;
        }
        virtual ~CVNetModuleACKRegMsg(){}

        string& get_module_app(void){return m_module_app;}
        int32& get_module_role(void){return m_module_role;}

        void set_module_app(const string &module_app){m_module_app = module_app;}
        void set_module_role(const int32 &module_role){m_module_role = module_role;}
            
        void print()
        {
            cout << "CVNetModuleACKRegMsg:" <<endl;
            cout << "   m_module_app     : " << m_module_app << endl;
            cout << "   m_module_role    : " << m_module_role << endl; 
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        }
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetModuleACKRegMsg);
            WRITE_STRING(m_module_app);
            WRITE_DIGIT(m_module_role);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetModuleACKRegMsg);
            READ_STRING(m_module_app);
            READ_DIGIT(m_module_role);
            DESERIALIZE_END();
        };
        
     public:
        string  m_module_app;
        int32  m_module_role;
};

enum PortGroupOper
{
    EN_PORT_GROUP_OPER_BEGIN = 0,
    EN_ADD_PORT_GROUP = 1,
    EN_DEL_PORT_GROUP = 2,
    EN_SET_PORT_GROUP = 3,
    EN_ADD_PG_TRUNK_VLANRANGE = 4,
    EN_DEL_PG_TRUNK_VLANRANGE = 5,
    EN_SET_PG_NATIVE_VLAN = 6,
    EN_PORT_GROUP_OPER_END,
};

typedef enum tagSwitchPortModeTypeValues
{
    EN_SWITCHPORTMODE_TYPE_TRUNK = 0,
    EN_SWITCHPORTMODE_TYPE_ACCESS = 1, 
}EN_SWITCHPORTMODE_TYPE_VALUES;

typedef enum tagIsolateTypeValues
{
    EN_ISOLATE_TYPE_VLAN       = 0,
    EN_ISOLATE_TYPE_VXLAN      = 1, 
    EN_ISOLATE_TYPE_VXLAN_VLAN = 2, 
}EN_ISOLATETYPE_VALUES;

typedef enum tagPortGroupTypeValues
{
    EN_PORTGROUP_TYPE_UPLINK = 0,
    EN_PORTGROUP_TYPE_KERNEL_MGR  = 10,     
    EN_PORTGROUP_TYPE_KERNEL_STORAGE = 11,
    EN_PORTGROUP_TYPE_VM_SHARE = 20,
    EN_PORTGROUP_TYPE_VM_PRIVATE = 21,
}T_PORTGROUP_TYPE_VALUES;

typedef enum tagSwitchPortModeValues
{
    EN_SWITCHPORT_MODE_TRUNK  = 0,
    EN_SWITCHPORT_MODE_ACCESS = 1, 
}T_SWITCHPORT_MODE_VALUES;

class CPortGroupMsg  : public VNetConfigBaseMsg
{
    public:
        explicit CPortGroupMsg() 
        {
            m_nPGType           = 0;
            m_nDefaultVlan      = 0;
            m_nPromisc          = 0;
            m_nMacVlanFilter    = 0;
            m_nVersion          = 0;
            m_nMacChange        = 0;
            m_nSwitchPortMode   = 0;   
            m_nIsCfgNetPlane    = 0;
            m_nIsSdn            = 0;
            m_nMTU              = 0;
            m_nAccessVlanNum    = 0;
            m_nAccessIsolateNum = 0; 
            m_nTrunkNativeVlan  = 0;
            m_nOper             = EN_PORT_GROUP_OPER_BEGIN;
            m_nIsolateType      = ISOLATE_VLAN;
            m_nSegmentId        = 0;            
        };
        virtual ~CPortGroupMsg() {};

    public:        
        int32 IsValidOper(void)
        {
            if((m_nOper > EN_PORT_GROUP_OPER_BEGIN) && (m_nOper < EN_PORT_GROUP_OPER_END))
            {
                return 1;
            }
            return 0;
        }  
        
        int32 CheckPGMTU(void)
        {
            if((m_nMTU < 68) || (m_nMTU > 9000))
            {
                return -1;
            }
            return 0;
        }
        
        int32 CheckPortGroupType(void)
        {
            switch(m_nPGType)
            {

                case EN_PORTGROUP_TYPE_UPLINK:
                case EN_PORTGROUP_TYPE_KERNEL_MGR:
                case EN_PORTGROUP_TYPE_KERNEL_STORAGE:
                case EN_PORTGROUP_TYPE_VM_SHARE:
                case EN_PORTGROUP_TYPE_VM_PRIVATE:
                {
                    return 0;
                }
                default: 
                    break;
            }
            return -1;
        }

        int32 CheckSwitchPortMode(void)
        {
            if((EN_SWITCHPORT_MODE_TRUNK == m_nSwitchPortMode) ||
               (EN_SWITCHPORT_MODE_ACCESS == m_nSwitchPortMode))
            {
                //uplink pg只能用trunk模式
                if((EN_PORTGROUP_TYPE_UPLINK == m_nPGType)&&(EN_SWITCHPORT_MODE_ACCESS == m_nSwitchPortMode))
                {
                    return -1;
                }
                return 0;
            }

            return -1;
        }        

        int32 CheckIsolateType(void)
        {
            if((EN_ISOLATE_TYPE_VLAN       == m_nIsolateType) ||
               (EN_ISOLATE_TYPE_VXLAN      == m_nIsolateType)||
               (EN_ISOLATE_TYPE_VXLAN_VLAN == m_nIsolateType))
            {
                //uplink pg不能只是vxlan技术
                if((EN_PORTGROUP_TYPE_UPLINK == m_nPGType)&&(EN_ISOLATE_TYPE_VXLAN == m_nIsolateType))
                {
                    return -1;
                }

                //不是uplink pg不能是VXLAN_VLAN技术
                if((!(EN_PORTGROUP_TYPE_UPLINK == m_nPGType))&&(EN_ISOLATE_TYPE_VXLAN_VLAN == m_nIsolateType))
                {
                    return -1;
                }                
                
                return 0;
            }

            return -1;
        }        
        
        void Print(void)
        {
            cout << "PortGroup operation code: " << m_nOper << endl;
            cout << "Name               : " << m_strName << endl;   
            cout << "PgType             : " << m_nPGType<< endl; 
            cout << "Uuid               : " << m_strUUID<< endl; 
            cout << "Acl                : " << m_strACL<< endl; 
            cout << "Qos                : " << m_strQOS<< endl; 
            cout << "AllowPriorities    : " << m_strPrior<< endl;  
            cout << "RcvBandwidthLmt    : " << m_strRcvBWLimit<< endl; 
            cout << "RcvBandwidthRsv    : " << m_strRcvBWRsv<< endl; 
            cout << "DftVlanId          : " << m_nDefaultVlan<< endl; 
            cout << "Promiscuous        : " << m_nPromisc<< endl; 
            cout << "MacVlanFltEnable   : " << m_nMacVlanFilter<< endl; 
            cout << "AllowTrmtMacs      : " << m_strTxMacs<< endl; 
            cout << "AllowTrmtMacVlans  : " << m_strTxMacvlans<< endl; 
            cout << "PlyBlkOver         : " << m_strPolicyBlkOver<< endl; 
            cout << "PlyVlanOver        : " << m_strPolicyVlanOver<< endl; 
            cout << "Version            : " << m_nVersion<< endl; 
            cout << "MgrId              : " << m_strMgrId<< endl; 
            cout << "TypeId             : " << m_strVSIType<< endl; 
            cout << "AllowMacChg        : " << m_nMacChange<< endl; 
            cout << "SwithportMode      : " << m_nSwitchPortMode<< endl; 
            cout << "IsCfgNetplane      : " << m_nIsCfgNetPlane<< endl; 
            cout << "NetplaneUuid       : " << m_strNetPlaneUUID<< endl;
            cout << "Sdn                : " << m_nIsSdn<< endl;
            cout << "Mtu                : " << m_nMTU<< endl; 
            cout << "TrunkNatvieVlanNum : " << m_nTrunkNativeVlan<< endl; 
            cout << "AccessVlanNum      : " << m_nAccessVlanNum<< endl; 
            cout << "AccessIsolateNo    : " << m_nAccessIsolateNum<< endl;                 
            cout << "IsolateType        : " << m_nIsolateType<< endl; 
            cout << "SegmentId          : " << m_nSegmentId<< endl;                 

        }    
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CPortGroupMsg);
            WRITE_VALUE(m_strName);
            WRITE_VALUE(m_strUUID);
            WRITE_VALUE(m_strdescription);
            WRITE_VALUE(m_nPGType);
            WRITE_VALUE(m_strACL);
            WRITE_VALUE(m_strQOS);
            WRITE_VALUE(m_strPrior);
            WRITE_VALUE(m_strRcvBWLimit);
            WRITE_VALUE(m_strRcvBWRsv);
            WRITE_VALUE(m_nDefaultVlan);
            WRITE_VALUE(m_nPromisc);
            WRITE_VALUE(m_nMacVlanFilter);
            WRITE_VALUE(m_strTxMacs);
            WRITE_VALUE(m_strTxMacvlans);
            WRITE_VALUE(m_strPolicyBlkOver);
            WRITE_VALUE(m_strPolicyVlanOver);
            WRITE_VALUE(m_nVersion);
            WRITE_VALUE(m_strMgrId);
            WRITE_VALUE(m_strVSIType);
            WRITE_VALUE(m_nMacChange);
            WRITE_VALUE(m_nSwitchPortMode);
            WRITE_VALUE(m_nIsCfgNetPlane);
            WRITE_VALUE(m_strNetPlaneUUID);
            WRITE_VALUE(m_nIsSdn);
            WRITE_VALUE(m_nMTU);
            WRITE_VALUE(m_nAccessVlanNum);
            WRITE_VALUE(m_nAccessIsolateNum);
            WRITE_VALUE(m_nTrunkNativeVlan);
            WRITE_VALUE(m_nOper);
            WRITE_VALUE(m_nIsolateType);
            WRITE_VALUE(m_nSegmentId);             
            SERIALIZE_PARENT_END(VNetConfigBaseMsg);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CPortGroupMsg);
            READ_VALUE(m_strName);
            READ_VALUE(m_strUUID);
            READ_VALUE(m_strdescription);
            READ_VALUE(m_nPGType);
            READ_VALUE(m_strACL);
            READ_VALUE(m_strQOS);
            READ_VALUE(m_strPrior);
            READ_VALUE(m_strRcvBWLimit);
            READ_VALUE(m_strRcvBWRsv);
            READ_VALUE(m_nDefaultVlan);
            READ_VALUE(m_nPromisc);
            READ_VALUE(m_nMacVlanFilter);
            READ_VALUE(m_strTxMacs);
            READ_VALUE(m_strTxMacvlans);
            READ_VALUE(m_strPolicyBlkOver);
            READ_VALUE(m_strPolicyVlanOver);
            READ_VALUE(m_nVersion);
            READ_VALUE(m_strMgrId);
            READ_VALUE(m_strVSIType);
            READ_VALUE(m_nMacChange);
            READ_VALUE(m_nSwitchPortMode);
            READ_VALUE(m_nIsCfgNetPlane);
            READ_VALUE(m_strNetPlaneUUID);
            READ_VALUE(m_nIsSdn);
            READ_VALUE(m_nMTU);
            READ_VALUE(m_nAccessVlanNum);
            READ_VALUE(m_nAccessIsolateNum);
            READ_VALUE(m_nTrunkNativeVlan);
            READ_VALUE(m_nOper);
            READ_VALUE(m_nIsolateType);
            READ_VALUE(m_nSegmentId);          
            DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
        };
      
    public:
        string m_strName;
        string m_strUUID;
        string m_strdescription;
        int32  m_nPGType;
        string m_strACL;
        string m_strQOS;;
        string m_strPrior;
        string m_strRcvBWLimit;
        string m_strRcvBWRsv;
        int32  m_nDefaultVlan;
        int32  m_nPromisc;
        int32  m_nMacVlanFilter;
        string m_strTxMacs;
        string m_strTxMacvlans;
        string m_strPolicyBlkOver;
        string m_strPolicyVlanOver;
        int32  m_nVersion;
        string m_strMgrId;
        string m_strVSIType;
        int32  m_nMacChange;
        int32  m_nSwitchPortMode;   // 0--access, 1--trunk
        int32  m_nIsCfgNetPlane;
        string m_strNetPlaneUUID;
        int32  m_nIsSdn;
        int32  m_nMTU;
        int32  m_nAccessVlanNum;
        int32  m_nAccessIsolateNum; 
        int32  m_nTrunkNativeVlan;
        int32  m_nOper;
        int32  m_nIsolateType;
        int32  m_nSegmentId;
        
};

class CPGTrunkVlanRangeMsg : public VNetConfigBaseMsg
{
    public:
        explicit CPGTrunkVlanRangeMsg() {m_nOper = EN_PORT_GROUP_OPER_BEGIN;}
        virtual ~CPGTrunkVlanRangeMsg() {};

    public:        
        int32 IsValidOper(void)
        {
            if((m_nOper >= EN_PORT_GROUP_OPER_BEGIN) && (m_nOper < EN_PORT_GROUP_OPER_END))
            {
                return 1;
            }
            return 0;
        }  
        
        void Print(void)
        {
            cout << "PGTrunkVlanRange operation code: " << m_nOper << endl;
            cout << "PG UUID: " << m_strPGUuid << endl;
            cout << "Vlan Range: [" << m_nVlanBegin <<", " << m_nVlanEnd << "] " << endl;
            cout << endl;
            return ;
        }
        
    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CPGTrunkVlanRangeMsg);
            WRITE_VALUE(m_strPGUuid);
            WRITE_VALUE(m_nVlanBegin);
            WRITE_VALUE(m_nVlanEnd);
            WRITE_VALUE(m_nOper);
            SERIALIZE_PARENT_END(VNetConfigBaseMsg);
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CPGTrunkVlanRangeMsg);
            READ_VALUE(m_strPGUuid);
            READ_VALUE(m_nVlanBegin);
            READ_VALUE(m_nVlanEnd);
            READ_VALUE(m_nOper);
            DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
        };
        
    public:
       // string m_strPGName;
        string m_strPGUuid;
        int32  m_nVlanBegin;
        int32  m_nVlanEnd;
        int32  m_nOper;  
};

enum SwitchType
{
    SWITCH_TYPE_INVALID  = 0,
    SOFT_VIR_SWITCH_TYPE = 1,  /* 软件虚拟交换 */
    EMB_VIR_SWITCH_TYPE  = 2,  /* 硬件虚拟交换 */
    PHYSICAL_SWITCH_TYPE = 3,  /* 物理交换 */
};

enum SwitchOper
{
    EN_SWITCH_OPER_BEGIN = 0,
    EN_ADD_SWITCH = 1,
    EN_DEL_SWITCH = 2,
    EN_MODIFY_SWITCH = 3,
    EN_ADD_SWITCH_PORT = 4,
    EN_DEL_SWITCH_PORT = 5,
    EN_SWITCH_OPER_END,
};

typedef enum tagDVSEvbMode
{
    EN_EVB_MODE_NORMAL = 0,
    EN_EVB_MODE_VEB    = 1, 
    EN_EVB_MODE_VEPA   = 2, 
}EN_DVS_EVB_MODE;

class CSwitchCfgMsg : public Serializable
{
public:
    explicit CSwitchCfgMsg() 
    {
        m_nType    = 0;
        m_nMTU     = 1500;
        m_nEvbMode = 0;
        m_nMaxVnic = 0;
        m_nOper    = EN_SWITCH_OPER_BEGIN;
    };
    virtual ~CSwitchCfgMsg() {};

public:
    string m_strName;       //switch name
    string m_strUuid;       //switch uuid
    int32  m_nType;         //switch type 0-physwitch，1-soft dvs, 2-emd dvs 
    int32  m_nMTU;          //switch MTU
    int32  m_nEvbMode;      //evb mode 
    int32  m_nMaxVnic;      //support max virtualnics
    string m_strPGUuid;     //port group
    int32  m_nOper;         //operator code
    
    void Print(void)
    {
        cout << "Switch operation code: " << m_nOper << endl;
        cout << "Switch name: " << m_strName << endl;
        cout << "Switch uuid: " << m_strUuid << endl;
        cout << "Switch type: " << m_nType << endl;
        cout << "Switch Attr: " << " MTU: " << m_nMTU << " EVB mode: " << m_nEvbMode 
             << " MaxVnic: "<< m_nMaxVnic << " PG: "<< m_strPGUuid << endl;
        return ;
    }
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CSwitchCfgMsg);
        WRITE_VALUE(m_strName);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_nType);
        WRITE_VALUE(m_nMTU);
        WRITE_VALUE(m_nEvbMode);      
        WRITE_VALUE(m_nMaxVnic);  
        WRITE_VALUE(m_strPGUuid);
        WRITE_VALUE(m_nOper);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CSwitchCfgMsg);
        READ_VALUE(m_strName);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_nType);
        READ_VALUE(m_nMTU);
        READ_VALUE(m_nEvbMode);
        READ_VALUE(m_nMaxVnic);
        READ_VALUE(m_strPGUuid);
        READ_VALUE(m_nOper);
        DESERIALIZE_END();
    };
};
        
class CSwitchPortCfgMsg : public Serializable
{
    public:    
    explicit CSwitchPortCfgMsg() 
    {
        m_nSwType   = 0;
        m_nBondMode = 0;
        m_nPortNum  = 0;
        m_nOper     = 0;
    };
    virtual ~CSwitchPortCfgMsg() {};

    void Print(void)
    {
        cout << "----------------------------------------------------------"<<endl;
        cout << "[VNM] Switch Port operation code: " << m_nOper << endl;
        cout << "Switch Uuid: " << m_strSwUuid << endl;
        cout << "Switch Name: " << m_strSwName << endl;
        cout << "Switch Type: " << m_nSwType   << endl;
        cout << "Bond   Mode: " << m_nBondMode   << endl;
        cout << "Request ID : " << m_strReqId   << endl;
        cout << "VNA    Uuid: " <<  m_strVnaUuid << endl;
        cout << "Port    NUM: " << m_nPortNum << endl;
        cout << "Host   Port: ";
        vector<string>::iterator it = m_vecHostPort.begin();
        for(; it != m_vecHostPort.end(); ++it)
        {
            cout << (*it) << " " <<endl; 
        }
        cout << "----------------------------------------------------------"<<endl;
        return ;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CSwitchPortCfgMsg);
        WRITE_VALUE(m_strSwName);
        WRITE_VALUE(m_strSwUuid);
        WRITE_VALUE(m_nSwType);
        WRITE_VALUE(m_strVnaUuid);
        WRITE_VALUE(m_vecHostPort);
        WRITE_VALUE(m_nBondMode);
        WRITE_VALUE(m_nPortNum);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_END(); 
};

    DESERIALIZE
{
        DESERIALIZE_BEGIN(CSwitchPortCfgMsg);
        READ_VALUE(m_strSwName);
        READ_VALUE(m_strSwUuid);
        READ_VALUE(m_nSwType);
        READ_VALUE(m_strVnaUuid);
        READ_VALUE(m_vecHostPort);
        READ_VALUE(m_nBondMode);
        READ_VALUE(m_nPortNum);
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_END();
    };
    
public:    
    string         m_strSwName;    //switch name
    string         m_strSwUuid;     //switch uuid
    int32          m_nSwType;       //switch type 
    string         m_strVnaUuid;    //vna uuid
    vector<string> m_vecHostPort;   //switch uplink port name
    int32          m_nBondMode;     //bond mode
    int32          m_nPortNum;      //switch uplink port num
    int32          m_nOper;         //operator code
    string         m_strReqId;      //request id
    
};

class CSwitchCfgAckMsg : public VNetConfigBaseMsg
{
public:
    string  m_strName;   //switch name
    string  m_strUuid;   //switch uuid 
    string  m_strReqId;  //request id
    int32   m_nOper;      //operator code
    int32   m_nProgress; //进度百分比
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CSwitchCfgAckMsg);
        WRITE_VALUE(m_strName);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strReqId);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_nProgress);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CSwitchCfgAckMsg);
        READ_VALUE(m_strName);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strReqId);
        READ_VALUE(m_nOper);
        READ_VALUE(m_nProgress);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

class CSdnCfgMsg : public VNetConfigBaseMsg
{
public:
    explicit CSdnCfgMsg() 
    {
        m_strSdnUuid      = "";
        m_strSdnConnType  = "";
        m_nSdnConnPort    = 0;
        m_strSdnConnIP    = "";
        m_strQuantumUuid  = "";
        m_strQuantumSvrIP = "";
        m_nSdnOper        = 0;
    };
    virtual ~CSdnCfgMsg() {};

public:
    string m_strSdnUuid;            //sdn uuid
    string m_strSdnConnType;        //sdn controller connect type
    int32  m_nSdnConnPort;          //sdn controller connect port
    string m_strSdnConnIP;          //sdn controller IP
    string m_strQuantumUuid;        //sdn uuid
    string m_strQuantumSvrIP;       //quantum server IP
    int32  m_nSdnOper;              //operator code
    
    void Print(void)
    {
        cout << "Sdn uuid    : " << m_strSdnUuid << endl;
        cout << "Sdn type    : " << m_strSdnConnType << endl;
        cout << "Sdn port    : " << m_nSdnConnPort << endl;
        cout << "Sdn IP      : " << m_strSdnConnIP << endl;
        cout << "Server uuid : " << m_strQuantumUuid << endl;
        cout << "Server IP   : " << m_strQuantumSvrIP << endl;
        cout << "Sdn oper    : " << m_nSdnOper << endl;
        return ;
    }
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CSdnCfgMsg);
        WRITE_VALUE(m_strSdnUuid);
        WRITE_VALUE(m_strSdnConnType);
        WRITE_VALUE(m_nSdnConnPort);
        WRITE_VALUE(m_strSdnConnIP);
        WRITE_VALUE(m_strQuantumUuid);
        WRITE_VALUE(m_strQuantumSvrIP);
        WRITE_VALUE(m_nSdnOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CSdnCfgMsg);
        READ_VALUE(m_strSdnUuid);
        READ_VALUE(m_strSdnConnType);
        READ_VALUE(m_nSdnConnPort);
        READ_VALUE(m_strSdnConnIP);
        READ_VALUE(m_strQuantumUuid);
        READ_VALUE(m_strQuantumSvrIP);
        READ_VALUE(m_nSdnOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

// SDN operation
typedef enum tagENSDNOper
{
    EN_INVALID_SDN_OPER = 0,
    EN_ADD_SDNCTRL,
    EN_DEL_SDNCTRL,
    EN_MOD_SDNCTRL,
    EN_ADD_QUANTUMRESTFUL,
    EN_DEL_QUANTUMRESTFUL,
    EN_MOD_QUANTUMRESTFUL,
    EN_SDNCTRL_OPER_ALL,        
}EN_SDN_OPER;




typedef enum tagWatchDogOper
{
    EN_START_TIMER_WATCH_DOG = 0,
    EN_STOP_TIMER_WATCH_DOG = 1, 
}EN_WATCH_DOG_OPER;


/**********************计算和VNA之间看门狗消息***********************/
//计算与VNA交互消息，由计算发起恢复和暂停看门狗定时器操作

class  CVNetWatchDogMsg :public WorkReq
{
    public:
        int64  m_vm_id;
        int32  m_operator;
        //带到vna
        MID    m_mid;
        
    public:
        CVNetWatchDogMsg(const int64 vmid,
                         const int32 oper)
        {
            m_vm_id = vmid;
            m_operator = oper;
        };

        void Print()
        {
            cout << "CVNetWatchDogMsg:"<< endl;
            cout << "   m_vm_id" << m_vm_id<<endl;
            cout << "   m_operator" << m_operator<<endl;
        };

        CVNetWatchDogMsg(){};
        virtual ~CVNetWatchDogMsg() {};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetWatchDogMsg);
            WRITE_VALUE(m_vm_id);
            WRITE_VALUE(m_operator);
            WRITE_OBJECT(m_mid);
            SERIALIZE_PARENT_END(WorkReq);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetWatchDogMsg);
            READ_VALUE(m_vm_id);
            READ_VALUE(m_operator);
            READ_OBJECT(m_mid);
            DESERIALIZE_PARENT_END(WorkReq);
        };
};

//计算与VNA交互消息，由VNA响应计算的发起的恢复和暂停看门狗定时器操作

class  CVNetWatchDogMsgAck :public WorkAck
{
    public:
        int64        vm_id;
        //带到vnetlib
        MID          m_mid;
        //CVnetVnicXML m_vnic;
        //MID   m_cVnaWdgMid;

        CVNetWatchDogMsgAck(){vm_id = INVALID_OID;};
        CVNetWatchDogMsgAck(const string &_action_id,
                                       int32        _state,
                                       int32        _progress,
                                       int64        _vid):WorkAck(_action_id,_state,_progress)
        {
           vm_id = _vid;
        };

       /* CVNetWatchDogMsgAck(const string &_action_id,
                                       int32        _state,
                                       int32        _progress,
                                        int64        _vid)
                                       :WorkAck(_action_id,_state,_progress),vm_id(_vid)
                                        
        {
              
        }; */
        void Print()
        {
            //cout << "CVNetWatchDogMsgAck:"<< endl;
            //m_vnic.Print();
        };
                
        virtual ~CVNetWatchDogMsgAck() {};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetWatchDogMsgAck);
            WRITE_VALUE(vm_id);
            WRITE_OBJECT(m_mid);
            //WRITE_OBJECT(m_vnic);
            //WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkAck);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetWatchDogMsgAck);
            READ_VALUE(vm_id);
            READ_OBJECT(m_mid);
            //READ_OBJECT(m_vnic);
            //READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkAck);
        };
};

//计算与VNA交互消息，由VNA向计算发起虚拟机看门狗定时器超时重启虚拟机

class CVNetNotifyVmRestatMsg :public WorkReq
{
    public:
        int64  m_vm_id;
        int32  m_operator;

    public:
        CVNetNotifyVmRestatMsg(const string &_action_id,
                                       const int64  vm_id, 
                                       const int32  operatator):WorkReq(_action_id)
        {
            m_vm_id = vm_id;
            m_operator = operatator;
        };

        CVNetNotifyVmRestatMsg(){};
        virtual ~CVNetNotifyVmRestatMsg() {};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetNotifyVmRestatMsg);
            WRITE_VALUE(m_vm_id);
            WRITE_VALUE(m_operator);
            //WRITE_OBJECT(m_mid);
            //WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkReq);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetNotifyVmRestatMsg);
            READ_VALUE(m_vm_id);
            READ_VALUE(m_operator);
            //READ_OBJECT(m_mid);
            //READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkReq);
        };

        void Print()
        {
            cout << "CVNetNotifyVmRestatMsg:"<< endl;
            cout << "   m_vm_id" << m_vm_id<<endl;
            cout << "   m_operator" << m_operator<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

//计算与VNA交互消息，由计算响应VNA发起的重启虚拟机操作

class CVNetNotifyHCrestatMsgAck :public WorkAck
{
    public:
        int64  m_vm_id;
        int32  m_operator;
        //MID    m_mid;
        //MID    m_cVnaWdgMid;
        
    public:
        CVNetNotifyHCrestatMsgAck(const int64  vm_id, const int32  operatator)
        {
            m_vm_id = vm_id;
            m_operator = operatator;
        };
        
        CVNetNotifyHCrestatMsgAck(const string id_of_action,
                                             const int64  vm_id, 
                                             const int32  operatator):WorkAck(id_of_action)
        {
            m_vm_id = vm_id;
            m_operator = operatator;
        };
        CVNetNotifyHCrestatMsgAck(){};
        virtual ~CVNetNotifyHCrestatMsgAck() {};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetNotifyHCrestatMsgAck);
            WRITE_VALUE(m_vm_id);
            WRITE_VALUE(m_operator);
            //WRITE_OBJECT(m_mid);
            //WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkAck);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetNotifyHCrestatMsgAck);
            READ_VALUE(m_vm_id);
            READ_VALUE(m_operator);
            //READ_OBJECT(m_mid);
            //READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkAck);
        };

        void Print()
        {
            cout << "CVNetNotifyHCrestatMsgAck:"<< endl;
            cout << "   m_vm_id" << m_vm_id<<endl;
            cout << "   m_operator" << m_operator<<endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
};

typedef enum tagWdgOper
{
    EN_START_WDG = 0,
    EN_START_WDG_ACK = 1,
    EN_STOP_WDG = 2, 
    EN_STOP_WDG_ACK = 3,
    EN_START_WDGTIMER = 4,
    EN_START_WDGTIMER_ACK = 5,
    EN_STOP_WDGTIMER = 6,
    EN_STOP_WDGTIMER_ACK = 7,
    EN_RR_VM = 8,
    EN_RR_VM_ACK = 9,
}EN_WDG_OPER;

/**********************看门狗和VNA之间看门狗消息***********************/

//看门狗与VNA交互消息，由VNA向看门狗发起启动、停止看门狗以及恢复和暂停定时器操作

class  CVNetWdgMsg :public WorkReq
{
public:

    void Print(void)
    {
        cout << "---------------------------"<<endl;
        cout << "VMID: " << m_qwVmId << endl;
        cout << "TIMEOUT: " << m_nTimerOut << endl;
        cout << "VMOPER: " << m_nOperator << endl;
        cout << serialize() << endl;
    }

public:
        int64  m_qwVmId;
        uint32  m_nTimerOut;
        int32  m_nOperator;
        MID    m_cMid;
        //期间保存计算的mid
        MID    m_cVnaWdgMid;
        
public:
        CVNetWdgMsg(const int64 vmid,
                const int32 timerout,
                         const int32 oper)
        {
            m_qwVmId = vmid;
            m_nTimerOut = timerout;
            m_nOperator = oper;
        };

        CVNetWdgMsg()
        {
            m_qwVmId = 0;
            m_nTimerOut = 0;
            m_nOperator = 0;
        };
        
        virtual ~CVNetWdgMsg() {};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetWdgMsg);
            WRITE_VALUE(m_qwVmId);
            WRITE_VALUE(m_nTimerOut);
            WRITE_VALUE(m_nOperator);
            WRITE_OBJECT(m_cMid);
            WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkReq);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetWdgMsg);
            READ_VALUE(m_qwVmId);
            READ_VALUE(m_nTimerOut);
            READ_VALUE(m_nOperator);            
            READ_OBJECT(m_cMid);
            READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkReq);
        };
};


//看门狗与VNA交互消息，由看门狗响应VNA发起的启动、停止看门狗以及恢复和暂停定时器操作

class  CVNetWdgAckMsg:public WorkAck
{

public:

    void Print(void)
    {
        cout << "---------------------------"<<endl;
        cout << "VMID: " << m_qwVmId << endl;
        cout << "VMOPER: " << m_nOperator << endl;
        m_cVnic.Print();
    }

public:
    int64  m_qwVmId;
    int32  m_nOperator;
    CVnetVnicXML m_cVnic;
    MID    m_cMid;
    //期间保存计算的mid
    MID    m_cVnaWdgMid;
        
public:
    CVNetWdgAckMsg(const int64 vmid, const int32 oper)
    {
        m_qwVmId = vmid;
        m_nOperator = oper;
    };
    CVNetWdgAckMsg()
    {
        m_qwVmId = 0;
        m_nOperator = 0;
    };
        virtual ~CVNetWdgAckMsg(){};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetWdgAckMsg);
            WRITE_VALUE(m_qwVmId);
            WRITE_VALUE(m_nOperator);
            WRITE_OBJECT(m_cVnic);
            WRITE_OBJECT(m_cMid);
            WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkAck);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetWdgAckMsg);
            READ_VALUE(m_qwVmId);
            READ_VALUE(m_nOperator);
            READ_OBJECT(m_cVnic);
            READ_OBJECT(m_cMid);
            READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkAck);
        };
};

//看门狗与VNA交互消息，由看门狗向VNA发起重启虚拟机操作

class CVNetWdgNotifyVMRRMsg :public WorkReq
{
    public:

    void Print(void)
    {
        cout << "---------------------------"<<endl;
        cout << "VMID: " << m_qwVmId << endl;
        cout << "VMOPER: " << m_nOperator << endl;
    }

public:
        int64  m_qwVmId;
        int32  m_nOperator;
        MID    m_cMid;
        MID    m_cVnaWdgMid;
    public:
        CVNetWdgNotifyVMRRMsg(const int64  vmid,
                          const int32  oper)
        {
            m_qwVmId = vmid;
            m_nOperator = oper;
        };
        CVNetWdgNotifyVMRRMsg()
        {
            m_qwVmId = 0;
            m_nOperator = 0;
        };
        virtual ~CVNetWdgNotifyVMRRMsg(){};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetWdgNotifyVMRRMsg);
            WRITE_VALUE(m_qwVmId);
            WRITE_VALUE(m_nOperator);
            WRITE_OBJECT(m_cMid);
            WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkReq);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetWdgNotifyVMRRMsg);
            READ_VALUE(m_qwVmId);
            READ_VALUE(m_nOperator);
            READ_OBJECT(m_cMid);
            READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkReq);
        };
};

//看门狗与VNA交互消息，VNA响应看门狗向VNA发起重启虚拟机操作

class CVNetWdgNotifyVMRRAckMsg :public WorkAck
{
    public:
    void Print(void)
    {
        cout << "---------------------------"<<endl;
        cout << "VMID: " << m_qwVmId << endl;
    }

public:
        int64  m_qwVmId;
        MID    m_cMid;
        MID    m_cVnaWdgMid;
        CVNetWdgNotifyVMRRAckMsg(const int64 vmid)
        {
            m_qwVmId = vmid;
        };        
        CVNetWdgNotifyVMRRAckMsg()
        {
            m_qwVmId = 0;
        };
        virtual ~CVNetWdgNotifyVMRRAckMsg(){};
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetWdgNotifyVMRRAckMsg);
            WRITE_VALUE(m_qwVmId);
            WRITE_OBJECT(m_cMid);
            WRITE_OBJECT(m_cVnaWdgMid);
            SERIALIZE_PARENT_END(WorkAck);
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetWdgNotifyVMRRAckMsg);
            READ_VALUE(m_qwVmId);
            READ_OBJECT(m_cMid);
            READ_OBJECT(m_cVnaWdgMid);
            DESERIALIZE_PARENT_END(WorkAck);
        };
};



#if 1


class CWildcastSwitchCfgMsg : public Serializable
{
public:
    string m_strName;       //switch name
    string m_strUuid;       //switch uuid
    int32  m_nType;         //switch type 0-physwitch，1-soft dvs, 2-emd dvs 
    int32  m_nMTU;          //switch MTU
    int32  m_nEvbMode;      //evb mode 
    int32  m_nMaxVnic;      //support max virtualnics
    string m_strPGUuid;     //port group
    int32  m_nOper;         //operator code
    string m_strReqId;      //request id  
    
    void Print(void)
    {
/*    
        cout << "WcSwitch operation code: " << m_nOper << endl;
        cout << "requst id  :" << m_strReqId << endl;
        cout << "WcSwitch name: " << m_strName << endl;
        cout << "WcSwitch uuid: " << m_strUuid << endl;
        cout << "WcSwitch type: " << m_nType << endl;
        cout << "Switch Attr: " << " MTU: " << m_nMTU << " EVB mode: " << m_nEvbMode 
             << " MaxVnic: "<< m_nMaxVnic << " PG: "<< m_strPGUuid << endl;
*/             
        cout << serialize() << endl;
        return ;
    }
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastSwitchCfgMsg);
        WRITE_VALUE(m_strName);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_nType);
        WRITE_VALUE(m_nMTU);
        WRITE_VALUE(m_nEvbMode);      
        WRITE_VALUE(m_nMaxVnic);  
        WRITE_VALUE(m_strPGUuid);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastSwitchCfgMsg);
        READ_VALUE(m_strName);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_nType);
        READ_VALUE(m_nMTU);
        READ_VALUE(m_nEvbMode);
        READ_VALUE(m_nMaxVnic);
        READ_VALUE(m_strPGUuid);
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_END();
    };
};
        
class CWildcastSwitchPortCfgMsg : public Serializable
{
    public:    
    explicit CWildcastSwitchPortCfgMsg() 
    {
        m_strSwName= m_strSwUuid = m_strReqId = "";
        m_nSwType   = 0;
        m_nBondMode = 0;
        m_nPortNum  = 0;
        m_nBondMode = 0;

        m_nPortType = m_nPortNum = 0;

        m_nOper = 0;
    };
    virtual ~CWildcastSwitchPortCfgMsg() {};

    void Print(void)
    {
#if 0    
        cout << "----------------------------------------------------------"<<endl;
        cout << "[VNM]Wildcast Switch Port operation code: " << m_nOper << endl;
        cout << "requst id  :" << m_strReqId << endl;
        cout << "Switch Uuid: " << m_strSwUuid << endl;
        cout << "Switch Name: " << m_strSwName << endl;
        cout << "Switch Type: " << m_nSwType   << endl;
        cout << "Bond   Mode: " << m_nBondMode   << endl;
        cout << "Request ID : " << m_strReqId   << endl; 
        cout << "Port    NUM: " << m_nPortNum << endl;
        cout << "Port list  : ";
        vector<string>::iterator it = m_vPort.begin();
        for(; it != m_vPort.end(); ++it)
        {
            cout << (*it) << " " <<endl; 
        }
        cout << "----------------------------------------------------------"<<endl;
#endif 
        cout << serialize() << endl;
        return ;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastSwitchPortCfgMsg);
        WRITE_VALUE(m_strSwName);
        WRITE_VALUE(m_strSwUuid);
        WRITE_VALUE(m_nSwType);    
        WRITE_VALUE(m_nPortType); 
        WRITE_VALUE(m_nPortNum); 
        WRITE_STRING_VECTOR(m_vPort);
        WRITE_VALUE(m_nBondMode); 
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_END(); 
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastSwitchPortCfgMsg);
        READ_VALUE(m_strSwName);
        READ_VALUE(m_strSwUuid);
        READ_VALUE(m_nSwType);
        READ_VALUE(m_nPortType);   
        READ_VALUE(m_nPortNum); 
        READ_STRING_VECTOR(m_vPort); 
        READ_VALUE(m_nBondMode); 
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_END();
    };
    
public:    
    string         m_strSwName;     //switch name
    string         m_strSwUuid;     //switch uuid
    int32          m_nSwType;       //switch type 
 
    int32         m_nPortType;     //switch uplink port type;   
    int32          m_nPortNum; 
    vector<string> m_vPort;       //uplink port list 
    int32          m_nBondMode;     //bond mode, if Port type = 1 valid.
     
    int32          m_nOper;         //operator code
    string         m_strReqId;      //request id    
};

class CWildcastSwitchCfgAckMsg : public VNetConfigBaseMsg
{
public:
    string  m_strName;   //switch name
    string  m_strUuid;   //switch uuid 
    string  m_strReqId;  //request id
    int32   m_nOper;      //operator code
    void Print(void)
    {
        cout << serialize() << endl;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastSwitchCfgAckMsg);
        WRITE_VALUE(m_strName);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strReqId);
        WRITE_VALUE(m_nOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastSwitchCfgAckMsg);
        READ_VALUE(m_strName);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strReqId);
        READ_VALUE(m_nOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

// 通配创建虚拟机端口操作类型
typedef enum tagENWildcastVirtualPortOper
{
    EN_INVALID_VIRTUAL_VPORT_OPER = 0,
    EN_ADD_WC_PORT,
    EN_DEL_WC_PORT,
    // EN_MOD_WC_PORT,
    EN_WC_OPER_ALL,        
}EN_WC_CREATE_VIRTUAL_PORT_OPER;

class CWildcastCreateVPortCfgMsg : public Serializable
{
public:
    string m_strUuid;       // uuid
    string m_strPortName;   // port name
    int32  m_nPortType;     // 
    int32  m_nIsHasKenelType; // is has kernel type
    int32  m_nKernelType;   // is kernel，valid.  kernel type( 10,11...)
    string m_strKernelPgUuid;// is kernel，valid. kernel port 选择的pg uuid 
    string m_strSwitchUuid;  // is kernel，valid. kernel port 所属的switch uuid 
    int32  m_nIsDhcp;       // is kernel，valid.  kernel port 是否采取DHCP方式来获取IP地址
    int32  m_nOper;         // operator code

    void Print(void)
    {
#if 0    
        cout << "WcCreateVPort operation code: " << m_nOper << endl;
        cout << "request id: " << m_strReqId << endl;
        cout << "uuid      : " << m_strUuid << endl;
        cout << "port name : " << m_strPortName << endl;
        cout << "port type : " << m_nPortType << endl;        
        cout << "IsHasKenelType: " << m_nIsHasKenelType << endl;
        cout << "KernelPgUuid: " << m_strKernelPgUuid << endl;
        cout << "SwitchUuid: " << m_strSwitchUuid << endl;
        cout << "IsDhcp    : " << m_nIsDhcp << endl;
#endif         
        cout << serialize() << endl;
        return ;
    }
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastCreateVPortCfgMsg);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strPortName);
        WRITE_VALUE(m_nPortType);
        WRITE_VALUE(m_nIsHasKenelType);
        WRITE_VALUE(m_nKernelType);
        WRITE_VALUE(m_strKernelPgUuid);
        WRITE_VALUE(m_strSwitchUuid);
        WRITE_VALUE(m_nIsDhcp);
        WRITE_VALUE(m_nOper);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastCreateVPortCfgMsg);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strPortName);
        READ_VALUE(m_nPortType);
        READ_VALUE(m_nIsHasKenelType);
        READ_VALUE(m_nKernelType);
        READ_VALUE(m_strKernelPgUuid);
        READ_VALUE(m_strSwitchUuid);
        READ_VALUE(m_nIsDhcp);
        READ_VALUE(m_nOper);
        DESERIALIZE_END();
    };
};

class CWildcastCreateVPortCfgAckMsg : public VNetConfigBaseMsg
{
public:
    string m_strUuid;       // uuid
    string m_strPortName;   // port name
    int32  m_nPortType;
    int32  m_nIsHasKenelType; // is has kernel type
    int32  m_nKernelType;   
    string m_strKernelPgUuid;
    string m_strSwitchUuid;
    int32  m_nIsDhcp;       // is kernel，valid.  kernel port 是否采取DHCP方式来获取IP地址
    int32  m_nOper;         // operator code
    string m_strReqId;      //request id 
    void Print(void)
    {
        cout << serialize() << endl;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastCreateVPortCfgAckMsg);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strPortName);
        WRITE_VALUE(m_nPortType);
        WRITE_VALUE(m_nIsHasKenelType);
        WRITE_VALUE(m_nKernelType);
        WRITE_VALUE(m_strKernelPgUuid);
        WRITE_VALUE(m_strSwitchUuid);
        WRITE_VALUE(m_nIsDhcp);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastCreateVPortCfgAckMsg);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strPortName);
        READ_VALUE(m_nPortType);
        READ_VALUE(m_nIsHasKenelType);
        READ_VALUE(m_nKernelType);
        READ_VALUE(m_strKernelPgUuid);
        READ_VALUE(m_strSwitchUuid);
        READ_VALUE(m_nIsDhcp);
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

// 通配创建虚拟机端口操作类型
typedef enum tagENWildcastLoopbackOper
{
    EN_INVALID_WC_LOOPBACK_OPER = 0,
    EN_ADD_WC_LOOPBACK_PORT,
    EN_DEL_WC_LOOPBACK_PORT,
    // EN_MOD_WC_PORT,
    EN_WC_LOOPBACk_OPER_ALL,        
}EN_WC_LOOPBACK_OPER;

class CWildcastLoopbackCfgMsg : public VNetConfigBaseMsg
{
public:
    string m_strUuid;       // wildcast port uuid
    string m_strPortName;   // port name
    int32  m_nIsLoop; // is loop
    int32  m_nOper;         // operator code
    
    void Print(void)
    {
#if 0    
        cout << "WcLoopback operation code: " << m_nOper << endl;
        cout << "uuid      : " << m_strUuid << endl;
        cout << "port name : " << m_strPortName << endl;    
        cout << "Is loop   : " << m_nIsLoop << endl;
#endif         
        cout << serialize() << endl;
        return ;
    }
        
    int32 IsValidOper(void) const
    {
        if((m_nOper > EN_INVALID_WC_LOOPBACK_OPER) && (m_nOper < EN_WC_LOOPBACk_OPER_ALL))
        {
            return 1;
        }
        return 0;
    }
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastLoopbackCfgMsg);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strPortName);
        WRITE_VALUE(m_nIsLoop);
        WRITE_VALUE(m_nOper);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastLoopbackCfgMsg);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strPortName);
        READ_VALUE(m_nIsLoop);
        READ_VALUE(m_nOper);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

class CWildcastLoopbackCfgAckMsg : public VNetConfigBaseMsg
{
public:
    string m_strUuid;       // uuid
    string m_strPortName;   // port name
    int32  m_nIsLoop;       // is Loop 
    int32  m_nOper;         // operator code
    string m_strReqId;      //request id 

    void Print(void)
    {
        cout << serialize() << endl;
        return ;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastLoopbackCfgAckMsg);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strPortName);
        WRITE_VALUE(m_nIsLoop);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastLoopbackCfgAckMsg);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strPortName);
        READ_VALUE(m_nIsLoop);
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

// wildcast switch/createvport/loopback task msg
class CWildcastTaskDelMsg : public Serializable
{
public:
    string m_strUuid;       //wildcast task uuid 目前只有DEL TASK MSG    
    int32  m_nOper;         //operator code
    string m_strReqId;      //request id  

    void Print(void)
    {
#if 0    
        cout << "CWildcastTaskDelMsg operation code: " << m_nOper << endl;
        cout << "uuid      : " << m_strUuid << endl;
        cout << "ReqId     : " << m_strReqId << endl;
#endif         
        cout << serialize() << endl;
        return ;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastTaskDelMsg);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastTaskDelMsg);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_END();
    };
};

class CWildcastTaskDelAckMsg : public VNetConfigBaseMsg
{
public:
    string  m_strUuid;    //wildcast task uuid 
    int32   m_nOper;      //operator code    
    string  m_strReqId;   //request id
    void Print(void)
    {
        cout << serialize() << endl;
        return ;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(CWildcastTaskDelAckMsg);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_nOper);
        WRITE_VALUE(m_strReqId);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CWildcastTaskDelAckMsg);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_nOper);
        READ_VALUE(m_strReqId);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

// wildcast task create vport msg


#endif 

class CNicInfo :public Serializable
{
public:
    int32   m_nNicIndex;                      //NicIndex,输入参数
    int32   m_nIsSriov;                        //IsSriov,输入参数
    int32   m_nIsLoop;                         //IsLoop,输入参数
    string  m_strLogicNetworkID;        //LogicNetwork UUID,输入参数
    string  m_strAdapterModel;           //网卡型号,输入参数
    string  m_strVSIProfileID;             //VSIProfile UUID,输出参数

    string  m_strPCIOrder;                  //SRIOVPCI,不相关
    string  m_strBridgeName;
    string  m_strIP;
    string  m_strNetmask ;
    string  m_strGateway;
    string  m_strMac;
    uint32  m_nCvlan; 
    
public:
    CNicInfo()
    {
        
    };
            
    CNicInfo(const int32 NicIndex,
             const int32 IsSriov,
             const int32 IsLoop,
             const string &LogicNetworkID,
             const string &AdapterModel,
             const string &VSIProfileID,
             const string &PCIOrder,
             const string &BridgeName,
             const string &Netmask,
             const string &Gateway,
             const string &Mac,
             const int32  Cvlan)
    {
         m_nNicIndex = NicIndex;
         m_nIsSriov  = IsSriov;                        
         m_nIsLoop   = IsLoop;                      
         m_strLogicNetworkID = LogicNetworkID ;      
         m_strAdapterModel  = AdapterModel;           
         m_strVSIProfileID = VSIProfileID;             
         m_strPCIOrder = PCIOrder;                 
         m_strBridgeName = BridgeName ;
         m_strNetmask  = Netmask;
         m_strGateway = Gateway;
         m_strMac = Mac;
         m_nCvlan = Cvlan; 
    };
    virtual ~CNicInfo() {};
    
    void Print()
    {
        cout << "Vnic Infomation:" << endl;
        cout << "   Vnic Index           :" << m_nNicIndex << endl;
        cout << "   IsSriov              :" << m_nIsSriov << endl;
        cout << "   IsLoop               :" << m_nIsLoop << endl;
        cout << "   LogicNetwork ID      :" << m_strLogicNetworkID << endl;
        cout << "   Adapter Model        :" << m_strAdapterModel << endl;
        cout << "   VSI UUID             :" << m_strVSIProfileID << endl;
        cout << "   PCI_Order            : " << m_strPCIOrder << endl;
        cout << "   Bridge Name          : " << m_strBridgeName << endl;
        cout << "   IP Address           : " << m_strIP << endl;
        cout << "   Netmask              : " << m_strNetmask << endl;
        cout << "   Gateway              : " << m_strGateway << endl;
        cout << "   MAC Address          : " << m_strMac << endl;
        cout << "   Cvlan                  : " << m_nCvlan << endl;       
    };
        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CNicInfo);
        WRITE_VALUE(m_nNicIndex);
        WRITE_VALUE(m_nIsSriov);
        WRITE_VALUE(m_nIsLoop);
        WRITE_VALUE(m_strLogicNetworkID);
        WRITE_VALUE(m_strAdapterModel);
        WRITE_VALUE(m_strVSIProfileID);
        WRITE_VALUE(m_strPCIOrder);
        WRITE_VALUE(m_strBridgeName);
        WRITE_VALUE(m_strIP);
        WRITE_VALUE(m_strNetmask);
        WRITE_VALUE(m_strGateway);
        WRITE_VALUE(m_strMac);
        WRITE_VALUE(m_nCvlan);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CNicInfo);
        READ_VALUE(m_nNicIndex);
        READ_VALUE(m_nIsSriov);
        READ_VALUE(m_nIsLoop);
        READ_VALUE(m_strLogicNetworkID);
        READ_VALUE(m_strAdapterModel);
        READ_VALUE(m_strVSIProfileID);
        READ_VALUE(m_strPCIOrder);
        READ_VALUE(m_strBridgeName);
        READ_VALUE(m_strIP);
        READ_VALUE(m_strNetmask);
        READ_VALUE(m_strGateway);
        READ_VALUE(m_strMac);
        READ_VALUE(m_nCvlan);
        DESERIALIZE_END();
    };
};


class CQueryVmNicInfoMsg: public WorkReq
{
public:
    int64  m_vm_id;       //由计算填写，虚拟机id
    MID    m_mid;
    
public:
    CQueryVmNicInfoMsg(){};
    
    CQueryVmNicInfoMsg(int64  vid)
    {
        m_vm_id = vid;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CQueryVmNicInfoMsg);
        WRITE_VALUE(m_vm_id);
        WRITE_OBJECT(m_mid);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CQueryVmNicInfoMsg);
        READ_VALUE(m_vm_id);
        READ_OBJECT(m_mid);
        DESERIALIZE_PARENT_END(WorkReq);
    };

    void Print()
    {
        cout << "CQueryVmNicInfoMsg:" << endl;
        cout << "   VM_ID           :" << m_vm_id << endl;
        cout << "   MID             :" << m_mid._application << "." 
            << m_mid._process << "." << m_mid._unit << endl;        

        cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };
};

class CQueryVmNicInfoMsgAck :public WorkAck
{
public:    
    MID                  m_mid;
    int64                m_vm_id;
    vector<CNicInfo>     m_infos;
    
public:   
    CQueryVmNicInfoMsgAck() {};
    CQueryVmNicInfoMsgAck(int64 vid,const vector<CNicInfo> &infos)
    {
        m_vm_id = vid;
        m_infos = infos;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CQueryVmNicInfoMsgAck);
        WRITE_OBJECT(m_mid);
        WRITE_VALUE(m_vm_id);
        WRITE_OBJECT_VECTOR(m_infos);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CQueryVmNicInfoMsgAck);
        READ_OBJECT(m_mid);
        READ_VALUE(m_vm_id);
        READ_OBJECT_VECTOR(m_infos);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    void Print()
    {
        cout << "CQueryVmNicInfoMsgAck:" << endl;
        cout << "   VM_ID           :" << m_vm_id << endl;
        cout << "   MID             :" << m_mid._application << "." 
            << m_mid._process << "." << m_mid._unit << endl;
        
        vector<CNicInfo>::iterator itr = m_infos.begin();
        for( ; itr != m_infos.end(); ++itr)
        {
            itr->Print();
        }

        cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };
};


//虚拟机迁移消息 cc->vnm START
class CVNetVmMigrateBeginMsg :public WorkReq
{
    public:
        MID     m_mid;
        int64   m_vm_id;                // 虚拟机ID
        vector<CVNetVnicConfig> m_VmNicList;
        CWatchDogInfo m_WatchDogInfo;
        string  m_src_cc_application;   // 源集群名称
        string  m_src_hc_application;   // 源主机名称
        string  m_dst_cc_application;   // 目的集群名称
        string  m_dst_hc_application;   // 目的主机名称
        
    public:    
        CVNetVmMigrateBeginMsg(){}
        virtual ~CVNetVmMigrateBeginMsg(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVmMigrateBeginMsg);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            WRITE_OBJECT_VECTOR(m_VmNicList);
            WRITE_OBJECT(m_WatchDogInfo);
            WRITE_STRING(m_src_cc_application);
            WRITE_STRING(m_src_hc_application);
            WRITE_STRING(m_dst_cc_application);
            WRITE_STRING(m_dst_hc_application);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVmMigrateBeginMsg);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            READ_OBJECT_VECTOR(m_VmNicList);
            READ_OBJECT(m_WatchDogInfo);
            READ_STRING(m_src_cc_application);
            READ_STRING(m_src_hc_application);
            READ_STRING(m_dst_cc_application);
            READ_STRING(m_dst_hc_application);
            DESERIALIZE_PARENT_END(WorkReq);
        };
};

//虚拟机迁移消息 cc->vnm END 
class CVNetVmMigrateEndMsg :public WorkReq
{
    public:
        MID     m_mid;
        int64   m_vm_id;              // vm_id
        vector<CVNetVnicConfig> m_VmNicList;
        CWatchDogInfo m_WatchDogInfo;
        string  m_src_cc_application; // 源集群名称
        string  m_src_hc_application; // 源主机名称
        string  m_dst_cc_application; // 目的集群名称
        string  m_dst_hc_application; // 目的主机名称
        int32   m_result;             // 迁移结果: =0 success; !=0 failed. 
    public:    
        CVNetVmMigrateEndMsg(){}
        virtual ~CVNetVmMigrateEndMsg(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVmMigrateEndMsg);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            WRITE_OBJECT_VECTOR(m_VmNicList);
            WRITE_OBJECT(m_WatchDogInfo);
            WRITE_STRING(m_src_cc_application);
            WRITE_STRING(m_src_hc_application);
            WRITE_STRING(m_dst_cc_application);
            WRITE_STRING(m_dst_hc_application);
            WRITE_VALUE(m_result);
            SERIALIZE_PARENT_END(WorkReq);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVmMigrateEndMsg);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            READ_OBJECT_VECTOR(m_VmNicList);
            READ_OBJECT(m_WatchDogInfo);
            READ_STRING(m_src_cc_application);
            READ_STRING(m_src_hc_application);
            READ_STRING(m_dst_cc_application);
            READ_STRING(m_dst_hc_application);
            READ_VALUE(m_result);
            DESERIALIZE_PARENT_END(WorkReq);
        };
};

//虚拟机迁移消息的应答 vnm->cc
class CVNetVmMigrateMsgAck :public WorkAck
{
    public:
        MID     m_mid;
        int64   m_vm_id;

    public:    
        CVNetVmMigrateMsgAck(){}
        virtual ~CVNetVmMigrateMsgAck(){}
        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNetVmMigrateMsgAck);
            WRITE_OBJECT(m_mid);
            WRITE_DIGIT(m_vm_id);
            SERIALIZE_PARENT_END(WorkAck);
        };     

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNetVmMigrateMsgAck);
            READ_OBJECT(m_mid);
            READ_DIGIT(m_vm_id);
            DESERIALIZE_PARENT_END(WorkAck);
        };  
};

/* 清理离线vna web->vnm */
class CVNetVnaDelete : public VNetConfigBaseMsg
{
public:
    string  m_strUuid;    //vna uuid 
    void Print(void)
    {
        cout << serialize() << endl;
        return ;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetVnaDelete);
        WRITE_VALUE(m_strUuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetVnaDelete);
        READ_VALUE(m_strUuid);
        DESERIALIZE_END();
    };
};

class CVNetVnaDeleteAck : public VNetConfigBaseMsg
{
public:
    string  m_strUuid;    //vna uuid 
    void Print(void)
    {
        cout << serialize() << endl;
        return ;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetVnaDeleteAck);
        WRITE_VALUE(m_strUuid);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetVnaDeleteAck);
        READ_VALUE(m_strUuid);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};


class CVNetLGNetID2Name : public VNetConfigBaseMsg
{
public:
    string  m_strUuid;    //LGNet uuid 
    string  m_strLGName;
    string  m_strUsername;
    
    void Print(void)
    {
        cout << serialize() << endl;
        return ;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetLGNetID2Name);
        WRITE_VALUE(m_strUuid);
        WRITE_VALUE(m_strLGName);
        WRITE_VALUE(m_strUsername);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetLGNetID2Name);
        READ_VALUE(m_strUuid);
        READ_VALUE(m_strLGName);
        READ_VALUE(m_strUsername);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
};

class CVNetPortGroupQuery:public VNetConfigBaseMsg
{
public:

    CVNetPortGroupQuery(long long tmp_start=0,
                         long long tmp_count =0,
                         long long tmp_type=0,
                         const string  &pg = "")
    {
        start_index = tmp_start;
        count = tmp_count;
    };

    void Print(void)
    {
        cout << serialize() << endl;
        return ;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetPortGroupQuery);
        WRITE_VALUE(start_index);
        WRITE_VALUE(count);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetPortGroupQuery);
        READ_VALUE(start_index);
        READ_VALUE(count);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
    int32   start_index;
    int32   count;
};

class CVnetPortGroup : public Serializable
{
public:
    string pg_name;
    string pg_uuid;
    int32  pg_type;
    int32  switch_port_mode;   // 0--access, 1--trunk
    int32  is_cfg_netplane;
    string  netplane_uuid;
    int32  mtu;
    int32  access_vlan_num;
    int32  access_isolate_num; 
    int32  trunk_native_vlan;
    int32  isolate_type;
    int32  segment_id;
    int32  m_nVxlanIsolateNo;
    int32  m_nIsSdn;


public:
    CVnetPortGroup()
    {
        pg_name = "";
        pg_uuid = "";
        switch_port_mode = 0;
        access_isolate_num = 0;
        pg_type = 0;
        is_cfg_netplane = 0;
        netplane_uuid = "";
        mtu = 0;
        access_vlan_num = 0;
        trunk_native_vlan = 0;
        isolate_type = 0;
        segment_id = 0;
        m_nVxlanIsolateNo = 0;
        m_nIsSdn = 0;
    }
    ~CVnetPortGroup()
    {
    }
    void set_pg_name(string name)
    {
        pg_name = name;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(CVnetPortGroup);
        WRITE_VALUE(pg_name);
        WRITE_VALUE(pg_uuid);
        WRITE_VALUE(pg_type);
        WRITE_VALUE(switch_port_mode);
        WRITE_VALUE(is_cfg_netplane);
        WRITE_VALUE(netplane_uuid);
        WRITE_VALUE(mtu);
        WRITE_VALUE(access_vlan_num);
        WRITE_VALUE(access_isolate_num);
        WRITE_VALUE(trunk_native_vlan);
        WRITE_VALUE(isolate_type);
        WRITE_VALUE(segment_id);
        WRITE_VALUE(m_nVxlanIsolateNo);
        WRITE_VALUE(m_nIsSdn);
        SERIALIZE_END();
    }
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVnetPortGroup);
        READ_VALUE(pg_name);
        READ_VALUE(pg_uuid);
        READ_VALUE(pg_type);
        READ_VALUE(switch_port_mode);
        READ_VALUE(is_cfg_netplane);
        READ_VALUE(netplane_uuid);
        READ_VALUE(mtu);
        READ_VALUE(access_vlan_num);
        READ_VALUE(access_isolate_num);
        READ_VALUE(trunk_native_vlan);
        READ_VALUE(isolate_type);
        READ_VALUE(segment_id);
        READ_VALUE(m_nVxlanIsolateNo);
        READ_VALUE(m_nIsSdn);
        DESERIALIZE_END();
    };

};

class CVnetPortGroupLists : public VNetConfigBaseMsg
{
public:
    CVnetPortGroupLists(){};
    ~CVnetPortGroupLists(){};

    void set_ret_code(int32 code)
    {
        ret_code = code;
        return;
    }
    void set_next_index(int32 index)
    {
        next_index = index;
        return;
    }
    
    void set_max_count(int32 count)
    {
        max_count = count;
        return;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(CVnetPortGroupLists);
        WRITE_VALUE(ret_code);
        WRITE_VALUE(next_index);
        WRITE_VALUE(max_count);
        WRITE_OBJECT_ARRAY(pg_info);
        SERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVnetPortGroupLists);
        READ_VALUE(ret_code);
        READ_VALUE(next_index);
        READ_VALUE(max_count);
        READ_OBJECT_ARRAY(pg_info);
        DESERIALIZE_PARENT_END(VNetConfigBaseMsg);
    };
    
    int32  ret_code;
    int32  next_index;
    int32  max_count;
    vector<CVnetPortGroup> pg_info;
};
#endif

