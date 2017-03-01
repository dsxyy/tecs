/*************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�smart_api.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����̼�����ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�guojsh
* ������ڣ�2011��8��20��
*
* �޸ļ�¼1��
*  �޸����ڣ�
*  �� �� �ţ�
*  �� �� �ˣ�
*  �޸����ݣ�
*************************************************************************/
#ifndef _SMART_H
#define _SMART_H

#include <vector>
#include "smart_api.h"


// ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�
// Ӧ����� private�� ��ʹ��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

namespace zte_tecs {
    
    
/**
@brief ��������: ����SMART���
@li @b ����˵������
    */
    class DiskSmart
    {
        
    public:
        DiskSmart();
        
        ~DiskSmart();
        
        /**
        @brief ��������:�����Լ�
        
        @li @b ����б�
        @verbatim
        testType        �Լ�����
        @endverbatim
          
        @li @b �����б�
        @verbatim
        ��
        @endverbatim
            
        @li @b ����ֵ�б�
        @verbatim
        _SMART_SUCCESS           �ɹ�
        _SMART_ERROR             ʧ��
        @endverbatim
              
        @li @b �ӿ����ƣ���
        @li @b ����˵������
        */
        static int diskSmartTest(unsigned int testType);
        
        /**
        @brief ��������:��ȡ����SMART״̬
        
        @li @b ����б�
        @verbatim
        ��
        @endverbatim
          
        @li @b �����б�
        @verbatim
        diskSmartInfo        ����SMART��Ϣ
        @endverbatim
    
        @li @b ����ֵ�б�
        @verbatim
        _SMART_SUCCESS           �ɹ�
        _SMART_ERROR             ʧ��
        @endverbatim
              
        @li @b �ӿ����ƣ���
        @li @b ����˵������
        */
        static int getSmartInfo(std::vector<T_DmPdSmartInfo> &diskSmartInfo);
        
    private:
        DISALLOW_COPY_AND_ASSIGN(DiskSmart);
    };
}//namespace zte_tecs

#endif /*_SMART_H*/


