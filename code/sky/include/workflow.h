/*******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�workflow.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������������ϵͳͷ�ļ�
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
    //�Ƿ�ʧ���˾��Զ�����,Ĭ��Ϊfalse,���ڲ�����ʧ�ܵķ�������action,������Ϊtrue
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

    //���ack�Ļ����Ϸ���
    bool Validate()
    {
        //����ֻ����[0,100]֮��
    	if(progress < 0 || progress > 100)
        {
            return false;
        }

        //��˵������������һ��action��Ӧ��
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
    int success_feedback;//0��ʾ���뷴������������ʾҪ����
    int failure_feedback;//0��ʾ���뷴������������ʾҪ����
    int timeout_feedback;//0��ʾ���뷴������������ʾҪ����
    int timeout; //��ʱʱ������λΪ��
    int progress_weight;
    bool skipped; //�Ƿ�����? �������������Զ����
    bool synchronous; //ͬ���ȴ����ս���������յ�running����Ϊ�ɹ���?
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

    //��������Add������Ҫ�����ڼ����ϵĴ���
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
    MID upstream_sender;       //Ƕ�׹�����������sender
    string upstream_action_id; //Ƕ�׹���������������action_id
    string description;
    WorkflowLabels labels;
    string originator; //�������ķ�����
	
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
    MID upstream_sender;       //Ƕ�׹�����������sender
    string upstream_action_id; //Ƕ�׹���������������action_id
    string originator; //�������ķ�����
    string failed_action_category;
    string failed_action_name;
    string create_time;
    string delete_time;
    string description;
    WorkflowLabels labels;
};

//�������������棬��ʼ��������ģ�飬��ҵ����̵�main�����е���
STATUS StartWorkflowEngine(SqlDB *db,int count);

//����ֻ����һ��action�ļ��׹�����
STATUS CreateWorkflow(Action& action);

//������֧�ֻع����Ƶ���ͨ������
STATUS CreateWorkflow(Workflow& workflow);

//����֧�ֻع����Ƶĸ��ӹ���������������ͻع����������������лع�������Ĭ�ϴ�����̬ͣ���������ˣ���FindWorkflowҲ�Ҳ�������
STATUS CreateWorkflow(Workflow& workflow,Workflow& rollback_workflow);

//��ͣһ�������������ٸ��ߣ����ǹ�������Ȼ���ڣ�������ʱresume����������ȥ
STATUS PauseWorkflow(const string& action_id);

//������������ͣ״̬�ָ�Ϊ������������״̬
STATUS ResumeWorkflow(const string& action_id);

//�ع�һ��������������������ɾ�������ع�����������̬ͣresumeΪ����̬
//��ɾ�������������ᱻ�Զ�����workflow_history����
STATUS RollbackWorkflow(const string& action_id);

//ɾ��һ��������������ɾ�����е�actions��labels����ɾ���Ĺ����������workflow_history��
STATUS DeleteWorkflow(const string& action_id);

//��action id��ȡ����action��������Ϣ
STATUS GetActionById(const string& action_id,Action& action);

//����ĳ��������������һ��action��id��ȡ�ù�������id
STATUS GetWorkflowByActionId(const string& action_id,string& workflow_id);

//����ĳ����������id��������������action��id����ȡ����Ӧ�Ļع���������id
STATUS GetRollbackWorkflow(const string& action_id,string& rollback_workflow_id);

//����ĳ��action��id��ȡ���Ӧ�Ļع�action��id
STATUS GetRollbackAction(const string& action_id,string& rollback_action_id);

//����ĳ����������id��������������action��id����ȡ�ù����������Ѿ�����������action��id
STATUS GetFinishedActions(const string& action_id,vector<string>& actions);

//����ĳ����������id��������������action��id����ȡ�ù��������´�����ͣ״̬������action��id
//ע����ν��ͣaction��ָ����ͣĳ��������ʱ��ǡ�ô��ڸ��߰�;�У�ײ��ǹ�������е���Щaction
STATUS GetPausedActions(const string& action_id,vector<string>& actions);

//����ĳ��action��id����ȡ�����ڹ������������޻�ͼ�н���������Щaction id
STATUS GetNextActions(const string& action_id, vector<string>& actions);

//����ĳ��action��id���޸ĸ�������������Ϣ
STATUS UpdateActionReq(const string& action_id, const Serializable& message_req);

//����ĳ��action��id���޸ĸ�������������Ϣ����
STATUS UpdateActionReq(const string& action_id, const string& message_req);

//����ĳ��action��id���޸ĸ�������������Ϣ����
STATUS UpdateActionAck(const string& action_id,const string& message_ack);

//����ĳ��action��id���޸ĸ�������������Ϣ���շ���ַ
STATUS UpdateActionReceiver(const string& action_id, const MID& receiver);

//����ĳ��action��id���������ķ������ԣ�0��ʾ����Ҫ����
STATUS SetActionFeedback(const string& action_id,int success_feedback,int failure_feedback,int timeout_feedback);

//����ĳ��action��id����ȡ���ķ������ԣ�0��ʾ����Ҫ����
STATUS GetActionFeedback(const string& action_id,Action::ActionFeedbackTypes type,int& feedback);

//����ĳ��action��id����������Ϊ����
STATUS SkipAction(const string& action_id);

//����ĳ��action��id����������������������
//ע��һ�㲻����ʹ�ã�����message_req��labels��receiver��feedback���Ѿ���ר�ŵĽӿ���
STATUS ModifyAction(Action& action);

//����ĳ��action��id����������������ǹ����������һ��action��������������Ҳ����ɾ��������workflow_history��
STATUS FinishAction(const string& action_id);

//����ĳ��action��id��������������������Ŀǰ��������ô��������0%���¸���
STATUS RestartAction(const string& action_id);

//����ĳ��action��id��������Ƿ�Ϊͬ���ģ���νͬ���ģ�������Ҫ�ȵ�receiver����100%��ʱ��Ż��յ�����
//������첽�ģ�ֻҪreceiver�����Ѿ���ִ���ˣ�sender�ͻ�õ�����ķ���
bool IsActionSynchronous(const string& action_id);

//����ĳ��action��id��������Ƿ��Ѿ�����
bool IsActionFinished(const string& action_id);

//����ĳ��action��id��������Ƿ�����Ϊ�Զ�����
bool IsActionSkipped(const string& action_id);

//����ĳ����������id��������������action��id�����ж�ĳ���������Ƿ�֧�ֻع��������ǲ���һ����������
bool IsSupportRollback(const string& action_id);

//����ĳ����������id��������������action��id�����ж�ĳ���������Ƿ���һ���ع����ʵĹ�����
bool IsRollbackWorkflow(const string& action_id);

//����ĳ����������id��������������action��id����ȡ�����Ļع����ԣ����ӻع�����������˷�������
STATUS UnsetRollbackAttr(const string& action_id); 

//����ĳ����������id��������������action��id����ȡ�����Ļع����ԣ����ӻع�����������˷�������
//��ͬʱ����һ��upstream_sender��upstream_action_id
STATUS UnsetRollbackAttr(const string& action_id,const MID& upstream_sender,const string& upstream_action_id); 

//����ĳ����������id��������������action��id������ȡ�������ĵ��ڽ���
int GetWorkflowProgress(const string& action_id);

//���ݹ�����id��������action id����ȡ��������category��name
STATUS GetWorkflowCategoryName(const string& action_id,string& category,string& name);

//����action id��ȡaction��category��name
STATUS GetActionCategoryName(const string& action_id,string& category,string& name);

//����ĳ����������id��������������action��id������ȡ�������ı�ǩ
STATUS GetWorkflowLabels(const string& action_id, WorkflowLabels& labels);

//����ĳ����������id��������������action��id�������ù������ı�ǩ
STATUS SetWorkflowLabels(const string& action_id, const WorkflowLabels& labels);

//����ĳ����������id��������������action��id������ȡ������������sender MID
STATUS GetUpstreamSender(const string& action_id,MID& upstream_sender);

//����ĳ����������id��������������action��id������ȡ����������������action��id
STATUS GetUpstreamActionId(const string& action_id, string& upstream_action_id);

//����ĳ����������id��������������action��id�����Լ���������ĳ��action�����֣���ȡ��action�ı�ǩ
STATUS GetActionLabels(const string& workflow_id, const string& action_name, ActionLabels& labels);

//����action��id�����ø�action�ı�ǩ
STATUS SetActionLabels(const string& action_id, const ActionLabels& labels);

//����ĳ����������id��������������action��id�����Լ�action�����֣����øù����������н�������ֵ�action��labels
STATUS SetActionLabels(const string& workflow_id, const string& action_name, const ActionLabels& labels);

//����action��id����ȡ��action�ı�ǩ
STATUS GetActionLabels(const string& action_id, ActionLabels& labels);

//����action��name����ȡ��action��������Ϣ
STATUS GetActionMessageReq(const string& workflow_id, const string& action_name, string& message);

//����action��id����ȡ��action��������Ϣ
STATUS GetActionMessageReq(const string& action_id, string& message);

//����action��id����ȡ��action����ϢӦ��
STATUS GetActionMessageAck(const string& workflow_id, const string& action_name, string& message);

//����ĳ����������id��������������action��id�����Լ�action�����֣���ȡ�ù����������н�������ֵ�action��id
STATUS GetActionsByName(const string& workflow_id, const string& action_name, vector<string>& actions);

//����ĳ���ع���������id��������������action��id������ȡ��Ӧ������������ʷ��¼
STATUS GetForwardWorkflowHistory(const string& action_id,WorkflowHistory& history);

//����ĳ����������id(ע�����ﲻ�ܴ���action_id����Ϊ�������Ѿ���ɾ����Ϊ��ʷ��)�������乤������ʷ��¼
STATUS UpdateWorkflowHistory(const string& workflow_id,const string& detail);

//����ĳ����������id��������������action��id�������ù������ķ�����
STATUS SetWorkflowOriginator(const string& action_id, const string& originator);

//���������ӿڼ���ɾ����������ʹ��
string FindWorkflow(const string& category, const string& name,const string& label_where);
string FindWorkflow(const string& category, const string& name,const string& label_where,bool is_rollback);
string FindWorkflow(const string& label_where);

//���ݹ�������category��name���Լ���ǩ�������Ƿ���������Ĺ�����ע�⣺��������������ȴ������ͣ״̬�Ĺ��������ᱻ���أ�����
STATUS FindWorkflow(vector<string>& workflows,const string& category, const string& name,const string& label_where);

//���ݹ�������category��name����ǩ���Լ��Ƿ�ع����ʣ������Ƿ���������Ĺ�����ע�⣺��������������ȴ������ͣ״̬�Ĺ��������ᱻ���أ�����
STATUS FindWorkflow(vector<string>& workflows,const string& category, const string& name,const string& label_where,bool is_rollback);

//���ݹ�������category��name���Լ���ǩ,�����Ƿ������������ʷ����
STATUS FindWorkflowHistory(vector<string>& workflows,const string& category, const string& name,const string& label_where);

//ɾ��һ��uuid������uuid_history���У��������һ��uuid����վ
STATUS DeleteUuid(const string& uuid);

//���ĳ��uuid�Ƿ��Ѿ�λ��uuid����վ��
bool IsUuidDeleted(const string& uuid);

//��ȡĳ��action��ִ�н��
STATUS GetActionState(const string& action_id,int& state);

//����ĳ����������id��ȡ�ù��������²��ɹ���action��
int  GetUnsuccessedActionsCount(const string& action_id);

STATUS LockWorkflow(const string& action_id);
STATUS UnlockWorkflow(const string& action_id);
void GetWorkflowEngine(const string& workflow_id,string &workflow_engine);


#endif

