
#include "vnet_monitor.h"
#include "vnet_function.h"

extern int g_monitor_times;

#define BC_NET_MONITOR_BOND_MAX_NUMBER (10) // 10次以后，强制设置

namespace zte_tecs
{

extern int g_dbgVNetMonFlag;

int g_dbgBondHandle = true;
void t_bond_handle(int i)
{
    g_dbgBondHandle = i;
}
DEFINE_DEBUG_FUNC(t_bond_handle);

// bond monitor object 
CVNetMonitorBond::CVNetMonitorBond()
{
}

CVNetMonitorBond::~CVNetMonitorBond()
{
}

int32 CVNetMonitorBond::Init()
{    
    // 判断是否存在bond模块 
#if 0    /* ovs bond 后无需 判断linux bond模块 */
    int is_support = FALSE;
    if( SUCCESS != IsSupportBond(is_support) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorMgr::InitVNetMonitorMgr: IsSupportBond failed.\n");
        return -1;
    }
    
    if( FALSE == is_support )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorMgr::InitVNetMonitorMgr: IsSupportBond is FALSE.\n");
        return -1;
    }
#endif     

    return 0;
}

int32 CVNetMonitorBond::GetBondFileTimeStamp(const string & bond, time_t & t)
{
    // 判断是否存在此bond
    string filePath("/proc/net/bonding/");
    filePath += bond;
    if (0 != access(filePath.c_str(), 0))
    {
        DbgPrint("CVNetMonitorBond::GetBondFileTimeStamp access (%s) failed.",filePath.c_str());
        return ERROR;
    }

    // 获取time stamp
    struct stat _filestat;                
    if( 0 != stat(filePath.c_str(),&_filestat) )
    {        
        DbgPrint("CVNetMonitorBond::GetBondFileTimeStamp get bond file (%s) stat failed.",filePath.c_str());
        return ERROR;
    }
    t = _filestat.st_mtime;
    
    return SUCCESS;
}

int32 CVNetMonitorBond::IsBondModify(CBondFile & bf, int32 & isModify)
{
    isModify = FALSE;
    
    CBondInfo real_info;
    if ( SUCCESS != GetBondInfo(bf.info.name,real_info) )
    {        
        DbgPrint("CVNetMonitorBond::IsBondModify get_bond_info (%s) failed.",bf.info.name.c_str());
        return ERROR;
    }

    // bond mode 是否变化
    if( bf.info.mode != real_info.mode )
    {
        isModify = TRUE;
        return SUCCESS;
    }

    // backup 模式下 需判断active和other nic
    if( bf.info.mode == EN_BOND_MODE_BACKUP )
    {
        if( bf.info.backup.active_nic != real_info.backup.active_nic )
        {
            isModify = TRUE;
            return SUCCESS;
        }

        if (bf.info.backup.other_nic.size() != real_info.backup.other_nic.size() )
        {
            isModify = TRUE;
            return SUCCESS;            
        }
        else
        {
            vector<string>::iterator it = bf.info.backup.other_nic.begin();
            for(; it != bf.info.backup.other_nic.end(); ++it)
            {
                int32 bExist = false;
                vector<string>::iterator it_r =  real_info.backup.other_nic.begin();
                for( ; it_r != real_info.backup.other_nic.end(); ++it_r)
                {
                    if( (*it_r ) == (*it) )
                    {
                        bExist = true;
                        break;
                    }                    
                }
                if( bExist == false)
                {
                    isModify = TRUE;
                    return SUCCESS; 
                }
            }
        }
    }

    if( bf.info.mode == EN_BOND_MODE_802 )
    {
        if( ( bf.info.lacp.nic_suc.size() != real_info.lacp.nic_suc.size() ) || \
            ( bf.info.lacp.nic_fail.size() != real_info.lacp.nic_fail.size() ) )
        {
            isModify = TRUE;
            return SUCCESS; 
        }

        // nic suc        
        vector<string>::iterator it = bf.info.lacp.nic_suc.begin();
        for(; it != bf.info.lacp.nic_suc.end(); ++it)
        {
            int32 bExist = false;
            vector<string>::iterator it_r =  real_info.lacp.nic_suc.begin();
            for( ; it_r != real_info.lacp.nic_suc.end(); ++it_r)
            {
                if( (*it_r ) == (*it) )
                {
                    bExist = true;
                    break;
                }                    
            }
            if( bExist == false)
            {
                isModify = TRUE;
                return SUCCESS; 
            }
        }

        // nic fail
        if( 1)
        {
            vector<string>::iterator it = bf.info.lacp.nic_fail.begin();
            for(; it != bf.info.lacp.nic_fail.end(); ++it)
            {
                int32 bExist = false;
                vector<string>::iterator it_r =  real_info.lacp.nic_fail.begin();
                for( ; it_r != real_info.lacp.nic_fail.end(); ++it_r)
                {
                    if( (*it_r ) == (*it) )
                    {
                        bExist = true;
                        break;
                    }                    
                }
                if( bExist == false)
                {
                    isModify = TRUE;
                    return SUCCESS; 
                }
            }
        }
    }
    
    return SUCCESS;
}

void CVNetMonitorBond::BondHandle()
{   
    // 0 获取SRIOV BONDS
    set<string> file_bonds;    
    set<string>::iterator it_bond;
    GetSriovBond(file_bonds);
    
    // 1 memory ---> file  del
    bool bond_exist = false;
    list<CBondFile>::iterator it_lst = _lst_bond.begin();
    for(; it_lst != _lst_bond.end(); )
    {
        bond_exist = false;
        it_bond = file_bonds.begin();
        for(;it_bond != file_bonds.end(); ++it_bond)
        {
            if( 0 == (*it_lst).info.name.compare((*it_bond)))
            {
                bond_exist = true;
                break;
            }
        }

        // 此bond已删除
        if( false == bond_exist)
        {
            // BOND VF 开启
            DbgPrint("BondDel %s.",(*it_lst).info.name.c_str());
            BondDel((*it_lst).info); 

            // list删除此bond
            it_lst = _lst_bond.erase(it_lst);
            continue;
        }
        ++it_lst;
    }

    // 2 file ---> memory modify/add        
    bool bExist = false;
    time_t timestamp =0;
    it_bond = file_bonds.begin();
    for(;it_bond != file_bonds.end(); ++it_bond)
    {
        bExist = false;
        it_lst = _lst_bond.begin();
        for(; it_lst != _lst_bond.end(); ++it_lst )
        {
            if( 0 == (*it_bond).compare((*it_lst).info.name))
            {
                bExist = true;
                break;
            }
        }
                
        // 已存在此bond，判断是否修改了 
        if(bExist)
        {
            // 判断bond 是否发生了变化 backup  lacp 
            int32 isModify = false;
            if( 0 == IsBondModify((*it_lst),isModify) )
            {
                if( isModify || \
                   (*it_lst)._num > BC_NET_MONITOR_BOND_MAX_NUMBER )
                {
                    if( SUCCESS == BondModify((*it_lst).info))
                    {
                        DbgPrint("BondModify %s success.",(*it_lst).info.name.c_str());
                        (*it_lst)._laststamp = timestamp;
                    }
                    else
                    {
                        DbgPrint("BondModify %s failed.",(*it_lst).info.name.c_str());
                    }
                }
                
                if( (*it_lst)._num > BC_NET_MONITOR_BOND_MAX_NUMBER )
                {
                    (*it_lst)._num = 0;
                }
                else
                {
                    (*it_lst)._num++;
                }
            }
        }
        else
        {
            CBondInfo  new_info;
            if( SUCCESS == BondAdd((*it_bond),new_info))
            {
                DbgPrint("BondAdd %s success.", new_info.name.c_str());
                
                CBondFile file_info;
                file_info.info = new_info;
                file_info._laststamp = timestamp;
                _lst_bond.push_back(file_info);
            }
        }
    }
}

void CVNetMonitorBond::NicHandle()
{
    // if ethtool eth0 is down, vf does not need OPEN
    int is_up = 0;
    
    map<string,uint32>::iterator it_nic = _nic_vf.begin();
    map<string,uint32>::iterator it_tmp;
    for(; it_nic != _nic_vf.end();)
    {
        // 确保端口是up的
        if(0 == get_nic_status(it_nic->first,is_up) )
        {
            if(is_up)
            {
                struct private_value vf_data;
                memset(&vf_data,0,sizeof(vf_data));
    
                // 直接设置    
                vf_data.cmd = PRIVATE_SETVF;
                vf_data.data = it_nic->second;
                if( SUCCESS != SendMsgToVF((it_nic->first).c_str(), &vf_data))
                {
                    ++it_nic;
                    continue;
                }
                else
                {
                    it_tmp = it_nic;
                    ++it_nic;
                    _nic_vf.erase(it_tmp);
                    continue;
                }                
            }
            else
            {
                // 直接删除
                it_tmp = it_nic;
                ++it_nic;
                _nic_vf.erase(it_tmp);
                continue;
            }
        }

        ++it_nic;
    }    
}
    

/*********************************************************
功能: 处理bond信息 add modify del
          nic信息  nic vf open/close
*********************************************************/
void CVNetMonitorBond::MonitorHandle()
{
    // 方便定位
    if( false == g_dbgBondHandle )
    {
        return;
    }
    
    DbgPrint("MonitorHandle~~~");
    
    // bond handle
    BondHandle();
    
    // nic handle
    NicHandle();
}

int32 CVNetMonitorBond::GetBondInfo(const string strBondName, CBondInfo& cInfo)
{
    string strMode = "";
    int32 nStatus = 0;

    if (0 != VNetGetBondModeOVS(strBondName, strMode))
    {
        return -1;
    }

    //分模式处理
    if (strMode == "1")
    {
        if (0 != VNetBackupInfo(strBondName, cInfo.backup.active_nic, cInfo.backup.other_nic))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorBond:[%s] GetBondInfo VNetBackupInfo failed.\n", strBondName.c_str());
            return -1;
        }

        cInfo.name = strBondName;
        cInfo.mode = EN_BOND_MODE_BACKUP;
        return 0;
    }
    else if (strMode == "4")
    {
        if (0 != VNetLacpInfo(strBondName, nStatus, cInfo.lacp.aggregator_id, cInfo.lacp.partner_mac, \
                              cInfo.lacp.nic_suc, \
                              cInfo.lacp.nic_fail))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorBond:[%s] GetBondInfo VNetLacpInfo failed.\n", strBondName.c_str());
            return -1;
        }

        if (0 != nStatus)
        {
            cInfo.name = strBondName;
            cInfo.mode = EN_BOND_MODE_802;
            cInfo.lacp.status = nStatus;
        }
        else
        {
            cInfo.lacp.nic_suc.clear();
            cInfo.lacp.nic_fail.clear();
            //协商失败，lacp重新获取slave
            if (0 != VNetGetBondSlavesOVS(strBondName, cInfo.lacp.nic_fail))
            {
                return -1;
            }
        }

        return 0;

    }
    else
    {
        return -1;
    }
}

int32 CVNetMonitorBond::BondNicsAllOpen(CBondInfo & info)
{
    switch(info.mode)
    {
        case EN_BOND_MODE_BACKUP:
        {
            if( 0 != info.backup.active_nic.compare(""))
            {
                _nic_vf[info.backup.active_nic] = PRIVATE_VF_VALUE_OPEN;
            }
            
            vector<string>::iterator it_nics;
            it_nics = info.backup.other_nic.begin();
            for(; it_nics != info.backup.other_nic.end(); ++it_nics)
            {
                _nic_vf[*it_nics] = PRIVATE_VF_VALUE_OPEN;
            }
        }
        break;
        case EN_BOND_MODE_802:
        {
            vector<string>::iterator it_nic = info.lacp.nic_suc.begin();
            for(; it_nic != info.lacp.nic_suc.end(); ++it_nic)
            {
                _nic_vf[*it_nic] = PRIVATE_VF_VALUE_OPEN;
            }
            it_nic = info.lacp.nic_fail.begin();
            for(; it_nic != info.lacp.nic_fail.end(); ++it_nic)
            {
                _nic_vf[*it_nic] = PRIVATE_VF_VALUE_OPEN;
            }
        }
        break;
        default: 
        {
        }
        break;
    }
    return SUCCESS;
}

int32 CVNetMonitorBond::BondNicsOperator(CBondInfo & info)
{
    switch(info.mode)
    {
        case EN_BOND_MODE_BACKUP:
        {
            if( 0 != info.backup.active_nic.compare(""))
            {
                _nic_vf[info.backup.active_nic] = PRIVATE_VF_VALUE_OPEN;
            }
            
            vector<string>::iterator it_nics = info.backup.other_nic.begin();
            for(; it_nics != info.backup.other_nic.end(); ++it_nics)
            {
                _nic_vf[*it_nics] = PRIVATE_VF_VALUE_CLOSE;
            }
        }
        break;
        case EN_BOND_MODE_802:
        {
            vector<string>::iterator it_nics = info.lacp.nic_suc.begin();
            for(; it_nics != info.lacp.nic_suc.end(); ++it_nics)
            {
                _nic_vf[*it_nics] = PRIVATE_VF_VALUE_OPEN;
            }

            it_nics = info.lacp.nic_fail.begin();
            for(; it_nics != info.lacp.nic_fail.end(); ++it_nics)
            {
                _nic_vf[*it_nics] = PRIVATE_VF_VALUE_CLOSE;
            }
        }
        break;
        default:
        {
        }
        break;
    }
    return SUCCESS;
}

// BondDel
int32 CVNetMonitorBond::BondDel(CBondInfo & info)
{
    return BondNicsAllOpen(info);
}

// Bond Modify
int32 CVNetMonitorBond::BondModify(CBondInfo & info)
{
    CBondInfo real_info;
    if ( SUCCESS != GetBondInfo(info.name,real_info) )
    {        
        DbgPrint("CVNetMonitorBond::BondModify get_bond_info (%s) failed.",info.name.c_str());
        return ERROR;
    }

    // 1 开启所有old bond nics
    BondNicsAllOpen(info);
    
    // 2 real bond nics oper
    BondNicsOperator(real_info);

    // 3 拷贝更新
    info = real_info;
    
    return SUCCESS;
}

// Bond Add
int32 CVNetMonitorBond::BondAdd(const string bond,CBondInfo & info)
{    
    if ( SUCCESS != GetBondInfo(bond,info) )
    {    
        DbgPrint("CVNetMonitorBond::BondAdd get_bond_info (%s) failed.",bond.c_str());
        return ERROR;
    }
    
    BondNicsOperator(info);
    return SUCCESS;
}

STATUS CVNetMonitorBond::SendMsgToVF(const char* ifname, struct private_value *pval)
{
    STATUS ret = send_ioctl(ifname,pval);
    DbgPrint("send_ioctl(%s %d %d) ret = %d.",ifname,pval->cmd,pval->data, ret);
    return ret;
}

/*************************************************
功能: 获取SRIOV bond
      检查bond nics是否都是sriov口
      bond模式必须是backup/lacp
*************************************************/
int32 CVNetMonitorBond::GetSriovBond(set<string> &sBond)
{

    sBond.clear();

    vector<string> bondings;  
    CBondInfo info;
    int  num_vf = 0;
    bool is_sriov = false;
    int ret = 0;
    vector<string>::iterator it_nics;
    vector<string>::iterator it_bond;
    
    VNetGetAllBondingsOVS(bondings);
    it_bond = bondings.begin();
    for(; it_bond != bondings.end(); ++it_bond)
    {    
        is_sriov = true;
        num_vf = 0;
        if ( SUCCESS != (ret = GetBondInfo((*it_bond),info)))
        {
            DbgPrint("CVNetMonitorBond::GetSriovBond get_bond_info(%s) failed. ret = %d",\
                (*it_bond).c_str(),ret);
            continue;
        }

        switch(info.mode)
        {
            case EN_BOND_MODE_BACKUP:
            {
                if( (SUCCESS != get_sriov_num(info.backup.active_nic,num_vf)) || \
                    (num_vf == 0))
                {
                    is_sriov = false;
                    break; 
                }

                bool all_sriov = true;
                it_nics = info.backup.other_nic.begin();
                for(; it_nics != info.backup.other_nic.end(); ++it_nics)
                {
                    if( (SUCCESS != get_sriov_num(*it_nics,num_vf)) || \
                        (num_vf == 0))
                    {
                        all_sriov = false;
                        break; 
                    }
                }
                if( false == all_sriov)
                {
                    is_sriov = false;
                    break;
                }
            }
            break;
            case EN_BOND_MODE_802:
            {
                // 如果没有成员口 也需将此bond过滤
                if( 0 == info.lacp.nic_suc.size() && \
                    0 == info.lacp.nic_fail.size() )
                {
                    is_sriov = false;
                    break;
                }
                
                bool all_sriov = true;
                it_nics = info.lacp.nic_suc.begin();
                for(; it_nics != info.lacp.nic_suc.end(); ++it_nics)
                {
                    if( (SUCCESS != get_sriov_num(*it_nics,num_vf)) || \
                        (num_vf == 0))
                    {
                        all_sriov = false;
                        break; 
                    }
                }
                if( false == all_sriov)
                {
                    is_sriov = false;
                    break;
                }
                it_nics = info.lacp.nic_fail.begin();
                for(; it_nics != info.lacp.nic_fail.end(); ++it_nics)
                {
                    if( (SUCCESS != get_sriov_num(*it_nics,num_vf)) || \
                        (num_vf == 0))
                    {
                        all_sriov = false;
                        break; 
                    }
                }
                if( false == all_sriov)
                {
                    is_sriov = false;
                    break;
                }
            }
            break;
            default: 
            {
                is_sriov = false;
            }
            break;
        }
        
        if(is_sriov)
        {
            sBond.insert(*it_bond);
        }        
    }
    return SUCCESS; 
}

void CVNetMonitorBond::Print()
{
    // 调试信息
    cout << "--------------------------Monitor Bond Info------------------------" << endl;
    cout << "--------------------Bond list info--------------------" << endl; 
    list<CBondFile>::iterator it_bond = _lst_bond.begin();
    for(; it_bond != _lst_bond.end(); ++it_bond)
    {
        (*it_bond).print();
    }
    cout << "" <<endl;
    cout << "--------------------Nic VF list info--------------------" << endl; 
    cout << "|" <<setw(16) << " Nic name " << "|" << setw(5) << " VF " << endl;
        
    map<string,uint32>::iterator it_nic = _nic_vf.begin();
    for(; it_nic != _nic_vf.end(); ++it_nic)
    {
        cout << "|" <<setw(16) << it_nic->first << "|" << setw(5) << it_nic->second  <<  endl;
    }

    cout << "" << endl;
    cout << "" << endl;
    cout << "--------------------Bond File info--------------------" << endl; 
    set<string> file_bonds;    
    set<string>::iterator it_fileBond;
    GetSriovBond(file_bonds);
    it_fileBond = file_bonds.begin();
    for(; it_fileBond != file_bonds.end(); ++it_fileBond)
    {
        cout << "Band name: " << (*it_fileBond) << endl; 
        class CBondInfo info;
        if ( SUCCESS == GetBondInfo((*it_fileBond),info) )
        {
            info.print();    
            switch(info.mode)
            {
                case EN_BOND_MODE_802:
                {
                }
                break;
            
                default:
                break;
            }
        }
        cout << "--------------------" << endl;
    }
}

STATUS CVNetMonitorBond::DbgPrint( const char* pcFmt,...)
{   
    if( false == g_dbgVNetMonFlag)
    {
        return 0;
    }
    
    va_list     vList;
    char tmp[256]={0};

    va_start(vList, pcFmt);    
    vsnprintf(tmp, sizeof(tmp), pcFmt, vList);
    cout << tmp << endl;
    va_end(vList);
    return 0;
}

void t_bond()
{
    cout << "-------------------- Bond File info--------------------" << endl; 
    vector<string> bondings;
    VNetGetAllBondingsOVS(bondings);
    vector<string>::iterator it_nics;
    vector<string>::iterator it_bond = bondings.begin();
    CBondInfo info;
    int ret = 0;
    CVNetMonitorBond cBond;
    for(; it_bond != bondings.end(); ++it_bond)
    {    
        
        if ( SUCCESS != (ret = cBond.GetBondInfo((*it_bond),info)))
        {        
            cout << "get_bond_info( " <<(*it_bond) << ") failed. ret = " << ret <<endl;
                
            continue;
        }
        else
        {
            info.print();
        }
    }

    cout << "--------------------SRIOV Bond File info--------------------" << endl; 
    set<string> file_bonds;    
    set<string>::iterator it_fileBond;

    CVNetMonitorBond sriov_bond;
    sriov_bond.GetSriovBond(file_bonds);
    it_fileBond = file_bonds.begin();
    for(; it_fileBond != file_bonds.end(); ++it_fileBond)
    {
        cout << "Band name: " << (*it_fileBond) << endl; 
        class CBondInfo info;
        if ( SUCCESS != sriov_bond.GetBondInfo((*it_fileBond),info) )
        {
            cout << "get_bond_info( " <<(*it_bond) << ") failed. ret = " << ret <<endl;
        }
        else
        {
            info.print();
        }
        cout << "--------------------" << endl;
    }
}
DEFINE_DEBUG_FUNC(t_bond);

void t_set_vf(const char * pcIfName, int data)
{
    struct private_value vf_data;
    memset(&vf_data,0,sizeof(vf_data));

    if( NULL == pcIfName)
    {
        cout << "param invalid." << endl;
        return;
    }
    string ifname = pcIfName; 
    
    // 直接设置    
    vf_data.cmd = PRIVATE_SETVF;
    vf_data.data = data;
    if( SUCCESS != send_ioctl(ifname.c_str(), &vf_data))
    {
        cout << "send_ioctl(" << ifname << "," << data << ") failed." << endl;
    }
    else
    {
        cout << "send_ioctl(" << ifname << "," << data << ") success." << endl;
    }             
}
DEFINE_DEBUG_FUNC(t_set_vf);

void t_bond_hanle()
{
    CVNetMonitorBond b;

    b.MonitorHandle();
}
DEFINE_DEBUG_FUNC(t_bond_hanle);

}



