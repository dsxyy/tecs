/*******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：engine.cpp
* 文件标识：见配置管理计划书
* 内容摘要：工作流引擎
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2013年2月18日
*
* 修改记录1：
*     修改日期：2013/02/18
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
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

#define EV_ENGINE_POWER_ON      EV_TIMER_1 //工作流引擎上电启动
#define EV_ACTION_TIMER      EV_TIMER_2 //周期运转定时器
#define EV_ENGINE_WAKEUP     EV_TIMER_3 //唤醒引擎
#define WORKFLOW_ENGINE_CYCLE   5       //引擎工作周期为5秒钟
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
        ENGINE_CYCLE = 1,   //引擎周期性执行任务
        ENGINE_WAKEUP = 2,   //引擎被临时唤醒执行任务
        ENGINE_ACK = 3,   //引擎接收到外部应答之后处理任务
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
            //最后一个引擎负责处理剩余的所有任务
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

    //唤醒引擎处理某个工作流
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
        //根据收到的消息id进行处理
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

                //创建设置循环定时器
                _work_timer = m->CreateTimer();
                TimeOut timeout;
                timeout.duration = 1000 * WORKFLOW_ENGINE_CYCLE;
                timeout.msgid = EV_ACTION_TIMER;
                timeout.type = LOOP_TIMER;
                m->SetTimer(_work_timer,timeout);

                //删除本引擎负责的已经完成的工作流
                _action_pool->DeleteFinishedWorkflow(_engine_where);

                //清理数据库
                if(_engine_index == 0)
                {
                    _action_pool->Vacuum();
                }
                
                //引擎发动成功，上电完成
                m->set_state(S_Work);

                //上电之后可以先运行一次
                Run();
                break;
            }

            case EV_ENGINE_WAKEUP:
            {

                //删除本引擎负责的已经完成的工作流
                //_action_pool->DeleteFinishedWorkflow(_engine_where);
                 
                //EV_ENGINE_WAKEUP消息携带的workflow_id一般都不为空
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
                //删除本引擎负责的已经完成的工作流
                //_action_pool->DeleteFinishedWorkflow(_engine_where);                 
                //注意: EV_ACTION_TIMER消息携带的workflow_id肯定为空
                //EV_ENGINE_WAKEUP消息携带的workflow_id一般都不为空 
 
                int count = Run();
                if (0 == count)
                {
                    //0号引擎有特别之处: 无事可做的时候负责清理一下工作流相关表
                    if(_engine_index == 0)
                    {
                        _action_pool->Vacuum();
                    }

                    _idle_count ++;
                    
                    //如果引擎只是在反复空转，10次之后，尝试将扫描周期延长
                    if (_idle_count >= 10)
                    {
                        _idle_count = 0;
                        Debug(LOG_WARNING,"Slow work timer for workflow engine %d!\n",
                            _engine_index);
                        timeout.duration = 1000 * 60;
                        m->SetTimer(_work_timer,timeout);
                        //没事做就将循环定时器周期放长一点，改为1分钟扫描一次
                    }
                }
                else
                {
                    _idle_count = 0;
                    //有事做就重设一下循环定时器，改为5秒钟扫描跟催一次
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
                //唤醒引擎开始处理该工作流
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
                
                /*查询完成后由于工作流的状态有自定义状态，
                             还有action的状态，需要进行简化处理*/
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
                    //已经成功了，却还没有100%?
                    //已经100%了，还说没成功?
                    Debug(LOG_CRIT,"Wrong state or progress of action: %s from %s!state = %d, progress = %d\n",
                                        ack.action_id.c_str(),
                                        message.option.sender().serialize().c_str(),
                                        ack.state,
                                        ack.progress);
                    SkyAssert(false);
                    return;
                }

                //检查是否为working actions?
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
                    //如果之前已经有结果了，不在跟催中的状态，就不接收应答
                    Debug(LOG_WARNING,"Too late ack for action: %s!\n",ack.action_id.c_str());
                    return;
                }

                action.message_ack_id = message.option.id();
                action.message_ack_priority = message.option.priority();
                action.message_ack_version = message.option.version();
                action.progress = ack.progress;
                action.message_ack = message.data;
                action.state = ack.state;
                
                //如果action需要重试，则忽略失败状态，仍然认为是running
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
                //先更新一下Action，再处理它
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
        //已确认完成
        if (action.workflow_paused)
        {
            return ACTION_PAUSED;
        }

        //已确认完成
        if (action.finished)
        {
            return ACTION_FINISHED;
        }

        //已成功
        if (action.progress == 100)
        {
            SkyAssert(SUCCESS == action.state);
            return ACTION_SUCCESS;
        }

        if(action.synchronous == false && 
            action.state != ERROR_ACTION_READY)
        {
            //要求异步，且已经有初步进度，则表明可以有结论了
            if(SUCCESS == action.state || 
               ERROR_ACTION_RUNNING == action.state)
            {
                //这种场景要求执行方只要合法性检查通过，
                //就必须保证跟催至成功，不允许返回失败
                return ACTION_SUCCESS;
            }
            else
            {
                //要求异步，却仍然返回失败，一般是非法操作请求
                //例如企图释放正在使用中的资源
                return ACTION_FAILED;
            }
        }
        
        if (action.state == ERROR_ACTION_TIMEOUT)
        {
            SkyAssert(100 > action.progress);
            return ACTION_TIMEOUT;
        }

        //已失败
        if (action.state != SUCCESS && 
            action.state != ERROR_ACTION_READY && 
            action.state != ERROR_ACTION_RUNNING)
        {
            SkyAssert(100 > action.progress);
            return ACTION_FAILED;
        }

        //已经超时了,但是尚未更新到数据库
        if (action.timeout > 0 &&
                action.history_repeat_count >= GetRepeatCount(action.timeout))
        {
            return ACTION_TIMEOUT;
        }

        //刚刚准备好，尚未开始跟催
        if (action.state == ERROR_ACTION_READY)
        {
            return ACTION_READY;
        }
        
        return ACTION_RUNNING;
    }

    //运行引擎
    int Run(const string& workflow_id = "")
    {
        //Debug("======================engine %d is running======================\n",_index);
        //可以只运行指定的工作流
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
                //数据库视图和表更新之间有缓存???
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

    //处理action
    STATUS DoAction(Action& action,EngineRunMode runmode)
    {
        STATUS ret = ERROR;
        ActionState action_state = GetActionState(action);

        //已经暂停或者完成的Action理论上不可能走到这个流程，已经用视图过滤了
        SkyAssert(ACTION_PAUSED != action_state && ACTION_FINISHED != action_state);

        if (ENGINE_WAKEUP == runmode && ACTION_READY != action_state)
        {
            //如果是被唤醒，只处理刚刚准备好，尚未跟催过的Action，其它都不管
            //Debug("workflow engine %d is wakeup, only process ready action!\n",_index);
            return SUCCESS;
        }

        if (ENGINE_ACK == runmode && (ACTION_READY == action_state || ACTION_RUNNING == action_state))
        {
            //如果是收到Ack，需要忽视刚准备好的Action，防止超时时间精度问题
            //尚处于运行状态的Action也不用更新了，因为在进入本函数之前已经保存了应答内容
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
                    //已经设置成skipped的action直接自动结束
                    Debug(LOG_NOTICE,"action %s(%s) is skipped, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    //注意这里要调用FinishAction，而不是_action_pool->FinishAction
                    //因为有可能已经是最后一个Action了，结束之后可能还要删除整个工作流
                    FinishAction(action.action_id);
                    //结束之后立刻重新唤醒引擎，因为本Action可能有后续任务要处理
                    Wakeup(action.workflow_id);
                }
                else
                {
                    if(!action.rollback_action_id.empty())
                    {
                        //当action不是skipped，有实事要做，需要被跟催，它的rollback_action就不能再跳过
                        ret = _action_pool->SetSkippedFlag(action.rollback_action_id,false);
                        if(SUCCESS != ret)
                        {
                            SkyAssert(false);
                            return ERROR_DB_UPDATE_FAIL;
                        }
                         Debug(LOG_NOTICE,"rollback action %s(of %s) will not be skipped!\n",
                              action.rollback_action_id.c_str(),action.name.c_str());
                    }
                
                    //刚刚准备好，就开始跟催
                    Debug(LOG_INFO,"workflow engine %d is sending message for ready action %s(%s)...\n",
                          _engine_index,action.action_id.c_str(),action.name.c_str());
                    //首先更新跟催次数和最后一次跟催的时间
                    _action_pool->RepeatAction(action);
                    //然后发送跟催消息
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
                    //已经设置成skipped的action直接自动结束
                    Debug(LOG_NOTICE,"action %s(%s) is skipped, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    //注意这里要调用FinishAction，而不是_action_pool->FinishAction
                    //因为有可能已经是最后一个Action了，结束之后可能还要删除整个工作流
                    FinishAction(action.action_id);
                    //结束之后立刻重新唤醒引擎，因为本Action可能有后续任务要处理
                    Wakeup(action.workflow_id);

                }
                else
                {
                    //已经在运行，就继续跟催
                    Debug(LOG_INFO,"workflow engine %d is sending message for running action %s(%s)...\n",
                          _engine_index,action.action_id.c_str(),action.name.c_str());
                    //首先更新跟催次数和最后一次跟催的时间
                    _action_pool->RepeatAction(action);
                    //然后发送跟催消息
                    MessageOption option(action.receiver,
                                         action.message_id,action.message_priority,action.message_version);
                    m->Send(action.message_req,option);
                }
                break;
            }

            case ACTION_SUCCESS:
            {
                //工作已经完成，并且是成功了
                if (action.success_feedback != NO_FEEDBACK)
                {
                    //如果需要反馈就发送消息给摊派任务的消息单元
                    Debug(LOG_NOTICE,"action %s(%s) successed, feedback!\n",
                          action.action_id.c_str(),action.name.c_str());
                    MessageOption option(action.sender,
                                         action.message_ack_id,action.message_ack_priority,action.message_ack_version);
                    m->Send(action.message_ack,option);
                }
                else
                {
                    //如果不需要反馈就结束该任务
                    Debug(LOG_NOTICE,"action %s(%s) successed, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    FinishAction(action.action_id);
                    //结束之后立刻重新唤醒引擎，因为本工作流可能有后续任务要处理
                    Wakeup(action.workflow_id);
                }
                break;
            }

            case ACTION_FAILED:
            {
                //已经明确失败，也要考虑是否反馈
                if (action.failure_feedback)
                {
                    //如果需要反馈就发送消息给摊派任务的消息单元
                    Debug(LOG_WARNING,"action %s(%s) failed, feedback!\n",
                          action.action_id.c_str(),action.name.c_str());
                    MessageOption option(action.sender,
                                         action.message_ack_id,action.message_ack_priority,action.message_ack_version);
                    m->Send(action.message_ack,option);
                }
                else
                {
                    //如果不需要反馈就结束该任务
                    Debug(LOG_WARNING,"action %s(%s) failed, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    FinishAction(action.action_id);
                    //结束之后立刻重新唤醒引擎，因为本工作流可能有后续任务要处理
                    Wakeup(action.workflow_id);
                }
                break;
            }

            case ACTION_TIMEOUT:
            {
                //如果跟催次数已满，就不用再跟催了，
                if (action.state != ERROR_ACTION_TIMEOUT)
                {
                    action.state = ERROR_ACTION_TIMEOUT;
                    _action_pool->UpdateActionState(action);
                }

                if (action.timeout_feedback)
                {
                    //直接反馈给任务派发者
                    Debug(LOG_WARNING,"action %s(%s) timeout, feedback!\n",
                          action.action_id.c_str(),action.name.c_str());
                    WorkAck ack(action.action_id,action.state,action.progress);
                    MessageOption option(action.sender,EV_ACTION_TIMEOUT,0,0);
                    m->Send(ack,option);
                }
                else
                {
                    //如果不需要反馈就结束该任务
                    Debug(LOG_WARNING,"action %s(%s) timeout, finish!\n",
                          action.action_id.c_str(),action.name.c_str());
                    FinishAction(action.action_id);
                    //结束之后立刻重新唤醒引擎，因为可能有后续任务要处理
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
            //没有预先做好回滚计划，没法回滚
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

        //唤醒引擎处理回滚工作流
        //注意回滚工作流和原先的正常工作流都是放在同一个引擎下负责处理的!
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
    //多个引擎轮流服务
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
        //收到应答了
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
        //收到应答了
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

    //给用户调用的接口，将会连同预先准备好的回滚工作流一并删除
    MessageFrame message;
    ret = tempmu.Wait(&message,3000);
    if (SUCCESS == ret && message.option.id() == EV_DELETE_WORKFLOW_ACK)
    {
        //收到应答了
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
        //没有回滚工作流可用，不支持回滚
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
        //收到应答了
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
    oss << WORKFLOW_ENGINE_PREFIX << engine_id;//zyb 宏定义
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

