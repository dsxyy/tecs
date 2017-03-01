/*******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：workflow.h
* 文件标识：见配置管理计划书
* 内容摘要：工作流引擎系统头文件
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
#ifndef _WORKFLOW_H
#define _WORKFLOW_H
#include "sky.h"
#include "pool_sql.h"

class WorkReq: public Serializable
{
public:
    WorkReq()
    {
        retry = false;
    };
    
    WorkReq(const string& id):action_id(id)
    {
        retry = false;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(WorkReq);
        WRITE_VALUE(action_id);
        WRITE_VALUE(retry);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(WorkReq);
        READ_VALUE(action_id);
        READ_VALUE(retry);
        DESERIALIZE_END();
    };

    string action_id;
    //是否失败了就自动重试,默认为false,对于不允许失败的反向工作流action,请设置为true
    bool retry;
};

class WorkAck: public Serializable
{
public:
    WorkAck()
    {
        state = ERROR;
        progress = 0;
    };
    
    WorkAck(const string& _action_id):action_id(_action_id)
    {
        state = ERROR;
        progress = 0;
    };

    WorkAck(const string& _action_id,int _state,int _progress):action_id(_action_id)
    {
        state = _state;
        progress = _progress;
    };
    
    WorkAck(const string& _action_id,int _state,int _progress,const string& _detail):action_id(_action_id),detail(_detail)
    {
        state = _state;
        progress = _progress;
    };

    //检查ack的基本合法性
    bool Validate()
    {
        //进度只能在[0,100]之间
    	if(progress < 0 || progress > 100)
        {
            return false;
        }

        //得说清楚这是针对哪一个action的应答
        if(action_id.empty())
        {
            return false;
        }

        return true;
    };
	
    SERIALIZE
    {
        SERIALIZE_BEGIN(WorkAck);
        WRITE_VALUE(action_id);
        WRITE_VALUE(state);
        WRITE_VALUE(progress);
        WRITE_VALUE(detail);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(WorkAck);
        READ_VALUE(action_id);
        READ_VALUE(state);
        READ_VALUE(progress);
        READ_VALUE(detail);
        DESERIALIZE_END();
    };   

    string action_id;
    int state;
    int progress;
    string detail;
};

class WorkflowAck: public Serializable
{
public:
    enum WorkflowState
    {
        WORKFLOW_SUCCESS = 0,
        WORKFLOW_FAILED = 1,
        WORKFLOW_RUNNING = 2,
        WORKFLOW_CANCELED = 3,
        WORKFLOW_UNKNOWN = 4, 
        WORKFLOW_END = 5
    };
    
    WorkflowAck()
    {
        state = WORKFLOW_UNKNOWN;
        progress = 0;
    };
    
    WorkflowAck(const string& _workdflow_id)
    {
        workflow_id = _workdflow_id;
        state = WORKFLOW_UNKNOWN;
        progress = 0;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(WorkflowAck);
        WRITE_VALUE(workflow_id);
        WRITE_VALUE(state);
        WRITE_VALUE(progress);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(WorkflowAck);
        READ_VALUE(workflow_id);
        READ_VALUE(state);
        READ_VALUE(progress);
        DESERIALIZE_END();
    };   

    string workflow_id;
    int state;
    int progress;
};

class ActionPool;
class Workflow;
class WorkflowHistory;
class WorkflowEngine;

class ActionLabels
{
public:
    ActionLabels()
    {
        label_int64_1 = -1;
        label_int64_2 = -1;
        label_int64_3 = -1;
        label_int64_4 = -1;
        label_int64_5 = -1;
        label_int64_6 = -1;
        label_int64_7 = -1;
        label_int64_8 = -1;
        label_int64_9 = -1;
    };

    void Print() const
    {
        cout << "label_int64_1: " << label_int64_1 << endl;
        cout << "label_int64_2: " << label_int64_2 << endl;
        cout << "label_int64_3: " << label_int64_3 << endl;
        cout << "label_int64_4: " << label_int64_4 << endl;
        cout << "label_int64_5: " << label_int64_5 << endl;
        cout << "label_int64_6: " << label_int64_6 << endl;
        cout << "label_int64_7: " << label_int64_7 << endl;
        cout << "label_int64_8: " << label_int64_8 << endl;
        cout << "label_int64_9: " << label_int64_9 << endl;
        cout << "label_string_1: " << label_string_1 << endl;
        cout << "label_string_2: " << label_string_2 << endl;
        cout << "label_string_3: " << label_string_3 << endl;
        cout << "label_string_4: " << label_string_4 << endl;
        cout << "label_string_5: " << label_string_5 << endl;
        cout << "label_string_6: " << label_string_6 << endl;
        cout << "label_string_7: " << label_string_7 << endl;
        cout << "label_string_8: " << label_string_8 << endl;
        cout << "label_string_9: " << label_string_9 << endl;
    };
    
    int64 label_int64_1;
    int64 label_int64_2;
    int64 label_int64_3;
    int64 label_int64_4;
    int64 label_int64_5;
    int64 label_int64_6;
    int64 label_int64_7;
    int64 label_int64_8;
    int64 label_int64_9;
    string label_string_1;
    string label_string_2;
    string label_string_3;
    string label_string_4;
    string label_string_5;
    string label_string_6;
    string label_string_7;
    string label_string_8;
    string label_string_9;
};

class WorkflowLabels
{
public:
    WorkflowLabels()
    {
        label_int64_1 = -1;
        label_int64_2 = -1;
        label_int64_3 = -1;
        label_int64_4 = -1;
        label_int64_5 = -1;
        label_int64_6 = -1;
        label_int64_7 = -1;
        label_int64_8 = -1;
        label_int64_9 = -1;
    };

    void Print() const
    {
        cout << "label_int64_1: " << label_int64_1 << endl;
        cout << "label_int64_2: " << label_int64_2 << endl;
        cout << "label_int64_3: " << label_int64_3 << endl;
        cout << "label_int64_4: " << label_int64_4 << endl;
        cout << "label_int64_5: " << label_int64_5 << endl;
        cout << "label_int64_6: " << label_int64_6 << endl;
        cout << "label_int64_7: " << label_int64_7 << endl;
        cout << "label_int64_8: " << label_int64_8 << endl;
        cout << "label_int64_9: " << label_int64_9 << endl;
        cout << "label_string_1: " << label_string_1 << endl;
        cout << "label_string_2: " << label_string_2 << endl;
        cout << "label_string_3: " << label_string_3 << endl;
        cout << "label_string_4: " << label_string_4 << endl;
        cout << "label_string_5: " << label_string_5 << endl;
        cout << "label_string_6: " << label_string_6 << endl;
        cout << "label_string_7: " << label_string_7 << endl;
        cout << "label_string_8: " << label_string_8 << endl;
        cout << "label_string_9: " << label_string_9 << endl;
    };
    
    int64 label_int64_1;
    int64 label_int64_2;
    int64 label_int64_3;
    int64 label_int64_4;
    int64 label_int64_5;
    int64 label_int64_6;
    int64 label_int64_7;
    int64 label_int64_8;
    int64 label_int64_9;
    string label_string_1;
    string label_string_2;
    string label_string_3;
    string label_string_4;
    string label_string_5;
    string label_string_6;
    string label_string_7;
    string label_string_8;
    string label_string_9;
};

class Action
{
#define NO_FEEDBACK 0
public:
    enum  ActionFeedbackTypes
    {
        ACT_SUCCESS = 0,
        ACT_FAILURE = 1,
        ACT_TIMEOUT = 2
    };
    
    enum WorkflowActionFields
    {
        ACTION_ID = 0,
        WORKFLOW_ID = 1,
        ROLLBACK_ACTION_ID = 2,
        APPLICATION = 3,
        PROCESS = 4,
        WORKFLOW_ENGINE = 5,
        WORKFLOW_PAUSED = 6,
        WORKFLOW_CREATE_TIME = 7,
        WORKFLOW_PAUSE_TIME = 8,
        CATEGORY = 9,
        NAME = 10,
        SENDER = 11,
        RECEIVER = 12,
        MESSAGE_ID = 13,
        MESSAGE_PRIORITY = 14,
        MESSAGE_VERSION = 15,
        MESSAGE_REQ = 16,
        MESSAGE_ACK_ID = 17,
        MESSAGE_ACK_PRIORITY = 18,
        MESSAGE_ACK_VERSION = 19,
        MESSAGE_ACK = 20,
        SUCCESS_FEEDBACK = 21,
        FAILURE_FEEDBACK = 22,
        TIMEOUT_FEEDBACK = 23,
        STATE = 24,
        PROGRESS = 25,
        PROGRESS_WEIGHT = 26,
        TIMEOUT = 27,
        HISTORY_REPEAT = 28,
        SKIPPED = 29,
        FINISHED = 30,
        SYNCHRONOUS = 31,
        ACTION_CREATE_TIME = 32,
        LAST_REPEAT_TIME = 33,
        FINISH_TIME = 34,
        LABEL_INT64_1 = 35,
        LABEL_INT64_2 = 36,
        LABEL_INT64_3 = 37,
        LABEL_INT64_4 = 38,
        LABEL_INT64_5 = 39,
        LABEL_INT64_6 = 40,
        LABEL_INT64_7 = 41,
        LABEL_INT64_8 = 42,
        LABEL_INT64_9 = 43,
        LABEL_STRING_1 = 44,
        LABEL_STRING_2 = 45,
        LABEL_STRING_3 = 46,
        LABEL_STRING_4 = 47,
        LABEL_STRING_5 = 48,
        LABEL_STRING_6 = 49,
        LABEL_STRING_7 = 50,
        LABEL_STRING_8 = 51,
        LABEL_STRING_9 = 52,
        DESCRIPTION = 53,
        LIMIT = 54
    };
    
    Action():
    category("default"),name("noname")
    {
        Init();
    }

    Action(const string& _category,const string& _name):
    category(_category),name(_name)
    {
        Init();
    }

    Action(const string& _category,const string& _name,int _message_id):
    category(_category),name(_name)
    {
        Init();
        message_id = _message_id;
    }

    Action(const string& _category,const string& _name,int _message_id,const MID& _sender):
    category(_category),name(_name),sender(_sender)
    {
        Init();
        message_id = _message_id;
    }

    Action(const string& _category,const string& _name,int _message_id,const MID& _sender,const MID& _receiver):
    category(_category),name(_name),sender(_sender),receiver(_receiver)
    {
        Init();
        message_id = _message_id;
    }
    
    const string& get_id() const
    {
        SkyAssert(!action_id.empty());
        return action_id;
    }

    const string& get_workflow_id() const
    {
        return workflow_id;
    }
    
    int get_engine() const
    {
        return workflow_engine;
    }

    int get_state() const
    {
        return state;
    }

    int get_progress() const
    {
        return progress;
    }

    int get_history_repeat_count() const
    {
        return history_repeat_count;
    }
    
    static bool IsActionId(const string& id)
    {
        if(0 == id.compare(0,2,"a-"))
        {
            return true;
        }
        return false;
    }
    
    bool Validate() const;
    void Show() const;

    template<typename T>
    void SetMessageReq(const T &msg)
    {
        T new_msg = msg;
        new_msg.action_id = get_id();

        message_req = new_msg.serialize();
    }

    string category;
    string name;
    string rollback_action_id;
    MID sender;
    MID receiver;
    int message_id;
    int message_priority;
    int message_version;
    string message_req;
    string message_ack;
    int success_feedback;//0表示无须反馈，其它都表示要反馈
    int failure_feedback;//0表示无须反馈，其它都表示要反馈
    int timeout_feedback;//0表示无须反馈，其它都表示要反馈
    int timeout; //超时时长，单位为秒
    int progress_weight;
    bool skipped; //是否跳过? 即引擎遇到就自动完成
    bool synchronous; //同步等待最终结果，还是收到running就认为成功了?
    ActionLabels labels;
    string description;
    
private:
    Action(const string& id):
    category("default"),name("noname"),action_id(id)
    {
        Init();
    }
    
    void Init()
    {
        if(action_id.empty())
        {
            action_id = "a-" + GenerateUUID();
            SkyAssert(!action_id.empty());
        }
        
        workflow_engine = 0;
        workflow_paused = false;
        message_id = 0;
        message_priority = 0;
        message_version = 0;
        message_ack_id = 0;
        message_ack_priority = 0;
        message_ack_version = 0;
        success_feedback = 1;
        failure_feedback = 1;
        timeout_feedback = 1;
        state = ERROR_ACTION_READY;
        progress = 0;
        timeout = 0;
        progress_weight = 1;
        history_repeat_count = 0;
        skipped = false;
        finished = false;
        synchronous = true;
    };

    string action_id;
    string workflow_id;
    int workflow_engine;
    bool workflow_paused;
    string workflow_pause_time;
    int message_ack_id;
    int message_ack_priority;
    int message_ack_version;
    bool finished;
    int history_repeat_count;
    string last_repeat_time;
    string create_time;
    bool finish_time;
    int state;
    int progress;

protected:    
    friend class ActionPool;
    friend class Workflow;
    friend class WorkflowEngine;
};

class ActionSequence
{
public:
    ActionSequence()
    {};
    
    ActionSequence(const string& _action_prev,const string& _action_next):
    action_prev(_action_prev),action_next(_action_next)
    {};
 
    string action_prev;
    string action_next;
};

#define SYS_WORKFLOW_ORIGINATOR "[system]"

class Workflow
{
public:
    enum WorkflowFields
    {
        WORKFLOW_ID = 0,
        ROLLBACK_WORKFLOW_ID = 1,
        CATEGORY = 2,
        NAME = 3,
        APPLICATION = 4,
        PROCESS = 5,
        ENGINE = 6,
        PAUSED = 7,
        CREATE_TIME = 8,
        PAUSE_TIME = 9,
        UPSTREAM_SENDER = 10,
        UPSTREAM_ACTION_ID = 11,
        ORIGINATOR = 12,
        DESCRIPTION = 13,
        LIMIT = 14
    };
    
    Workflow()
    {
        Init();
    };

    Workflow(const string& _category, const string& _name):category(_category),name(_name)
    {
        Init();
    };
    
    Workflow(const Action& a)
    {
        Init();
        Add(a);
    };
    
    Workflow(const Action& prev, const Action& next)
    {
        Init();
        Add(prev,next);
    };

    Workflow(Action* p_prev, Action* p_next)
    {
        Init();
        Add(p_prev,p_next);
    };

    const string& get_id() const
    {
        SkyAssert(!workflow_id.empty());
        return workflow_id;
    }

    const string& get_rollback_workflow_id() const
    {   
        return rollback_workflow_id;
    }
    
    static bool IsWorkflowId(const string& id)
    {
        if(0 == id.compare(0,2,"w-"))
        {
            return true;
        }
        return false;
    }

    void Add(const Action& a);
    void Add(const Action& prev, const Action& next);
    void Add(const vector<Action>& prevs);
    void Add(const vector<Action>& prevs, const Action& next);
    void Add(const Action& prev, const vector<Action>& nexts);
    void Add(const vector<Action>& prevs, const vector<Action>& nexts);

    //以下两个Add函数主要是用于兼容老的代码
    void Add(const Action* p)
    {
        if (!p)
        {
            SkyAssert(false);
           return;
        }
        Add(*p);
    }

    void Add(const Action* p_prev, const Action* p_next)
    {
        if (!p_prev || !p_next)
        {
            SkyAssert(false);
           return;
        }
        Add(*p_prev,*p_next);
    }

    bool Validate() const;
    void Show() const;
    
    string category;
    string name;
    MID upstream_sender;       //嵌套工作流的上游sender
    string upstream_action_id; //嵌套工作流的上流请求action_id
    string description;
    WorkflowLabels labels;
    string originator; //工作流的发起人
	
private:
    void Init()
    {
        if(workflow_id.empty())
        {
            workflow_id = "w-" + GenerateUUID();
            SkyAssert(!workflow_id.empty());
        }
        
        paused = false;
        originator = SYS_WORKFLOW_ORIGINATOR;
        engine = 0;
    }
    
    bool ValidateSequence() const;
    void DeleteOldSequence(const string& prev_id,const string& next_id);
    STATUS GetActionNameById(const string& action_id,string& name) const;
    vector<Action> actions;
    vector<ActionSequence> sequences;
    string workflow_id;
    string rollback_workflow_id;
    int engine;
    bool paused;
    string pause_time;
    string create_time;
    string application;
    string process;

protected:    
    friend class ActionPool;
    friend class WorkflowEngine;
    friend class WorkflowHistory;
};

class ActionPool: public SqlCallbackable
{
public:
    static ActionPool* GetInstance()
    {
        return instance;
    };

    static ActionPool* CreateInstance(SqlDB *_db)
    {
        if(NULL == instance)
        {
            instance = new ActionPool(_db); 
        }
        
        return instance;
    };
    
    virtual ~ActionPool()
    {};
    STATUS Init();
    STATUS CreateWorkflow(Action& action);
    STATUS CreateWorkflow(Workflow& workflow);
    STATUS CreateWorkflow(Workflow& workflow,Workflow& rollback_workflow);
    STATUS DeleteWorkflow(const string& workflow_id,bool with_rollback);
    STATUS GetWorkflowById(const string& workflow_id,Workflow& workflow);
    STATUS SetWorkflowPauseFlag(const string& workflow_id,bool flag);
    bool IsWorkflowPaused(const string& workflow_id);
    bool IsRollbackWorkflow(const string& workflow_id);
    STATUS UnsetRollbackAttr(const string& workflow_id); 
    STATUS UnsetRollbackAttr(const string& workflow_id,const MID& upstream_sender,const string& upstream_action_id);
    STATUS GetWorkflowCategory(const string& workflow_id,string& category);
    STATUS GetWorkflowName(const string& workflow_id,string& name);
    STATUS GetUpstreamSender(const string& workflow_id,MID& upstream_sender);
    STATUS GetUpstreamActionId(const string& workflow_id,string& upstream_action_id);
    STATUS GetWorkflowByUpstreamActionId(const string& upstream_action_id,string& workflow_id);
    STATUS SetWorkflowUpstream(const string& workflow_id,const MID& sender,const string& action_id);
    STATUS SetWorkflowOriginator(const string& workflow_id, const string& originator);
    STATUS MakeWorkflowHistory(const string& workflow_id,WorkflowHistory& history);
    STATUS SaveWorkflowHistory(const WorkflowHistory& history);
    int GetWorkflowProgress(const string& workflow_id);
    string GetRollbackWorkflow(const string& workflow_id);
    STATUS GetAction(Action& action);
    STATUS GetActionById(const string& action_id,Action& action);
    STATUS GetActionCategory(const string& action_id,string& category);
    STATUS GetActionName(const string& action_id,string& name);
    STATUS GetActionState(const string& action_id,int& state);
    STATUS GetWorkflowByActionId(const string& action_id,string& workflow_id);
    STATUS GetRunningActions(const string& where,vector<string>& actions);
    STATUS GetRunningAction(const string& where,Action& action);
    STATUS GetFinishedActions(const string& workflow_id,vector<string>& actions);
    int GetUnfinishedActionsCount(const string& workflow_id);
    int GetUnsuccessedActionsCount(const string& workflow_id);
    STATUS GetActionsByName(const string& workflow_id,const string& action_name,vector<string>& actions);
    STATUS GetPausedActions(const string& workflow_id,vector<string>& actions);
    STATUS GetNextActions(const string& action_id, vector<string>& actions);
    STATUS GetRollbackWorkflow(const string& workflow_id,string& rollback_workflow_id);
    STATUS GetRollbackAction(const string& action_id,string& rollback_action_id);
    STATUS GetAdjacentActions(const string& action_id, vector<ActionSequence>& sequences);
    STATUS GetActionLabels(const string& action_id, ActionLabels& labels);
    STATUS GetActionLabels(const string& workflow_id, const string& action_name, ActionLabels& labels);
    STATUS SetActionLabels(const string& action_id,const ActionLabels& labels);
    STATUS GetActionMessageReq(const string& action_id, string& message);
    STATUS GetActionMessageReq(const string& workflow_id,const string& action_name,string& message);
    STATUS GetActionMessageAck(const string& workflow_id,const string& action_name,string& message);
    STATUS SetActionFeedback(const string& action_id,int success_feedback,int failure_feedback,int timeout_feedback);
    STATUS GetActionFeedback(const string& action_id,Action::ActionFeedbackTypes type,int& feedback);
    STATUS UpdateActionReq(const string& action_id, const string& message_req);
    STATUS UpdateActionAck(const string& action_id,const string& message_ack);
    STATUS UpdateActionReceiver(const string& action_id, const MID& receiver);
    STATUS ModifyAction(const Action& action);
    STATUS FinishAction(const string& action_id);
    STATUS RestartAction(const string& action_id);
    bool IsActionFinished(const string& action_id);
    bool IsActionSynchronous(const string& action_id);
    bool IsActionSkipped(const string& action_id);
    int GetEngineOfAction(const string& action_id);
    int GetEngineOfWorkflow(const string& workflow_id);
    STATUS SetWorkflowLabels(const string& workflow_id,const WorkflowLabels& labels);
    STATUS GetWorkflowLabels(const string& workflow_id,WorkflowLabels& labels);
    string GetWorkflowIdOfAction(const string& action_id);
    STATUS SetSkippedFlag(const string& action_id,bool skipped);
    STATUS  GetForwardWorkflowHistory(const string& workflow_id,WorkflowHistory& history);
    int GetWorkflowHistoryProgress(const string& workflow_id);
    STATUS GetWorkflowHistory(const string& workflow_id,WorkflowHistory& history);
    STATUS UpdateWorkflowHistory(const string& workflow_id,const string& detail);
    STATUS GetForwardWorkflow(const string& workflow_id,string& forward_workflow_id);
    string FindWorkflow(const string& where);
    STATUS FindWorkflow(const string& where,vector<string>& workflows);
    STATUS FindWorkflowHistory(const string& where,vector<string>& workflows);
    STATUS Vacuum();
    STATUS DeleteUuid(const string& uuid);
    bool IsUuidDeleted(const string& uuid);
    
protected:    
    friend class WorkflowEngine;
    
private:
    ActionPool(SqlDB *db);
    STATUS CreateWorkflowTables();
    STATUS InsertAction(const Action& action);
    STATUS InsertActionLabels(const string& action_id,const ActionLabels& labels);
    STATUS InsertWorkflow(const Workflow& workflow);
    STATUS InsertWorkflowLabels(const string& workflow_id,const WorkflowLabels& labels);
    STATUS InsertSequence(const ActionSequence& sequence);
    STATUS UpdateActionState(const Action& action);
    STATUS RepeatAction(Action& action);
    STATUS DeleteFinishedWorkflow(const string& where);
    int SelectWorkflowCallback(void *nil, SqlColumn *columns);
    int SelectWorkflowHistoryCallback(void *nil, SqlColumn *columns);
    int SelectActionCallback(void *nil, SqlColumn *columns);
    int SelectSequencesCallback(void *nil, SqlColumn *columns);
    int SelectActionLabelsCallback(void *nil, SqlColumn *columns);
    int SelectWorkflowLabelsCallback(void *nil, SqlColumn *columns);
    int SelectFailedActionCallback(void *nil, SqlColumn *columns);
    DISALLOW_COPY_AND_ASSIGN(ActionPool);
    static ActionPool *instance;
    SqlDB *db;
    string application;
    string process;
};

class WorkflowHistory
{
public:
    enum WfHistoryFields
    {
        WORKFLOW_ID = 0,
        ROLLBACK_WORKFLOW_ID = 1,
        CATEGORY = 2,
        NAME = 3,
        APPLICATION = 4,
        PROCESS = 5,
        STATE = 6,
        DETAIL = 7,
        PROGRESS = 8,
        FAILED_ACTION_CATEGORY = 9,
        FAILED_ACTION_NAME = 10,
        ORIGINATOR = 11,
        CREATE_TIME = 12,
        DELETE_TIME = 13,
        DESCRIPTION = 14,
        LIMIT = 15
    };

    WorkflowHistory()
    {
        state = SUCCESS;
        progress = 0;
    };

    WorkflowHistory(const Workflow& w)
    {
        state = SUCCESS;
        workflow_id = w.workflow_id;
        rollback_workflow_id = w.rollback_workflow_id;
        category = w.category;
        name = w.name;
        upstream_sender = w.upstream_sender;
        upstream_action_id = w.upstream_action_id;
        originator = w.originator;
        application = w.application;
        process = w.process;
        create_time = w.create_time;
    };

    void Show() const;
    
    string workflow_id;
    string rollback_workflow_id;
    string category;
    string name;
    string application;
    string process;
    
    //string result;  //finished,deleted,rollbacked
    int state;      //error code
    string detail;
    int progress;
    MID upstream_sender;       //嵌套工作流的上游sender
    string upstream_action_id; //嵌套工作流的上流请求action_id
    string originator; //工作流的发起人
    string failed_action_category;
    string failed_action_name;
    string create_time;
    string delete_time;
    string description;
    WorkflowLabels labels;
};

//启动工作流引擎，初始化工作流模块，在业务进程的main函数中调用
STATUS StartWorkflowEngine(SqlDB *db,int count);

//创建只包含一个action的简易工作流
STATUS CreateWorkflow(Action& action);

//创建不支持回滚机制的普通工作流
STATUS CreateWorkflow(Workflow& workflow);

//创建支持回滚机制的复杂工作流，包含正向和回滚两个工作流，其中回滚工作流默认处于暂停态（被隐藏了），FindWorkflow也找不到它。
STATUS CreateWorkflow(Workflow& workflow,Workflow& rollback_workflow);

//暂停一个工作流，不再跟催，但是工作流仍然存在，可以随时resume继续跟催下去
STATUS PauseWorkflow(const string& action_id);

//将工作流从暂停状态恢复为正常跟催运行状态
STATUS ResumeWorkflow(const string& action_id);

//回滚一个工作流。将正向工作流删除，将回滚工作流由暂停态resume为运行态
//被删除的正向工作流会被自动放入workflow_history表中
STATUS RollbackWorkflow(const string& action_id);

//删除一个工作流，级联删除所有的actions和labels，已删除的工作流会进入workflow_history表
STATUS DeleteWorkflow(const string& action_id);

//从action id获取整个action的数据信息
STATUS GetActionById(const string& action_id,Action& action);

//根据某工作流名下任意一个action的id获取该工作流的id
STATUS GetWorkflowByActionId(const string& action_id,string& workflow_id);

//根据某个工作流的id（或其名下任意action的id）获取它对应的回滚工作流的id
STATUS GetRollbackWorkflow(const string& action_id,string& rollback_workflow_id);

//根据某个action的id获取其对应的回滚action的id
STATUS GetRollbackAction(const string& action_id,string& rollback_action_id);

//根据某个工作流的id（或其名下任意action的id）获取该工作流名下已经结束的所有action的id
STATUS GetFinishedActions(const string& action_id,vector<string>& actions);

//根据某个工作流的id（或其名下任意action的id）获取该工作流名下处于暂停状态的所有action的id
//注：所谓暂停action，指的暂停某个工作流时，恰好处于跟催半途中，撞到枪口上中招的那些action
STATUS GetPausedActions(const string& action_id,vector<string>& actions);

//根据某个action的id，获取它所在工作流的有向无环图中紧跟其后的那些action id
STATUS GetNextActions(const string& action_id, vector<string>& actions);

//根据某个action的id，修改更新它的请求消息
STATUS UpdateActionReq(const string& action_id, const Serializable& message_req);

//根据某个action的id，修改更新它的请求消息内容
STATUS UpdateActionReq(const string& action_id, const string& message_req);

//根据某个action的id，修改更新它的请求消息内容
STATUS UpdateActionAck(const string& action_id,const string& message_ack);

//根据某个action的id，修改更新它的请求消息接收方地址
STATUS UpdateActionReceiver(const string& action_id, const MID& receiver);

//根据某个action的id，设置它的反馈属性，0表示不需要反馈
STATUS SetActionFeedback(const string& action_id,int success_feedback,int failure_feedback,int timeout_feedback);

//根据某个action的id，获取它的反馈属性，0表示不需要反馈
STATUS GetActionFeedback(const string& action_id,Action::ActionFeedbackTypes type,int& feedback);

//根据某个action的id，将其设置为跳过
STATUS SkipAction(const string& action_id);

//根据某个action的id，更新它的所有配置数据
//注：一般不建议使用，更新message_req，labels，receiver，feedback都已经有专门的接口了
STATUS ModifyAction(Action& action);

//根据某个action的id，结束它，如果这是工作流中最后一个action，则整个工作流也将被删除，移入workflow_history表
STATUS FinishAction(const string& action_id);

//根据某个action的id，将其重新启动，无论目前进度条怎么样，都从0%重新跟催
STATUS RestartAction(const string& action_id);

//根据某个action的id，检查它是否为同步的，所谓同步的，就是它要等到receiver返回100%的时候才会收到反馈
//如果是异步的，只要receiver返回已经在执行了，sender就会得到引擎的反馈
bool IsActionSynchronous(const string& action_id);

//根据某个action的id，检查它是否已经结束
bool IsActionFinished(const string& action_id);

//根据某个action的id，检查它是否设置为自动跳过
bool IsActionSkipped(const string& action_id);

//根据某个工作流的id（或其名下任意action的id），判断某个工作流是否支持回滚，即它是不是一个正向工作流
bool IsSupportRollback(const string& action_id);

//根据某个工作流的id（或其名下任意action的id），判断某个工作流是否是一个回滚性质的工作流
bool IsRollbackWorkflow(const string& action_id);

//根据某个工作流的id（或其名下任意action的id），取消它的回滚属性，即从回滚工作流变成了反向工作流
STATUS UnsetRollbackAttr(const string& action_id); 

//根据某个工作流的id（或其名下任意action的id），取消它的回滚属性，即从回滚工作流变成了反向工作流
//并同时更新一下upstream_sender和upstream_action_id
STATUS UnsetRollbackAttr(const string& action_id,const MID& upstream_sender,const string& upstream_action_id); 

//根据某个工作流的id（或其名下任意action的id），获取工作流的当期进度
int GetWorkflowProgress(const string& action_id);

//根据工作流id（或任意action id）获取工作流的category和name
STATUS GetWorkflowCategoryName(const string& action_id,string& category,string& name);

//根据action id获取action的category和name
STATUS GetActionCategoryName(const string& action_id,string& category,string& name);

//根据某个工作流的id（或其名下任意action的id），获取工作流的标签
STATUS GetWorkflowLabels(const string& action_id, WorkflowLabels& labels);

//根据某个工作流的id（或其名下任意action的id），设置工作流的标签
STATUS SetWorkflowLabels(const string& action_id, const WorkflowLabels& labels);

//根据某个工作流的id（或其名下任意action的id），获取工作流的上游sender MID
STATUS GetUpstreamSender(const string& action_id,MID& upstream_sender);

//根据某个工作流的id（或其名下任意action的id），获取工作流的上游请求action的id
STATUS GetUpstreamActionId(const string& action_id, string& upstream_action_id);

//根据某个工作流的id（或其名下任意action的id），以及工作流中某个action的名字，获取该action的标签
STATUS GetActionLabels(const string& workflow_id, const string& action_name, ActionLabels& labels);

//根据action的id，设置该action的标签
STATUS SetActionLabels(const string& action_id, const ActionLabels& labels);

//根据某个工作流的id（或其名下任意action的id），以及action的名字，设置该工作流中所有叫这个名字的action的labels
STATUS SetActionLabels(const string& workflow_id, const string& action_name, const ActionLabels& labels);

//根据action的id，获取该action的标签
STATUS GetActionLabels(const string& action_id, ActionLabels& labels);

//根据action的name，获取该action的请求消息
STATUS GetActionMessageReq(const string& workflow_id, const string& action_name, string& message);

//根据action的id，获取该action的请求消息
STATUS GetActionMessageReq(const string& action_id, string& message);

//根据action的id，获取该action的消息应答
STATUS GetActionMessageAck(const string& workflow_id, const string& action_name, string& message);

//根据某个工作流的id（或其名下任意action的id），以及action的名字，获取该工作流中所有叫这个名字的action的id
STATUS GetActionsByName(const string& workflow_id, const string& action_name, vector<string>& actions);

//根据某个回滚工作流的id（或其名下任意action的id），获取对应的正向工作流历史记录
STATUS GetForwardWorkflowHistory(const string& action_id,WorkflowHistory& history);

//根据某个工作流的id(注意这里不能传入action_id，因为工作流已经被删除成为历史了)，更新其工作流历史记录
STATUS UpdateWorkflowHistory(const string& workflow_id,const string& detail);

//根据某个工作流的id（或其名下任意action的id），设置工作流的发起人
STATUS SetWorkflowOriginator(const string& action_id, const string& originator);

//以下两个接口即将删除，不建议使用
string FindWorkflow(const string& category, const string& name,const string& label_where);
string FindWorkflow(const string& category, const string& name,const string& label_where,bool is_rollback);
string FindWorkflow(const string& label_where);

//根据工作流的category，name，以及标签，查找是否存在这样的工作，注意：符合条件，但是却处于暂停状态的工作流不会被返回！！！
STATUS FindWorkflow(vector<string>& workflows,const string& category, const string& name,const string& label_where);

//根据工作流的category，name，标签，以及是否回滚性质，查找是否存在这样的工作，注意：符合条件，但是却处于暂停状态的工作流不会被返回！！！
STATUS FindWorkflow(vector<string>& workflows,const string& category, const string& name,const string& label_where,bool is_rollback);

//根据工作流的category，name，以及标签,查找是否存在这样的历史工作
STATUS FindWorkflowHistory(vector<string>& workflows,const string& category, const string& name,const string& label_where);

//删除一个uuid，放入uuid_history表中，这个表是一个uuid回收站
STATUS DeleteUuid(const string& uuid);

//检查某个uuid是否已经位于uuid回收站中
bool IsUuidDeleted(const string& uuid);

//获取某个action的执行结果
STATUS GetActionState(const string& action_id,int& state);

//根据某个工作流的id获取该工作流名下不成功的action数
int  GetUnsuccessedActionsCount(const string& action_id);

STATUS LockWorkflow(const string& action_id);
STATUS UnlockWorkflow(const string& action_id);
void GetWorkflowEngine(const string& workflow_id,string &workflow_engine);


#endif

