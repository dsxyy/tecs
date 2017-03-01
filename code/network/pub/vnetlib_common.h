/*********************************************************************
* 版权所有 (C)2009, 中兴通讯股份有限公司。
*
* 文件名称： vnetlib_common.h
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
#ifndef    _PUB_VNET_COMMON_H_
#define    _PUB_VNET_COMMON_H_

/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "../../sky/include/sky.h"


//MODULE role 取值
#define VNA_MODULE_ROLE_NONE   0x00
#define VNA_MODULE_ROLE_HC        0x01
#define VNA_MODULE_ROLE_CC        0x02
#define VNA_MODULE_ROLE_DHCP      0x3
#define VNA_MODULE_ROLE_NAT       0x4
#define VNA_MODULE_ROLE_WATCHDOG  0x05
#define VNA_MODULE_ROLE_TC        0x06
#define VNA_MODULE_ROLE_APISVR    0x7
//#define VNA_MODULE_ROLE_OTHER     0x80

#define VNETLIB_RESULT_OK              (0) /* ok */
#define VNETLIB_RESULT_LINK_NOREADY    (1) /* vnetlib与vna或者vnm直接的链路不正常，以至请求发送不出去 */
#define VNETLIB_RESULT_NO_RESOURCE     (2)/* 资源申请失败 */
#define VNETLIB_RESULT_OTHER_ERROR     (3)/* 其他错误 ，后续完善补充 */


#define VNET_LIB_WC_TASK_SWITCH       0
#define VNET_LIB_WC_TASK_VPORT        1
#define VNET_LIB_WC_TASK_LOOP         2 


class VNetConfigBaseMsg:public Serializable
{
public: 
    VNetConfigBaseMsg(){}
    virtual ~VNetConfigBaseMsg(){}
    
public:
    void SetResult(int32 result)
    {
        m_ncfgresult = result;
    }
    
    int32 GetResult(void)
    {
        return m_ncfgresult;
    }

    void SetResultinfo(const string& info)
    {
        m_strcfgretinfo = info;
    }

    string GetResultinfo(void)
    {
        return m_strcfgretinfo;
    }
    
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(VNetConfigBaseMsg);
        WRITE_VALUE(m_ncfgresult);
        WRITE_VALUE(m_strcfgretinfo);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VNetConfigBaseMsg);
        READ_VALUE(m_ncfgresult);
        READ_VALUE(m_strcfgretinfo);
        DESERIALIZE_END();
    };
private:
   int32   m_ncfgresult; 
   string  m_strcfgretinfo;
};

#endif