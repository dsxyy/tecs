/*********************************************************************
* ��Ȩ���� (C)2009, ����ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� vnetlib_common.h
* �ļ���ʶ��
* ����˵���� ���ļ�������vnetlib������¼��ӿ�
* ��ǰ�汾�� V1.0
* ��    �ߣ� chen.zhiwei
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2013��01��31��
*    �� �� �ţ�V1.0
*    �� �� �ˣ���־ΰ
*    �޸����ݣ�����
**********************************************************************/

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
#ifndef    _PUB_VNET_COMMON_H_
#define    _PUB_VNET_COMMON_H_

/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "../../sky/include/sky.h"


//MODULE role ȡֵ
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
#define VNETLIB_RESULT_LINK_NOREADY    (1) /* vnetlib��vna����vnmֱ�ӵ���·�����������������Ͳ���ȥ */
#define VNETLIB_RESULT_NO_RESOURCE     (2)/* ��Դ����ʧ�� */
#define VNETLIB_RESULT_OTHER_ERROR     (3)/* �������� ���������Ʋ��� */


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