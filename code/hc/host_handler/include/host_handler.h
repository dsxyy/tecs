/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_handler.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostHandler�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HH_HOST_HANDLER_H
#define HH_HOST_HANDLER_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{
/**
@brief ��������: �����������ʵ����\n
@li @b ����˵��: ��
*/
class HostHandler : public MessageHandler
{
public:
    static HostHandler* GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new HostHandler(); 
        }
        return _instance;
    };

    STATUS Init()
    {
        return StartMu(MU_HOST_HANDLER);
    }
    
    virtual ~HostHandler() {}   // Ĭ����������
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    HostHandler();              // ���캯��
    STATUS StartMu(const string& name);

    static HostHandler* _instance;
    string      _cluster_name;  // �����������ļ�Ⱥ����
    MessageUnit *_mu;           // ��Ϣ��Ԫ

    void DoStartUp();

#if 0
    /**************************************************************************/
    /**
    @brief ��������: �������Լ�Ⱥ��ԭʼ��������

    @li @b ����б�
    @verbatim
        req     
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void DoRawHandle(const MessageFrame &message);
#endif

    /**************************************************************************/
    /**
    @brief ��������: �������Լ�Ⱥ��Ԥ�����������

    @li @b ����б�
    @verbatim
        req     
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void DoDefHandle(const MessageFrame &message);

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(HostHandler);   
};

} // namespace zte_tecs
#endif // #ifndef HH_HOST_HANDLER_H


