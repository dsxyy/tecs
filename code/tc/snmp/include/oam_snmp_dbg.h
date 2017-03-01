#ifndef _OAM_LM_DBG_H_
#define _OAM_LM_DBG_H_

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "oam_pub.h"

/***********************************************************
 *                        ��������                         *
***********************************************************/
/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/



/*****************************************************************************/
/**
@brief ����������Snmp Job���Ժ���������Ϣ��

@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n OMP���߶������壬OAMMGR��

@li @b ������ʾ��
\n�ޡ�
 
@li @b ����˵����

@li @b ���ʾ����
<pre>
                   [Snmp Help Function Table]

  Index, Function Name,                             Description
=============================================================================================

      1, OAM_DbgSendSnmpAlarm(WORD32 dwFuncId),     Send SNMP alarm.

      2, OAM_DbgShowSnmpDdmAckInfo(),               Show all SnmpDDMAckInfo.

      3, OAM_DbgShowSnmpRegInfo(),                  Show all register information.

      4, OAM_DbgShowSnmpSecurity(),                 Show SNMP Security  information.

      5, OAM_DbgShowSnmpXmlInfo(),                  Show XML information.
=============================================================================================
</pre>
@li @b �޸ļ�¼��
-#  ��EC�����޸�ʱ�䣬�޸��ˣ�EC������
*/
/*****************************************************************************/
void SnmpHelp(void);

/*****************************************************************************/
/**
@brief ��������������һ��alarm��

@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n OMP���߶������壬OAMMGR��

@li @b ������ʾ��
\n�ޡ�
 
@li @b ����˵����

@li @b ���ʾ����
<pre>
</pre>
@li @b �޸ļ�¼��
-#  ��EC�����޸�ʱ�䣬�޸��ˣ�EC������
*/
/*****************************************************************************/
void OAM_DbgSendSnmpAlarm(WORD32 dwFuncId);

/*****************************************************************************/
/**
@brief ������������ʾDDMӦ����Ϣ��

@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n OMP���߶������壬OAMMGR��

@li @b ������ʾ��
\n�ޡ�
 
@li @b ����˵����

@li @b ���ʾ����
<pre>
[SNMP Ddm Ack  Function ID Table]
wIndex,   dwFuncId
==================
     1,       0x7b
==================
</pre>
@li @b �޸ļ�¼��
-#  ��EC�����޸�ʱ�䣬�޸��ˣ�EC������
*/
/*****************************************************************************/
void OAM_DbgShowSnmpDdmAckInfo(void);

/*****************************************************************************/
/**
@brief ������������ʾSNMP��ע��Ĺ�����Ϣ.

@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n OMP���߶������壬OAMMGR��

@li @b ������ʾ��
\n�ޡ�
 
@li @b ����˵����

@li @b ���ʾ����
<pre>
                [SNMP SHM Register Information Table]
wIndex,  wFuncType,        Jno,  Module,   Unit,  SUnit,  SubSystem,   ucXMLAckFlag
========================================================================================
     1,        421,   0x8a10001,       1,  65535,    255,         255,             0
     2,        419,   0x8a10001,       1,  65535,    255,         255,             1
     3,        402,   0x8a10001,       1,  65535,    255,         255,             0
     4,        403,   0x8a10001,       1,  65535,    255,         255,             0
     5,        409,   0x8a10001,       1,  65535,    255,         255,             0
========================================================================================
</pre>
@li @b �޸ļ�¼��
-#  ��EC�����޸�ʱ�䣬�޸��ˣ�EC������
*/
/*****************************************************************************/
void OAM_DbgShowSnmpRegInfo(void);

/*****************************************************************************/
/**
@brief ������������ʾSNMP�е�ACL��Ϣ��

@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n OMP���߶������壬OAMMGR��

@li @b ������ʾ��
\n�ޡ�
 
@li @b ����˵����

@li @b ���ʾ����
<pre>
            [SNMP Security  Information Table]
index,    community,        network,            secName
=======================================================
00001,     platform,         128.0.31.0,    mynetwork    
00002,     platform,         10.44.40.0,    mynetwork    
00003,     platform,         10.44.42.0,    mynetwork    
00004,     platform,        129.160.1.0,    mynetwork    
00005,     platform,          129.0.0.0,    mynetwork    
=======================================================
</pre>
@li @b �޸ļ�¼��
-#  ��EC�����޸�ʱ�䣬�޸��ˣ�EC������
*/
/*****************************************************************************/
void OAM_DbgShowSnmpSecurity(void);

/*****************************************************************************/
/**
@brief ����������SNMP �е�xml��Ϣ��

@li @b ����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n OMP���߶������壬OAMMGR��

@li @b ������ʾ��
\n�ޡ�
 
@li @b ����˵����

@li @b ���ʾ����
<pre>
                [SNMP XML Information Table]
wIndex,   FuncID,     OidType,   AckType,    FuncType,   IndexNum,   StructLen,    ucXMLAckFlag
===============================================================================================
     1,   458753,      SCALAR,   RESPONSE,     IN/OUT,          0,           4,            1
1.3.6.1.4.1.3902.150.11.5.10.19.458753
===============================================================================================
oidNO.,  PosNO.,  ParaType,    ArrayLen,  InOutProp,  IndexProp,  RowStateProp,  RwProp,     wParaLen,  defValue,  maxValue,  minValue
======================================================================================================================================
     1,       1,         MAP,         1,     IN/OUT,  NOT_INDEX,  NOT_ROWSTATUS,  READ/WRITE,         4,      NULL,    1     ,  80     
======================================================================================================================================
wIndex,   FuncID,     OidType,   AckType,    FuncType,   IndexNum,   StructLen,    ucXMLAckFlag
===============================================================================================
     2,   458754,      SCALAR,   RESPONSE,     IN/OUT,          0,          20,            1
1.3.6.1.4.1.3902.150.11.5.10.19.458754
===============================================================================================
</pre>
@li @b �޸ļ�¼��
-#  ��EC�����޸�ʱ�䣬�޸��ˣ�EC������
*/
/*****************************************************************************/
void OAM_DbgShowSnmpXmlInfo((WORD16 wFuncType);
#ifdef WIN32
    #pragma pack()
#endif

#endif
