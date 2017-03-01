#ifndef LIGHT_COMMON_H
#define LIGHT_COMMON_H
#include "sky.h"

#define  LIGHT_SOCKET_PATH        "/tmp/tecs/light"
#define  LIGHT_AGENT_SOCKET_PATH  LIGHT_SOCKET_PATH"/agent"

#define  LIGHT_MANAGER_SOCKET  LIGHT_SOCKET_PATH"/manager.socket"

#define LIGHT_INFO_MIN_PRIORITY        ((int)(0))
#define LIGHT_INFO_MAX_PRIORITY        ((int)(100))

#define LI_MAX_SETTER_LEN    ((int)128)
#define LI_MAX_REMARK_LEN    ((int)256)
#define LI_MAX_SOCKFILE_LEN  ((int)256)

enum LightType
{
    LIGHT_TYPE_ACT,
    LIGHT_TYPE_HOST,
    LIGHT_TYPE_MAX
};

enum LightState
{
    LIGHT_NO_WORKING,
    LIGHT_WORKING,
    LIGHT_RUN_OK,
    LIGHT_NO_JOINED,
    LIGHT_NIC_ERR,
    LIGHT_DISK_ERR,
    LIGHT_RAM_ERR,
    LIGHT_CRITICAL_ERR,
    LIGHT_STATE_MAX
};

class LightInfo
{
public:
    bool compare(const LightInfo &info) const
    {
        if (!memcmp(_setter,info._setter,LI_MAX_SETTER_LEN-1)
            && (_type == info._type)
            && (_state == info._state)
            && (_priority == info._priority))
        {
            return true;
        }
        
        return false;
    }
    bool valid(void) const
    {
        if ((LIGHT_TYPE_MAX < _type)
            || (LIGHT_STATE_MAX < _state)
            || (LIGHT_INFO_MIN_PRIORITY > _priority)
            || (LIGHT_INFO_MAX_PRIORITY < _priority))
        {
            return false;
        }
        return true;
    }
    void copy(LightInfo &info) const
    {
        info._type     = _type;
        info._state    = _state;
        info._priority = _priority;
        strncpy(info._setter,_setter,LI_MAX_SETTER_LEN-1);
        strncpy(info._remark,_remark,LI_MAX_REMARK_LEN-1); 
    }
    void print(void) const
    {
        printf("LightInfo:type:%d,state:%d,priority:%d,setter:%s,remark:%s\r\n",
                _type,
                _state,
                _priority,
                _setter,
                _remark);
    }
    LightInfo(LightType type,LightState state,int pri,const char *setter,const char *remark)
    {
        _type = type;
        _state = state;
        _priority = pri;
        strncpy(_setter,setter,LI_MAX_SETTER_LEN-1);
        strncpy(_remark,remark,LI_MAX_REMARK_LEN-1);  
    }
    LightInfo()
    {
        _type = LIGHT_TYPE_MAX;
        _state = LIGHT_STATE_MAX;
        _priority = LIGHT_INFO_MIN_PRIORITY; 
        bzero(_setter,LI_MAX_SETTER_LEN);
        bzero(_remark,LI_MAX_REMARK_LEN);
    }

    ~LightInfo()
    {
    }
    /**
    @brief ��������: ������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: LIGHT_TYPE_MAX
    @li @b ����˵��: 
    */

    LightType _type;
    /**
    @brief ��������: ���״̬
    @li @b �Ƿ��û�����: ʱ
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: LIGHT_STATE_MAX
    @li @b ����˵��: 
    */

    LightState _state;
    /**
    @brief ��������: ���ȼ�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: LIGHT_INFO_MIN_PRIORITY
    @li @b ����˵��: 
    */

    int _priority;
    /**
    @brief ��������: ���ִ����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: NULL
    @li @b ����˵��: 
    */

    char _setter[LI_MAX_SETTER_LEN];
    /**
    @brief ��������: �������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: NULL
    @li @b ����˵��: 
    */

    char _remark[LI_MAX_REMARK_LEN];         
};
class LightProcess
{
public:
    /**
    @brief ��������: �����Ϣ�б�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: NULL
    @li @b ����˵��: 
    */
    list<LightInfo>_info_list;
    /**
    @brief ��������: ���һ�θ��µ����Ϣ�б�ʱ��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: NULL
    @li @b ����˵��: 
    */
    time_t _last_time;
    /**
    @brief ��������: uds�ļ�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: NULL
    @li @b ����˵��: 
    */
    string _socket_file;
};
#endif


