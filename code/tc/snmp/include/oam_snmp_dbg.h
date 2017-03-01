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
 *                    其它条件编译选项                     *
***********************************************************/
/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "oam_pub.h"

/***********************************************************
 *                        常量定义                         *
***********************************************************/
/***********************************************************
 *                       全局宏                            *
***********************************************************/



/*****************************************************************************/
/**
@brief 功能描述，Snmp Job调试函数帮助信息。

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n OMP或者独立单板，OAMMGR。

@li @b 风险提示：
\n无。
 
@li @b 其它说明：

@li @b 输出示例：
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
@li @b 修改记录：
-#  ：EC单，修改时间，修改人：EC单描述
*/
/*****************************************************************************/
void SnmpHelp(void);

/*****************************************************************************/
/**
@brief 功能描述，发送一个alarm。

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n OMP或者独立单板，OAMMGR。

@li @b 风险提示：
\n无。
 
@li @b 其它说明：

@li @b 输出示例：
<pre>
</pre>
@li @b 修改记录：
-#  ：EC单，修改时间，修改人：EC单描述
*/
/*****************************************************************************/
void OAM_DbgSendSnmpAlarm(WORD32 dwFuncId);

/*****************************************************************************/
/**
@brief 功能描述，显示DDM应答信息。

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n OMP或者独立单板，OAMMGR。

@li @b 风险提示：
\n无。
 
@li @b 其它说明：

@li @b 输出示例：
<pre>
[SNMP Ddm Ack  Function ID Table]
wIndex,   dwFuncId
==================
     1,       0x7b
==================
</pre>
@li @b 修改记录：
-#  ：EC单，修改时间，修改人：EC单描述
*/
/*****************************************************************************/
void OAM_DbgShowSnmpDdmAckInfo(void);

/*****************************************************************************/
/**
@brief 功能描述，显示SNMP中注册的功能信息.

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n OMP或者独立单板，OAMMGR。

@li @b 风险提示：
\n无。
 
@li @b 其它说明：

@li @b 输出示例：
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
@li @b 修改记录：
-#  ：EC单，修改时间，修改人：EC单描述
*/
/*****************************************************************************/
void OAM_DbgShowSnmpRegInfo(void);

/*****************************************************************************/
/**
@brief 功能描述，显示SNMP中的ACL信息。

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n OMP或者独立单板，OAMMGR。

@li @b 风险提示：
\n无。
 
@li @b 其它说明：

@li @b 输出示例：
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
@li @b 修改记录：
-#  ：EC单，修改时间，修改人：EC单描述
*/
/*****************************************************************************/
void OAM_DbgShowSnmpSecurity(void);

/*****************************************************************************/
/**
@brief 功能描述，SNMP 中的xml信息。

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n OMP或者独立单板，OAMMGR。

@li @b 风险提示：
\n无。
 
@li @b 其它说明：

@li @b 输出示例：
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
@li @b 修改记录：
-#  ：EC单，修改时间，修改人：EC单描述
*/
/*****************************************************************************/
void OAM_DbgShowSnmpXmlInfo((WORD16 wFuncType);
#ifdef WIN32
    #pragma pack()
#endif

#endif
