/*******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�test_workflow.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�����������������֤����
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
#include "postgresql_db.h"
#include "workflow.h"

#define EV_TEST_REQ_1          1001
#define EV_TEST_REQ_2          1002
#define EV_TEST_REQ_3          1003
#define EV_TEST_REQ_4          1004
#define EV_TEST_REQ_5          1005
#define EV_TEST_REQ_6          1006
#define EV_TEST_REQ_7          1007
#define EV_TEST_REQ_8          1008
#define EV_TEST_REQ_9          1009

#define EV_TEST_ACK        1500

#define EV_POWER_ON          2000
#define EV_WORKFLOW_1        2001
#define EV_WORKFLOW_2        2002
#define EV_WORKFLOW_3        2003
#define EV_WORKFLOW_4        2004
#define EV_WORKFLOW_5        2005

static int workflow_test_print_on = 1;  
DEFINE_DEBUG_VAR(workflow_test_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(workflow_test_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

static const char *workflow_bootstrap = "CREATE TABLE workflow(  \
    workflow_id character varying(64) NOT NULL,  \
    engine integer NOT NULL DEFAULT 0,  \
    paused integer NOT NULL DEFAULT 0,  \
    create_time character varying(32) NOT NULL,  \
    pause_time character varying(32),  \
    CONSTRAINT pk_workflow_0 PRIMARY KEY (workflow_id))";

static const char *actions_bootstrap = "CREATE TABLE action_pool ( \
    action_id character varying(64) NOT NULL, \
    workflow_id character varying(64) NOT NULL, \
    category character varying(256) NOT NULL, \
    name character varying(256) NOT NULL, \
    application character varying(64) NOT NULL, \
    process character varying(64) NOT NULL, \
    engine integer NOT NULL DEFAULT 0, \
    sender character varying(512) NOT NULL, \
    receiver character varying(512) NOT NULL, \
    message_id integer NOT NULL, \
    message_priority integer NOT NULL DEFAULT 0, \
    message_version integer NOT NULL DEFAULT 0, \
    message_req text, \
    message_ack_id integer NOT NULL DEFAULT 0, \
    message_ack_priority integer NOT NULL DEFAULT 0, \
    message_ack_version integer NOT NULL DEFAULT 0, \
    message_ack text, \
    success_feedback integer NOT NULL DEFAULT 1, \
    failure_feedback integer NOT NULL DEFAULT 1, \
    timeout_feedback integer NOT NULL DEFAULT 0, \
    state integer NOT NULL DEFAULT 1, \
    progress integer NOT NULL DEFAULT 0, \
    timeout integer NOT NULL DEFAULT 0, \
    history_repeat_count integer NOT NULL DEFAULT 0, \
    skipped integer NOT NULL DEFAULT 0, \
    finished integer NOT NULL DEFAULT 0, \
    finish_time character varying(32), \
    last_repeat_time character varying(32), \
    create_time character varying(32) NOT NULL, \
    label_int64_1 numeric(19,0), \
    label_int64_2 numeric(19,0), \
    label_int64_3 numeric(19,0), \
    label_string_1 character varying(512), \
    label_string_2 character varying(512), \
    label_string_3 character varying(512), \
    description character varying(1024), \
    CONSTRAINT pk_action_pool PRIMARY KEY (action_id), \
    CONSTRAINT fk_action_pool FOREIGN KEY (workflow_id) \
    REFERENCES workflow (workflow_id) MATCH SIMPLE \
    ON UPDATE NO ACTION ON DELETE CASCADE)";

static const char *action_sequences_bootstrap = "CREATE TABLE action_sequences(  \
    action_prev character varying(64) NOT NULL,  \
    action_next character varying(64) NOT NULL,  \
    CONSTRAINT idx_action_sequences PRIMARY KEY (action_prev, action_next),  \
    CONSTRAINT fk_action_sequences_0 FOREIGN KEY (action_prev)      \
    REFERENCES action_pool (action_id) MATCH SIMPLE      \
    ON UPDATE NO ACTION ON DELETE CASCADE,  \
    CONSTRAINT fk_action_sequences_1 FOREIGN KEY (action_next)      \
    REFERENCES action_pool (action_id) MATCH SIMPLE      \
    ON UPDATE NO ACTION ON DELETE CASCADE)";

static const char* workflow_actions_bootstrap = "CREATE OR REPLACE VIEW workflow_actions AS  \
    SELECT a.action_id, a.workflow_id, w.engine, w.paused AS workflow_paused, \
    w.create_time AS workflow_create_time, w.pause_time AS workflow_pause_time, \
    a.category, a.name, a.application, a.process, a.sender, a.receiver, \
    a.message_id, a.message_priority, a.message_version, a.message_req, \
    a.message_ack_id, a.message_ack_priority, a.message_ack_version, a.message_ack, \
    a.success_feedback, a.failure_feedback, a.timeout_feedback, \
    a.state, a.progress, a.timeout, a.history_repeat_count, a.skipped, a.finished, \
    a.create_time AS action_create_time, a.last_repeat_time, a.finish_time, \
    a.label_int64_1, a.label_int64_2, a.label_int64_3, \
    a.label_string_1, a.label_string_2, a.label_string_3, \
    a.description  \
    FROM action_pool a, workflow w  WHERE a.workflow_id::text = w.workflow_id::text;";

static const char* workflow_progress_bootstrap = "CREATE OR REPLACE VIEW workflow_progress AS \
    SELECT action_pool.workflow_id, sum(action_pool.progress) / count(*) AS progress \
    FROM action_pool  GROUP BY action_pool.workflow_id;";

static const char *valid_action_sequences_bootstrap = "CREATE OR REPLACE VIEW valid_action_sequences AS \
    SELECT action_sequences.action_prev, action_sequences.action_next  \
    FROM action_sequences  WHERE (action_sequences.action_prev::text IN ( SELECT action_pool.action_id   \
    FROM action_pool WHERE action_pool.finished = 0)) AND (action_sequences.action_next::text IN \
    (SELECT action_pool.action_id FROM action_pool WHERE action_pool.finished = 0));";

static const char *running_actions_bootstrap = "CREATE OR REPLACE VIEW running_actions AS  \
    SELECT a.action_id, a.workflow_id, a.workflow_engine, a.application, a.process  \
    FROM workflow_actions a   LEFT JOIN valid_action_sequences s ON a.action_id::text = s.action_next::text \
    WHERE s.action_prev IS NULL AND a.workflow_paused = 0 AND a.finished = 0;";

static const char *paused_actions_bootstrap = "CREATE OR REPLACE VIEW paused_actions AS \
    SELECT a.action_id, a.workflow_id, a.workflow_engine, a.application, a.process  \
    FROM workflow_actions a   LEFT JOIN valid_action_sequences s ON a.action_id::text = s.action_next::text \
    WHERE s.action_prev IS NULL AND a.workflow_paused = 1 AND a.finished = 0;";

static SqlDB *s_pTestDbConnection = NULL;

static SqlDB *GetDB()
{
    if(!s_pTestDbConnection)
    {
        string database("sky");
        s_pTestDbConnection = new PostgreSqlDB("127.0.0.1",5432,"tecs","tecs",database);
    }
    return s_pTestDbConnection;
}
            
class CreateDiskReq: public WorkReq
{
public:
    string disk;
    int64 size;
    CreateDiskReq():disk("noname")
    {
        size = 0;
    }
    
    CreateDiskReq(const string& action_id):WorkReq(action_id),disk("noname")
    {
        size = 0;
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CreateDiskReq);
        WRITE_VALUE(disk);
        WRITE_VALUE(size);
        SERIALIZE_PARENT_END(WorkReq);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CreateDiskReq);
        READ_VALUE(disk);
        READ_VALUE(size);
        DESERIALIZE_PARENT_END(WorkReq); 
    }
};

class DeleteDiskReq: public WorkReq
{
public:
    string disk;
    DeleteDiskReq():disk("noname")
    {
    }
    
    DeleteDiskReq(const string& action_id):WorkReq(action_id),disk("noname")
    {
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(DeleteDiskReq);
        WRITE_VALUE(disk);
        SERIALIZE_PARENT_END(WorkReq);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeleteDiskReq);
        READ_VALUE(disk);
        DESERIALIZE_PARENT_END(WorkReq); 
    }
};

class DiskAck: public WorkAck
{
public:
    string disk;
    DiskAck():disk("noname")
    {
    }
    
    DiskAck(const string& action_id):WorkAck(action_id),disk("noname")
    {
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(DiskAck);
        WRITE_VALUE(disk);
        SERIALIZE_PARENT_END(WorkAck);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DiskAck);
        READ_VALUE(disk);
        DESERIALIZE_PARENT_END(WorkAck); 
    }
};
    
class StorageManager: public MessageHandlerTpl<MessageUnit>
{
public:
    StorageManager()
    {
        progress[EV_TEST_REQ_1] = 0;
        progress[EV_TEST_REQ_2] = 0;
        progress[EV_TEST_REQ_3] = 0;
        progress[EV_TEST_REQ_4] = 0;
        progress[EV_TEST_REQ_5] = 0;
        progress[EV_TEST_REQ_6] = 0;
        progress[EV_TEST_REQ_7] = 0;
    };
    
    void MessageEntry(const MessageFrame& message)
    {
        bool ret;
        int message_id = message.option.id();
        switch (message.option.id())
        {
            case EV_TEST_REQ_1:
            case EV_TEST_REQ_2:
            case EV_TEST_REQ_3:
            {
                CreateDiskReq req;
                ret = req.deserialize(message.data);
                if(!ret)
                {
                    cout << "unexpected req: " << endl << message.data << endl;
                    SkyAssert(false);
                }
                //Debug("StorageManager receives an action: %s\n",req.action_id.c_str());

                progress[message_id] += 60;
                DiskAck ack(req.action_id);
                
                if(progress[message_id] < 100)
                {
                    ack.state = ERROR_ACTION_RUNNING;
                    //ע�⣬��ʶ�������б���ʹ�����������ר�õĴ�����!!!
                    ack.progress = progress[message_id];
                }
                else
                {
                    ack.state = SUCCESS;
                    ack.progress = 100;
                    progress[message_id] = 0;
                }

                MessageOption option(message.option.sender(),EV_TEST_ACK,0,0);
                m->Send(ack,option);
                break;
            }
            
            case EV_TEST_REQ_4:
            {
                #if 0//������ģ�ⳬʱ
                #else
                WorkReq req;
                req.deserialize(message.data);
                DiskAck ack(req.action_id);
                ack.state = ERROR;
                ack.detail = "something error? just a test!";
                MessageOption option(message.option.sender(),EV_TEST_ACK,0,0);
                m->Send(ack,option);
                #endif
                break;
            }
            
            case EV_TEST_REQ_5:
            case EV_TEST_REQ_6:
            case EV_TEST_REQ_7:
            {
                DeleteDiskReq req;
                ret = req.deserialize(message.data);
                if(!ret)
                {
                    cout << "unexpected req: " << endl << message.data << endl;
                    SkyAssert(false);
                }
                //Debug("StorageManager receives an action: %s\n",req.action_id.c_str());

                progress[message_id] += 24;
                DiskAck ack(req.action_id);
                if(progress[message_id] < 100)
                {
                    ack.state = ERROR_ACTION_RUNNING; 
                    //ע�⣬��ʶ�������б���ʹ�����������ר�õĴ�����!!!
                    ack.progress = progress[message_id];
                }
                else
                {
                    ack.state = SUCCESS;
                    ack.progress = 100;
                    progress[message_id] = 0;
                }
                
                MessageOption option(message.option.sender(),EV_TEST_ACK,0,0);
                m->Send(ack,option);
                break;
            }
            
            case EV_TEST_REQ_8:
            {
                WorkReq req;
                req.deserialize(message.data);
                DiskAck ack(req.action_id);
                ack.state = ERROR;
                ack.progress = 90;               
                MessageOption option(message.option.sender(),EV_TEST_ACK,0,0);
                m->Send(ack,option);
                break;
            }
            
            default:
                break;
        }
    }
    
private:
    map<int,int> progress;
};

class VMManagerWorkflowLabels: public WorkflowLabels
{
public:
    VMManagerWorkflowLabels():
        vid(label_int64_1),l_vid("label_int64_1"),
        hid(label_int64_2),l_hid("label_int64_2"),
        description(label_string_1),l_description("label_string_1")
    {
        //�������ڵ������������õķ�ʽ��labelȡһ��ҵ���Լ�������
        //���ڱ���ʱʶ���ʹ��
        label_int64_1 = -1;
        label_int64_2 = -1;
    };

    string get_label_where() const
    {
        ostringstream where;
        where << l_vid << "=" << vid << " AND "
                      << l_hid << "=" << hid;
        return where.str();
    }
    
    int64& vid;
    string l_vid;
    
    int64& hid;
    string l_hid;
    
    string& description;
    string l_description;
};

class StepALabels: public ActionLabels
{
public:
    StepALabels():
        disk(label_string_1),l_disk("label_string_1"),
        size(label_int64_1),l_size("label_int64_1")
    {
        //�������ڵ������������õķ�ʽ��labelȡһ��ҵ���Լ�������
        //���ڱ���ʱʶ���ʹ��
    };

    string& disk;
    string l_disk;
    
    int64& size;
    string l_size;
};

class VMManager: public MessageHandlerTpl<MessageUnit>
{
public:
    VMManager()
    {
        db = NULL;
    }
    
    static VMManager* GetInstance()
    {
        if (NULL == instance)
        {
            instance = new VMManager();
        }
        return instance;
    };

    STATUS Init()
    {
        db = GetDB();
        if(!db)
        {
            return ERROR;
            SkyAssert(false);
        }
        return SUCCESS;
    }
    
    void PowerOn()
    {
        //���Լ���һ���ϵ�֪ͨ��Ϣ
        DemoMessage msg("Power on!");
        MessageOption option("VMManager",EV_POWER_ON,0,0);
        MessageFrame frame(msg,option);
        m->Receive(frame);
        //cout << m->name() << " will power on ... "<< endl;
    };

    STATUS CreateWorkflow1()
    {
            printf("==============workflow test: EV_WORKFLOW_1==============\n");
            //���幤�����ĵ�һ����Ϣ
            Action action1("storage","step_a",EV_TEST_REQ_1,MID(m->name()),MID("StorageManager"));                
            CreateDiskReq req(action1.get_id());
            req.disk = "disk1";
            req.size = 2048;
            action1.message_req = req.serialize();
            StepALabels step_A_labels;
            step_A_labels.disk = req.disk;
            step_A_labels.size = req.size;
            action1.labels = step_A_labels;
            
            //����������, ����һ����򵥵Ĺ�������ֻ��һ��action
            Workflow w(action1);
            w.category = "storage";
            w.name = "disk";
            w.originator = "user1";
            VMManagerWorkflowLabels wf_labels;
            wf_labels.vid = 12345;
            wf_labels.hid = 67890;
            w.labels = wf_labels;
            return CreateWorkflow(w);
    }

    STATUS CreateWorkflow2(const MID& upstream_sender)
    {
            printf("==============workflow test: EV_WORKFLOW_2==============\n");
            //���幤�����ĵ�һ����Ϣ
            Action action1("storage","step_a",EV_TEST_REQ_1,MID(m->name()),MID("StorageManager"));                
            CreateDiskReq req(action1.get_id());
            req.disk = "disk1";
            req.size = 2048;
            action1.message_req = req.serialize();
            StepALabels step_A_labels;
            step_A_labels.disk = req.disk;
            step_A_labels.size = req.size;
            action1.labels = step_A_labels;
                
            //���幤�����ĵڶ�����Ϣ
            Action action2("storage","step_b",EV_TEST_REQ_2,MID(m->name()),MID("StorageManager"));
            CreateDiskReq req2(action2.get_id());
            req2.disk = "disk2";
            action2.message_req = req2.serialize();

            //���幤�����ĵ�������Ϣ
            Action action3("storage","step_c",EV_TEST_REQ_3,MID(m->name()),MID("StorageManager"));
            CreateDiskReq req3(action3.get_id());
            req3.disk = "disk3";
            action3.message_req = req3.serialize();
            action3.synchronous = false; //�첽�����ȴ����
            
            //���幤�����ĵ�������Ϣ
            Action action4("storage","step_d",EV_TEST_REQ_4,MID(m->name()),MID("StorageManager"));
            action4.timeout = 10; //ʮ���ӳ�ʱ
            DeleteDiskReq req4(action4.get_id());
            req4.disk = "disk4";
            action4.message_req = req4.serialize();

            //���幤�����ĵ�������Ϣ
            Action action5("storage","step_e",EV_TEST_REQ_5,MID(m->name()),MID("StorageManager"));
            DeleteDiskReq req5(action5.get_id());
            req5.disk = "disk5";
            action5.message_req = req5.serialize();

            //���幤�����ĵ�������Ϣ
            Action action6("storage","step_f",EV_TEST_REQ_6,MID(m->name()),MID("StorageManager"));
            DeleteDiskReq req6(action6.get_id());
            req5.disk = "disk6";
            action6.message_req = req6.serialize();
            action6.skipped = true; //��Action�ᱻ�Զ��������
            
            //���幤�����ĵ�������Ϣ
            Action action7("storage","step_g",EV_TEST_REQ_7,MID(m->name()),MID("StorageManager"));
            DeleteDiskReq req7(action7.get_id());
            req7.disk = "disk7";
            action7.message_req = req7.serialize();
            
            //����������
            Workflow w("storage","disk");
            w.Add(&action1,&action2);
            w.Add(&action1,&action3);
            w.Add(&action2,&action4);
            w.Add(&action2,&action3);
            //w.Add(&action3,&action2,ERROR);
            w.Add(&action3,&action5);
            w.Add(&action3,&action6);
            w.Add(&action4,&action3);
            //w.Add(&action7,&action3);
            w.Add(action7);

            //��¼���ε���Ϣ����������ֻ�ǵ���ģ�⣬��ʵû��upstream_action_id
            w.upstream_action_id = "no action id";
            w.upstream_sender = upstream_sender;
            w.originator = "user2";

            VMManagerWorkflowLabels wf_labels;
            wf_labels.hid = 12345;
            wf_labels.vid = 54321;
            wf_labels.description = "this is a workflow";
            w.labels = wf_labels;
            return CreateWorkflow(w);
    }

    STATUS CreateWorkflow3(const MID& upstream_sender)
    {
            printf("==============workflow test: EV_WORKFLOW_3==============\n"); 
            //���������������ĵ�һ����Ϣ
            Action action1("storage","step_a",EV_TEST_REQ_3,MID(m->name()),MID("StorageManager"));                
            CreateDiskReq req(action1.get_id());
            req.disk = "disk1";
            req.size = 2048;
            action1.message_req = req.serialize();

            //���������������ĵڶ�����Ϣ
            Action action2("storage","step_b",EV_TEST_REQ_4,MID(m->name()),MID("StorageManager"));
            CreateDiskReq req2(action2.get_id());
            req2.disk = "disk2";
            action2.message_req = req2.serialize();
            action2.timeout = 3; //������󽫻ᳬʱ�����»ع�
            
            //����ع��������ĵ�һ����Ϣ
            Action action2_rollback("storage","step_b_rollback",EV_TEST_REQ_5,MID(m->name()),MID("StorageManager"));                
            DeleteDiskReq req_rollback(action2_rollback.get_id());
            req_rollback.disk = "disk2";
            action2_rollback.message_req = req_rollback.serialize();

            //�ع�ActionĬ�϶���skipped��
            action2_rollback.skipped = true;
            //�������������е�action2�Ļع�action����Ϊaction2_rollback
            action2.rollback_action_id = action2_rollback.get_id();

            //����ع��������ĵڶ�����Ϣ
            Action action1_rollback("storage","step_a_rollback",EV_TEST_REQ_6,MID(m->name()),MID("StorageManager"));
            DeleteDiskReq req2_rollback(action1_rollback.get_id());
            req2_rollback.disk = "disk1";
            action1_rollback.message_req = req2_rollback.serialize();

            //�ع�ActionĬ�϶���skipped��
            action1_rollback.skipped = true;
            //�������������е�action1�Ļع�action����Ϊaction1_rollback
            action1.rollback_action_id = action1_rollback.get_id();

            //����ع�������
            Workflow w_rollback("storage","disk_rollback");
            w_rollback.Add(&action2_rollback,&action1_rollback);
            
            //��������������
            Workflow w("storage","disk");
            w.Add(&action1,&action2);

            //��¼���ε���Ϣ����������ֻ�ǵ���ģ�⣬��ʵû��upstream_action_id
            w.upstream_action_id = "no action id";
            w.upstream_sender = upstream_sender;
            w.originator = "user3";
            
            //���������������ͻع���������������֤
            return CreateWorkflow(w,w_rollback);
    }

    STATUS CreateWorkflow4(const MID& upstream_sender)
    {
            printf("==============workflow test: EV_WORKFLOW_4==============\n");
            //���������������ĵ�һ����Ϣ
            Action action0("storage","step_x",EV_TEST_REQ_4,MID(m->name()),MID("StorageManager"));
            CreateDiskReq req0(action0.get_id());
            req0.disk = "disk0";
            action0.message_req = req0.serialize();
            action0.progress_weight = 2;
            action0.timeout = 6;
            
            //���������������ĵڶ�����Ϣ
            Action action1("storage","step_a",EV_TEST_REQ_1,MID(m->name()),MID("StorageManager"));                
            CreateDiskReq req(action1.get_id());
            req.disk = "disk1";
            req.size = 2048;
            action1.message_req = req.serialize();
            action1.progress_weight = 8;

            #if 1
            //���������������ĵ�������Ϣ
            Action action2("storage","step_b",EV_TEST_REQ_4,MID(m->name()),MID("StorageManager"));
            CreateDiskReq req2(action2.get_id());
            req2.disk = "disk2";
            action2.message_req = req2.serialize();
            action2.progress_weight = 2;
            action2.timeout = 6;
            #else
            //���������������ĵڶ�����Ϣ
            Action action2("storage","step_B",EV_TEST_REQ_2,MID(m->name()),MID("StorageManager"));
            CreateDiskReq req2(action2.get_id());
            req2.disk = "disk2";
            action2.message_req = req2.serialize();
            #endif
            
            //����ع��������ĵ�һ����Ϣ
            Action action2_rollback("storage","step_b_rollback",EV_TEST_REQ_5,MID(m->name()),MID("StorageManager"));                
            DeleteDiskReq req_rollback(action2_rollback.get_id());
            req_rollback.disk = "disk2";
            action2_rollback.message_req = req_rollback.serialize();
            action2_rollback.progress_weight = action2.progress_weight;

            //�ع�ActionĬ�϶���skipped��
            action2_rollback.skipped = true;
            //�������������е�action2�Ļع�action����Ϊaction2_rollback
            action2.rollback_action_id = action2_rollback.get_id();

            //����ع��������ĵڶ�����Ϣ
            Action action1_rollback("storage","step_a_rollback",EV_TEST_REQ_6,MID(m->name()),MID("StorageManager"));
            DeleteDiskReq req2_rollback(action1_rollback.get_id());
            req2_rollback.disk = "disk1";
            action1_rollback.message_req = req2_rollback.serialize();
            action1_rollback.progress_weight = action1.progress_weight;

            //�ع�ActionĬ�϶���skipped��
            action1_rollback.skipped = true;
            
            //�������������е�action1�Ļع�action����Ϊaction1_rollback
            action1.rollback_action_id = action1_rollback.get_id();
            //�������������е�action2�Ļع�action����Ϊaction2_rollback
            action2.rollback_action_id = action2_rollback.get_id();

            VMManagerWorkflowLabels wf_labels;
            wf_labels.vid = 12345;
            wf_labels.hid = 67890;
            
            //����ع�������
            Workflow w_rollback("storage","disk_rollback");
            w_rollback.labels = wf_labels;
            w_rollback.Add(&action2_rollback,&action1_rollback);
            
            //��������������
            Workflow w("storage","disk");
            w.labels = wf_labels;
            w.Add(&action0,&action1);
            w.Add(&action1,&action2);

            //��¼���ε���Ϣ����������ֻ�ǵ���ģ�⣬��ʵû��upstream_action_id
            w.upstream_action_id = "no action id";
            w.upstream_sender = upstream_sender;
            w.originator = "user4";
            
            //���������������ͻع���������������֤
            return CreateWorkflow(w,w_rollback);
    }
    
    STATUS CreateWorkflow5()
    {
            printf("==============workflow test: EV_WORKFLOW_5==============\n");
            //���幤�����ĵ�һ����Ϣ
            Action action1("storage","step_a",EV_TEST_REQ_8,MID(m->name()),MID("StorageManager"));                
            CreateDiskReq req(action1.get_id());
            req.disk = "disk1";
            req.size = 2048;
            //ʧ����Ҫ�Զ�����
            req.retry = true; 
            action1.failure_feedback = false;
            
            action1.message_req = req.serialize();
            StepALabels step_A_labels;
            step_A_labels.disk = req.disk;
            step_A_labels.size = req.size;
            action1.labels = step_A_labels;
                
            //����������, ����һ����򵥵Ĺ�������ֻ��һ��action
            Workflow w(action1);
            w.category = "storage";
            w.name = "disk";
            VMManagerWorkflowLabels wf_labels;
            wf_labels.vid = 12345;
            wf_labels.hid = 67890;
            w.labels = wf_labels;
            w.originator = "user5";
            
            return CreateWorkflow(w);
    }
    
    void MessageEntry(const MessageFrame& message)
    {
        STATUS ret = ERROR;
        //�����յ�����Ϣid���д���
        switch (message.option.id())
        {
            case EV_POWER_ON:
            {
                ret = Transaction::Enable(db);
                SkyAssert(SUCCESS == ret);
                break;
            }
            
            case EV_WORKFLOW_1:
            {
                ret = CreateWorkflow1();
                SkyAssert(SUCCESS == ret);
                break;
            }   
            
            case EV_WORKFLOW_2:
            {
                ret = CreateWorkflow2(message.option.sender());
                SkyAssert(SUCCESS == ret);
                break;
            }

            case EV_WORKFLOW_3:
            {
                ret = CreateWorkflow3(message.option.sender());
                SkyAssert(SUCCESS == ret);
                break;
            }

            case EV_WORKFLOW_4:
            {
                ret = CreateWorkflow4(message.option.sender());
                SkyAssert(SUCCESS == ret);
                break;
            }
            
            case EV_WORKFLOW_5:
            {
                ret = CreateWorkflow5();
                SkyAssert(SUCCESS == ret);
                break;
            }
            
            case EV_TEST_ACK:
            {
                STATUS ret = ERROR;
                //�յ�Ӧ���ˣ�������������ģ��receiver����ҵ��Ӧ����Ϣ
                DiskAck ack;
                ack.deserialize(message.data);               
                
                if(true == IsActionFinished(ack.action_id))
                {
                    //�п����յ��Ѿ���ʱ��Action������һ��Ҫ�ȹ���һ��
                    break;
                }

                //���ֽӿ���ʾ
                //����action_id��ȡAction
                Action action;
                if(SUCCESS != GetActionById(ack.action_id,action))
                {
                    break;
                }
                
                Debug("VMManager receives action %s(%s) feedback!\n",
                    ack.action_id.c_str(),
                    action.name.c_str());

                string workflow_id;
                ret = GetWorkflowByActionId(ack.action_id,workflow_id);
                SkyAssert(SUCCESS == ret);
                
                //��ȡ��ǰ��������category��name
                string workflow_category,workflow_name;
                ret = GetWorkflowCategoryName(ack.action_id, workflow_category,workflow_name);
                SkyAssert(SUCCESS == ret);
                
                Debug("Current workflow(%s) category: %s, name: %s!\n",
                    workflow_id.c_str(),workflow_category.c_str(),workflow_name.c_str());
                
                //����Action�����ֲ��ұ��������е�Action
                vector<string> actions;
                ret = GetActionsByName(ack.action_id,action.name,actions);
                if(SUCCESS == ret && !(actions.empty()) )
                {
                    Debug("this workflow has %d actions named %s!\n",(int)actions.size(),action.name.c_str());
                }
                else
                {
                    Debug("this workflow has no actions named %s! ret = %d\n",action.name.c_str(),ret);
                }

                Debug("====SetWorkflowLabels====\n");
                //����workflow�ı�ǩ
                VMManagerWorkflowLabels labels1;
                labels1.vid = 2222;
                labels1.hid = 3333;
                ret = SetWorkflowLabels(ack.action_id, labels1);
                SkyAssert(SUCCESS == ret);

                //��ȡworkflow�ı�ǩ
                VMManagerWorkflowLabels labels2;
                ret = GetWorkflowLabels(ack.action_id, labels2);
                if(SUCCESS != ret)
                {
                    SkyAssert(false);
                }
                SkyAssert(labels2.vid ==  2222);
                SkyAssert(labels2.hid == 3333);

                Debug("====SetActionLabels====\n");
                //����action�ı�ǩ
                StepALabels labels3;
                labels3.disk = "hdc"; 
                labels3.size = 40960; 
                ret = SetActionLabels(ack.action_id, action.name, labels3);
                SkyAssert(SUCCESS == ret);

                //��ȡaction�ı�ǩ
                StepALabels labels4;
                ret  = GetActionLabels(ack.action_id,action.name, labels4);
                SkyAssert(SUCCESS == ret);
                SkyAssert(labels4.disk ==  "hdc");
                SkyAssert(labels4.size == 40960);

                //��ȡ��ǰ��������Ӧ�Ļع�������
                string rollback_workflow_id;
                ret = GetRollbackWorkflow(ack.action_id,rollback_workflow_id);
                if(SUCCESS == ret && !rollback_workflow_id.empty())
                {
                    Debug("this workflow has a rollback: %s!\n",rollback_workflow_id.c_str());
                }

                //���ҹ������Ƿ����
                VMManagerWorkflowLabels labels5;
                labels5.vid = 12345;
                labels5.hid = 67890;
                
                workflow_id = FindWorkflow("storage","disk",labels5.get_label_where(),false);
                if(!workflow_id.empty())
                {
                    Debug("Yes, a disk workflow is running, id = %s!\n",workflow_id.c_str());
                }
                else
                {
                    Debug("No disk workflow!\n");
                }
                
                //ͨ��ack.state����֪����ʧ�ܻ��ǳɹ�
                if(SUCCESS == ack.state)
                {
                    Debug("action %s success!\n",ack.action_id.c_str());
                }
                else if(ERROR_ACTION_RUNNING == ack.state)
                {
                    Debug("action %s is already running, ok!\n",ack.action_id.c_str());
                }
                else
                {
                    Debug("action %s failed!\n",ack.action_id.c_str());

                    //���ͷ��Լ�Ҳ���Ը���һ��Ӧ��������д����ϸ�Ĵ�����Ϣ
                    ack.detail += ", this is more detail written by action sender!";
                    UpdateActionAck(ack.action_id,ack.serialize());
                    
                    //ĳ������ʧ���ˣ��ܻع��ͻع���
                    string rollback_workflow_id;
                    ret = GetRollbackWorkflow(ack.action_id, rollback_workflow_id);
                    SkyAssert(SUCCESS == ret);
                        //��Ȼ���سɹ��ˣ����ǻ�Ҫ�ж�һ��rollback_workflow_id�Ƿ�Ϊ��
                        if(!rollback_workflow_id.empty())
                        {
                            Debug("OK! Let's rollback workflow %s!\n",ack.action_id.c_str());
                            RollbackWorkflow(ack.action_id);
                        }
                        else
                        {
                            Debug("No rollback support, delete workflow %s!\n",ack.action_id.c_str());
                            ret = DeleteWorkflow(ack.action_id);
                            SkyAssert(SUCCESS == ret);
                        }
                    return;
                }
                
                //���������Ǹ��ع����ʵĹ�������Ҳ�в��������������
                if(IsRollbackWorkflow(ack.action_id))
                {
                    Debug("This is a rollback workflow: %s!\n",ack.action_id.c_str());
                    //�ڻع����������������л�ȡ��Ӧ������������ʷ��¼��������ʧ�ܵ�ԭ��
                    WorkflowHistory history;
                    ret = GetForwardWorkflowHistory(ack.action_id,history);
                    SkyAssert(SUCCESS == ret);
                    history.Show();

                    //�鿴һ�»ع��������Ľ��ȣ�����ʵ���ϰ�����������������������
                    int progress = GetWorkflowProgress(ack.action_id);
                    SkyAssert(progress >= 0);
                    Debug("This rollback workflow has a progress: %d!\n",progress);
                    
                    //ȡ���ع����ԣ����ع���������Ϊ����������ע��ҲҪ����upstream_sender��upstream_action_id!!!
                    Debug("UnsetRollbackAttr ...\n");
                    UnsetRollbackAttr(ack.action_id,MID("new_storage_manager"),"a-aabbccdd");
                    MID upstream_sender;
                    string upstream_action_id;
                    ret = GetUpstreamSender(ack.action_id,upstream_sender);
                    SkyAssert(SUCCESS == ret);
                    Debug("Now upstream_sender = %s ...\n",upstream_sender.serialize().c_str());
                    
                    ret = GetUpstreamActionId(ack.action_id,upstream_action_id);
                    SkyAssert(SUCCESS == ret);
                    SkyAssert(upstream_action_id == "a-aabbccdd");
                    
                    if(IsRollbackWorkflow(ack.action_id))
                    {
                        SkyAssert(false);
                    }
                    else
                    {
                        //�ٿ�һ�·��������Ľ���
                        Debug("Now, this is not a rollback workflow!\n");
                        progress = GetWorkflowProgress(ack.action_id);
                        SkyAssert(progress >= 0);
                        Debug("This backward workflow has a progress: %d!\n",progress);
                    }
                }
 
                VMManagerWorkflowLabels wf_labels;
                ret = GetWorkflowLabels(ack.action_id,wf_labels);
                SkyAssert(SUCCESS == ret);
                //wf_labels.Print();

                StepALabels labels;
                ret = GetActionLabels(ack.action_id,action.name,labels);
                SkyAssert(SUCCESS == ret);
                //labels.Print();
                
                //���ڳɹ���ʧ����֮��Ҫ��Щʲô����ҵ���Լ����Ű��!!!
                //��ʾ: һ���������ɹ��˶����ջ��Ͷ��ɹ���ʧ���˶���������������                
                //todo: ?

                //�������ˣ����������������
                Transaction::Begin();
                cout << "===finish action in transaction!===" << endl;
                FinishAction(ack.action_id);
                Transaction::Commit();
                break;
            }

            case EV_ACTION_TIMEOUT:
            {
                //EV_ACTION_TIMEOUT�����Ϣ����ͳһ����ģ�����ʹ���˹�������Ҫ����
                //��ҵ������ʱ�ˣ����ڸ���������ҵ��Ӧ������ֻ����WorkAck�����л�
                WorkAck ack;
                ack.deserialize(message.data);
                Action action;
                if(SUCCESS != GetActionById(ack.action_id,action))
                {
                    //�п����յ��Ѿ���ʱ��Action������һ��Ҫ�ȹ���һ��
                    break;
                }
                
                Debug("VMManager receives action %s(%s) timeout feedback!\n",
                    ack.action_id.c_str(),
                    action.name.c_str());
                //��ʱҪ��Щʲô����ҵ���Լ����Ű��!!!

                if(IsRollbackWorkflow(ack.action_id))
                {
                    Debug("This is a rollback workflow!\n");
                }
                
                if(IsSupportRollback(ack.action_id))
                {
                    //��Ȼ��ʱ�ˣ������������Ϊ�ܻع��Ļ������Իع�һ��
                    Debug("VMManager will rollback workflow of action: %s!\n",ack.action_id.c_str());
                    ret = RollbackWorkflow(ack.action_id);
                    if(SUCCESS != ret)
                    {
                        Debug("VMManager failed to rollback workflow of action: %s!\n",ack.action_id.c_str());
                        break;
                    }

                    //������RollbackWorkflow֮��Ͳ���Ҫ�ٵ���FinishAction��
                    Debug("VMManager rollback workflow of action: %s successfully!\n",ack.action_id.c_str());
                }
                else
                {
                    //��ʱҲֻ���Ǵ������ˣ���ǿ��������������ڰ�
                    //���������ʾ�ã�һ������³�ʱ�ˣ��ǲ��ܾ�������ǿ����ɵ�!!!
                    FinishAction(ack.action_id);
                }
            }
            
            default:
                break;
        }
    };

private:
    static VMManager *instance;
    SqlDB *db;
};

VMManager* VMManager::instance = NULL;

STATUS CreateWorkflowTables(SqlDB *db)
{
    int ret = db->Open();
    if (0 != ret)
    {
        SkyAssert(false);
        return NULL;
    }

    //����workflow��
    ostringstream oss1(workflow_bootstrap);
    ret = db->Execute(oss1);
    if (0 != ret)
    {
        SkyAssert(false);
    }
    
    //����actions��
    ostringstream oss2(actions_bootstrap);
    ret = db->Execute(oss2);
    if (0 != ret)
    {
        SkyAssert(false);
    }

    //����actions��ϵ��
    ostringstream oss3(action_sequences_bootstrap);
    ret = db->Execute(oss3);
    if (0 != ret)
    {
        SkyAssert(false);
    }

    ostringstream oss4(workflow_actions_bootstrap);
    ret = db->Execute(oss4);
    if (0 != ret)
    {
        SkyAssert(false);
    }
    
    ostringstream oss5(valid_action_sequences_bootstrap);
    ret = db->Execute(oss5);
    if (0 != ret)
    {
        SkyAssert(false);
    }

    ostringstream oss6(running_actions_bootstrap);
    ret = db->Execute(oss6);
    if (0 != ret)
    {
        SkyAssert(false);
    }
    
    ostringstream oss7(paused_actions_bootstrap);
    ret = db->Execute(oss7);
    if (0 != ret)
    {
        SkyAssert(false);
    }

    ostringstream oss8(workflow_progress_bootstrap);
    ret = db->Execute(oss8);
    if (0 != ret)
    {
        SkyAssert(false);
    }
    return ret;
}

int main( int argc , char *argv[])
{
    //��ʼ��sky��Ĳ��֣�ÿ����������ֻҪͳһ����һ�ξͿ�����
    SkyConfig config;
    //application�ֶ���TC����cloud�����֣���CC����cluster�����֣���HC����host������
    config.messaging.application = "mycloud";
    //���������exe���ж������ģ�飬��������Ҫ�ۺ���Щģ����ص�����һ���и����Եĺ��ʵ�����
    config.messaging.process = "workflow_test";
    //��Ϣ�������ĵ�ַ�Ͷ˿ں�
    //config.messaging.broker_url = "localhost";
    //config.messaging.broker_port = 5672;
    //��Ϣ������������ѡ��
    //config.messaging.broker_option = "";

    ConnectConfig connect;
    connect.broker_url = "localhost";
    connect.broker_port = 5672;
    connect.broker_option = "";
    config.messaging.connects.push_back(connect);
    config.timecfg.tv_msec = 100;

    config.exccfg.debug = true;
    
    //ֻ�������ʼ���ɹ��ˣ����ܴ�����������Ϣ��Ԫ
    if (SUCCESS != SkyInit(config))
    {
        printf("sky init failed!\n");
        return -1;
    }

    STATUS ret = CreateTransactionKey();
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        SkyExit(-1,"CreateTransactionKey failed!\n");
    }

    #if 0
    if (SUCCESS != CreateWorkflowTables(db))
    {
        printf("failed to CreateWorkflowTables!\n");
        return -1;
    }
    #endif
    
    if (SUCCESS != StartWorkflowEngine(GetDB(),3))
    {
        printf("failed to start workflow engine!\n");
        return -1;
    }

    VMManager *pv = VMManager::GetInstance();
    if (SUCCESS != pv->Start("VMManager"))
    {
        printf("VMManager start failed!\n");
        return -1;
    }
    
    //֪ͨ�ϵ�!
    pv->Init();
    pv->PowerOn();

    StorageManager *ps = new StorageManager;
    ps->Start("StorageManager");

    Shell("workflow-> ");
    while (1)
    {
        sleep(1);
    }
    return 0;
}

void DbgTestWorkflow(int event)
{
    int message_id = EV_WORKFLOW_1;
    if(0 != event)
    {
        message_id = event;
    }
    MessageOption option("VMManager",message_id,0,0);
    MessageUnit::SendFromTmp("test!",option);
}
DEFINE_DEBUG_FUNC(DbgTestWorkflow);

void DbgTestAllWorkflow()
{
    DbgTestWorkflow(2001);
    DbgTestWorkflow(2002);
    DbgTestWorkflow(2003);
    DbgTestWorkflow(2004);
    DbgTestWorkflow(2005);
}
DEFINE_DEBUG_CMD(go,DbgTestAllWorkflow);

void DbgTestUuid()
{
    Timespan interval;
    interval.Begin();
    string uuid = GenerateUUID();
    EXIT_POINT();
    interval.End();
    interval.ShowSpan();
    printf("UUID = %s\n", uuid.c_str());
}
DEFINE_DEBUG_FUNC(DbgTestUuid);

void DbgShowWorkflowHistory(const char* category,const char* name)
{
    vector<string> workflows;
    string where("1=1");

    if (SUCCESS != Transaction::Enable(GetDB()))
    {
        cerr << "Transaction::Enable failed!" << endl;
        return;
    }
    
    Transaction::Begin();
    STATUS ret = FindWorkflowHistory(workflows,category,name,where);
    if(SUCCESS != ret)
    {
        cerr << " Failed to FindWorkflowHistory! ret = " << ret << endl;
        return;
    }
    cout << "Before Transaction::Commit, workflows.count = " << workflows.size() << endl;
    Transaction::Commit();
    cout << "After Transaction::Commit, workflows.count = " << workflows.size() << endl;
    return;
}
DEFINE_DEBUG_FUNC(DbgShowWorkflowHistory);


