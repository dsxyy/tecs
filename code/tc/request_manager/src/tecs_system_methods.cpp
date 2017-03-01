/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�tecs_system_method.cpp
* �ļ���ʶ��
* ����ժҪ��tecsϵͳ���������ʵ���ļ�
* ��ǰ�汾��1.0 
* ��    �ߣ����Ľ�
* ������ڣ�2012��7��5��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/7/5
*    �� �� �ţ�V1.0
*    �� �� �ˣ����Ľ�
*    �޸����ݣ�����
*
* �޸ļ�¼2��
*    �޸����ڣ�2012/7/10
*    �� �� �ţ�V1.1
*    �� �� �ˣ����Ľ�
*    �޸����ݣ���set_runtime_state������ʵ���ƶ������ļ���,
*              ��Ϊ������Կ���һЩ�ڲ��ĺ궨�壬��S_Startup/S_Work
*******************************************************************************/
#include "tecs_system_methods.h"
#include <boost/algorithm/string.hpp>
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "usergroup.h"
#include "usergroup.h"
#include "license_common.h"
#include "vnet_libnet.h"



using namespace boost;
using namespace xmlrpc_c;

static int sysinfo_print = 0;  
static int sysinfo_timer_wait_len = 5000;
DEFINE_DEBUG_VAR(sysinfo_timer_wait_len);
DEFINE_DEBUG_VAR(sysinfo_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(sysinfo_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

//��tecs�ڲ��������Ϣ��Ԫ����ʱ��Ϣ���ݽṹת��Ϊ�����api�ṹ
ApiMuRuntimeInfo::ApiMuRuntimeInfo(const MuRuntimeInfo& info)
{
    name = info.name;
    exist = info.exist;
    age = info.age;
    if(exist)
    {
        state = info.state;
        pid = info.pid;
        push_count = info.push_count;
        pop_count = info.pop_count;
        queued_count = info.queued_count;
        last_message = info.last_message;
        timer_count = info.timer_count;
    }
}

void ApiMuRuntimeInfo::set_runtime_state()
{
    runtime_state = RUNTIME_STATE_NORMAL;
    if(exist == false)
    {
        if(age < 3000)
        {
            //�ԴӵǼ�Ϊ�ؼ���Ϣ��Ԫ֮��û�г���3���ӣ�״̬Ϊ��Ҫ��ע
            runtime_state = RUNTIME_STATE_NOTICE;
        }
        else
        {
            //����3������Ϣ��Ԫ��Ȼ������ʱ��״̬Ϊ����
            runtime_state = RUNTIME_STATE_ERROR;
        }
        //������Ϣ������Ч�ģ��Ͳ����ټ����
        return;
    }

    if(queued_count >= 3)
    {
        //����Ϣ��Ԫ��ѹ����Ϣ��������3����״̬Ϊ�澯
        runtime_state = MAX(runtime_state,RUNTIME_STATE_WARNNING);
    }

    if(queued_count >= 1)
    {
        //����Ϣ��Ԫ��ѹ����Ϣ��������1����״̬Ϊ��Ҫע��
        runtime_state = MAX(runtime_state,RUNTIME_STATE_NOTICE);
    }
   
    switch(state)
    {
        case S_Startup:
            runtime_state = MAX(runtime_state,RUNTIME_STATE_NOTICE);
            break;
            
        case S_Work:
            runtime_state = MAX(runtime_state,RUNTIME_STATE_NORMAL);
            break;
            
        case S_Exit:
            runtime_state = MAX(runtime_state,RUNTIME_STATE_WARNNING);
            break;
            
        default:
            runtime_state = MAX(runtime_state,RUNTIME_STATE_ERROR);
            break;
    }
}

//��tecs�ڲ�����Ľ�������ʱ��Ϣ���ݽṹת��Ϊ�����api�ṹ
ApiProcRuntimeInfo::ApiProcRuntimeInfo(const ProcRuntimeInfo& info)
{
    name = info.name;
    exist = info.exist;
    if(exist)
    {
        pid = info.pid;
        running_seconds = info.running_seconds;
        exceptions = info.exceptions;
        asserts = info.asserts;
        vector<MuRuntimeInfo>::const_iterator it;
        for(it = info.muinfo.begin(); it != info.muinfo.end(); it++)
        {
            ApiMuRuntimeInfo mu(*it);
            Debug("api Sys message: %d\n" 
                "state:%d\n"
                "pid:%d\n"
                "tid:0x%lx\n"
                "push_count:%llu\n"
                "pop_count:%llu\n"
                "queued_count:%d\n"
                "timer_count:%d\n"
                "last_message:%d!\n",
                (*it).exist,(*it).state,(*it).pid,(*it).tid,
                (*it).push_count,(*it).pop_count,(*it).queued_count,
                (*it).timer_count,(*it).last_message);
            muinfo.push_back(mu);
        }
    }
}

void ApiProcRuntimeInfo::set_runtime_state()
{
    runtime_state = RUNTIME_STATE_NORMAL;
    if(exist == false)
    {
        //����û��Ӧ��״̬Ϊ����
        runtime_state = RUNTIME_STATE_ERROR;
        //������Ϣ������Ч�ģ��Ͳ����ټ����
        return;
    }

    if(exceptions > 0 || !asserts.empty())
    {
        runtime_state = RUNTIME_STATE_ERROR;
        //�������쳣�����ԣ���Ϣ��Ԫ��״̬����Ҫ��飬
        //������Ȼ�Ѿ���error������Ĺ�������ʡ��
    }
    
    //���������и�����Ϣ��Ԫ��������״̬�����̵�״̬ȡ���������ص�
    vector<ApiMuRuntimeInfo>::iterator it;
    int mu_state = RUNTIME_STATE_NORMAL;
    for(it = muinfo.begin(); it != muinfo.end(); it ++)
    {
        it->set_runtime_state();
        mu_state = it->runtime_state;
        runtime_state = MAX(runtime_state,mu_state);
    }
}
    
//��tecs�ڲ������application�ڵ�����ʱ��Ϣ���ݽṹת��Ϊ�����api�ṹ 
ApiAppRuntimeInfo::ApiAppRuntimeInfo(const AppRuntimeInfo& info)
{
    name = info.name;
    collect_time = info.dt.tointeger();
    vector<ProcRuntimeInfo>::const_iterator it;
    for(it = info.procinfo.begin(); it != info.procinfo.end(); it++)
    {
        ApiProcRuntimeInfo proc(*it);
        procinfo.push_back(proc);
    }
}
            
void ApiAppRuntimeInfo::set_runtime_state()
{
    runtime_state = RUNTIME_STATE_NORMAL;
    int proc_state = RUNTIME_STATE_NORMAL;
   
    //�����������̣�������״̬��application��״̬ȡ���������ص�
    vector<ApiProcRuntimeInfo>::iterator it;
    for(it = procinfo.begin(); it != procinfo.end(); it ++)
    {
        it->set_runtime_state();
        proc_state = it->runtime_state;
        runtime_state = MAX(runtime_state,proc_state);
    }
}
        
static STATUS GetSysLicense(ApiLicenseInfo& lic)
{
    License tecs_license = license();
    lic.customer_name = tecs_license.customer_name;
    lic.customer_company = tecs_license.customer_company;
    lic.customer_address = tecs_license.customer_address;
    lic.customer_phone = tecs_license.customer_phone;
    lic.customer_email = tecs_license.customer_email;
    lic.customer_description = tecs_license.customer_description;
    lic.max_created_vms = tecs_license.max_created_vms;
    lic.max_deployed_vms = tecs_license.max_deployed_vms;
    lic.max_images = tecs_license.max_images;
    lic.max_users = tecs_license.max_users;
    lic.max_clusters = tecs_license.max_clusters;
    lic.effective_days = tecs_license.effective_days;
    lic.expired_date = tecs_license.expired_date;
    lic.remaining_days = license_days_remained();
    lic.hardware_fingerprints = tecs_license.hardware_fingerprints;
    //lic.special_cpus = tecs_license.special_cpus;
    
    RunningStat stat;
    GetRunningStat(stat);
    lic.current_created_vms = stat.created_vms;
    lic.current_deployed_vms = stat.deployed_vms;
    lic.current_images = stat.images;
    lic.current_users = stat.users;
    lic.current_clusters = stat.clusters;   
    
    return SUCCESS;
}

namespace zte_tecs
{
STATUS SysRuntimeInfoQueryMethod::GetSysInfo(const string& type,const string& target,AppRuntimeInfo& app_running_info)
{
	MessageUnit tmu(TempUnitName("sysinfo"));
    tmu.Register();

    if(type.empty())
    {
        return ERROR;
    }
    else if(type == "caculate")
    {
        return GetCalcNodeInfo(tmu,target,app_running_info);
    }
    else if(type == "storage")
    {
	return GetStorageNodeInfo(tmu,target,app_running_info);
    }
    else if(type == "network")
    {
       return GetNetWorkNodeInfo(tmu,target,app_running_info);
    }

    return SUCCESS;
}

STATUS SysRuntimeInfoQueryMethod::Query(const string &appname,const string &hopname,const string &hoppro,MessageUnit &mu,AppRuntimeInfoReq &req,AppRuntimeInfo& app_running_info)
    {
    STATUS ret;

	/*as for a unaccessible  node, send message to the node agent that then transfers message to all of processes on the destination*/
	if (1 < req.message_route.size())
        {
	    MessageOption option(MID(hopname,hoppro,MESSAGE_AGENT),EV_RUNTIME_INFO_REQ,0,0);
	    ret = mu.Send(req,option);
            if(SUCCESS != ret)
            {
	        cerr << "failed to send req! ret = " << ret << endl;
                return ret;
            }
        }
	else/*as for a accessible  node, directly send message to all of processes on it*/
    {
        vector<string>::iterator it;
        for(it = req.processes.begin(); it != req.processes.end(); it++)
        {
			//������ÿ�����̵���Ϣ����
			MessageOption option(MID(hopname,*it,MESSAGE_AGENT),EV_RUNTIME_INFO_REQ,0,0);
			ret = mu.Send(req,option);
            if(SUCCESS != ret)
            {
                cerr << "failed to send req to "<< *it <<"! ret = " << ret << endl;
                return ret;
            }
        }
    }
    cout << req.serialize() << endl;

    //�Ƚ��������Ӧ��Ľ����б���뵽���յĽ����ȥ
    //���ʵ��û�л��ĳ�����̵�Ӧ����exist��ʶ��ʼ��Ϊfalse
    app_running_info.name = appname;
    vector<string>::iterator it;
    for(it = req.processes.begin(); it != req.processes.end(); it++)
    {
        ProcRuntimeInfo procinfo(*it);
        procinfo.exist = false;
        app_running_info.procinfo.push_back(procinfo);
    }

    //����һ���ռ��ȴ���ʱ�Ķ�ʱ��
    TIMER_ID timer_id = mu.CreateTimer();
    if(INVALID_TIMER_ID == timer_id)
    {
        return ERROR_NO_TIMER;
    }

    #define EV_QUERY_TIMEOUT EV_TIMER_1
    TimeOut timeout(EV_QUERY_TIMEOUT,sysinfo_timer_wait_len);
    ret = mu.SetTimer(timer_id,timeout);
    if(SUCCESS != ret)
    {
        return ret;
    }
    
    //��ʼ����ȴ��������̵�Ӧ��
    uint32 wait_count = 0;
    uint32 wait_limit = app_running_info.procinfo.size();
    MessageFrame ack;
    while(1)
    {
        if(wait_count >= wait_limit)
        {
            //���н��̶��Ѿ��ȴ����ˣ����㶨ʱ��û����Ҳ����
            //����Ķ�ʱ����Դ��������ʱ��Ϣ��Ԫ�������Զ��ͷ�
            cout << "=== All ack received! ===" << endl;
            break;
        }

        //��Ϊ�Ѿ��������ռ���ʱ�����������һֱ����ȥ�����õ����޷�����        
        ret = mu.Wait(&ack,WAIT_FOREVER);
        if(SUCCESS != ret)
        {
            //�ȴ�����ʧ��
            cerr << "Wait failed! ret = " << ret << endl;
            return ret;
        }

        if(ack.option.id() == EV_QUERY_TIMEOUT)
        {
            //�ռ���ʱ����ʱ��, �������ﳬʱ��������ʧ�� 
            cerr << "Time out! " << wait_count << " ack received!" << endl;
            break;
        }

        if(ack.option.id() != EV_RUNTIME_INFO_ACK)
        {
            //�յ��Ĳ�����������Ҫ�ȵ���Ϣ
            cerr << "Unknown message! id = " << ack.option.id() << endl;
            continue;
        }

        //�յ�һ��Ӧ����
        ProcRuntimeInfo info;
        info.deserialize(ack.data);
        vector<ProcRuntimeInfo>::iterator pit = app_running_info.procinfo.begin();
        for(; pit != app_running_info.procinfo.end(); pit ++)
        {
            //��Ӧ���������б�
            if(pit->name == info.name)
            {
                *pit = info;
                vector<MuRuntimeInfo>::const_iterator it;
                for(it = info.muinfo.begin(); it != info.muinfo.end(); it++)
                {
                    Debug("Sys message: %d\n" 
                        "state:%d\n"
                        "pid:%d\n"
                        "tid:0x%lx\n"
                        "push_count:%llu\n"
                        "pop_count:%llu\n"
                        "queued_count:%d\n"
                        "timer_count:%d\n"
                        "last_message:%d!\n",
                        (*it).exist,(*it).state,(*it).pid,(*it).tid,
                        (*it).push_count,(*it).pop_count,(*it).queued_count,
                        (*it).timer_count,(*it).last_message);
                }
                pit->exist = true;
            }
        }
        wait_count++;
    }

    app_running_info.dt.refresh();
    mu.KillTimer(timer_id);
    return SUCCESS;
}

STATUS SysRuntimeInfoQueryMethod::GetCalcNodeInfo(MessageUnit &mu,const string& target,AppRuntimeInfo& app_running_info)
{
    string appname,ccname,hcname,hopname,hoppro;
	
    if(target.empty())
    {
        //Ҫ��ѯ����tc
        appname = ApplicationName();
		hopname = appname;
    }
    else
    {
        string::size_type found = target.find_first_of(',');
        if(string::npos == found)
        {
            //Ҫ��ѯ����cc
            ccname = target;
            appname = ccname;
			hopname = appname;
        }
        else
        {
            //Ҫ��ѯ����hc
            ccname = target.substr(0,target.find_first_of(','));
            hcname = target.substr(target.find_first_of(',') + 1);
            appname = hcname;
			hopname = ccname;
			hoppro = PROC_CC;
            
            //���ж��ţ���������Ҳ����
            if(ccname.empty() || hcname.empty())
            {
                cerr << "information not enough!" << endl;
                return ERROR_INVALID_ARGUMENT;
            }
            
            //hcname���治�����ж��ŷָ�����
            if(string::npos != hcname.find_first_of(','))
            {
                cerr << "too many commas!" << endl;
                return ERROR_INVALID_ARGUMENT;
            }
        }
    }

    cout << "ccname = " << ccname << endl;
    cout << "hcname = " << hcname << endl;

    AppRuntimeInfoReq req;
    if(!hcname.empty())
    {
        SkyAssert(!ccname.empty());
        //����ͨ��hc��·�ɱ�
        MID hop_tmu(ApplicationName(),ProcessName(),mu.name());
        MID hop_cc(ccname,PROC_CC,MESSAGE_AGENT);
        MID hop_hc(hcname,"","");
        req.message_route.push_back(hop_tmu);
        req.message_route.push_back(hop_cc);
        req.message_route.push_back(hop_hc);
        req.processes.push_back(PROC_HC);

    }
    else if(!ccname.empty())
    {
        //cc�Ĳ�ѯ����ʱ��Ϣ��Ԫֱ�ӷ��������·�ɱ�ֻ�����cc�ڵ�
        MID hop_cc(ccname,"","");
        req.message_route.push_back(hop_cc);
        //Ҫ��ѯ��cc�ڵ�����б�
        req.processes.push_back(PROC_CC);
        req.processes.push_back(PROC_IMAGE_AGENT);

    }
    else
    {
        //tc�Ĳ�ѯ����ʱ��Ϣ��Ԫֱ�ӷ��������·�ɱ�ֻ�����tc�ڵ�
        MID hop_tc(ApplicationName(),"","");
        req.message_route.push_back(hop_tc);
        //Ҫ��ѯ��tc�ڵ�����б�
        req.processes.push_back(PROC_APISVR);
        req.processes.push_back(PROC_TC);
        req.processes.push_back(PROC_IMAGE_MANAGER);
        req.processes.push_back(PROC_FILE_MANAGER);
    }
	
    return Query(appname,hopname,hoppro,mu,req,app_running_info);
}

STATUS SysRuntimeInfoQueryMethod::GetStorageNodeInfo(MessageUnit &mu,const string& target,AppRuntimeInfo& app_running_info)
{
	string appname,scname,saname,hopname,hoppro;
	
	if(target.empty())
	{
		//Ҫ��ѯ����sc
		appname = ApplicationName();
		scname = appname;
		hopname = scname;
	}
	else
	{
		//Ҫ��ѯ����sa
		saname = target;
		appname = saname;
		hopname = saname;
	}

	cout << "scname = " << scname << endl;
	cout << "saname = " << saname << endl;

	AppRuntimeInfoReq req;
	if(!saname.empty())
	{
		//sa�Ĳ�ѯ����ʱ��Ϣ��Ԫֱ�ӷ��������·�ɱ�ֻ�����sa�ڵ�
		MID hop_sa(saname,"","");
		req.message_route.push_back(hop_sa);
		//Ҫ��ѯ��sa�ڵ�����б�
		req.processes.push_back(PROC_SA);
	}
	else
	{
		//sc�Ĳ�ѯ����ʱ��Ϣ��Ԫֱ�ӷ��������·�ɱ�ֻ�����sc�ڵ�
		MID hop_sc(ApplicationName(),"","");
		req.message_route.push_back(hop_sc);
		//Ҫ��ѯ��sc�ڵ�����б�
		req.processes.push_back(PROC_SC);
	}
	
	return Query(appname,hopname,hoppro,mu,req,app_running_info);
}

STATUS SysRuntimeInfoQueryMethod::GetNetWorkNodeInfo(MessageUnit &mu,const string& target,AppRuntimeInfo& app_running_info)
{
	#define PROC_VNM	"proc_vnm"
	#define PROC_VNA	"proc_vna"
    string appname,vnmname,vnaname,hopname,hoppro;
	
	CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
    if(pvnetlib == NULL)
    {
    	return TECS_ERROR_VNET_NOT_WORKING;
    }
	vnmname = pvnetlib->GetVnmApplication();
    if(target.empty())
    {
    	//Ҫ��ѯ����vnm
        appname = vnmname;
		hopname = vnmname;
    }
    else
    {
        //Ҫ��ѯ����vna
        vnaname = target;
        appname = vnaname;
		hopname = vnmname;
		hoppro = PROC_VNM;
        
        //���ж��ţ���������Ҳ����
        if(vnmname.empty() || vnaname.empty())
        {
            cerr << "information not enough!" << endl;
            return ERROR_INVALID_ARGUMENT;
        }
        
        //vnaname���治�����ж��ŷָ�����
        if(string::npos != vnaname.find_first_of(','))
        {
            cerr << "too many commas!" << endl;
            return ERROR_INVALID_ARGUMENT;
        }
    }

    cout << "vnmname = " << vnmname << endl;
    cout << "vnaname = " << vnaname << endl;


    AppRuntimeInfoReq req;
    if(!vnaname.empty())
    {
        SkyAssert(!vnmname.empty());
        //����ͨ��vna��·�ɱ�
        MID hop_tmu(ApplicationName(),ProcessName(),mu.name());
        MID hop_vnm(vnmname,PROC_VNM,MESSAGE_AGENT);
        MID hop_vna(vnaname,"","");
        req.message_route.push_back(hop_tmu);
        req.message_route.push_back(hop_vnm);
        req.message_route.push_back(hop_vna);
        req.processes.push_back(PROC_VNA);

    }
    else if(!vnmname.empty())
    {	
        //vnm�Ĳ�ѯ����ʱ��Ϣ��Ԫֱ�ӷ��������·�ɱ�ֻ�����vnm�ڵ�
        MID hop_vnm(vnmname,"","");
        req.message_route.push_back(hop_vnm);
        //Ҫ��ѯ��vnm�ڵ�����б�
        req.processes.push_back(PROC_VNM);
    }

    cout << req.serialize() << endl;

    return Query(appname,hopname,hoppro,mu,req,app_running_info);
}

/******************************************************************************/
void SysRuntimeInfoQueryMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ostringstream oss;
    int64 uid = get_userid();
	string type = xSTR(paramList.getString(1));
    string query_target = xSTR(paramList.getString(2));

    int role = _upool->Get(uid,false)->get_role();

    //ֻ�й���Ա����Ȩ���ñ��ӿ�
    if (role != UserGroup::CLOUD_ADMINISTRATOR)
    {
        oss << "Error, only administrators can query system runtime information!";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return;
    }
        
    AppRuntimeInfo info;
    STATUS ret = GetSysInfo(type,query_target,info);
    if(SUCCESS != ret)
    {
        oss << "Error, failed to get system runtime info!";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(RpcError(ret)),xSTR(oss.str()));
    }

    ApiAppRuntimeInfo info_api(info);
    info_api.target = query_target;
    info_api.set_runtime_state();
    xRET2(xINT(TECS_SUCCESS),info_api.to_rpc());
    return;	
}

/******************************************************************************/
void LicenseQueryMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    User* user = _upool->Get(get_userid(), false);  
    if(user == NULL || user->get_role() != UserGroup::CLOUD_ADMINISTRATOR)
    {
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED), xSTR("Error, only administrators can query license information!")); 
        return;
    }

    ApiLicenseInfo license;
    GetSysLicense(license);
    xRET2(xINT(TECS_SUCCESS),license.to_rpc());
    return;
}

/******************************************************************************/
void HardwareFingerprintQueryMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    User* user = _upool->Get(get_userid(), false);  
    if(user == NULL || user->get_role() != UserGroup::CLOUD_ADMINISTRATOR)
    {
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED), xSTR("Error, only administrators can query hardware fingerprints!")); 
        return;
    }

    string hardware_fingerprints;
    hardware_fingerprints = get_hardware_fingerprint();
    if(hardware_fingerprints.empty())
    {
        xRET2(xINT(TECS_ERR_UNKNOWN),xSTR(hardware_fingerprints));
    }
    else
    {
        xRET2(xINT(TECS_SUCCESS),xSTR(hardware_fingerprints));
    }
    return;
}

void DbgSysInfo(const char* target)
{
    string _target;
    if(target)
    {
        _target = target;
    }
    AppRuntimeInfo result;
    STATUS ret = SysRuntimeInfoQueryMethod::GetSysInfo("storage",_target,result);
    if(SUCCESS != ret)
    {
        cerr << "failed to get AppRuntimeInfo! ret = " << ret << endl;
        return;
    }
    cout << result.serialize() << endl;
}

DEFINE_DEBUG_FUNC(DbgSysInfo);
DEFINE_DEBUG_FUNC_ALIAS(sysinfo,DbgSysInfo);

void DbgShowLicense()
{
    license_print();
}
DEFINE_DEBUG_CMD(license,DbgShowLicense);
}


    
