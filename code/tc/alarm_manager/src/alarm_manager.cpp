/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�alarm_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��AlarmManager��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��27��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/27
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
*******************************************************************************/
//#include <map>
//#include <vector>
//#include <set>
#include "alarm_manager.h"
#include "postgresql_db.h"
#include "registered_system.h"

using namespace xmlrpc_c;

namespace zte_tecs
{
AlarmManager* AlarmManager::instance = NULL;
/************************************************************
* �������ƣ� AlarmManager
* ���������� ���캯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� cap       ��ǰ�澯���ݿ�ָ��
*            hap       ��ʷ�澯���ݿ�ָ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
AlarmManager::AlarmManager()
{
    mu = NULL;
}

/************************************************************
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS AlarmManager::StartMu(const string& name)
{
    if(mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    mu = new MessageUnit(name);
    if (!mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    mu->Print();
    return SUCCESS;
}

/************************************************************
* �������ƣ� ~AlarmManager
* ���������� ��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
AlarmManager::~AlarmManager()
{
    if (mu)
        delete mu;
    OutPut(SYS_DEBUG, "delete alarm_manager\n");
}

/************************************************************
* �������ƣ� Receive
* ���������� ����������Ϣ�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmManager::Receive(const MessageFrame& message)
{
    return mu->Receive(message);
}

/************************************************************
* �������ƣ� JoinGroup
* ���������� ��ALARM_MANAGER������Ϣ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmManager::JoinGroup()
{
    //����һ���鲥��
    return mu->JoinMcGroup(ALARM_GROUP);
}

/************************************************************
* �������ƣ� BroadcastPoweronMsg
* ���������� �ϵ紦����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::BroadcastPoweronMsg()
{
    //���鲥�鷢���鲥��Ϣ
    DemoMessage msg("Alarm Manager Power on!");
    MID target("group",ALARM_GROUP);
    MessageOption option(target,EV_ALARM_BROADCAST,0,0);
    mu->Send(msg, option);
}

/************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� message  ��Ϣ��Ϣ
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::MessageEntry(const MessageFrame& message)
{
    switch (mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",mu->name().c_str());
            break;
        }
        default:
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_ALARM_REPORT:
        {
            AgentAlarm alarmmsg;
            alarmmsg.deserialize(message.data);

            AlarmMReceiveAlarm(alarmmsg);
            break;
        }
        case EV_INFORM_REPORT:
        {
            AgentAlarm informmsg;
            informmsg.deserialize(message.data);

            AlarmMAddRecentInform(informmsg);
            break;
        }

        case EV_ALARM_TIMER_SYNC:
        {
            AlarmMOmpSyncPP();
            break;
        }

        default:
            break;
        }
        break;
    }

    default:
        break;
    }
}

/************************************************************
* �������ƣ� start
* ���������� ��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
STATUS AlarmManager::Start()
{
    STATUS ret = ERROR;
    vector<xmlrpc_c::value> arrayCurrentAlarm;
    string strWhere = "1=1";

    TimeOut timeout;
    timeout.duration = ALARM_MAMANGER_MA_SYNC;
    timeout.msgid = EV_ALARM_TIMER_SYNC;
    timeout.type = LOOP_TIMER;

    TIMER_ID mytid = mu->CreateTimer();
    if (INVALID_TIMER_ID == mytid)
    {
        OutPut(SYS_ERROR, "Create timer for alarm manager ma sync failed!\n");
        return ERROR_NO_TIMER;
    }
    else
    {
        if (SUCCESS != mu->SetTimer(mytid,timeout))
        {
            OutPut(SYS_ERROR, "Set timer for alarm manager ma sync failed!\n");
            return ERROR_NO_TIMER;
        }
    }

    //�����鲥��
    ret = JoinGroup();
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return ret;
    }

    //������AGENT�����ϵ�㲥
    BroadcastPoweronMsg();

    //��ȡ��ǰaid
    _dwAlarmID = 0;
    if (_cap->Dump(arrayCurrentAlarm,strWhere) < 0)
    {
        OutPut(SYS_ERROR,"fail to operate current_alarm table\n");
    }
    else
    {
        WORD32 dwMaxTemp = 0;
        vector<xmlrpc_c::value>::iterator it_struct = arrayCurrentAlarm.begin();
        for (; it_struct != arrayCurrentAlarm.end(); ++it_struct)
        {
            map<string, xmlrpc_c::value> p = value_struct(*it_struct);
            map<string, xmlrpc_c::value>::iterator it;

            if (p.end() != (it = p.find("alarmid")))
            {
                if (xmlrpc_c::value_int(it->second) > (int)dwMaxTemp)
                    dwMaxTemp = xmlrpc_c::value_int(it->second);
            }
        }
        _dwAlarmID = dwMaxTemp;
    }
    return SUCCESS;
}

/************************************************************
* �������ƣ� alarmkey_to_str
* ���������� ���ݿ�ؼ��ֹ��캯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ptAlarmInfo   �澯��Ϣ�ṹ
* ��������� str  �ؼ����ַ�����Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
string& AlarmManager::alarmkey_to_str(string& str, const AgentAlarm &alarmmsg)
{
    ostringstream   os;

    os <<
    "ALARMADDRESS = "  << alarmmsg.tAlarmAddr << " / "<<
    "ALARMCODE = "  << alarmmsg.dwAlarmCode   << " / "<<
    "CRCCODE0 = "  << alarmmsg.dwCRCCode[0]  << " / " <<
    "CRCCODE1 = "  << alarmmsg.dwCRCCode[1]  << " / " <<
    "CRCCODE2 = "  << alarmmsg.dwCRCCode[2]  << " / " <<
    "CRCCODE3 = "  << alarmmsg.dwCRCCode[3]  << endl;

    str = os.str();

    return str;
}

/************************************************************
* �������ƣ� AlarmMReceiveAlarm
* ���������� �յ��ϱ��澯
* ���ʵı� ��
* �޸ĵı� ��
* ��������� &alarmmsg      �澯�ϱ���Ϣ
* ��������� ��
* �� �� ֵ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::AlarmMReceiveAlarm( AgentAlarm &alarmmsg)
{
    CurrentAlarm *ca = NULL;
    string str;
    string error_str;
    int64 oid = 0;

    if (_cap==NULL)
    {
        OutPut(SYS_ERROR,"_cap is NULL!\n");
        return;
    }

    if (_hap==NULL)
    {
        OutPut(SYS_ERROR,"_hap is NULL!\n");
    }

    //���ݸ澯�ؼ���Ϣ�������ݿ�ؼ�����Ϣ
    alarmkey_to_str(str,alarmmsg);

    if (alarmmsg.wAlarmFlag == OAM_REPORT_ALARM)
    {
        ca = _cap->Get(str,false);
        if (ca != NULL)
        {
            OutPut(SYS_DEBUG,"this alarm already exist!\n");
            return;
        }
        //�����µĸ澯���󣬲���澯��
        _dwAlarmID++;
        T_CurrentAlarm cur_alarm(_dwAlarmID,
                                 alarmmsg.dwAlarmCode,
                                 alarmmsg.tAlarmTime.tointeger(),
                                 alarmmsg.tAlarmAddr,
                                 alarmmsg.alarm_location.strSubSystem,
                                 alarmmsg.alarm_location.strLevel1,
                                 alarmmsg.alarm_location.strLevel2,
                                 alarmmsg.alarm_location.strLevel3,
                                 alarmmsg.alarm_location.strLocation,
                                 alarmmsg.aucMaxAddInfo,
                                 str,
                                 alarmmsg.dwCRCCode[0],
                                 alarmmsg.dwCRCCode[1],
                                 alarmmsg.dwCRCCode[2],
                                 alarmmsg.dwCRCCode[3]);
        if (_cap->Allocate(&oid, cur_alarm, error_str) < 0 )
        {
            OutPut(SYS_ERROR,"fail to operate current_alarm table,because of %s\n",error_str.c_str());
        }
        else
        {
            OutPut(SYS_DEBUG,"add a new alarm\n");
            //�ϱ���SNMP AGENT
            string str1 ;
            GetRegisteredSystem(str1);
    
            MID receiver(str1,PROC_APISVR,SNMP_AGENT);
            MessageOption option(receiver,EV_ALARM_REPORT,0,0);

            alarmmsg.dwAlarmID = _dwAlarmID;
            Datetime  time_temp((time_t)0);
            alarmmsg.tRestoreTime = time_temp;
            mu->Send(alarmmsg,option);
        }
    }
    else if (alarmmsg.wAlarmFlag == OAM_REPORT_RESTORE)
    {
        //�ڵ�ǰ�澯�ز����Ƿ���ڴ˸澯�������ڼ�����
        OutPut(SYS_DEBUG,"receive a restore alarm\n");
        ca = _cap->Get(str, true);
        if (ca == NULL)
        {
            OutPut(SYS_ERROR,"no such alarm\n");
            return;
        }
        //�����µ���ʷ�澯���󣬲�����ʷ�澯��
        T_CurrentAlarm cur_alarm = ca->get_current_alarm();
        HistoryAlarm historyalarm(cur_alarm.AlarmID,
                                 cur_alarm.AlarmCode,
                                 cur_alarm.AlarmTime,
                                 cur_alarm.AlarmAddr,
                                 cur_alarm.SubSystem,
                                 cur_alarm.Level1,
                                 cur_alarm.Level2,
                                 cur_alarm.Level3,
                                 cur_alarm.Location,
                                 cur_alarm.AddInfo,
                                 alarmmsg.tAlarmTime.tointeger(),
                                 alarmmsg.wRestoreType);

        OutPut(SYS_DEBUG,"drop alarm!\n");
        string str2 ;
        GetRegisteredSystem(str2);
    
        MID receiver(str2,PROC_APISVR,SNMP_AGENT);
        MessageOption option(receiver,EV_ALARM_REPORT,0,0);

        alarmmsg.dwAlarmID = _dwAlarmID;
        alarmmsg.tRestoreTime = alarmmsg.tAlarmTime;
        alarmmsg.tAlarmTime = cur_alarm.AlarmTime;
        mu->Send(alarmmsg,option);

        if (_cap->Drop(ca) < 0 )
            OutPut(SYS_ERROR,"fail to operate current_alarm table,because of %s\n",error_str.c_str());
        if (_hap->Allocate(historyalarm) != SUCCESS)
            OutPut(SYS_ERROR,"fail to operate history_alarm table.\n");
        else
            OutPut(SYS_DEBUG,"add a history alarm\n");

        ca->UnLock();
    }
    else
    {
        OutPut(SYS_ERROR,"wrong alarm flag!\n");
    }
}

/************************************************************
* �������ƣ� AlarmMOmpSyncPP()
* ���������� AlarmManager�����뵥��ĸ澯ͬ��
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::AlarmMOmpSyncPP()
{
    set<string> S_BoardAddr;
    vector<xmlrpc_c::value> arrayCurrentAlarm;

    if (_cap==NULL)
    {
        OutPut(SYS_ERROR,"_cap is NULL!\n");
        return;
    }

    if (_cap->Dump(arrayCurrentAlarm,"1=1") < 0)
    {
        OutPut(SYS_ERROR,"fail to operate current_alarm table\n");
    }
    else
    {
        vector<xmlrpc_c::value>::iterator it_struct = arrayCurrentAlarm.begin();
        for (; it_struct != arrayCurrentAlarm.end(); ++it_struct)
        {
            map<string, xmlrpc_c::value> p = value_struct(*it_struct);
            map<string, xmlrpc_c::value>::iterator it;

            if (p.end() != (it = p.find("alarmaddr")))
            {
                S_BoardAddr.insert(xmlrpc_c::value_string(it->second));
            }
        }
    }

    set<string>::iterator it_sBoardAddr = S_BoardAddr.begin();
    for (; it_sBoardAddr != S_BoardAddr.end(); ++it_sBoardAddr)
    {
        string BoardAddr = *it_sBoardAddr;
        AlarmMFindBrdAlarm(BoardAddr);
    }

}/* OamAlarmMOmpSyncPP */
/************************************************************
* �������ƣ� AlarmMFindBrdAlarm
* ���������� ����������
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::AlarmMFindBrdAlarm(const string &BoardAddr)
{
    AlarmSync as;
    ostringstream oss;
    vector<xmlrpc_c::value> arrayAlarm;

    if (_cap==NULL)
    {
        OutPut(SYS_ERROR,"_cap is NULL!\n");
        return;
    }

    string str = "location='" + BoardAddr +"'";
    if (_cap->Dump(arrayAlarm,str) < 0)
    {
        OutPut(SYS_ERROR,"fail to operate current_alarm table\n");
        return;
    }

    vector<xmlrpc_c::value>::iterator it_struct = arrayAlarm.begin();
    for (; it_struct != arrayAlarm.end(); ++it_struct)
    {
        map<string, xmlrpc_c::value> p = value_struct(*it_struct);
        map<string, xmlrpc_c::value>::iterator it;

        if (p.end() != (it = p.find("alarmcode")))
        {
            unsigned int alarmcode = xmlrpc_c::value_int(it->second);
            as.dwData.push_back(alarmcode);
        }
        if (p.end() != (it = p.find("crccode0")))
        {
            unsigned int crccode0 = xmlrpc_c::value_int(it->second);
            as.dwData.push_back(crccode0);
        }
        if (p.end() != (it = p.find("crccode1")))
        {
            unsigned int crccode1 = xmlrpc_c::value_int(it->second);
            as.dwData.push_back(crccode1);
        }
        if (p.end() != (it = p.find("crccode2")))
        {
            unsigned int crccode2 = xmlrpc_c::value_int(it->second);
            as.dwData.push_back(crccode2);
        }
        if (p.end() != (it = p.find("crccode3")))
        {
            unsigned int crccode3 = xmlrpc_c::value_int(it->second);
            as.dwData.push_back(crccode3);
        }
        as.wAlarmIDNum ++;
    }

    /* ���������� */
    if ( 0 != as.wAlarmIDNum)
    {
        as.tAlarmAddr = BoardAddr;
        as.serialize();

        /* ���ݵ�ַ��Ϣ����MID */
        int first =  BoardAddr.find_first_of("/");
        string application;
        application.append (BoardAddr , 0, first);
        first++;
        int second = BoardAddr.find_first_of("/",first);
        string process;
        process.append (BoardAddr , first , second -first);
        second++;
        string unit;
        unit.append (BoardAddr , second , BoardAddr.length());
        MID target(application,process,unit);

        MessageOption option(target,EV_ALARM_M2A_SYNC_REQ,0,0);
        mu->Send(as,option);

    }
}
/************************************************************
* �������ƣ� AlarmMAddRecentInform()
* ���������� �澯֪ͨ���洦��
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::AlarmMAddRecentInform(AgentAlarm &informmsg)
{
    T_AgtInform tInform;
    string str;
        
    tInform.dwAlarmCode = informmsg.dwAlarmCode;
    tInform.tAlarmTime  = informmsg.tAlarmTime;
    memcpy(tInform.tAlarmAddr,informmsg.tAlarmAddr.c_str(),informmsg.tAlarmAddr.length());
    memcpy(tInform.tAddInfo.aucMaxAddInfo,informmsg.aucMaxAddInfo.c_str(),informmsg.aucMaxAddInfo.length());
    
    _informpool.insert(_informpool.begin(),tInform);
    
    if (_informpool.size() > OAM_ALARM_RECENT_INFORM_MAX)
        _informpool.erase(_informpool.end());
    
    GetRegisteredSystem(str);
    
    MID receiver(str,PROC_APISVR,SNMP_AGENT);
    MessageOption option(receiver,EV_INFORM_REPORT,0,0);
            
    Datetime  time_temp((time_t)0);     
    informmsg.tRestoreTime = time_temp;
    mu->Send(informmsg,option);

    OAM_RETURN;
}/* OamAlarmMAddRecentInform */

/************************************************************
* �������ƣ� AlarmMShowRecentInform
* ���������� ֪ͨ��ѯ����
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void AlarmManager::AlarmMShowRecentInform(void)
{
    cout <<"------------------------Start---------------------"<<endl;

    vector<T_AgtInform>::iterator itpool = _informpool.begin();

    for (; itpool != _informpool.end(); ++itpool)
    {
        cout << "ACode : " << itpool->dwAlarmCode << endl;
        cout << "postion : " << itpool->tAlarmAddr << endl;
        cout << "AlarmTime : " << itpool->tAlarmTime.tostr() << endl;
        cout << "AddInfo : " << itpool->tAddInfo.aucMaxAddInfo << endl;
        cout<<endl;
    }

    cout <<"Inform Query Num = "<<_informpool.size()<<endl;
    cout <<"-------------------------End----------------------"<<endl;
}
/************************************************************
* �������ƣ� OamDbgAlarmMShowRecentInform
* ���������� ֪ͨ��ѯ����
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/10         V1.0       ����         ����
***************************************************************/
void OamDbgAlarmMShowRecentInform(void)
{

    AlarmManager *am = AlarmManager::GetInstance();

    if (NULL != am)
        am->AlarmMShowRecentInform();
    else
        cout<<"AlarmManager instance is NULL"<<endl;
}
DEFINE_DEBUG_FUNC(OamDbgAlarmMShowRecentInform);
}

