/*******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�action.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�������������ڵ����ݿ����
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2013��2��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/02/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
* �޸ļ�¼2��
*     �޸����ڣ�2013/03/14
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ����Ӷ����ȡlabels�ӿں���
*******************************************************************************/
#include "sky.h"
#include "workflow.h"

static int workflow_action_print_on = 0;
DEFINE_DEBUG_VAR(workflow_action_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(workflow_action_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            else\
            {\
                Log(level,"[%s:%d]"fmt,__func__,__LINE__, ##arg);\
            }\
        }while(0)

const static string table_action_pool("action_pool");
const static string table_action_labels("action_labels");
const static string table_workflow("workflow");
const static string table_workflow_history("workflow_history");
const static string table_workflow_labels("workflow_labels");
const static string table_workflow_labels_history("workflow_labels_history");
const static string table_action_sequences("action_sequences");
const static string table_uuid_history("uuid_history");
const static string view_actions("view_actions");
const static string view_workflows("view_workflows");
const static string view_workflow_history("view_workflow_history");
const static string view_workflow_actions("workflow_actions");
const static string view_workflow_progress("workflow_progress");
const static string view_running_actions("running_actions");
const static string view_paused_actions("paused_actions");

const static string columns_workflow("\
        workflow_id,rollback_workflow_id,category,name,\
        application,process,engine,paused,create_time,pause_time,\
        upstream_sender,upstream_action_id,originator,description");
        
const static string columns_workflow_history("\
        workflow_id,rollback_workflow_id,category,name,application,process,\
        state,detail,progress, failed_action_category,failed_action_name,originator,create_time,delete_time,description");
        
const static string columns_workflow_actions("\
        action_id,workflow_id,rollback_action_id,\
        application,process,engine,workflow_paused,workflow_create_time,workflow_pause_time,\
        category,name,sender,receiver,\
        message_id,message_priority,message_version,message_req,\
        message_ack_id,message_ack_priority,message_ack_version,message_ack,\
        success_feedback,failure_feedback,timeout_feedback,state,progress,progress_weight,\
        timeout,history_repeat_count,skipped,finished,synchronous,action_create_time,last_repeat_time,finish_time,\
        label_int64_1,label_int64_2,label_int64_3,\
        label_int64_4,label_int64_5,label_int64_6,\
        label_int64_7,label_int64_8,label_int64_9,\
        label_string_1,label_string_2,label_string_3,\
        label_string_4,label_string_5,label_string_6,\
        label_string_7,label_string_8,label_string_9,\
        description");
        
const static string columns_action_labels("\
        label_int64_1,label_int64_2,label_int64_3, \
        label_int64_4,label_int64_5,label_int64_6,\
        label_int64_7,label_int64_8,label_int64_9,\
        label_string_1,label_string_2,label_string_3,\
        label_string_4,label_string_5,label_string_6,\
        label_string_7,label_string_8,label_string_9");
        
const static string columns_workflow_labels("\
        label_int64_1,label_int64_2,label_int64_3, \
        label_int64_4,label_int64_5,label_int64_6,\
        label_int64_7,label_int64_8,label_int64_9,\
        label_string_1,label_string_2,label_string_3,\
        label_string_4,label_string_5,label_string_6,\
        label_string_7,label_string_8,label_string_9");
        
extern int GetAvailableEngine();
extern int GetRepeatCount(int seconds);
extern STATUS WakeupEngine(int engine_index,const string& workflow_id);

static bool IsValidName(const string& name)
{
    //workflow,action��category��nameֻ��ʹ��Сд��ĸ���»���
    char c;
    int size = name.size();
    for(int i = 0; i < size; ++i)
    {
        c = name.at(i);
        if((c >= 'a' && c <= 'z') || c == '_')
            continue;
        else
            return false;
    }

    return true;
}

bool Action::Validate() const
{
    if (!IsActionId(action_id))
    {
        SkyAssert(false);
        return false;
    }

    if (workflow_id.empty())
    {
        SkyAssert(false);
        return false;
    }
    
    if (0 > workflow_engine)
    {
        SkyAssert(false);
        return false;
    }

    if (category.empty())
    {
        SkyAssert(false);
        return false;
    }

    if (name.empty())
    {
        SkyAssert(false);
        return false;
    }

    if(!IsValidName(category))
    {
        cerr << "Invalid action category: " << category << endl;
        cerr << "Only lower case letters and underscore can be used!!!" << endl;
        SkyAssert(false);
        return false;
    }
    
    if(!IsValidName(name))
    {
        cerr << "Invalid action name: " << name << endl;
        cerr << "Only lower case letters and underscore can be used!!!" << endl;
        SkyAssert(false);
        return false;
    }
    
    if (0 > success_feedback || 0 > failure_feedback || 0 > timeout_feedback)
    {
        cerr << "illegal feedback flag!" << endl;
        SkyAssert(false);
        return false;
    }
        
    if (0 >= message_id)
    {
        SkyAssert(false);
        return false;
    }

    WorkReq req;
    req.deserialize(message_req);
    if(req.action_id.empty() || req.action_id != action_id)
    {
        //������Ϣ�е�action_idӦ�úͱ�action��id��ͬ
        SkyAssert(false);
        return false;
    }

    if(req.retry && failure_feedback)
    {
        //��Ȼʧ�������Զ����ԣ��Ͳ�Ҫָ����ʧ�ܷ�����
        SkyAssert(false);
        return false;
    }
    
    if (-1 > timeout)
    {
        SkyAssert(false);
        return false;
    }

    if (0 > progress)
    {
        SkyAssert(false);
        return false;
    }

    if (0 > progress_weight)
    {
        SkyAssert(false);
        return false;
    }

    if (0 > history_repeat_count)
    {
        SkyAssert(false);
        return false;
    }

    return true;
}

void Action::Show() const
{
    cout << "action id: " << action_id << endl;
    cout << "workflow id: " << workflow_id << endl;
    cout << "workflow engine: " << workflow_engine << endl;
    cout << "rollback action id: " << rollback_action_id << endl;
    cout << "category: " << category << endl;
    cout << "name: " << name << endl;
    cout << "sender: " << sender << endl;
    cout << "receiver: " << receiver << endl;
    cout << "success feedback: " << success_feedback << endl;
    cout << "failure feedback: " << failure_feedback << endl;
    cout << "timeout feedback: " << timeout_feedback<< endl;
    cout << "skipped: " << skipped << endl;
    cout << "synchronous: " << synchronous << endl;
    cout << "state: " << state << endl;
    cout << "progress: " << progress << endl;
    cout << "progress weight: " << progress_weight << endl;
    cout << "timeout: " << timeout << " seconds"<< endl;
    cout << "history repeat count: " << history_repeat_count << endl;
    cout << "description: " << description << endl;
    labels.Print();
}

void Workflow::Add(const Action& a)
{
    vector<Action>::iterator it;
    for(it=actions.begin(); it != actions.end(); it++)
    {
        if(it->action_id == a.action_id)
        {
            //Debug("action %s is already in workflow %s!",a.action_id.c_str(),workflow_id.c_str());
            return;
        }
    }

    actions.push_back(a);
}

void Workflow::Add(const Action& prev, const Action& next)
{
    SkyAssert(prev.action_id != next.action_id);
    //�ȼ��Sequence�Ƿ��Ѿ�����
    bool b_sequence_exist = false;
    vector<ActionSequence>::iterator its = sequences.begin();
    while (its != sequences.end())
    {
        if (its->action_prev == prev.action_id && its->action_next == next.action_id)
        {
            //�Ѿ�������
            b_sequence_exist = true;
            Debug(LOG_ERR,"action sequences (%s -> %s) is already in workflow %s!",
                prev.action_id.c_str(),next.action_id.c_str(),workflow_id.c_str());
            break;
        }

        if (its->action_prev == next.action_id && its->action_next == prev.action_id)
        {
            //�γ��Ի���
            SkyAssert(false);
            return;
        }
        its++;
    }

    if (!b_sequence_exist)
    {
        ActionSequence s(prev.action_id,next.action_id);
        sequences.push_back(s);
    }

    Add(prev);
    Add(next);
}

void Workflow::Add(const vector<Action>& prevs)
{
    vector<Action>::const_iterator it;
    for(it = prevs.begin(); it != prevs.end(); it++)
    {
        Add(*it);
    }
}

void Workflow::Add(const vector<Action>& prevs, const Action& next)
{
    vector<Action>::const_iterator it;
    for(it = prevs.begin(); it != prevs.end(); it++)
    {
        Add(*it,next);
    }
}

void Workflow::Add(const Action& prev, const vector<Action>& nexts)
{
    vector<Action>::const_iterator it;
    for(it = nexts.begin(); it != nexts.end(); it++)
    {
        Add(prev,*it);
    }
}

void Workflow::Add(const vector<Action>& prevs, const vector<Action>& nexts)
{
    if(prevs.empty() || nexts.empty())
    {
        SkyAssert(false);
        return;
    }

    int n1 = prevs.size();
    int n2 = nexts.size();
    if(n1 != n2)
    {
        SkyAssert(false);
        return;
    }

    int i = 0;
    for(i=0;i<n1;i++)
    {
        Add(prevs.at(i),nexts.at(i));
    }
}

bool Workflow::ValidateSequence() const
{
    vector<ActionSequence> s = sequences;
    map<string,int> t;
    map<string,int>::iterator itm;
    string action_id;

    while (1)
    {
        vector<ActionSequence>::iterator it;
        t.clear();
        for (it = s.begin(); it != s.end(); it++)
        {
            itm = t.find(it->action_prev);
            if (itm == t.end())
            {
                t.insert(make_pair(it->action_prev,0));
            }
            itm = t.find(it->action_next);
            if (itm == t.end())
            {
                t.insert(make_pair(it->action_next,0));
            }
        }

        int erased = 0;
        for (it = s.begin(); it != s.end(); it++)
        {
            t[it->action_next]++;
        }

        for (itm = t.begin(); itm != t.end(); itm++)
        {
            if (itm->second == 0)
            {
                it = s.begin();
                while (it != s.end())
                {
                    //ɾ��û��ǰ���Ľڵ�
                    if (it->action_prev == itm->first || it->action_next == itm->first)
                    {
                        it = s.erase(it);
                        erased++;
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }

        if (erased == 0)
        {
            //�Ѿ�û�нڵ����ɾ����
            break;
        }
    }

    if (s.size() == 0)
    {
        return true;
    }
    else
    {
        SkyAssert(false);
        return false;
    }
}

bool Workflow::Validate() const
{
    if (!IsWorkflowId(workflow_id))
    {
        SkyAssert(false);
        return false;
    }

    if (category.empty())
    {
        SkyAssert(false);
        return false;
    }
    
    if (name.empty())
    {
        SkyAssert(false);
        return false;
    }

    if(!IsValidName(category))
    {
        cerr << "Invalid workflow category: " << category << endl;
        cerr << "Only lower case letters and underscore can be used!!!" << endl;
        SkyAssert(false);
        return false;
    }
    
    if(!IsValidName(name))
    {
        cerr << "Invalid workflow name: " << name << endl;
        cerr << "Only lower case letters and underscore can be used!!!" << endl;
        SkyAssert(false);
        return false;
    }
    
    vector<Action>::const_iterator it = actions.begin();
    //������Action�ĺϷ���
    while (it != actions.end())
    {
        if (it->Validate() != true)
        {
            SkyAssert(false);
            return false;
        }
        it++;
    }

    //���Sequence�Ļ����Ϸ���
    vector<ActionSequence>::const_iterator its = sequences.begin();
    while (its != sequences.end())
    {
        if (its->action_prev == its->action_next)
        {
            SkyAssert(false);
            return false;
        }
        its++;
    }

    return ValidateSequence();
}

STATUS Workflow::GetActionNameById(const string& action_id,string& name) const
{
    vector<Action>::const_iterator it = actions.begin();
    while (it != actions.end())
    {
        if (it->action_id == action_id)
        {
            name = it->name;
            return SUCCESS;
        }
        it++;
    }
    return ERROR_OBJECT_NOT_EXIST;
}

void Workflow::Show() const
{
    if (actions.empty())
    {
        printf("No actions.");
        return;
    }

    if (sequences.empty())
    {
        printf("No action sequences.");
        return;
    }

    string prev_name,next_name; 
    vector<ActionSequence>::const_iterator it = sequences.begin();
    printf("------workflow sequence------\n");
    while (it != sequences.end())
    {
        GetActionNameById(it->action_prev,prev_name);
        GetActionNameById(it->action_next,next_name);

        printf("%s(%s) ------SUCCESS------> %s(%s)\n",
               it->action_prev.c_str(),
               prev_name.c_str(),
               it->action_next.c_str(),
               next_name.c_str());

        it++;
    }
    labels.Print();
}

void WorkflowHistory::Show() const
{
    cout << "====history of workflow====" << endl;
    cout << "workflow_id: " << workflow_id << endl;
    cout << "rollback_workflow_id: " << rollback_workflow_id << endl;
    cout << "category: " << category << endl;
    cout << "name: " << name << endl;
    cout << "application: " << application << endl;
    cout << "process: " << process << endl;
    cout << "upstream_sender: " << upstream_sender << endl;
    cout << "upstream_action_id: " << upstream_action_id << endl;
    cout << "originator: " << originator << endl;
    cout << "state: " << state << endl;
    cout << "detail: " << detail << endl;
    cout << "progress: " << progress << endl;
    cout << "failed_action_category: " << failed_action_category << endl;
    cout << "failed_action_name: " << failed_action_name << endl;
    cout << "create_time: " << create_time << endl;
    cout << "delete_time: " << delete_time << endl;
    cout << "description: " << description << endl;
    cout << "====history labels====" << endl;
    labels.Print();
}

ActionPool* ActionPool::instance = NULL;

ActionPool::ActionPool(SqlDB *pDb):SqlCallbackable(pDb)
{
    db = pDb;
}

STATUS ActionPool::Init()
{
    application = ApplicationName();
    process = ProcessName();
    if (application.empty() || process.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
    return SUCCESS;
}

STATUS ActionPool::InsertWorkflow(const Workflow& workflow)
{
    SqlCommand sql(db,table_workflow,SqlCommand::WITH_TRANSACTION);
    sql.Add("workflow_id",workflow.workflow_id);
    sql.Add("category",workflow.category);
    sql.Add("name",workflow.name);
    sql.Add("upstream_sender",workflow.upstream_sender);
    sql.Add("upstream_action_id",workflow.upstream_action_id);
    sql.Add("rollback_workflow_id",workflow.rollback_workflow_id);
    sql.Add("application",application);
    sql.Add("process",process);
    sql.Add("engine",workflow.engine);
    sql.Add("paused",workflow.paused);
    sql.Add("originator",workflow.originator);
    Datetime now;
    sql.Add("create_time",now);
    sql.Add("description",workflow.description);
    STATUS ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::InsertWorkflowLabels(const string& workflow_id,const WorkflowLabels& labels)
{
    SqlCommand sql(db,table_workflow_labels,SqlCommand::WITH_TRANSACTION);
    sql.Add("workflow_id",workflow_id);
    sql.Add("label_int64_1",labels.label_int64_1);
    sql.Add("label_int64_2",labels.label_int64_2);
    sql.Add("label_int64_3",labels.label_int64_3);
    sql.Add("label_int64_4",labels.label_int64_4);
    sql.Add("label_int64_5",labels.label_int64_5);
    sql.Add("label_int64_6",labels.label_int64_6);
    sql.Add("label_int64_7",labels.label_int64_7);
    sql.Add("label_int64_8",labels.label_int64_8);
    sql.Add("label_int64_9",labels.label_int64_9);
    sql.Add("label_string_1",labels.label_string_1);
    sql.Add("label_string_2",labels.label_string_2);
    sql.Add("label_string_3",labels.label_string_3);
    sql.Add("label_string_4",labels.label_string_4);
    sql.Add("label_string_5",labels.label_string_5);
    sql.Add("label_string_6",labels.label_string_6);
    sql.Add("label_string_7",labels.label_string_7);
    sql.Add("label_string_8",labels.label_string_8);
    sql.Add("label_string_9",labels.label_string_9);
    STATUS ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::InsertAction(const Action& action)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("action_id",action.action_id);
    sql.Add("workflow_id",action.workflow_id);
    sql.Add("rollback_action_id",action.rollback_action_id);
    sql.Add("category",action.category);
    sql.Add("name",action.name);
    sql.Add("sender",action.sender);
    sql.Add("receiver",action.receiver);
    sql.Add("message_id",action.message_id);
    sql.Add("message_priority",action.message_priority);
    sql.Add("message_version",action.message_version);
    sql.Add("message_req",action.message_req);
    //sql.Add("message_ack",action.message_ack);
    sql.Add("success_feedback",action.success_feedback);
    sql.Add("failure_feedback",action.failure_feedback);
    sql.Add("timeout_feedback",action.timeout_feedback);
    sql.Add("state",action.state);
    sql.Add("progress",action.progress);
    sql.Add("progress_weight",action.progress_weight);
    sql.Add("timeout",action.timeout);
    sql.Add("history_repeat_count",action.history_repeat_count);
    sql.Add("skipped",action.skipped);
    sql.Add("finished",action.finished);
    sql.Add("synchronous",action.synchronous);
    //sql.Add("last_repeat_time",action.last_repeat_time);
    //sql.Add("last_repeat_time",action.last_repeat_time);
    Datetime now;
    sql.Add("create_time",now);
    sql.Add("description",action.description);
    STATUS ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::InsertActionLabels(const string& action_id,const ActionLabels& labels)
{
    SqlCommand sql(db,table_action_labels,SqlCommand::WITH_TRANSACTION);
    sql.Add("action_id",action_id);
    sql.Add("label_int64_1",labels.label_int64_1);
    sql.Add("label_int64_2",labels.label_int64_2);
    sql.Add("label_int64_3",labels.label_int64_3);
    sql.Add("label_int64_4",labels.label_int64_4);
    sql.Add("label_int64_5",labels.label_int64_5);
    sql.Add("label_int64_6",labels.label_int64_6);
    sql.Add("label_int64_7",labels.label_int64_7);
    sql.Add("label_int64_8",labels.label_int64_8);
    sql.Add("label_int64_9",labels.label_int64_9);
    sql.Add("label_string_1",labels.label_string_1);
    sql.Add("label_string_2",labels.label_string_2);
    sql.Add("label_string_3",labels.label_string_3);
    sql.Add("label_string_4",labels.label_string_4);
    sql.Add("label_string_5",labels.label_string_5);
    sql.Add("label_string_6",labels.label_string_6);
    sql.Add("label_string_7",labels.label_string_7);
    sql.Add("label_string_8",labels.label_string_8);
    sql.Add("label_string_9",labels.label_string_9);
    STATUS ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::InsertSequence(const ActionSequence& sequence)
{
    SqlCommand sql(db,table_action_sequences,SqlCommand::WITH_TRANSACTION);
    sql.Add("action_prev",sequence.action_prev);
    sql.Add("action_next",sequence.action_next);
    STATUS ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::CreateWorkflow(Action& action)
{
    Workflow workflow(action);
    workflow.engine = GetAvailableEngine();
    workflow.category = action.category;
    workflow.name = action.name;

    action.workflow_id = workflow.workflow_id;
    action.workflow_engine = workflow.engine;

    if (action.Validate() == false)
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    Transaction::Begin();
    InsertWorkflow(workflow);
    InsertWorkflowLabels(workflow.workflow_id,workflow.labels);
    InsertAction(action);
    InsertActionLabels(action.action_id,action.labels);
    STATUS ret = Transaction::Commit();
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return ret;
    }

    Debug(LOG_NOTICE,"Create workflow %s successfully! waking up engine %d...\n",
          workflow.workflow_id.c_str(),workflow.engine);
    return WakeupEngine(action.workflow_engine,action.workflow_id);
}

STATUS ActionPool::CreateWorkflow(Workflow& workflow)
{
    if (workflow.actions.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    workflow.engine = GetAvailableEngine();

    vector<Action>::iterator ita;
    for (ita = workflow.actions.begin(); ita != workflow.actions.end(); ++ita)
    {
        ita->workflow_engine = workflow.engine;
        ita->workflow_id = workflow.workflow_id;
        if (ita->Validate() == false)
        {
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
            break;
        }
    }

    if (!workflow.Validate())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    //���֮ǰ��ӡһ�¹�����������ͼ
    if (workflow_action_print_on)
    {
        workflow.Show();
    }

    Transaction::Begin();
    InsertWorkflow(workflow);
    InsertWorkflowLabels(workflow.workflow_id,workflow.labels);

    for (ita = workflow.actions.begin(); ita != workflow.actions.end(); ++ita)
    {
        InsertAction(*ita);
        InsertActionLabels(ita->action_id,ita->labels);
    }

    vector<ActionSequence>::const_iterator its;
    for (its = workflow.sequences.begin(); its != workflow.sequences.end(); ++its)
    {
        InsertSequence(*its);
    }

    STATUS ret = Transaction::Commit();
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return ret;
    }

    Debug(LOG_NOTICE,"Create workflow %s successfully! waking up engine %d...\n",
          workflow.workflow_id.c_str(),workflow.engine);
    return WakeupEngine(workflow.engine,workflow.workflow_id);
}

STATUS ActionPool::CreateWorkflow(Workflow& workflow,Workflow& rollback_workflow)
{
    if (workflow.actions.empty() || rollback_workflow.actions.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    workflow.engine = GetAvailableEngine();
    workflow.rollback_workflow_id = rollback_workflow.workflow_id;

    rollback_workflow.engine = workflow.engine;
    rollback_workflow.paused = true; //�ع������������������Ĭ����ͣ��
    //�ع��������������и���һ���Ļع�������rollback_workflow_id�ֶμ�¼�����������ع�˭��
    rollback_workflow.rollback_workflow_id = "rollbackof:" + workflow.workflow_id;

    //�ع���������������ϢĬ�ϵ���������������Ӧ����
    if(rollback_workflow.upstream_action_id.empty())
    {
        rollback_workflow.upstream_action_id = workflow.upstream_action_id;
    }

    if(rollback_workflow.upstream_sender._unit.empty())
    {
        rollback_workflow.upstream_sender = workflow.upstream_sender;
    }

    //�ع��������ķ�����Ĭ�ϵ�ͬ����������
    rollback_workflow.originator = workflow.originator;
    
    //���Ʋ�У��ع�������
    vector<Action>::iterator ita;
    for (ita = rollback_workflow.actions.begin(); ita != rollback_workflow.actions.end(); ++ita)
    {
        ita->workflow_engine = rollback_workflow.engine;
        ita->workflow_id = rollback_workflow.workflow_id;
        if (ita->Validate() == false)
        {
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
            break;
        }
    }

    if (!rollback_workflow.Validate())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    //���Ʋ�У������ҵ������
    for (ita = workflow.actions.begin(); ita != workflow.actions.end(); ++ita)
    {
        ita->workflow_engine = workflow.engine;
        ita->workflow_id = workflow.workflow_id;
        if (ita->Validate() == false)
        {
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
            break;
        }
    }

    if (!workflow.Validate())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    //���֮ǰ��ӡһ�¹�����������ͼ
    if (workflow_action_print_on)
    {
        workflow.Show();
        rollback_workflow.Show();
    }

    Transaction::Begin();
    InsertWorkflow(workflow);
    InsertWorkflowLabels(workflow.workflow_id,workflow.labels);
    for (ita = workflow.actions.begin(); ita != workflow.actions.end(); ++ita)
    {
        InsertAction(*ita);
        InsertActionLabels(ita->action_id,ita->labels);
    }

    InsertWorkflow(rollback_workflow);
    InsertWorkflowLabels(rollback_workflow.workflow_id,rollback_workflow.labels);
    for (ita = rollback_workflow.actions.begin(); ita != rollback_workflow.actions.end(); ++ita)
    {
        InsertAction(*ita);
        InsertActionLabels(ita->action_id,ita->labels);
    }

    vector<ActionSequence>::const_iterator its;
    for (its = rollback_workflow.sequences.begin(); its != rollback_workflow.sequences.end(); ++its)
    {
        InsertSequence(*its);
    }

    for (its = workflow.sequences.begin(); its != workflow.sequences.end(); ++its)
    {
        InsertSequence(*its);
    }

    STATUS ret = Transaction::Commit();
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return ret;
    }

    Debug(LOG_NOTICE,"Create workflow %s successfully! waking up engine %d...\n",
          workflow.workflow_id.c_str(),workflow.engine);
    return WakeupEngine(workflow.engine,workflow.workflow_id);
}

STATUS ActionPool::DeleteWorkflow(const string& workflow_id,bool with_rollback)
{
    //ע��:���ڹ�������ʷ��¼�������ﱣ��ģ�
    //�������ɾ���������Ĳ������ֻ�ܵ��ñ�����!!!
    Workflow workflow;
    if(SUCCESS != GetWorkflowById(workflow_id,workflow))
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    //������ȡ�����Ĺ�������Ϣ������������ʷ������Ϣ
    WorkflowHistory history(workflow);
    STATUS ret = MakeWorkflowHistory(workflow_id,history);
    if(SUCCESS != ret)
    {
        Debug(LOG_CRIT,"failed to make history of workflow %s! ret = %d\n",
            workflow_id.c_str(),ret);
        return ret;
    }
    
    ostringstream where;
    where << "WHERE workflow_id = '" << workflow_id << "'";

    if (with_rollback)
    {
        string rollback_workflow_id = GetRollbackWorkflow(workflow_id);
        if (!rollback_workflow_id.empty())
        {
            //����ù���������Ԥ��׼���õĻع���������ҲҪһ��ɾ��
            where << " OR workflow_id = '" << rollback_workflow_id << "'";
        }
    }

    Transaction::Begin();
    Debug(LOG_INFO,"save workflow history %s ...\n",history.workflow_id.c_str());
    ret = SaveWorkflowHistory(history);
    if(SUCCESS != ret)
    {
        Transaction::Cancel();
        Debug(LOG_ERR,"failed to save history of workflow %s! ret = %d\n",workflow_id.c_str(),ret);
        return ret;
    }
    
    SqlCommand sql(db,table_workflow,SqlCommand::WITH_TRANSACTION);
    ret = sql.Delete(where.str());
    if (SQLDB_OK != ret)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return ERROR_DB_DELETE_FAIL;
    }
    ret = Transaction::Commit();
    if(SUCCESS == ret)
    {
        Debug(LOG_INFO,"delete workflow %s successfully! where = %s\n",workflow_id.c_str(),where.str().c_str());
    }
    else
    {
        Debug(LOG_ERR,"failed to delete workflow %s! ret = %d\n",workflow_id.c_str(),ret);
    }
    return ret;
}

STATUS ActionPool::GetWorkflowById(const string& workflow_id,Workflow& workflow)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectWorkflowCallback),(void *)&workflow);
    sql << "SELECT " << columns_workflow << " FROM " << table_workflow
    << " WHERE workflow_id = '" << workflow_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    //���������µ�actions,sequences��labels�ݲ���ȡ��������Ҫʱ��ʵ��
    return SUCCESS;
}

int ActionPool::GetWorkflowProgress(const string& workflow_id)
{
    int progress = -1;
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(view_workflow_progress,"progress",where.str(),progress);
    if (SQLDB_OK != ret)
    {
        return -1;
    }

    return progress;
}

string ActionPool::GetRollbackWorkflow(const string& workflow_id)
{
    string rollback_workflow_id;
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"rollback_workflow_id",where.str(),rollback_workflow_id);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        Debug(LOG_ERR,"Failed to get rollback workflow id of %s, ret = %d!",workflow_id.c_str(),ret);
        SkyAssert(false);
        return "";
    }
    return rollback_workflow_id;
}

int ActionPool::GetUnfinishedActionsCount(const string& workflow_id)
{
    ostringstream oss;
    int count = -1;
    oss << " workflow_id = '" << workflow_id << "' AND finished = 0";
    int ret = SelectColumn(table_action_pool,"COUNT(*)",oss.str(),count);
    if (SQLDB_OK != ret)
    {
        Debug(LOG_ERR,"Failed to get unfinished action count! sql: %s\n",oss.str().c_str());
        SkyAssert(false);
        return -1;
    }
    return count;
}

int ActionPool::GetUnsuccessedActionsCount(const string& workflow_id)
{
    ostringstream oss;
    int count = -1;
    oss << " workflow_id = '" << workflow_id << "' AND state != 0";
    int ret = SelectColumn(table_action_pool,"COUNT(*)",oss.str(),count);
    if (SQLDB_OK != ret)
    {
        Debug(LOG_ERR,"Failed to get unsuccessed action count! sql: %s\n",oss.str().c_str());
        SkyAssert(false);
        return -1;
    }
    return count;
}

STATUS ActionPool::DeleteFinishedWorkflow(const string& where)
{
    vector<string> finished_workflows;
    ostringstream oss;
    oss << where << " AND workflow_id IN "
    << "(SELECT DISTINCT workflow_id FROM action_pool WHERE workflow_id NOT IN "
    <<"(SELECT DISTINCT workflow_id FROM action_pool WHERE finished = 0))";
    int ret = SelectColumn(table_workflow,"workflow_id",oss.str(),finished_workflows);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        //Debug("No finished workflow! sql: %s\n",oss.str().c_str());
        return SUCCESS;
    }

    if (SQLDB_OK != ret)
    {
        Debug(LOG_ERR,"Failed to find finished workflow! sql: %s\n",oss.str().c_str());
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    //���ɾ���Ѿ���ɵĹ�����
    vector<string>::iterator it;
    for (it = finished_workflows.begin(); it != finished_workflows.end(); it++)
    {
        //ע��: ��Ҫ��ͬ�ع�������һ��ɾ��
        Debug(LOG_NOTICE,"Delete finished workflow %s ...\n",it->c_str());
        DeleteWorkflow(*it,true);
    }
    return SUCCESS;
}

STATUS ActionPool::SetWorkflowPauseFlag(const string& workflow_id,bool flag)
{
    SqlCommand sql(db,table_workflow,SqlCommand::WITH_TRANSACTION);
    sql.Add("paused",flag);
    if (flag)
    {
        Datetime now;
        sql.Add("pause_time",now);
    }
    else
    {
        sql.Add("pause_time","");
    }

    int ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    return SUCCESS;
}

bool ActionPool::IsWorkflowPaused(const string& workflow_id)
{
    int paused = 0;
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"paused",where.str(),paused);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        //�Ѿ�ɾ����workflow��Ȼ��paused
        return true;
    }

    if (SQLDB_OK != ret)
    {
        //����Ķ��Բ�����!!!!
        //�����û�һ���ܲ�Ľ���û�кô�!!!
        SkyAssert(false);
        return false;
    }

    return (bool)paused;

}

STATUS ActionPool::UnsetRollbackAttr(const string& workflow_id) 
{
    //ֻ�ܶԻع����������������
    if(false == IsRollbackWorkflow(workflow_id))
    {
        Debug(LOG_ERR,"%s is not a rollback workflow! UnsetRollbackAttr is impossible!\n",workflow_id.c_str());
        SkyAssert(false);
        return ERROR_NOT_PERMITTED;
    }

    Debug(LOG_INFO,"UnsetRollbackAttr of workflow %s ...\n",workflow_id.c_str());
        
    //����������������"����ع�"���"����"
    SqlCommand sql(db,table_workflow,SqlCommand::WITH_TRANSACTION);
    sql.Add("rollback_workflow_id","");
    int ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::UnsetRollbackAttr(const string& workflow_id,const MID& upstream_sender,const string& upstream_action_id)
{
    //ֻ�ܶԻع����������������
    if(false == IsRollbackWorkflow(workflow_id))
    {
        Debug(LOG_ERR,"%s is not a rollback workflow! UnsetRollbackAttr is impossible!\n",workflow_id.c_str());
        SkyAssert(false);
        return ERROR_NOT_PERMITTED;
    }

    Debug(LOG_INFO,"UnsetRollbackAttr of workflow %s ...\n",workflow_id.c_str());
        
    //����������������"����ع�"���"����"
    //ͬʱҲ����һ��upstream_sender��upstream_action_id
    SqlCommand sql(db,table_workflow,SqlCommand::WITH_TRANSACTION);
    sql.Add("rollback_workflow_id","");
    sql.Add("upstream_sender",upstream_sender);
    sql.Add("upstream_action_id",upstream_action_id);
    int ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    return SUCCESS;
}

bool ActionPool::IsRollbackWorkflow(const string& workflow_id)
{
    string rollback_workflow_id = GetRollbackWorkflow(workflow_id);
    if (0 == rollback_workflow_id.compare(0,11,"rollbackof:"))
    {
        //��rollbackof:��ͷʱ��ʾ�������ڻع���Ĺ������Ļع�������
        return true;
    }

    return false;
}

int ActionPool::SelectWorkflowCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns || Workflow::LIMIT != columns->get_column_num())
    {
        SkyAssert(false);
        return -1;
    }
        
    Workflow* p = static_cast<Workflow*>(nil);
    columns->GetValue(Workflow::WORKFLOW_ID, p->workflow_id);
    columns->GetValue(Workflow::ROLLBACK_WORKFLOW_ID, p->rollback_workflow_id);
    columns->GetValue(Workflow::APPLICATION, p->application);
    columns->GetValue(Workflow::PROCESS, p->process);
    columns->GetValue(Workflow::CATEGORY, p->category);
    columns->GetValue(Workflow::NAME, p->name);
    columns->GetValue(Workflow::ENGINE,p->engine);
    columns->GetValue(Workflow::CREATE_TIME, p->create_time);
    columns->GetValue(Workflow::PAUSED, p->paused);
    columns->GetValue(Workflow::PAUSE_TIME, p->pause_time);
    columns->GetValue(Workflow::UPSTREAM_SENDER, p->upstream_sender);
    columns->GetValue(Workflow::UPSTREAM_ACTION_ID, p->upstream_action_id);
    columns->GetValue(Workflow::ORIGINATOR, p->originator);
    columns->GetValue(Workflow::DESCRIPTION, p->description);
    return 0;
}

int ActionPool::SelectWorkflowHistoryCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns || WorkflowHistory::LIMIT != columns->get_column_num())
    {
        SkyAssert(false);
        return -1;
    }
        
    WorkflowHistory* p = static_cast<WorkflowHistory*>(nil);
    columns->GetValue(WorkflowHistory::WORKFLOW_ID, p->workflow_id);
    columns->GetValue(WorkflowHistory::ROLLBACK_WORKFLOW_ID, p->rollback_workflow_id);
    columns->GetValue(WorkflowHistory::APPLICATION, p->application);
    columns->GetValue(WorkflowHistory::PROCESS, p->process);
    columns->GetValue(WorkflowHistory::CATEGORY, p->category);
    columns->GetValue(WorkflowHistory::NAME, p->name);
    columns->GetValue(WorkflowHistory::STATE, p->state);
    columns->GetValue(WorkflowHistory::DETAIL, p->detail);
    columns->GetValue(WorkflowHistory::PROGRESS, p->progress);
    columns->GetValue(WorkflowHistory::FAILED_ACTION_CATEGORY, p->failed_action_category);
    columns->GetValue(WorkflowHistory::FAILED_ACTION_NAME, p->failed_action_name);
    columns->GetValue(WorkflowHistory::ORIGINATOR, p->originator);
    columns->GetValue(WorkflowHistory::CREATE_TIME, p->create_time);
    columns->GetValue(WorkflowHistory::DELETE_TIME, p->delete_time);
    columns->GetValue(WorkflowHistory::DESCRIPTION, p->description);
    return 0;
}

int ActionPool::SelectActionCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns || Action::LIMIT != columns->get_column_num())
    {
        SkyAssert(false);
        return -1;
    }

    Action* p = static_cast<Action*>(nil);
    columns->GetValue(Action::ACTION_ID, p->action_id);
    columns->GetValue(Action::WORKFLOW_ID, p->workflow_id);
    columns->GetValue(Action::ROLLBACK_ACTION_ID, p->rollback_action_id);
    //columns->GetValue(APPLICATION, p->application);
    //columns->GetValue(PROCESS, p->process);
    columns->GetValue(Action::WORKFLOW_ENGINE, p->workflow_engine);
    columns->GetValue(Action::CATEGORY, p->category);
    columns->GetValue(Action::NAME,p->name);
    columns->GetValue(Action::WORKFLOW_PAUSED, p->workflow_paused);
    columns->GetValue(Action::WORKFLOW_PAUSE_TIME, p->workflow_pause_time);
    string sender;
    columns->GetValue(Action::SENDER, sender);
    p->sender.deserialize(sender);
    string receiver;
    columns->GetValue(Action::RECEIVER,receiver);
    p->receiver.deserialize(receiver);
    columns->GetValue(Action::MESSAGE_ID, p->message_id);
    columns->GetValue(Action::MESSAGE_PRIORITY, p->message_priority);
    columns->GetValue(Action::MESSAGE_VERSION, p->message_version);
    columns->GetValue(Action::MESSAGE_REQ, p->message_req);
    columns->GetValue(Action::MESSAGE_ACK_ID, p->message_ack_id);
    columns->GetValue(Action::MESSAGE_ACK_PRIORITY, p->message_ack_priority);
    columns->GetValue(Action::MESSAGE_ACK_VERSION, p->message_ack_version);
    columns->GetValue(Action::MESSAGE_ACK, p->message_ack);
    columns->GetValue(Action::SUCCESS_FEEDBACK, p->success_feedback);
    columns->GetValue(Action::FAILURE_FEEDBACK, p->failure_feedback);
    columns->GetValue(Action::TIMEOUT_FEEDBACK, p->timeout_feedback);
    columns->GetValue(Action::STATE, p->state);
    columns->GetValue(Action::PROGRESS, p->progress);
    columns->GetValue(Action::PROGRESS_WEIGHT, p->progress_weight);
    columns->GetValue(Action::TIMEOUT, p->timeout);
    columns->GetValue(Action::HISTORY_REPEAT, p->history_repeat_count);
    columns->GetValue(Action::SKIPPED, p->skipped);
    columns->GetValue(Action::SYNCHRONOUS, p->synchronous);
    columns->GetValue(Action::FINISHED, p->finished);
    columns->GetValue(Action::FINISH_TIME, p->finish_time);
    columns->GetValue(Action::LAST_REPEAT_TIME, p->last_repeat_time);
    columns->GetValue(Action::ACTION_CREATE_TIME, p->create_time);
    columns->GetValue(Action::LABEL_INT64_1, p->labels.label_int64_1);
    columns->GetValue(Action::LABEL_INT64_2, p->labels.label_int64_2);
    columns->GetValue(Action::LABEL_INT64_3, p->labels.label_int64_3);
    columns->GetValue(Action::LABEL_INT64_4, p->labels.label_int64_4);
    columns->GetValue(Action::LABEL_INT64_5, p->labels.label_int64_5);
    columns->GetValue(Action::LABEL_INT64_6, p->labels.label_int64_6);
    columns->GetValue(Action::LABEL_INT64_7, p->labels.label_int64_7);
    columns->GetValue(Action::LABEL_INT64_8, p->labels.label_int64_8);
    columns->GetValue(Action::LABEL_INT64_9, p->labels.label_int64_9);
    columns->GetValue(Action::LABEL_STRING_1, p->labels.label_string_1);
    columns->GetValue(Action::LABEL_STRING_2, p->labels.label_string_2);
    columns->GetValue(Action::LABEL_STRING_3, p->labels.label_string_3);
    columns->GetValue(Action::LABEL_STRING_4, p->labels.label_string_4);
    columns->GetValue(Action::LABEL_STRING_5, p->labels.label_string_5);
    columns->GetValue(Action::LABEL_STRING_6, p->labels.label_string_6);
    columns->GetValue(Action::LABEL_STRING_7, p->labels.label_string_7);
    columns->GetValue(Action::LABEL_STRING_8, p->labels.label_string_8);
    columns->GetValue(Action::LABEL_STRING_9, p->labels.label_string_9);
    columns->GetValue(Action::DESCRIPTION, p->description);
    return 0;
}

int ActionPool::SelectSequencesCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns || 2 != columns->get_column_num())
    {
        SkyAssert(false);
        return -1;
    }

    ActionSequence sequence;
    columns->GetValue(0, sequence.action_prev);
    columns->GetValue(1, sequence.action_next);
    vector<ActionSequence>* p = static_cast<vector<ActionSequence> *>(nil);
    p->push_back(sequence);
    return 0;
}

int ActionPool::SelectActionLabelsCallback(void *nil, SqlColumn *columns)
{
    //��ǰ��18����ǩ
    if (!nil || !columns || 18 != columns->get_column_num())
    {
        SkyAssert(false);
        return -1;
    }

    ActionLabels *p = static_cast<ActionLabels*>(nil);
    columns->GetValue(0, p->label_int64_1);
    columns->GetValue(1, p->label_int64_2);
    columns->GetValue(2, p->label_int64_3);
    columns->GetValue(3, p->label_int64_4);
    columns->GetValue(4, p->label_int64_5);
    columns->GetValue(5, p->label_int64_6);
    columns->GetValue(6, p->label_int64_7);
    columns->GetValue(7, p->label_int64_8);
    columns->GetValue(8, p->label_int64_9);
    columns->GetValue(9, p->label_string_1);
    columns->GetValue(10, p->label_string_2);
    columns->GetValue(11, p->label_string_3);
    columns->GetValue(12, p->label_string_4);
    columns->GetValue(13, p->label_string_5);
    columns->GetValue(14, p->label_string_6);
    columns->GetValue(15, p->label_string_7);
    columns->GetValue(16, p->label_string_8);
    columns->GetValue(17, p->label_string_9);
    return 0;
}

int ActionPool::SelectWorkflowLabelsCallback(void *nil, SqlColumn *columns)
{
    //��ǰ��18����ǩ
    if (!nil || !columns || 18 != columns->get_column_num())
    {
        return -1;
    }

    WorkflowLabels *p = static_cast<WorkflowLabels*>(nil);
    columns->GetValue(0, p->label_int64_1);
    columns->GetValue(1, p->label_int64_2);
    columns->GetValue(2, p->label_int64_3);
    columns->GetValue(3, p->label_int64_4);
    columns->GetValue(4, p->label_int64_5);
    columns->GetValue(5, p->label_int64_6);
    columns->GetValue(6, p->label_int64_7);
    columns->GetValue(7, p->label_int64_8);
    columns->GetValue(8, p->label_int64_9);
    columns->GetValue(9, p->label_string_1);
    columns->GetValue(10, p->label_string_2);
    columns->GetValue(11, p->label_string_3);
    columns->GetValue(12, p->label_string_4);
    columns->GetValue(13, p->label_string_5);
    columns->GetValue(14, p->label_string_6);
    columns->GetValue(15, p->label_string_7);
    columns->GetValue(16, p->label_string_8);
    columns->GetValue(17, p->label_string_9);
    return 0;
}

int ActionPool::SelectFailedActionCallback(void *nil, SqlColumn *columns)
{
    //��ǰ��6����ǩ
    if (!nil || !columns || 4 != columns->get_column_num())
    {
        return -1;
    }

    WorkflowHistory *p = static_cast<WorkflowHistory*>(nil);
    columns->GetValue(0, p->failed_action_category);
    columns->GetValue(1, p->failed_action_name);
    columns->GetValue(2, p->state);
    columns->GetValue(3, p->detail);
    return 0;
}

STATUS ActionPool::GetActionById(const string& action_id,Action& action)
{
    if (action_id.empty())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectActionCallback),(void *)&action);
    sql << "SELECT " << columns_workflow_actions << " FROM " << view_workflow_actions
    << " WHERE action_id = '" << action_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetActionCategory(const string& action_id,string& category)
{
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"category",where.str(),category);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::GetActionName(const string& action_id,string& name)
{
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"name",where.str(),name);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}
    
STATUS ActionPool::GetActionState(const string& action_id,int& state)
{
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"state",where.str(),state);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::GetWorkflowByActionId(const string& action_id,string& workflow_id)
{
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"workflow_id",where.str(),workflow_id);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetRollbackWorkflow(const string& workflow_id,string& rollback_workflow_id)
{
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"rollback_workflow_id",where.str(),rollback_workflow_id);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    if (0 == rollback_workflow_id.compare(0,11,"rollbackof:"))
    {
        rollback_workflow_id = "";
    }
    return SUCCESS;
}

STATUS ActionPool::GetRollbackAction(const string& action_id,string& rollback_action_id)
{
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"rollback_action_id",where.str(),rollback_action_id);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetAction(Action& action)
{
    if (action.action_id.empty())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectActionCallback),(void *)&action);
    sql << "SELECT " << columns_workflow_actions << " FROM " << view_workflow_actions
    << " WHERE action_id = '" << action.action_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetRunningAction(const string& where,Action& action)
{
    ostringstream oss;
    vector<string> actions;
    oss << where << " AND action_id = '" << action.action_id << "'";
    int ret = SelectColumn(view_running_actions,"action_id",oss.str(),actions);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    if (actions[0] == action.action_id)
        return GetAction(action);
    else
        return ERROR;
}

STATUS ActionPool::GetRunningActions(const string& where,vector<string>& actions)
{
    int ret = SelectColumn(view_running_actions,"action_id",where,actions);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::GetFinishedActions(const string& workflow_id,vector<string>& actions)
{
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "' AND finished = 1";
    int ret = SelectColumn(table_action_pool,"action_id",where.str(),actions);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetPausedActions(const string& workflow_id,vector<string>& actions)
{
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(view_paused_actions,"action_id",where.str(),actions);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

int ActionPool::GetEngineOfAction(const string& action_id)
{
    int engine = -1;
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(view_workflow_actions,"engine",where.str(),engine);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return -2;
    }

    if (SQLDB_OK != ret)
    {
        return -1;
    }

    return engine;
}

int ActionPool::GetEngineOfWorkflow(const string& workflow_id)
{
    int engine = -1;
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"engine",where.str(),engine);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return -2;
    }

    if (SQLDB_OK != ret)
    {
        return -1;
    }

    return engine;
}

string ActionPool::GetWorkflowIdOfAction(const string& action_id)
{
    string workflow_id;
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"workflow_id",where.str(),workflow_id);
    if (SQLDB_OK != ret)
    {
        return "";
    }

    return workflow_id;
}

STATUS ActionPool::UpdateActionState(const Action& action)
{
    Action action_old(action.action_id);
    STATUS ret = GetAction(action_old);
    if (SUCCESS != ret)
    {
        return ret;
    }

    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("message_ack_id",action.message_ack_id);
    sql.Add("message_ack_priority",action.message_ack_priority);
    sql.Add("message_ack_version",action.message_ack_version);
    sql.Add("message_ack",action.message_ack);
    sql.Add("state",action.state);
    sql.Add("progress",action.progress);
    sql.Add("history_repeat_count",action.history_repeat_count);
    sql.Add("last_repeat_time",action.last_repeat_time);
    ret = sql.Update("WHERE action_id = '" + action.action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::SetActionLabels(const string& action_id,const ActionLabels& labels)
{
    SqlCommand sql(db,table_action_labels,SqlCommand::WITH_TRANSACTION);
    sql.Add("label_int64_1",labels.label_int64_1);
    sql.Add("label_int64_2",labels.label_int64_2);
    sql.Add("label_int64_3",labels.label_int64_3);
    sql.Add("label_int64_4",labels.label_int64_4);
    sql.Add("label_int64_5",labels.label_int64_5);
    sql.Add("label_int64_6",labels.label_int64_6);
    sql.Add("label_int64_7",labels.label_int64_7);
    sql.Add("label_int64_8",labels.label_int64_8);
    sql.Add("label_int64_9",labels.label_int64_9);
    sql.Add("label_string_1",labels.label_string_1);
    sql.Add("label_string_2",labels.label_string_2);
    sql.Add("label_string_3",labels.label_string_3);
    sql.Add("label_string_4",labels.label_string_4);
    sql.Add("label_string_5",labels.label_string_5);
    sql.Add("label_string_6",labels.label_string_6);
    sql.Add("label_string_7",labels.label_string_7);
    sql.Add("label_string_8",labels.label_string_8);
    sql.Add("label_string_9",labels.label_string_9);
    STATUS ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::ModifyAction(const Action& action)
{
    //��������Ҫ��ҵ����ã�����δ����run״̬��action���б༭
    Action action_old(action.action_id);
    STATUS ret = GetAction(action_old);
    if (SUCCESS != ret)
    {
        return ret;
    }

    if (action_old.finished != 0)
    {
        //�Ѿ�������action��ֹ���޸���!!!
        return ERROR_NOT_PERMITTED;
    }

    if (action_old.history_repeat_count > 0)
    {
        //�Ѿ��ڸ��ߴ����action��ֹ���޸���!!!
        return ERROR_NOT_PERMITTED;
    }

    //��������Ǹ������û�ʹ�õģ��������е��ֶζ��ܸ�!!!
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("category",action.category);
    sql.Add("name",action.name);
    sql.Add("sender",action.sender);
    sql.Add("receiver",action.receiver);
    sql.Add("message_id",action.message_id);
    sql.Add("message_priority",action.message_priority);
    sql.Add("message_version",action.message_version);
    sql.Add("message_req",action.message_req);
    //sql.Add("message_ack",action.message_ack);
    sql.Add("success_feedback",action.success_feedback);
    sql.Add("failure_feedback",action.failure_feedback);
    sql.Add("timeout_feedback",action.timeout_feedback);
    //sql.Add("state",action.state);
    //sql.Add("progress",action.progress);
    sql.Add("timeout",action.timeout);
    //sql.Add("history_repeat_count",action.history_repeat_count);
    //sql.Add("enabled",action.enabled);
    //sql.Add("last_repeat_time",action.last_repeat_time);
    sql.Add("description",action.description);
    ret = sql.Update("WHERE action_id = '" + action.action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    return SetActionLabels(action.action_id,action.labels);
}

STATUS ActionPool::RepeatAction(Action& action)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    action.history_repeat_count ++;
    sql.Add("history_repeat_count",action.history_repeat_count);
    Datetime now;
    sql.Add("last_repeat_time",now);
    int ret = sql.Update("WHERE action_id = '" + action.action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::SetSkippedFlag(const string& action_id, bool skipped)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("skipped",skipped);
    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::FinishAction(const string& action_id)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("finished",1);
    Datetime now;
    sql.Add("finish_time",now);
    sql.Add("progress",100); //��������Ϊ100%? ��֤��������ʾ����
    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::RestartAction(const string& action_id)
{
    if (IsActionFinished(action_id))
    {
        //�Ѿ���ɵ�Action����������
        return ERROR_PERMISSION_DENIED;
    }

    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("progress",0);
    sql.Add("state",ERROR_ACTION_RUNNING);
    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

bool ActionPool::IsActionFinished(const string& action_id)
{
    int finished = 0;
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"finished",where.str(),finished);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        //�Ѿ�ɾ����Action��Ȼ����ɵ�
        return true;
    }

    if (SQLDB_OK != ret)
    {
        //����Ķ��Բ�����!!!!
        //�����û�һ���ܲ�Ľ���û�кô�!!!
        SkyAssert(false);
        return false;
    }

    return (bool)finished;
}

bool ActionPool::IsActionSynchronous(const string& action_id)
{
    int synchronous = -1;
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"synchronous",where.str(),synchronous);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        //�Ѿ�ɾ����ActionĬ����ͬ����
        return true;
    }

    if (SQLDB_OK != ret)
    {
        //����Ķ��Բ�����!!!!
        //�����û�һ���ܲ�Ľ���û�кô�!!!
        SkyAssert(false);
        return false;
    }

    return (bool)synchronous;
}

bool ActionPool::IsActionSkipped(const string& action_id)
{
    int skipped = -1;
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,"skipped",where.str(),skipped);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        //�Ѿ�ɾ����ActionĬ���Ƿ�skipped��
        return true;
    }

    if (SQLDB_OK != ret)
    {
        //����Ķ��Բ�����!!!!
        //�����û�һ���ܲ�Ľ���û�кô�!!!
        SkyAssert(false);
        return false;
    }

    return (bool)skipped;
}
    
STATUS ActionPool::SetActionFeedback(const string& action_id,int success_feedback,int failure_feedback,int timeout_feedback)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("success_feedback",success_feedback);
    sql.Add("failure_feedback",failure_feedback);
    sql.Add("timeout_feedback",timeout_feedback);
    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    //�û���Ҫ�Լ�����FinishAction����������
    return SUCCESS;
}

STATUS ActionPool::GetActionFeedback(const string& action_id,Action::ActionFeedbackTypes type,int& feedback)
{
    string field;
    switch(type)
    {
        case Action::ACT_SUCCESS:
            field = "success_feedback";
            break;
        case Action::ACT_FAILURE:
            field = "failure_feedback";
            break;
        case Action::ACT_TIMEOUT:
            field = "timeout_feedback";
            break;
            
        default:
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
    }
    
    ostringstream where;
    where << "action_id = '" << action_id << "'";
    int ret = SelectColumn(table_action_pool,field,where.str(),feedback);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::SetWorkflowUpstream(const string& workflow_id,const MID& sender,const string& action_id)
{
    SqlCommand sql(db,table_workflow,SqlCommand::WITH_TRANSACTION);
    sql.Add("upstream_sender",sender);
    sql.Add("upstream_action_id",action_id);
    int ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::GetWorkflowCategory(const string& workflow_id,string& category)
{
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"category",where.str(),category);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::GetWorkflowName(const string& workflow_id,string& name)
{
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"name",where.str(),name);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::GetUpstreamSender(const string& workflow_id,MID& upstream_sender)
{
    string sender;
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"upstream_sender",where.str(),sender);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    upstream_sender.deserialize(sender);
    return SUCCESS;
}

STATUS ActionPool::GetUpstreamActionId(const string& workflow_id,string& upstream_action_id)
{
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"upstream_action_id",where.str(),upstream_action_id);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetWorkflowByUpstreamActionId(const string& upstream_action_id,string& workflow_id)
{
    ostringstream where;
    where << "upstream_action_id = '" << upstream_action_id << "'";
    int ret = SelectColumn(table_workflow,"workflow_id",where.str(),workflow_id);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::UpdateActionReq(const string& action_id,const string& message_req)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("message_req",message_req);
    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    //�û���Ҫ�Լ�����FinishAction����������
    return SUCCESS;
}

STATUS ActionPool::UpdateActionAck(const string& action_id,const string& message_ack)
{
    WorkAck ack;
    bool ok = ack.deserialize(message_ack);
    SkyAssert(ok);
    
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("message_ack",message_ack);
    sql.Add("state",ack.state);
    sql.Add("progress",ack.progress);

    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::UpdateActionReceiver(const string& action_id,const MID& receiver)
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITH_TRANSACTION);
    sql.Add("receiver",receiver);
    int ret = sql.Update("WHERE action_id = '" + action_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }

    //�û���Ҫ�Լ�����FinishAction����������
    return SUCCESS;
}

STATUS ActionPool::GetActionsByName(const string& workflow_id,const string& action_name,vector<string>& actions)
{
    ostringstream oss;
    oss << " workflow_id = '" << workflow_id << "' AND name = '" << action_name << "'" ;
    int ret = SelectColumn(view_workflow_actions,"action_id",oss.str(),actions);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY  != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}
    
STATUS ActionPool::GetNextActions(const string& action_id,vector<string>& actions)
{
    ostringstream where;
    where << "action_prev = '" << action_id << "'";
    int ret = SelectColumn(table_action_sequences, "action_next", where.str(), actions);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY  != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetAdjacentActions(const string& action_id,vector<ActionSequence>& sequences)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectSequencesCallback),(void *)&sequences);
    sql << "SELECT action_prev,action_next FROM " << table_action_sequences
    << " WHERE action_prev = '" << action_id << "' OR action_next = '" << action_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY  != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetActionLabels(const string& action_id,ActionLabels& labels)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectActionLabelsCallback),(void *)&labels);
    sql << "SELECT " << columns_action_labels << " FROM " << table_action_labels
    << " WHERE action_id = '" << action_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetActionLabels(const string& workflow_id, const string& action_name, ActionLabels& labels)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectActionLabelsCallback),(void *)&labels);
    sql << "SELECT " << columns_action_labels << " FROM " << view_actions
    << " WHERE workflow_id = '" << workflow_id << "' AND name = '" << action_name << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetActionMessageReq(const string& workflow_id,const string& action_name,string& message)
{
    ostringstream oss;
    oss << " workflow_id = '" << workflow_id << "' AND name = '" << action_name << "'" ;
    int ret = SelectColumn(view_workflow_actions,"message_req",oss.str(),message);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::GetActionMessageReq(const string& action_id, string& message)
{
    ostringstream oss;
    oss << " action_id = '" << action_id << "'" ;
    int ret = SelectColumn(table_action_pool,"message_req",oss.str(),message);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::GetActionMessageAck(const string& workflow_id,const string& action_name,string& message)
{
    ostringstream oss;
    oss << " workflow_id = '" << workflow_id << "' AND name = '" << action_name << "'" ;
    int ret = SelectColumn(view_workflow_actions,"message_ack",oss.str(),message);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::SetWorkflowLabels(const string& workflow_id,const WorkflowLabels& labels)
{
    SqlCommand sql(db,table_workflow_labels,SqlCommand::WITH_TRANSACTION);
    sql.Add("label_int64_1",labels.label_int64_1);
    sql.Add("label_int64_2",labels.label_int64_2);
    sql.Add("label_int64_3",labels.label_int64_3);
    sql.Add("label_int64_4",labels.label_int64_4);
    sql.Add("label_int64_5",labels.label_int64_5);
    sql.Add("label_int64_6",labels.label_int64_6);
    sql.Add("label_int64_7",labels.label_int64_7);
    sql.Add("label_int64_8",labels.label_int64_8);
    sql.Add("label_int64_9",labels.label_int64_9);
    sql.Add("label_string_1",labels.label_string_1);
    sql.Add("label_string_2",labels.label_string_2);
    sql.Add("label_string_3",labels.label_string_3);
    sql.Add("label_string_4",labels.label_string_4);
    sql.Add("label_string_5",labels.label_string_5);
    sql.Add("label_string_6",labels.label_string_6);
    sql.Add("label_string_7",labels.label_string_7);
    sql.Add("label_string_8",labels.label_string_8);
    sql.Add("label_string_9",labels.label_string_9);
    STATUS ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::GetWorkflowLabels(const string& workflow_id,WorkflowLabels& labels)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectWorkflowLabelsCallback),(void *)&labels);
    sql << "SELECT " << columns_workflow_labels << " FROM " << table_workflow_labels
    << " WHERE workflow_id = '" << workflow_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::SetWorkflowOriginator(const string& workflow_id, const string& originator)
{
    SqlCommand sql(db,table_workflow,SqlCommand::WITHOUT_TRANSACTION);
    sql.Add("originator",originator);
    int ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::MakeWorkflowHistory(const string& workflow_id,WorkflowHistory& history)
{
    //û���ҵ�ʧ�ܵ�Action������ȴ����û��ɵ�Action
    //˵���������Ǳ�����������
    int count = GetUnfinishedActionsCount(workflow_id);
    if(count < 0)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    if(count == 0)
    {
        //���������������Action������ˣ�������״̬�ǳɹ�
        history.state = SUCCESS;
        history.detail = "workflow is finished.";
        history.progress = 100;
        return SUCCESS;
    }

    if(count > 0 && Transaction::On())
    {
        //������������񣬿���action�Ѿ������ˣ�ֻ��û����⣬����Ҫ���һ�¸�action�ǲ�������Ѿ��ɹ���
        map<string,string>& kv = Transaction::KeyValue();
        if(!kv.empty())
        {
            int state = SUCCESS;
            STATUS ret = ERROR;
            string action_id;
            
            map<string,string>::iterator it;
            for(it = kv.begin(); it != kv.end(); ++it)
            {
                //�������Ѿ���������û����������action�Ƿ��ǳɹ�����һ��û�ɹ����˳���
                if(it->second == workflow_id)
                {
                    action_id = it->first;
                    ret = GetActionState(action_id,state);
                    if(SUCCESS != ret)
                    {
                        return ERROR_DB_SELECT_FAIL;
                    }

                    if(SUCCESS != state)
                    {
                        break;
                    }
                }
            }
            
            if(SUCCESS == state)
            {
                //���������������Action������ˣ�����û����ʽ��⣬������״̬Ҳ�ǳɹ�
                Debug(LOG_NOTICE,"workflow %s finished in transaction(last action: %s)!\n",
                    workflow_id.c_str(),action_id.c_str());
                history.state = SUCCESS;
                history.detail = "workflow is finished.";
                history.progress = 100;
                return SUCCESS;
            }
        }
    }
    
    //��ȡ�������ĵ�ǰ����
    history.progress = GetWorkflowProgress(workflow_id);
    if(history.progress < 0)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    //����������Actionδ��ɣ��ж��Ǳ���Ϊ�жϵĻ���ʧ�ܵ�
    ostringstream oss;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectFailedActionCallback),(void *)&history);
    oss << "SELECT category,name,state,message_ack FROM " << view_workflow_actions
        << " WHERE workflow_id = '" << workflow_id << "'" 
        << " AND finished = 0 "
        << " AND state != " << SUCCESS
        << " AND state != " << ERROR_ACTION_READY
        << " AND state != " << ERROR_ACTION_RUNNING;
        
    int ret = db->Execute(oss, this);
    UnsetCallback();

    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    if (SQLDB_RESULT_EMPTY != ret)
    {
        //�ҵ����¹�����ʧ�ܵĳ���Action
        if(ERROR_ACTION_TIMEOUT == history.state)
        {
            //�����Action�ǳ�ʱ�ģ�detailֻ���Լ���д��
            history.detail = "action " + history.failed_action_category + ":" + 
                             history.failed_action_name + " timeout!";
        }
        else
        {
            //�����Ack����ȡdetail
            WorkAck ack;
            ack.deserialize(history.detail);
            history.detail = ack.detail;
        }
    }
    else
    {
        //û��ʧ�ܵ�Action��˵���ù������Ǳ�����������
        history.state = ERROR_ACTION_CANCELLED;
        history.detail = "workflow is cancelled.";
    }

    return SUCCESS;
}

STATUS ActionPool::SaveWorkflowHistory(const WorkflowHistory& history)
{   
    //�����������������ʷ��¼��
    SqlCommand sql(db,table_workflow_history,SqlCommand::WITH_TRANSACTION);
    sql.Add("workflow_id",history.workflow_id);
    sql.Add("rollback_workflow_id",history.rollback_workflow_id);
    sql.Add("category",history.category);
    sql.Add("name",history.name);
    sql.Add("upstream_sender",history.upstream_sender);
    sql.Add("upstream_action_id",history.upstream_action_id);
    sql.Add("originator",history.originator);
    sql.Add("application",history.application);
    sql.Add("process",history.process);
    sql.Add("state",history.state);
    sql.Add("detail",history.detail);
    sql.Add("progress",history.progress);
    sql.Add("failed_action_category",history.failed_action_category);
    sql.Add("failed_action_name",history.failed_action_name);
    sql.Add("create_time",history.create_time);
    Datetime now;
    sql.Add("delete_time",now);
    sql.Add("description",history.description);
    int ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }

    //���ù�������labelsҲ������Ӧ����ʷ��¼��
    SqlCommand sql2(db,table_workflow_labels_history,SqlCommand::WITH_TRANSACTION);
    ostringstream oss;
    oss << "INSERT INTO " << table_workflow_labels_history
        << " SELECT workflow_id," << columns_workflow_labels 
        << " FROM " << table_workflow_labels
        << " WHERE workflow_id = " << "'" << history.workflow_id << "'";
    ret = sql2.Execute(oss.str());
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_INSERT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::GetForwardWorkflow(const string& workflow_id,string& forward_workflow_id)
{
    //���ݻع�������rollback_workflow_id�ֶ��м�¼�Ķ�Ӧ������������id
    ostringstream where;
    forward_workflow_id = "";
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow,"rollback_workflow_id",where.str(),forward_workflow_id);
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    if (SQLDB_RESULT_EMPTY == ret)
    {
        Debug(LOG_ERR,"rollback workflow %s does not exist?!\n",workflow_id.c_str());
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    if (0 != forward_workflow_id.compare(0,11,"rollbackof:"))
    {
        //��Ӧ����һ��rollbackof:��ͷ���ַ����Ŷԣ�
        //������ǣ����ܴ����workflow_id�����Ͳ���һ���ع����ʵĹ�����
        Debug(LOG_ERR,"workflow %s is not a rollback?!\n",workflow_id.c_str());
        return ERROR_NOT_SUPPORT;
    }

    //ȥ����ͷ��"rollbackof:"�͵õ�����������id
    forward_workflow_id = forward_workflow_id.substr(11);
    return SUCCESS;
}

int ActionPool::GetWorkflowHistoryProgress(const string& workflow_id)
{
    int progress = -1;
    ostringstream where;
    where << "workflow_id = '" << workflow_id << "'";
    int ret = SelectColumn(table_workflow_history,"progress",where.str(),progress);
    if (SQLDB_OK != ret)
    {
        return -1;
    }

    return progress;
}

STATUS  ActionPool::GetWorkflowHistory(const string& workflow_id,WorkflowHistory& history)
{   
    //�ӹ�������ʷ��¼������ȡ�ù���������Ϣ
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectWorkflowHistoryCallback),(void *)&history);
    sql << "SELECT " << columns_workflow_history << " FROM " << table_workflow_history
           << " WHERE workflow_id = '" << workflow_id << "'";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        Debug(LOG_ERR,"workflow %s history is cleared?!\n",workflow_id.c_str());
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    //��ȡ��������ʷ��ǩ
    ostringstream sql2;
    SetCallback(static_cast<Callbackable::Callback>(&ActionPool::SelectWorkflowLabelsCallback),(void *)&history.labels);
    sql2 << "SELECT " << columns_workflow_labels << " FROM " << table_workflow_labels_history
            << " WHERE workflow_id = '" << workflow_id << "'";
    ret = db->Execute(sql2, this);
    UnsetCallback();
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
}

STATUS ActionPool::UpdateWorkflowHistory(const string& workflow_id,const string& detail)
{
    SqlCommand sql(db,table_workflow_history,SqlCommand::WITH_TRANSACTION);
    sql.Add("workflow_id",workflow_id);
    //sql.Add("state",state);
    sql.Add("detail",detail);
    //sql.Add("progress",progress);
    //sql.Add("failed_action_category",failed_action_category);
    //sql.Add("failed_action_name",failed_action_name);

    int ret = sql.Update("WHERE workflow_id = '" + workflow_id +"'");
    if (SQLDB_OK != ret)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

string ActionPool::FindWorkflow(const string& where)
{
    string workflow_id;
    int ret = SelectColumn(view_workflows,"workflow_id",where,workflow_id);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        Debug(LOG_ERR,"failed to find workflow! where = %s, ret = %d!\n",where.c_str(),ret);
        SkyAssert(false);
        return "";
    }

    return workflow_id;
}

STATUS ActionPool::FindWorkflow(const string& where,vector<string>& workflows)
{
    int ret = SelectColumn(view_workflows,"workflow_id",where,workflows);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        Debug(LOG_ERR,"failed to find workflow! where = %s, ret = %d!\n",where.c_str(),ret);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::FindWorkflowHistory(const string& where,vector<string>& workflows)
{
    int ret = SelectColumn(view_workflow_history,"workflow_id",where,workflows);
    if (SQLDB_OK != ret && SQLDB_RESULT_EMPTY != ret)
    {
        Debug(LOG_ERR,"failed to find workflowhistory! where = %s, ret = %d!\n",where.c_str(),ret);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

STATUS ActionPool::Vacuum()
{
    SqlCommand sql(db,table_action_pool,SqlCommand::WITHOUT_TRANSACTION);
    Debug(LOG_INFO,"vacuum table %s ...\n",table_action_pool.c_str());
    int ret = sql.Vacuum(SqlCommand::VACUUM_NORMAL);
    if (SQLDB_OK != ret)
    {
        Debug(LOG_ERR,"failed to vacuum %s! ret = %d!\n",table_action_pool.c_str(),ret);
        return ERROR_DB_UNKNOWN_FAIL;
    }

    sql.SetTable(table_workflow);
    Debug(LOG_INFO,"vacuum table %s ...\n",table_workflow.c_str());
    ret = sql.Vacuum(SqlCommand::VACUUM_NORMAL);
    if (SQLDB_OK != ret)
    {
        Debug(LOG_ERR,"failed to vacuum %s! ret = %d!\n",table_workflow.c_str(),ret);
        return ERROR_DB_UNKNOWN_FAIL;
    }
    return SUCCESS;
}

STATUS ActionPool::DeleteUuid(const string& uuid)
{
    SkyAssert(!uuid.empty());
    SqlCommand sql(db,table_uuid_history,SqlCommand::WITH_TRANSACTION);
    sql.Add("uuid",uuid);
    Datetime now;
    sql.Add("delete_time",now);
    STATUS ret = sql.Insert();
    if (SQLDB_OK != ret)
    {
        if(IsUuidDeleted(uuid))
        {
            //�Ѿ�ɾ������
            return SUCCESS;
        }
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

bool ActionPool::IsUuidDeleted(const string& uuid)
{
    SkyAssert(!uuid.empty());
    string _uuid;
    ostringstream where;
    where << "uuid = '" << uuid << "'";
    int ret = SelectColumn(table_uuid_history,"uuid",where.str(),_uuid);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return false;
    }
    if (SQLDB_OK != ret)
    {
        return true;
    }
    SkyAssert(uuid == _uuid);
    return true;
}

//ĳЩ��������μȿ��Դ���workflow_id��Ҳ���Դ����workflow���µ�����һ��action_id
//��ֻ��Ϊ�û��ṩ���뷽�㣬�ڽ���ActionPool֮ǰ����ͳһת��Ϊworkflow_id������ʹ�ú���ͳһ����
#define TO_WORKFLOW_ID(action_id,workflow_id) \
    if (true == Action::IsActionId(action_id)) \
    {\
        ActionPool *pa = ActionPool::GetInstance(); \
        SkyAssert(NULL != pa);\
        workflow_id = pa->GetWorkflowIdOfAction(action_id);\
        if (workflow_id.empty())\
        {\
            return ERROR_INVALID_ARGUMENT;\
        }\
    }\
    else\
    {\
        workflow_id = action_id;\
    }

#define MUST_BE_WORKFLOW_ID(id) \
    if (false == Workflow::IsWorkflowId(id)) \
    {\
        SkyAssert(false);\
        return ERROR_INVALID_ARGUMENT;\
    }
    
#define MUST_BE_ACTION_ID(id) \
    if (false == Action::IsActionId(id)) \
    {\
        SkyAssert(false);\
        return ERROR_INVALID_ARGUMENT;\
    }

#define MUST_BE_ACTION_ID_EX(id,ret) \
    if (false == Action::IsActionId(id)) \
    {\
        SkyAssert(false);\
        return ret;\
    }

#define CALL(call)\
    ActionPool *p = ActionPool::GetInstance();\
    SkyAssert(NULL != p);\
    return p->call;

STATUS GetActionById(const string& action_id,Action& action)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionById(action_id,action);
}

STATUS GetActionCategoryName(const string& action_id,string& category,string& name)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);


    STATUS ret = p->GetActionCategory(action_id,category);
    if(SUCCESS != ret)
    {
        return ret;
    }
    return p->GetActionName(action_id,name);
}

STATUS GetWorkflowByActionId(const string& action_id,string& workflow_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetWorkflowByActionId(action_id,workflow_id);
}

STATUS GetRollbackWorkflow(const string& action_id,string& rollback_workflow_id)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    return p->GetRollbackWorkflow(workflow_id,rollback_workflow_id);
}

STATUS GetRollbackAction(const string& action_id,string& rollback_action_id)
{
     //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
     
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetRollbackAction(action_id,rollback_action_id);
}

STATUS ModifyAction(Action& action)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->ModifyAction(action);
}

STATUS GetFinishedActions(const string& action_id,vector<string>& actions)
{   
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetFinishedActions(workflow_id,actions);
}

STATUS GetPausedActions(const string& action_id,vector<string>& actions)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetPausedActions(workflow_id,actions);
}

STATUS GetNextActions(const string& action_id, vector<string>& actions)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetNextActions(action_id,actions);
}

STATUS FinishAction(const string& action_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    //���ù������Ƿ�ֻ����һ��δ��ɵ�action������ǣ���������ɾ������������
    string workflow_id = p->GetWorkflowIdOfAction(action_id);
    if (workflow_id.empty())
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    
    int count = p->GetUnfinishedActionsCount(workflow_id);
    if(0 > count)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    
    if(0 == count)
    {
        Debug(LOG_NOTICE,"The whole workflow %s is already finished!\n",workflow_id.c_str());
        return SUCCESS;
    }


    //ע��: ��action��state�ֶ��Ѿ���UpdateActionAck�и���Ϊ���ղ������!
    //����ֻ����ʽ����ǰ��Action������������finished��finish_time�ֶ�
    STATUS ret = p->FinishAction(action_id);
    if (SUCCESS != ret)
    {
        return ret;
    }

    if (workflow_action_print_on)
    {
        if (IsActionFinished(action_id))
            Debug(LOG_NOTICE,"action %s is finished!\n",action_id.c_str());
    }

    //��������������������֮����Ҫ�ڵ���p->FinishAction֮ǰ�ͻ�ȡcount��
    //����Ϊ�������п��ܱ������ڸ���һ������ݿ�������
    if(1 == count)
    {
        Debug(LOG_NOTICE,"The last action(%s) of workflow %s is finished!\n", action_id.c_str(),workflow_id.c_str());
        Transaction::KeyValue().clear();
        //��������action��¼�����������ĵĻ����У���ֹ�������Ǳ��������ʽ��װʹ��
        Transaction::KeyValue().insert(make_pair(action_id,workflow_id));
        ret = p->DeleteWorkflow(workflow_id,true);
        Transaction::KeyValue().clear();
        return ret;
    }

    SkyAssert(0 < count);
    
    //����������л���������Actionδ��ɣ��ͻ�������
    //��������Ǹ��û�ʹ�õģ���Ҫ�������¼�������
    int engine = p->GetEngineOfAction(action_id);
    if (-2 == engine)
    {
        //�������Ѿ������һ��action���������������Ѿ���ɾ����
        //���������ٸ���
        return SUCCESS;
    }
    else if (0 > engine)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return WakeupEngine(engine,workflow_id);
}

STATUS RestartAction(const string& action_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    STATUS ret = p->RestartAction(action_id);
    if (SUCCESS != ret)
    {
        return ret;
    }

    Debug(LOG_NOTICE,"action %s is restarted!\n",action_id.c_str());
    //��������Ǹ��û�ʹ�õģ���Ҫ�������¼�������
    int engine = p->GetEngineOfAction(action_id);
    if (0 > engine)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    string workflow_id = p->GetWorkflowIdOfAction(action_id);
    if (workflow_id.empty())
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return WakeupEngine(engine,workflow_id);
}

STATUS SkipAction(const string& action_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->SetSkippedFlag(action_id,true);
}

STATUS UpdateActionReq(const string& action_id, const Serializable& message_req)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    //������Ϣ��Я����action_id�����б�
    WorkReq req;
    req.deserialize(message_req.serialize());
    SkyAssert(req.action_id == action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UpdateActionReq(action_id,message_req.serialize());
}

STATUS UpdateActionReq(const string& action_id, const string& message_req)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    //������Ϣ��Я����action_id�����б�
    WorkReq req;
    req.deserialize(message_req);
    SkyAssert(req.action_id == action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UpdateActionReq(action_id,message_req);
}

STATUS UpdateActionAck(const string& action_id,const string& message_ack)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);

    if(IsActionFinished(action_id))
    {
        return ERROR_NOT_PERMITTED;
    }

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UpdateActionAck(action_id,message_ack);
}

STATUS UpdateActionReceiver(const string& action_id, const MID& receiver)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UpdateActionReceiver(action_id,receiver);
}

STATUS SetActionFeedback(const string& action_id,int success_feedback,int failure_feedback,int timeout_feedback)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->SetActionFeedback(action_id,success_feedback,failure_feedback,timeout_feedback);
}

STATUS GetActionFeedback(const string& action_id,Action::ActionFeedbackTypes type,int& feedback)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionFeedback(action_id,type,feedback);
}

bool IsActionSynchronous(const string& action_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID_EX(action_id,false);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->IsActionSynchronous(action_id);
}

bool IsActionFinished(const string& action_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID_EX(action_id,false);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->IsActionFinished(action_id);
}

bool IsActionSkipped(const string& action_id)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID_EX(action_id,false);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->IsActionSkipped(action_id);
}

bool IsSupportRollback(const string& action_id)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    string rollback_workflow_id = p->GetRollbackWorkflow(workflow_id);
    if (rollback_workflow_id.empty())
    {
        //û�лع����������ã���֧�ֻع�
        return false;
    }
    
    return true;
}

bool IsRollbackWorkflow(const string& action_id)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->IsRollbackWorkflow(workflow_id);
}

   
STATUS UnsetRollbackAttr(const string& action_id) 
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UnsetRollbackAttr(workflow_id);
}

STATUS UnsetRollbackAttr(const string& action_id,const MID& upstream_sender,const string& upstream_action_id)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UnsetRollbackAttr(workflow_id,upstream_sender,upstream_action_id);
}

int GetWorkflowProgress(const string& action_id)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);

    //��ǰ������������action����������?
    int unfinished_count = p->GetUnfinishedActionsCount(workflow_id);
    if(0 > unfinished_count)
    {
        //SkyAssert(false);
        return -1;
    }

    //�ȼ��һ�µ�ǰ�Ĺ���������
    int current_progress = p->GetWorkflowProgress(workflow_id);
    if(0 > current_progress)
    {
        //SkyAssert(false);
        return -1;
    }

    int progress = 0;
    if(!p->IsRollbackWorkflow(workflow_id))
    {
        progress = current_progress;
    }
    else
    {
        //������Ǹ��ع�����������Ҫ������Ӧ�����������Ľ�����Ҳ����
        string forward_workflow_id;
        STATUS ret = p->GetForwardWorkflow(workflow_id, forward_workflow_id);
        if(SUCCESS != ret)
        {
            //SkyAssert(false);
            return -1;
        }

        int forward_progress = p->GetWorkflowHistoryProgress(forward_workflow_id);
        if(0 > forward_progress)
        {
            SkyAssert(false);
            return -1;
        }

        //�ع����1:��������Ȼ������ǰ�ߣ����޽ӽ���100
        //progress =  current_progress*(100 - forward_progress)/100 + forward_progress;

        //�ع����2:��������ʼ���ˣ�ֱ���˵�0Ϊֹ���ع�����
        progress = forward_progress - (forward_progress * current_progress)/100;
    }

    //�������actionû����ɣ�����100%ҲҪ��Ϊ��99%
    if(unfinished_count > 0 && 100 == progress)
    {
        return 99;
    }

    return progress;
}

STATUS GetActionLabels(const string& action_id, ActionLabels& labels)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionLabels(action_id,labels);
}

STATUS GetActionLabels(const string& workflow_id, const string& action_name, ActionLabels& labels)
{
    //���workflow_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string _workflow_id;
    TO_WORKFLOW_ID(workflow_id,_workflow_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionLabels(_workflow_id,action_name,labels);
}

STATUS SetActionLabels(const string& action_id, const ActionLabels& labels)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->SetActionLabels(action_id,labels);
}

STATUS SetActionLabels(const string& workflow_id, const string& action_name, const ActionLabels& labels)
{
    //���workflow_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string _workflow_id;
    TO_WORKFLOW_ID(workflow_id,_workflow_id);

    vector<string> actions;
    STATUS ret = GetActionsByName(_workflow_id,action_name,actions);
    if(SUCCESS != ret)
    {
        return ret;
    }

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    vector<string>::iterator it;
    for(it = actions.begin(); it != actions.end(); it++)
    {
        ret = p->SetActionLabels(*it,labels);
        if(SUCCESS != ret)
        {
            return ret;
        }
    }

    return SUCCESS;
}

STATUS GetActionMessageReq(const string& workflow_id, const string& action_name, string& message)
{
    //���workflow_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string _workflow_id;
    TO_WORKFLOW_ID(workflow_id,_workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionMessageReq(_workflow_id,action_name,message);
}

//����action��id����ȡ��action��������Ϣ
STATUS GetActionMessageReq(const string& action_id, string& message)
{
    //���ֻ���Ǹ�action id
    MUST_BE_ACTION_ID(action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionMessageReq(action_id,message);
}

STATUS GetActionMessageAck(const string& workflow_id, const string& action_name, string& message)
{
    //���workflow_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string _workflow_id;
    TO_WORKFLOW_ID(workflow_id,_workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionMessageAck(_workflow_id,action_name,message);
}

STATUS GetActionsByName(const string& workflow_id, const string& action_name, vector<string>& actions)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string _workflow_id;
    TO_WORKFLOW_ID(workflow_id,_workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionsByName(_workflow_id,action_name,actions);
}

STATUS GetUpstreamSender(const string& action_id,MID& upstream_sender)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetUpstreamSender(workflow_id,upstream_sender);
}

STATUS GetUpstreamActionId(const string& action_id, string& upstream_action_id)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetUpstreamActionId(workflow_id,upstream_action_id);
}
int GetUnsuccessedActionsCount(const string& action_id)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetUnsuccessedActionsCount(workflow_id);
}
STATUS GetWorkflowByUpstreamActionId(const string& upstream_action_id,string& workflow_id)
{
    //���ֻ���Ǹ�action id������ֻ���Ǹ�workflow id
    MUST_BE_ACTION_ID(upstream_action_id);
    
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetWorkflowByUpstreamActionId(upstream_action_id,workflow_id);
}

STATUS GetWorkflowCategoryName(const string& action_id,string& category,string& name)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    STATUS ret = p->GetWorkflowCategory(workflow_id,category);
    if(SUCCESS != ret)
    {
        return ret;
    }
    return p->GetWorkflowName(workflow_id,name);
}

STATUS GetWorkflowLabels(const string& action_id, WorkflowLabels& labels)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetWorkflowLabels(workflow_id,labels);
}

STATUS SetWorkflowLabels(const string& action_id, const WorkflowLabels& labels)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->SetWorkflowLabels(workflow_id,labels);
}

STATUS SetWorkflowOriginator(const string& action_id, const string& originator)
{
    //���action_id������ĳ��workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->SetWorkflowOriginator(workflow_id,originator);
}

STATUS UpdateWorkflowHistory(const string& workflow_id,const string& detail)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->UpdateWorkflowHistory(workflow_id,detail);
}

STATUS GetForwardWorkflowHistory(const string& action_id,WorkflowHistory& history)
{
    //���action_id������ĳ���ع�workflow��id��Ҳ�����Ǹ�workflow���µ�����һ��action��id
    string workflow_id;
    TO_WORKFLOW_ID(action_id,workflow_id);

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    
    //�ȵõ�����������id
    string forward_workflow_id;
    STATUS ret = p->GetForwardWorkflow(workflow_id,forward_workflow_id);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //�ٸ�������������id��ȡ����ʷ��¼
    return p->GetWorkflowHistory(forward_workflow_id,history);
}

STATUS FindWorkflowHistory(vector<string>& workflows,const string& category, const string& name,const string& label_where)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
     
    ostringstream sql;
    sql  << " category = '" << category << "'"
            << " AND name = '" << name << "'"
            << " AND " << label_where;

    return p->FindWorkflowHistory(sql.str(),workflows);
}

STATUS FindWorkflow(vector<string>& workflows,const string& category, const string& name,const string& label_where)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    
    ostringstream sql;
    sql  << " paused = 0 "
            << " AND category = '" << category << "'"
            << " AND name = '" << name << "'"
            << " AND " << label_where;
           
    return p->FindWorkflow(sql.str(),workflows);
}

STATUS FindWorkflow(vector<string>& workflows,const string& category, const string& name,const string& label_where,bool is_rollback)
{
    ostringstream sql;
    sql << " paused = 0 "
            << " AND category = '" << category << "'"
            << " AND name = '" << name << "'"
            << " AND " << label_where;
    if(is_rollback)
    {
        sql << " AND rollback_workflow_id LIKE 'rollbackof:'";
    }

    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->FindWorkflow(sql.str(),workflows);
}

string FindWorkflow(const string& category, const string& name,const string& label_where)
{
    vector<string> workflows;
    STATUS ret = FindWorkflow(workflows,category,name,label_where);
    if(SUCCESS != ret || workflows.empty())
    {
        return "";
    }

    if(!workflows.empty())
    {
        return workflows[0];
    }
           
    return "";
}

string FindWorkflow(const string& category, const string& name,const string& label_where,bool is_rollback)
{
    vector<string> workflows;
    STATUS ret = FindWorkflow(workflows,category,name,label_where,is_rollback);
    if(SUCCESS != ret || workflows.empty())
    {
        return "";
    }

    if(!workflows.empty())
    {
        return workflows[0];
    }
           
    return "";
}

string FindWorkflow(const string& where)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->FindWorkflow(where);
}

STATUS DeleteUuid(const string& uuid)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->DeleteUuid(uuid);
}

bool IsUuidDeleted(const string& uuid)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->IsUuidDeleted(uuid);
}

STATUS GetActionState(const string& action_id,int& state)
{
    ActionPool *p = ActionPool::GetInstance();
    SkyAssert(NULL != p);
    return p->GetActionState(action_id,state);
}

void DbgShowAction(const char* action_id)
{
    if (!action_id)
    {
        printf("No action id?!\n");
        return;
    }

    if (true != Action::IsActionId(action_id))
    {
        printf("string(%s) is NOT a valid action id!\n",action_id);
        return;
    }

    Action action;
    ActionPool *_action_pool = ActionPool::GetInstance();
    SkyAssert(NULL != _action_pool);
    STATUS ret = _action_pool->GetActionById(string(action_id),action);
    if (SUCCESS != ret)
    {
        printf("failed to get action by id(%s)!\n",action_id);
        return;
    }

    action.Show();
}
DEFINE_DEBUG_FUNC(DbgShowAction);

void DbgShowRunningActions()
{
    ActionPool *_action_pool = ActionPool::GetInstance();
    ostringstream where;
    where << "application = '" << ApplicationName() << "' AND process = '" << ProcessName() << "'";
    vector<string> running_actions;
    _action_pool->GetRunningActions(where.str(),running_actions);

    if (running_actions.empty())
    {
        cout << "no running actions." << endl;
        return;
    }

    vector<string>::iterator it;
    for (it = running_actions.begin(); it != running_actions.end(); ++it)
    {
        Action action;
        if (SUCCESS != _action_pool->GetActionById(*it,action))
        {
            cerr << "failed to get action: " << action.get_id() << endl;
            break;
        }

        action.Show();
    }
}
DEFINE_DEBUG_FUNC(DbgShowRunningActions);

void DbgUpdateWorkflowHistory(const char* action_id,const char* detail)
{
    if(!action_id || !detail)
    {
        return;
    }

    STATUS ret = UpdateWorkflowHistory(action_id,detail);
    cout << "UpdateWorkflowHistory return: " << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgUpdateWorkflowHistory);   

void DbgDeleteUuid(const char* uuid)
{
    STATUS ret = DeleteUuid(uuid);
    cout << "DeleteUuid return: " << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgDeleteUuid);

void DbgIsUuidDeleted(const char* uuid)
{
    bool ret = IsUuidDeleted(uuid);
    cout << "IsUuidDeleted return: " << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgIsUuidDeleted);

void DbgSetWorkflowOriginator(const char* action_id,const char* originator)
{
    if(!action_id || !originator)
    {
        cerr << "invalid input!" << endl;
        return;
    }
    
    STATUS ret = SetWorkflowOriginator(action_id,originator);
    cout << "SetWorkflowOriginator return: " << ret << endl;
}
DEFINE_DEBUG_FUNC(DbgSetWorkflowOriginator);


