/*******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�engine.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������������
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2013��2��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/02/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "sky.h"
#include "pool_sql.h"
#include "workflow.h"

static int workflow_engine_print_on = 0;
DEFINE_DEBUG_VAR(workflow_engine_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(workflow_engine_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            else\
            {\
                Log(level,"[%s:%d]"fmt,__func__,__LINE__, ##arg);\
            }\
        }while(0)

class WorkflowEngine;

#define EV_ENGINE_POWER_ON      EV_TIMER_1 //�����������ϵ�����
#define EV_ACTION_TIMER      EV_TIMER_2 //������ת��ʱ��
#define EV_ENGINE_WAKEUP     EV_TIMER_3 //��������
#define WORKFLOW_ENGINE_CYCLE   5       //���湤������Ϊ5����
#define WORKFLOW_ENGINE_PREFIX "workflow_engine_"

static int number_of_engines = 0;
static int alternate_engine = 0;
static vector<WorkflowEngine*> engines;

typedef vector<pid_t> WorkflowTid;
static Mutex work_flow_mutex;
static map<string,WorkflowTid> work_flow_locks;

static void InitWorkFlowMutex()
{
	work_flow_mutex.SetName("work_flow_mutex");
	//work_flow_mutex.SetDebug(true);
	work_flow_mutex.Init();
}

static STATUS GetWorkflow(const string& action_id,string& workflow_id)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    if(true == Action::IsActionId(action_id))
    {
        workflow_id = p->GetWorkflowIdOfAction(action_id);
        if(workflow_id.empty())
        {
            return ERROR_INVALID_ARGUMENT;
        }
    }
    else
    {
        workflow_id = action_id;
    }

    return SUCCESS;
}

bool IsLockWorkflow(const string& workflow_id)
{
    MutexLock lock (work_flow_mutex);
    map<string, WorkflowTid>::iterator lockit = work_flow_locks.find(workflow_id);	
    if (work_flow_locks.end() == lockit)
    {
        Debug(LOG_DEBUG,"workflow %s is unlock!\n",workflow_id.c_str());
        return FALSE;
    }
    Debug(LOG_DEBUG,"workflow %s is lock!\n",workflow_id.c_str());
    return TRUE;
}

STATUS LockWorkflow(const string& action_id)
{
    MutexLock lock (work_flow_mutex);
    string workflow_id;

    STATUS ret = GetWorkflow(action_id,workflow_id);
    if (SUCCESS != ret)
    {
        Debug(LOG_ERR,"can't find work flow %s!\n",action_id.c_str());
        return ret;
    }

    pid_t tid = (pid_t)syscall(__NR_gettid);
    map<string, WorkflowTid>::iterator lockit = work_flow_locks.find(workflow_id);

    if (work_flow_locks.end() == lockit)
    { 
        WorkflowTid tids;
        tids.push_back(tid);
        work_flow_locks[workflow_id] = tids;
        Debug(LOG_DEBUG,"tid %d start to lock work flow %s!\n",tid,action_id.c_str());
        return SUCCESS;
    }

    vector<pid_t>::iterator iter = find(lockit->second.begin(),lockit->second.end(),tid);
    if (lockit->second.end() == iter)
    {
        Debug(LOG_DEBUG,"tid %d continue to lock work flow %s!\n",tid,action_id.c_str());
        lockit->second.push_back(tid);
        return SUCCESS;
    }
    Debug(LOG_DEBUG,"tid %d has already locked work flow %s!\n",tid,action_id.c_str());
    return SUCCESS;
}

STATUS UnlockWorkflow(const string& action_id)
{
    MutexLock lock (work_flow_mutex);
    string workflow_id;
    STATUS ret = GetWorkflow(action_id,workflow_id);
    if (SUCCESS != ret)
    {
        Debug(LOG_ERR,"can't find work flow %s!\n",action_id.c_str());
        return ret;
    }

    pid_t tid = (pid_t)syscall(__NR_gettid);

    map<string, WorkflowTid>::iterator lockit = work_flow_locks.find(workflow_id);

    if (work_flow_locks.end() == lockit)
    {
        Debug(LOG_DEBUG," work flow %s didn't be locked curently!\n",action_id.c_str());
        return SUCCESS;
    }

    vector<pid_t>::iterator iter = lockit->second.begin();
    while( iter != lockit->second.end())
    {
        if (*iter == tid)
        {
            iter = lockit->second.erase( iter );
            Debug(LOG_DEBUG,"tid %d unlock work flow %s!\n",tid,action_id.c_str());
            if (lockit->second.empty())
            {
                Debug(LOG_DEBUG,"work flow %s lock is released completely!\n",action_id.c_str());
                work_flow_locks.erase(workflow_id);
            }
            return SUCCESS;
        }
        else
        {
            ++iter;
        }
    }
    Debug(LOG_ERR,"tid %d did't lock work flow %s curently!\n",tid,action_id.c_str());
    return SUCCESS;
}

void DbgLockWorkflow(const char* action_id)
{
    printf("lock:%s\n",action_id);
    LockWorkflow(action_id);
}
DEFINE_DEBUG_FUNC(DbgLockWorkflow);


void DbgUnlockWorkflow(const char* action_id)
{
    printf("un lock:%s\n",action_id);
    UnlockWorkflow(action_id);
}

DEFINE_DEBUG_FUNC(DbgUnlockWorkflow);

void DbgShowWorkflowLocks(int lock_flag)
{
    if (lock_flag)
    MutexLock lock (work_flow_mutex);
    map<string,WorkflowTid>::iterator lockit;
    for (lockit=work_flow_locks.begin() ; lockit != work_flow_locks.end(); lockit++)
    {
        printf("work flow %s is locked by thread below:\n",lockit->first.c_str());

        vector<pid_t>::iterator it;
        for (it=lockit->second.begin() ; it != lockit->second.end(); it++)
        {
            printf(" %ld\n",*it);
        }
    }
}

DEFINE_DEBUG_FUNC(DbgShowWorkflowLocks);

int GetRepeatCount(int seconds)
{
    if (0 < seconds)
        return seconds/WORKFLOW_ENGINE_CYCLE + 1;
    else
        return -1;
};

class WorkflowEngine: public MessageHandlerTpl<MessageUnit>
{
public:
    enum EngineRunMode
    {
        ENGINE_CYCLE = 1,   //����������ִ������
        ENGINE_WAKEUP = 2,   //���汻��ʱ����ִ������
        ENGINE_ACK = 3,   //������յ��ⲿӦ��֮��������
    };

    enum ActionState
    {
        ACTION_READY = 1,
        ACTION_RUNNING = 2,
        ACTION_TIMEOUT = 3,
        ACTION_SUCCESS = 4,
        ACTION_FAILED = 5,
        ACTION_PAUSED = 6,
        ACTION_FINISHED = 7
    };

    WorkflowEngine(int index,SqlDB *db)
    {
        _engine_index = index;
        _db = db;
        _application = ApplicationName();
        _process = ProcessName();
        _action_pool = ActionPool::GetInstance();
        _idle_count = 0;

        ostringstream oss;
        oss << "application = '" << _application << "' AND process = '" << _process << "'";
        if (IsLastEngine(_engine_index))
        {
            //���һ�����渺����ʣ�����������
            oss << " AND engine >= " << _engine_index;
        }
        else
        {
            oss << " AND engine = " << _engine_index;
        }
        _engine_where = oss.str();
    }

    const string& get_name() const
    {
        return m->name();
    }

    void PowerOn()
    {
        MessageOption option(m->name(),EV_ENGINE_POWER_ON,0,0);
        MessageFrame frame("PowerOn!",option);
        m->Receive(frame);
    }

    //�������洦��ĳ��������
    STATUS Wakeup(const string& workflow_id="")
    {
        uint32 message_id = 0;
        if(workflow_id.empty())
        {
            message_id = EV_ACTION_TIMER;
        }
        else
        {
            message_id = EV_ENGINE_WAKEUP;
        }
        MessageOption option(m->name(),message_id,0,0);
        MessageFrame frame(workflow_id,option);
        return m->Receive(frame);
    }

    void Show()
    {
        printf("========workflow engine %d========\n",_engine_index);
        printf("application: %s\n",_application.c_str());
        printf("process: %s\n",_process.c_str());
        printf("work timer id: %d\n",_work_timer);
        printf("idle count: %d\n",_idle_count);
        printf("\n");
    }

    void MessageEntry(const MessageFrame& message)
    {
        STATUS ret = ERROR;
        //�����յ�����Ϣid���д���
        switch (message.option.id())
        {
            case EV_ENGINE_POWER_ON:
            {
                ret = Transaction::Enable(_db);
                if (SUCCESS != ret)
                {
                    SkyAssert(false);
                    SkyExit(-1,"Transaction::Enable failed!\n");
                }

                //��������ѭ����ʱ��
                _work_timer = m->CreateTimer();
                TimeOut timeout;
                timeout.duration = 1000 * WORKFLOW_ENGINE_CYCLE;
                timeout.msgid = EV_ACTION_TIMER;
                timeout.type = LOOP_TIMER;
                m->SetTimer(_work_timer,timeout);

                //ɾ�������渺����Ѿ���ɵĹ�����
                _action_pool->DeleteFinishedWorkflow(_engine_where);

                //�������ݿ�
                if(_engine_index == 0)
                {
                    _action_pool->Vacuum();
                }
                
                //���淢���ɹ����ϵ����
                m->set_state(S_Work);

                //�ϵ�֮�����������һ��
                Run();
                break;
            }

            case EV_ENGINE_WAKEUP:
            {

                //ɾ�������渺����Ѿ���ɵĹ�����
                //_action_pool->DeleteFinishedWorkflow(_engine_where);
                 
                //EV_ENGINE_WAKEUP��ϢЯ����workflow_idһ�㶼��Ϊ��
                string workflow_id;
                    workflow_id = message.data;
                
                Run(workflow_id);

                T_TimeValue remained;
                if(SUCCESS != m->GetRemainedTime(_work_timer,&remained))
                {
                    Debug(LOG_WARNING,"Get work timer for workflow engine fail!\n");
                    break;
                }
                if(remained.second > 5)
                {
                    TimeOut timeout;
                    timeout.msgid = EV_ACTION_TIMER;
                    timeout.type = LOOP_TIMER;
                    timeout.duration = 1000 * 5;
                    m->SetTimer(_work_timer,timeout);
                }
                break;
                }
            case EV_ACTION_TIMER:
            {
                
                TimeOut timeout;
                timeout.msgid = EV_ACTION_TIMER;
                timeout.type = LOOP_TIMER;
                //ɾ�������渺����Ѿ���ɵĹ�����
                //_action_pool->DeleteFinishedWorkflow(_engine_where);                 
                //ע��: EV_ACTION_TIMER��ϢЯ����workflow_id�϶�Ϊ��
                //EV_ENGINE_WAKEUP��ϢЯ����workflow_idһ�㶼��Ϊ�� 
 
                int count = Run();
                if (0 == count)
                {
                    //0���������ر�֮��: ���¿�����ʱ��������һ�¹�������ر�
                    if(_engine_index == 0)
                    {
                        _action_pool->Vacuum();
                    }

                    _idle_count ++;
                    
                    //�������ֻ���ڷ�����ת��10��֮�󣬳��Խ�ɨ�������ӳ�
                    if (_idle_count >= 10)
                    {
                        _idle_count = 0;
                        Debug(LOG_WARNING,"Slow work timer for workflow engine %d!\n",
                            _engine_index);
                        timeout.duration = 1000 * 60;
                        m->SetTimer(_work_timer,timeout);
                        //û�����ͽ�ѭ����ʱ�����ڷų�һ�㣬��Ϊ1����ɨ��һ��
                    }
                }
                else
                {
                    _idle_count = 0;
                    //������������һ��ѭ����ʱ������Ϊ5����ɨ�����һ��
                    timeout.duration = 1000 * WORKFLOW_ENGINE_CYCLE;
                    m->SetTimer(_work_timer,timeout);
                }
                break;
            }

            case EV_RESUME_WORKFLOW:
            {
                string workflow_id = message.data;
                Debug(LOG_NOTICE,"workflow engine %d will resume workflow: %s ...\n",
                    _engine_index,workflow_id.c_str());
                ret = _action_pool->SetWorkflowPauseFlag(workflow_id,false);
                //�������濪ʼ����ù�����
                Wakeup(workflow_id);
                MessageOption option(message.option.sender(), EV_RESUME_WORKFLOW_ACK,0,0);
                m->Send(SkyInt(ret),option);
                break;
            }

            case EV_PAUSE_WORKFLOW:
            {
                string workflow_id = message.data;
                Debug(LOG_NOTICE,"workflow engine %d will pause workflow: %s ...\n",
                    _engine_index,workflow_id.c_str());
                ret = _action_pool->SetWorkflowPauseFlag(workflow_id,true);
                MessageOption option(message.option.sender(), EV_PAUSE_WORKFLOW_ACK,0,0);
                m->Send(SkyInt(ret),option);
                Wakeup();
                break;
            }

            case EV_DELETE_WORKFLOW:
            {
                string workflow_id = message.data;
                Debug(LOG_NOTICE,"workflow engine %d will delete workflow: %s ...\n",
                    _engine_index,workflow_id.c_str());
                ret = _action_pool->DeleteWorkflow(workflow_id,true);
                MessageOption option(message.option.sender(), EV_DELETE_WORKFLOW_ACK,0,0);
                m->Send(SkyInt(ret),option);
                Wakeup();
                break;
            }
            
            case EV_ROLLBACK_WORKFLOW:
            {
                string workflow_id = message.data;
                Debug(LOG_NOTICE,"workflow engine %d will rollback workflow: %s ...\n",
                    _engine_index,workflow_id.c_str());
                ret = Rollback(workflow_id);
                MessageOption option(message.option.sender(), EV_ROLLBACK_WORKFLOW_ACK,0,0);
                m->Send(SkyInt(ret),option);
                Wakeup();
                break;
            }

            case EV_QUERY_WORKFLOW:
            {
                string workflow_id = message.data;
                Debug(LOG_NOTICE,"workflow engine %d will query workflow: %s ...\n",
                    _engine_index,workflow_id.c_str());

                MessageOption option(message.option.sender(), EV_QUERY_WORKFLOW_ACK,0,0);
                WorkflowAck query_info(workflow_id);
                
                /*��ѯ��ɺ����ڹ�������״̬���Զ���״̬��
                             ����action��״̬����Ҫ���м򻯴���*/
                int progress = _action_pool->GetWorkflowProgress(workflow_id);
                if(-1 != progress)
                {
                    query_info.progress = progress;
                    query_info.state = WorkflowAck::WORKFLOW_RUNNING;
                    m->Send(query_info,option);
                    break;
                }
                WorkflowHistory history;
                ret = _action_pool->GetWorkflowHistory(workflow_id,history);
                if(SUCCESS != ret)
                {
                    query_info.state = WorkflowAck::WORKFLOW_UNKNOWN;
                    query_info.progress = 0;
                    m->Send(query_info,option);
                    break;
                }

                query_info.state = WorkflowAck::WORKFLOW_FAILED;
                if (SUCCESS == history.state)
                    query_info.state = WorkflowAck::WORKFLOW_SUCCESS;                
                else if (ERROR_ACTION_CANCELLED == history.state)
                    query_info.state = WorkflowAck::WORKFLOW_CANCELED;                
                
                query_info.progress = history.progress;
                m->Send(query_info,option);
                break;
            }

            default:
            {
                WorkAck ack;
                if (false == ack.deserialize(message.data))
                {
                    SkyAssert(false);
                    return;
                }

                if(!ack.Validate())
                {
                    Debug(LOG_ERR,"Invalid action ack: %s from %s! state = %d, progress = %d\n",
                                        ack.action_id.c_str(),
                                        message.option.sender().serialize().c_str(),
                                        ack.state,
                                        ack.progress);
                    return;
                }
                
                //Debug("workflow engine %d receives action ack: %s!\n",_index,ack.action_id.c_str());
                if ((ack.progress != 100 && ack.state == SUCCESS) || (ack.progress == 100 && ack.state != SUCCESS))
                {
                    //�Ѿ��ɹ��ˣ�ȴ��û��100%?
                    //�Ѿ�100%�ˣ���˵û�ɹ�?
                    Debug(LOG_CRIT,"Wrong state or progress of action: %s from %s!state = %d, progress = %d\n",
                                        ack.action_id.c_str(),
                                        message.option.sender().serialize().c_str(),
                                        ack.state,
                                        ack.progress);
                    SkyAssert(false);
                    return;
                }

                //����Ƿ�Ϊworking actions?
                Action action(ack.action_id);
                ret = _action_pool->GetRunningAction(_engine_where,action);
                if (SUCCESS != ret)
                {
                    Debug(LOG_WARNING,"No running action in pool: %s!\n",ack.action_id.c_str());
                    return;
                }

                ActionState state = GetActionState(action);
                if (ACTION_RUNNING != state && ACTION_READY != state)
                {
                    //���֮ǰ�Ѿ��н���ˣ����ڸ����е�״̬���Ͳ�����Ӧ��
                    Debug(LOG_WARNING,"Too late ack for action: %s!\n",ack.action_id.c_str());
                    return;
                }

                action.message_ack_id = message.option.id();
                action.message_ack_priority = message.option.priority();
                action.message_ack_version = message.option.version();
                action.progress = ack.progress;
                action.message_ack = message.data;
                action.state = ack.state;
                
                //���action��Ҫ���ԣ������ʧ��״̬����Ȼ��Ϊ��running
                if(SUCCESS != action.state && ERROR_ACTION_RUNNING != action.state)
                {
                    WorkReq req;
                    req.deserialize(action.message_req);
                    if(req.retry)
                    {
                        Debug(LOG_ERR,"action %s(%s) failed! retry ...\n",
                            action.name.c_str(),ack.action_id.c_str());
                        action.state = ERROR_ACTION_RUNNING;
                        SkyAssert(action.progress < 100);
                    }
                }
                
                _action_pool->UpdateActionState(action);
                Debug(LOG_INFO,"action %s(%s) has progress: %d%%\n",
                      ack.action_id.c_str(),action.name.c_str(),ack.progress);
                //�ȸ���һ��Action���ٴ�����
                DoAction(action,ENGINE_ACK);
                break;
            }
        }
    };

private:
    bool IsLastEngine(int index)
    {
        if (index == number_of_engines - 1)
            return true;
        else
            return false;
    }

    ActionState GetActionState(const Action& action)
    {
        //��ȷ�����
        if (action.workflow_paused)
        {
            return ACTION_PAUSED;
        }

        //��ȷ�����
        if (action.finished)
        {
            return ACTION_FINISHED;
        }

        //�ѳɹ�
        if (action.progress == 100)
        {
            SkyAssert(SUCCESS == action.state);
            return ACTION_SUCCESS;
        }

        if(action.synchronous == false && 
            action.state != ERROR_ACTION_READY)
        {
            //Ҫ���첽�����Ѿ��г������ȣ�����������н�����
            if(SUCCESS == action.state || 
               ERROR_ACTION_RUNNING == action.state)
            {
                //���ֳ���Ҫ��ִ�з�ֻҪ�Ϸ��Լ��ͨ����
                //�ͱ��뱣֤�������ɹ�����������ʧ��
                return ACTION_SUCCESS;
            }
            else
            {
                //Ҫ���첽��ȴ��Ȼ����ʧ�ܣ�һ���ǷǷ���������
                //������ͼ�ͷ�����ʹ���е���Դ
                return ACTION_FAILED;
            }
        }
        
        if (action.state == ERROR_ACTION_TIMEOUT)
        {
            SkyAssert(100 > action.progress);
            return ACTION_TIMEOUT;
        }

        //��ʧ��
        if (action.state != SUCCESS && 
            action.state != ERROR_ACTION_READY && 
            action.state != ERROR_ACTION_RUNNING)
        {
            SkyAssert(100 > action.progress);
            return ACTION_FAILED;
        }

        //�Ѿ���ʱ��,������δ���µ����ݿ�
        if (action.timeout > 0 &&
                action.history_repeat_count >= GetRepeatCount(action.timeout))
        {
            return ACTION_TIMEOUT;
        }

        //�ո�׼���ã���δ��ʼ����
        if (action.state == ERROR_ACTION_READY)
        {
            return ACTION_READY;
        }
        
        return ACTION_RUNNING;
    }

    //��������
    int Run(const string& workflow_id = "")
    {
        //Debug("======================engine %d is running======================\n",_index);
        //����ֻ����ָ���Ĺ�����
        ostringstream oss;
        EngineRunMode runmode;
        oss << _engine_where;
        if (!workflow_id.empty())
        {
            runmode = ENGINE_WAKEUP;
            oss << " AND workflow_id = '" << workflow_id << "'";
        }
        else
        {
            runmode = ENGINE_CYCLE;
        }

        vector<string> running_actions;
        STATUS ret = _action_pool->GetRunningActions(oss.str(),running_actions);
        if (SUCCESS != ret)
        {
            Debug(LOG_ERR,"workflow engine %d failed to get running actions!\n",_engine_index);
            return -1;
        }

        vector<string>::iterator it;
        for (it = running_actions.begin(); it != running_actions.end(); ++it)
        {
            Action action(*it);
            if (SUCCESS == _action_pool->GetAction(action))
            {
                //���ݿ���ͼ�ͱ����֮���л���???
                if(action.workflow_paused != true && action.finished != true)
                {
                    DoAction(action,runmode);
                }
            }
            else
            {
                Debug(LOG_ERR,"workflow engine %d failed to get action: %s!\n",
                    _engine_index,action.action_id.c_str());
            }
        }
        return running_actions.size();
    }

    //����action
    STATUS DoAction(Action& action,EngineRunMode runmode)
    {
        STATUS ret = ERROR;
        ActionState action_state = GetActionState(action);

        //�Ѿ���ͣ������ɵ�Action�����ϲ������ߵ�������̣��Ѿ�����ͼ������
        SkyAssert(ACTION_PAUSED != action_state && ACTION_FINISHED != action_state);

        if (ENGINE_WAKEUP == runmode && ACTION_READY != action_state)
        {
            //����Ǳ����ѣ�ֻ����ո�׼���ã���δ���߹���Action������������
            //Debug("workflow engine %d is wakeup, only process ready action!\n",_index);
            return SUCCESS;
        }

        if (ENGINE_ACK == runmode && (ACTION_READY == action_state || ACTION_RUNNING == action_state))
        {
            //������յ�Ack����Ҫ���Ӹ�׼���õ�Action����ֹ��ʱʱ�侫������
            //�д�������״̬��ActionҲ���ø����ˣ���Ϊ�ڽ��뱾����֮ǰ�Ѿ�������Ӧ������
            //Debug("workflow engine %d is passive, will ignore ready action!\n",_index);
            return SUCCESS;
        }

        //Debug("workflow engine %d is running, engine state: %d, action state: %d!\n",_index,engine_state,action_state);
        switch (action_state)
        {
            case ACTION_READY:
            {
                if (action.skipped)
                {
                    //�Ѿ����ó�skipped��actionֱ���Զ�����
                    Debug(LOG_NOTICE,"action %s(%s) is skipped, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    //ע������Ҫ����FinishAction��������_action_pool->FinishAction
                    //��Ϊ�п����Ѿ������һ��Action�ˣ�����֮����ܻ�Ҫɾ������������
                    FinishAction(action.action_id);
                    //����֮���������»������棬��Ϊ��Action�����к�������Ҫ����
                    Wakeup(action.workflow_id);
                }
                else
                {
                    if(!action.rollback_action_id.empty())
                    {
                        //��action����skipped����ʵ��Ҫ������Ҫ�����ߣ�����rollback_action�Ͳ���������
                        ret = _action_pool->SetSkippedFlag(action.rollback_action_id,false);
                        if(SUCCESS != ret)
                        {
                            SkyAssert(false);
                            return ERROR_DB_UPDATE_FAIL;
                        }
                         Debug(LOG_NOTICE,"rollback action %s(of %s) will not be skipped!\n",
                              action.rollback_action_id.c_str(),action.name.c_str());
                    }
                
                    //�ո�׼���ã��Ϳ�ʼ����
                    Debug(LOG_INFO,"workflow engine %d is sending message for ready action %s(%s)...\n",
                          _engine_index,action.action_id.c_str(),action.name.c_str());
                    //���ȸ��¸��ߴ��������һ�θ��ߵ�ʱ��
                    _action_pool->RepeatAction(action);
                    //Ȼ���͸�����Ϣ
                    MessageOption option(action.receiver,
                                         action.message_id,action.message_priority,action.message_version);
                    m->Send(action.message_req,option);
                }
                break;
            }

            case ACTION_RUNNING:
            {
                if (action.skipped)
                {
                    //�Ѿ����ó�skipped��actionֱ���Զ�����
                    Debug(LOG_NOTICE,"action %s(%s) is skipped, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    //ע������Ҫ����FinishAction��������_action_pool->FinishAction
                    //��Ϊ�п����Ѿ������һ��Action�ˣ�����֮����ܻ�Ҫɾ������������
                    FinishAction(action.action_id);
                    //����֮���������»������棬��Ϊ��Action�����к�������Ҫ����
                    Wakeup(action.workflow_id);

                }
                else
                {
                    //�Ѿ������У��ͼ�������
                    Debug(LOG_INFO,"workflow engine %d is sending message for running action %s(%s)...\n",
                          _engine_index,action.action_id.c_str(),action.name.c_str());
                    //���ȸ��¸��ߴ��������һ�θ��ߵ�ʱ��
                    _action_pool->RepeatAction(action);
                    //Ȼ���͸�����Ϣ
                    MessageOption option(action.receiver,
                                         action.message_id,action.message_priority,action.message_version);
                    m->Send(action.message_req,option);
                }
                break;
            }

            case ACTION_SUCCESS:
            {
                //�����Ѿ���ɣ������ǳɹ���
                if (action.success_feedback != NO_FEEDBACK)
                {
                    //�����Ҫ�����ͷ�����Ϣ��̯���������Ϣ��Ԫ
                    Debug(LOG_NOTICE,"action %s(%s) successed, feedback!\n",
                          action.action_id.c_str(),action.name.c_str());
                    MessageOption option(action.sender,
                                         action.message_ack_id,action.message_ack_priority,action.message_ack_version);
                    m->Send(action.message_ack,option);
                }
                else
                {
                    //�������Ҫ�����ͽ���������
                    Debug(LOG_NOTICE,"action %s(%s) successed, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    FinishAction(action.action_id);
                    //����֮���������»������棬��Ϊ�������������к�������Ҫ����
                    Wakeup(action.workflow_id);
                }
                break;
            }

            case ACTION_FAILED:
            {
                //�Ѿ���ȷʧ�ܣ�ҲҪ�����Ƿ���
                if (action.failure_feedback)
                {
                    //�����Ҫ�����ͷ�����Ϣ��̯���������Ϣ��Ԫ
                    Debug(LOG_WARNING,"action %s(%s) failed, feedback!\n",
                          action.action_id.c_str(),action.name.c_str());
                    MessageOption option(action.sender,
                                         action.message_ack_id,action.message_ack_priority,action.message_ack_version);
                    m->Send(action.message_ack,option);
                }
                else
                {
                    //�������Ҫ�����ͽ���������
                    Debug(LOG_WARNING,"action %s(%s) failed, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    FinishAction(action.action_id);
                    //����֮���������»������棬��Ϊ�������������к�������Ҫ����
                    Wakeup(action.workflow_id);
                }
                break;
            }

            case ACTION_TIMEOUT:
            {
                //������ߴ����������Ͳ����ٸ����ˣ�
                if (action.state != ERROR_ACTION_TIMEOUT)
                {
                    action.state = ERROR_ACTION_TIMEOUT;
                    _action_pool->UpdateActionState(action);
                }

                if (action.timeout_feedback)
                {
                    //ֱ�ӷ����������ɷ���
                    Debug(LOG_WARNING,"action %s(%s) timeout, feedback!\n",
                          action.action_id.c_str(),action.name.c_str());
                    WorkAck ack(action.action_id,action.state,action.progress);
                    MessageOption option(action.sender,EV_ACTION_TIMEOUT,0,0);
                    m->Send(ack,option);
                }
                else
                {
                    //�������Ҫ�����ͽ���������
                    Debug(LOG_WARNING,"action %s(%s) timeout, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    FinishAction(action.action_id);
                    //����֮���������»������棬��Ϊ�����к�������Ҫ����
                    Wakeup(action.workflow_id);
                }
                break;
            }

            default:
                SkyAssert(false);
                break;
        }

        return SUCCESS;
    }

    STATUS Rollback(const string& workflow_id)
    {
        if(workflow_id.empty())
        {
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
        }

        string rollback_workflow_id = _action_pool->GetRollbackWorkflow(workflow_id);
        if(rollback_workflow_id.empty())
        {
            //û��Ԥ�����ûع��ƻ���û���ع�
            return ERROR_NOT_SUPPORT;
        }
        
        Transaction::Begin();
        _action_pool->DeleteWorkflow(workflow_id,false);
        _action_pool->SetWorkflowPauseFlag(rollback_workflow_id,false);
        STATUS ret = Transaction::Commit();
        if(SUCCESS != ret)
        {
            return ret;
        }

        //�������洦��ع�������
        //ע��ع���������ԭ�ȵ��������������Ƿ���ͬһ�������¸������!
        return Wakeup(rollback_workflow_id);
    }
    
    int _engine_index;
    string _application;
    string _process;
    int _idle_count;
    string _engine_where;
    TIMER_ID _work_timer;
    ActionPool *_action_pool;
    SqlDB *_db;
};

static STATUS GetWorkflowAndEngine(const string& action_id,string& workflow_id,int& engine_index)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    if(true == Action::IsActionId(action_id))
    {
        workflow_id = p->GetWorkflowIdOfAction(action_id);
        if(workflow_id.empty())
        {
            return ERROR_INVALID_ARGUMENT;
        }
    }
    else
    {
        workflow_id = action_id;
    }

    engine_index = p->GetEngineOfWorkflow(workflow_id);
    if (0 > engine_index)
    {
        if(-2 == engine_index)
        {
            return ERROR_OBJECT_NOT_EXIST;
        }
        else
        {
            return ERROR;
        }    
    }

    if (engine_index > number_of_engines - 1)
    {
        engine_index = number_of_engines - 1;
    }

    return SUCCESS;
}

int GetAvailableEngine()
{
    //���������������
    return (alternate_engine++) % number_of_engines;
}

STATUS StartWorkflowEngine(SqlDB *db,int count)
{
    STATUS ret = ERROR;
    if (count <= 0)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    ActionPool *p = ActionPool::CreateInstance(db);
    p->Init();

    InitWorkFlowMutex();

    number_of_engines = count;
    int i = 0;
    for (i = 0; i < number_of_engines; i++)
    {
        WorkflowEngine* engine = new WorkflowEngine(i,db);
        ostringstream oss;
        oss << WORKFLOW_ENGINE_PREFIX << i;
        engines.push_back(engine);
        ret = engine->Start(oss.str());
        if (SUCCESS != ret)
        {
            printf("failed to start workflow engine %d! ret = %d\n",i,ret);
            return ret;
        }
        engine->PowerOn();
        AddKeyMu(oss.str());
    }
    return SUCCESS;
}

STATUS WakeupEngine(int engine_index,const string& workflow_id)
{
    if (engine_index < 0 || engine_index > number_of_engines)
    {
        printf("Invalid workflow engine index: %d!\n",engine_index);
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    Debug(LOG_NOTICE,"Wakeup workflow engine %d ...\n",engine_index);
    return engines[engine_index]->Wakeup(workflow_id);
}

STATUS CreateWorkflow(Action& action)
{
    if (number_of_engines <= 0)
    {
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->CreateWorkflow(action);
}

STATUS CreateWorkflow(Workflow& workflow)
{
    if (number_of_engines <= 0)
    {
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->CreateWorkflow(workflow);
}

STATUS CreateWorkflow(Workflow& workflow,Workflow& rollback_workflow)
{
    if (number_of_engines <= 0)
    {
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->CreateWorkflow(workflow,rollback_workflow);
}

STATUS PauseWorkflow(const string& action_id)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    string workflow_id;
    int engine_index = -1;
    STATUS ret = GetWorkflowAndEngine(action_id,workflow_id,engine_index);
    if(SUCCESS != ret)
    {
        return ret;
    }
    
    SkyAssert(!workflow_id.empty() && engine_index != -1);
    Debug(LOG_NOTICE,"Pause workflow %s on engine %d ...\n",workflow_id.c_str(),engine_index);
    
    MessageUnit tempmu(TempUnitName("PauseWorkflow"));
    tempmu.Register();
    MessageOption option(engines[engine_index]->get_name(),EV_PAUSE_WORKFLOW,0,0);
    tempmu.Send(workflow_id,option);

    MessageFrame message;
    ret = tempmu.Wait(&message,3000);
    if (SUCCESS == ret && message.option.id() == EV_PAUSE_WORKFLOW_ACK)
    {
        //�յ�Ӧ����
        SkyInt ack(message.data);
        return ack.value;
    }
    return ret;
}

STATUS ResumeWorkflow(const string& action_id)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    string workflow_id;
    int engine_index = -1;
    STATUS ret = GetWorkflowAndEngine(action_id,workflow_id,engine_index);
    if(SUCCESS != ret)
    {
        return ret;
    }

    SkyAssert(!workflow_id.empty() && engine_index != -1);
    Debug(LOG_NOTICE,"Resume workflow %s on engine %d ...\n",workflow_id.c_str(),engine_index);
    
    MessageUnit tempmu(TempUnitName("ResumeWorkflow"));
    tempmu.Register();
    MessageOption option(engines[engine_index]->get_name(),EV_RESUME_WORKFLOW,0,0);
    tempmu.Send(workflow_id,option);

    MessageFrame message;
    ret = tempmu.Wait(&message,3000);
    if (SUCCESS == ret && message.option.id() == EV_RESUME_WORKFLOW_ACK)
    {
        //�յ�Ӧ����
        SkyInt ack(message.data);
        return ack.value;
    }
    return ret;
}

STATUS DeleteWorkflow(const string& action_id)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    MutexLock lock (work_flow_mutex);
    string workflow_id;
    int engine_index = -1;
    STATUS ret = GetWorkflowAndEngine(action_id,workflow_id,engine_index);
    if(SUCCESS != ret)
    {
        return ret;
    }

    if (IsLockWorkflow(workflow_id))
        return ERROR_OP_RUNNING;

    SkyAssert(!workflow_id.empty() && engine_index != -1);
    Debug(LOG_NOTICE,"Delete workflow %s on engine %d ...\n",workflow_id.c_str(),engine_index);
    MessageUnit tempmu(TempUnitName("DeleteWorkflow"));
    tempmu.Register();
    MessageOption option(engines[engine_index]->get_name(),EV_DELETE_WORKFLOW,0,0);
    tempmu.Send(workflow_id,option);

    //���û����õĽӿڣ�������ͬԤ��׼���õĻع�������һ��ɾ��
    MessageFrame message;
    ret = tempmu.Wait(&message,3000);
    if (SUCCESS == ret && message.option.id() == EV_DELETE_WORKFLOW_ACK)
    {
        //�յ�Ӧ����
        SkyInt ack(message.data);
        return ack.value;
    }
    return ret;
}

STATUS RollbackWorkflow(const string& action_id)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    MutexLock lock (work_flow_mutex);
    string workflow_id;
    int engine_index = -1;
    STATUS ret = GetWorkflowAndEngine(action_id,workflow_id,engine_index);
    if(SUCCESS != ret)
    {
        return ret;
    }

    if (IsLockWorkflow(workflow_id))
        return ERROR_OP_RUNNING;

    string rollback_workflow_id = p->GetRollbackWorkflow(workflow_id);
    if(rollback_workflow_id.empty())
    {
        //û�лع����������ã���֧�ֻع�
        return ERROR_NOT_SUPPORT;
    }

    SkyAssert(!rollback_workflow_id.empty() && !workflow_id.empty() && engine_index != -1);
    Debug(LOG_NOTICE,"Rollback workflow %s on engine %d ...\n",workflow_id.c_str(),engine_index);
    
    MessageUnit tempmu(TempUnitName("RollbackWorkflow"));
    tempmu.Register();
    MessageOption option(engines[engine_index]->get_name(),EV_ROLLBACK_WORKFLOW,0,0);
    tempmu.Send(workflow_id,option);

    MessageFrame message;
    ret = tempmu.Wait(&message,3000);
    if (SUCCESS == ret && message.option.id() == EV_ROLLBACK_WORKFLOW_ACK)
    {
        //�յ�Ӧ����
        SkyInt ack(message.data);
        return ack.value;
    }
    return ret;
}

void GetWorkflowEngine(const string &workflow_id,string &workflow_engine)//zyb
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    if (workflow_id.empty())
    {
        Debug(LOG_NOTICE,"Worflow id is null\n");
        workflow_engine = "";
        return;
    }

    int engine_id = p->GetEngineOfWorkflow(workflow_id);
    if (engine_id < 0)
        engine_id = 0;
    
    ostringstream oss;
    oss << WORKFLOW_ENGINE_PREFIX << engine_id;//zyb �궨��
    workflow_engine = ApplicationName() + "." + ProcessName() + "." + oss.str(); 
}

void DbgWakeupEngine(int engine_index)
{
    WakeupEngine(engine_index,"");
}
DEFINE_DEBUG_FUNC(DbgWakeupEngine);

void DbgShowWorkflowEngine()
{
    printf("%d engines in this process:\n",number_of_engines);
    WorkflowEngine* p = NULL;
    vector<WorkflowEngine*>::iterator it;
    for (it = engines.begin(); it != engines.end(); ++it)
    {
        p = *it;
        p->Show();
    }
}
DEFINE_DEBUG_FUNC(DbgShowWorkflowEngine);

void DbgPauseWorkflow(const char* action_id)
{
    if (!action_id)
    {
        printf("No action id?!\n");
        return;
    }

    string prompt;
    if(true == Action::IsActionId(action_id))
    {
        prompt = string("workflow of action ") + action_id;
    }
    else
    {
        prompt = string("workflow ") + action_id;
    }
    
    STATUS ret = PauseWorkflow(action_id);
    if (SUCCESS == ret)
    {
        printf("Pause %s successfully!\n",prompt.c_str());
    }
    else
    {
        printf("Failed to pause %s! ret = %d\n",prompt.c_str(),ret);
    }
}
DEFINE_DEBUG_FUNC(DbgPauseWorkflow);

void DbgResumeWorkflow(const char* action_id)
{
    if (!action_id)
    {
        printf("No action id?!\n");
        return;
    }

    string prompt;
    if(true == Action::IsActionId(action_id))
    {
        prompt = string("workflow of action ") + action_id;
    }
    else
    {
        prompt = string("workflow ") + action_id;
    }
    
    STATUS ret = ResumeWorkflow(action_id);
    if (SUCCESS == ret)
    {
        printf("Resume %s successfully!\n",prompt.c_str());
    }
    else
    {
        printf("Failed to resume %s! ret = %d\n",prompt.c_str(),ret);
    }
}
DEFINE_DEBUG_FUNC(DbgResumeWorkflow);

void DbgRollbackWorkflow(const char* action_id)
{
    if (!action_id)
    {
        printf("No action id?!\n");
        return;
    }

    string prompt;
    if(true == Action::IsActionId(action_id))
    {
        prompt = string("workflow of action ") + action_id;
    }
    else
    {
        prompt = string("workflow ") + action_id;
    }
    
    STATUS ret = RollbackWorkflow(action_id);
    if (SUCCESS == ret)
    {
        printf("Rollback %s successfully!\n",prompt.c_str());
    }
    else
    {
        printf("Failed to rollback %s! ret = %d\n",prompt.c_str(),ret);
    }
}
DEFINE_DEBUG_FUNC(DbgRollbackWorkflow);

void DbgDeleteWorkflow(const char* action_id)
{
    if (!action_id)
    {
        printf("No action id?!\n");
        return;
    }

    STATUS ret = DeleteWorkflow(action_id);
    if (SUCCESS == ret)
    {
        printf("Delete workflow of action %s successfully!\n",action_id);
    }
    else
    {
        printf("Failed to delete workflow of action %s! ret = %d\n",action_id,ret);
    }
}
DEFINE_DEBUG_FUNC(DbgDeleteWorkflow);

