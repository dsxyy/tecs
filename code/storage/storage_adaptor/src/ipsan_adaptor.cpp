#include "sky.h"
#include "mid.h"
#include "event.h"
#include "storage_adaptor_impl.h"
#include "errcode.h"
#include "log_agent.h"

extern "C"
{
#include "usp_capi.h"
#include "usp_error_code.h"
}

using namespace zte_tecs;

#define STORAGE_ERR_BEGIN                    0xf00
#define STORAGE_RESOURCE_OPR_FAIL            STORAGE_ERR_BEGIN+1
#define STORAGE_RESOURCE_OPR_LVNameOccupied  STORAGE_ERR_BEGIN+2
#define STORAGE_GET_VGMAP_FAIL               STORAGE_ERR_BEGIN+3

#define ERR_STR_UNKNOW_ERROR                 "UNKOWN_ERR"

const char *GetErrString(int errorCode)
{
    const char *retStr = NULL;
    int     idx  = 0;
    for (idx = 0; sg_tCommonError[idx].pcErrDes; idx++)
    {
        if (errorCode == sg_tCommonError[idx].sdwErrID)
        {
            retStr = (const char*)sg_tCommonError[idx].pcErrDes;
            break;
        }
    }

    if (retStr == NULL)
    {
        retStr = ERR_STR_UNKNOW_ERROR;
    }

    return retStr;
}

void IpsanAdaptor::ProcessCapiRet(int errcode)
{
    if (errcode==0)
    {
        return ;
    }
    
    if(-E_SCS_CAPI_CONNECT_FAIL==errcode || -E_SMA_AUTHEN ==errcode 
        || -E_SCS_CAPI_RECV_MSG_TIMEOUT== errcode ||-E_SCS_CAPI_RECV_RSP_ERROR== errcode)
    {
        T_AmsUser   tUser;
        int ret;
        memset(&tUser,0,sizeof(T_AmsUser));
        strncpy(tUser.acUsername,_cfg._user.c_str(),MAX_USER_NAME_LEN);
        strncpy(tUser.acPassword,_cfg._password.c_str(),MAX_USER_PSW_LEN);
        ret=Login(&tUser);
        OutPut(SYS_DEBUG,"relogin ipsan ret:%d\n",ret);
    }
    const char *errStr  = NULL;
    errStr=GetErrString(-errcode);

    OutPut(SYS_ERROR,"IpsanAdaptor:CapiRet [E0x%x] %s.\r\n",  -errcode, errStr);
}

STATUS IpsanAdaptor::Init(const sa_cfg &cfg)
{
    T_SvrSocket tSvr;
    T_AmsUser   tUser;
    int         ret=-1;
    _cfg=cfg;
    if (_cfg._control_ip.empty())
    {
        return ERROR;
    }
    tSvr.sdwSvrPort=9999;
    strncpy(tSvr.ucSvrIpAddr,_cfg._control_ip.c_str(),MAX_IP_LEN);
    ret=SetSvrSocket(&tSvr);
    if (ret==0)
    {
        memset(&tUser,0,sizeof(T_AmsUser));
        strncpy(tUser.acUsername,_cfg._user.c_str(),MAX_USER_NAME_LEN);
        strncpy(tUser.acPassword,_cfg._password.c_str(),MAX_USER_PSW_LEN);
        ret=Login(&tUser);
        OutPut(SYS_NOTICE,"IpsanAdaptor: Login ret is %d\n",ret);
        ProcessCapiRet(ret);
    }
    else
    {
        OutPut(SYS_NOTICE,"IpsanAdaptor: SetSvrSocket err  \n");
    }
    return ret;
}

STATUS IpsanAdaptor::AllocVol(const StAllocVolReq & req,string &vol_uuid)
{
    int         ret=-1;
    SWORD64     sqwCapacity;
    T_CreateVol tCreateVol;
    /*转换成k*/
    sqwCapacity = req._volsize*1024;
    strncpy(tCreateVol.cVdName,req._vgname.c_str(),MAX_BLK_NAME_LEN);
    strncpy(tCreateVol.cVolName,req._volname.c_str(),MAX_BLK_NAME_LEN);
    tCreateVol.sdwChunkSize=32;
    tCreateVol.sqwCapacity=sqwCapacity;
    tCreateVol.sdwCtlPrefer=0;
    tCreateVol.sdwCachePolicy=1;
    tCreateVol.sdwAheadReadSize=8;
    ret=CreateVol(&tCreateVol);
    if (-E_VDS_OBJECT_EXIST==ret)
    {
        T_VolInfo  tVolInfo;
        memset(&tVolInfo,0,sizeof(T_VolInfo));
        strncpy(tVolInfo.cVdName,req._vgname.c_str(),MAX_BLK_NAME_LEN);
        strncpy(tVolInfo.cVolName,tCreateVol.cVolName,MAX_BLK_NAME_LEN);
        ret=GetVolInfo(&tVolInfo);
        if (ret==0)
        {
            if (tVolInfo.sqwCapacity!=sqwCapacity)
            {               
                OutPut(SYS_CRITICAL,"IpsanAdaptor: EV_SA_CREATE_VOL vol exist and size dismatch %ld--%ld\n",tVolInfo.sqwCapacity,sqwCapacity);
                ret=ERROR_PERMISSION_DENIED;
            }
            else
            {
                ret=0;
            }
        }
        else
        {
            OutPut(SYS_ERROR,"IpsanAdaptor: EV_SA_CREATE_VOL GetVolInfo ret is %d\n",ret);
        }
    }
    if(ret==0)
    {
        T_VolInfo  tVolInfo;        
        memset(&tVolInfo,0,sizeof(T_VolInfo));
        strncpy(tVolInfo.cVdName,req._vgname.c_str(),MAX_BLK_NAME_LEN);
        strncpy(tVolInfo.cVolName,tCreateVol.cVolName,MAX_BLK_NAME_LEN);        
        ret=GetVolInfo(&tVolInfo);        
        ProcessCapiRet(ret);
        string  tmp;
        if(ret==0)
        {
            char buf[32];
            sprintf(buf,"%02x%02x%02x%02x%02x%02x%02x%02x",tVolInfo.ucVolUuid[0],tVolInfo.ucVolUuid[1],
                tVolInfo.ucVolUuid[2],tVolInfo.ucVolUuid[3],tVolInfo.ucVolUuid[4],tVolInfo.ucVolUuid[5],
                tVolInfo.ucVolUuid[6],tVolInfo.ucVolUuid[7]);
            printf("buf is %s\n",buf);
            vol_uuid.assign((const char *)buf);
        }        
    }
    ProcessCapiRet(ret);
    
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_CREATE_VOL name %s   ret is %d  uuid is %s\n",req._volname.c_str(),ret,vol_uuid.c_str());
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::DeleteVol(const StDelVolReq & req)
{
    int        ret=-1;
    T_VolName  tvolname;
    memset(tvolname.cVolName,0,MAX_BLK_NAME_LEN);
    strncpy(tvolname.cVolName,req._volname.c_str(),MAX_BLK_NAME_LEN);
    ret=DelVol(&tvolname);
    if(-E_VDS_NO_VOL==ret)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_DELETE_VOL name %s   ret is %d \n",req._volname.c_str(),ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::AllocSnapshot(const StAllocSnapshotReq & req,string &vol_uuid)
{
    int ret = -1;
    T_SvolCreate tSvolCreate;
    strncpy(tSvolCreate.cVolName,req._base_name.c_str(),MAX_BLK_NAME_LEN);
    strncpy(tSvolCreate.cSvolName,req._volname.c_str(),MAX_BLK_NAME_LEN);
    tSvolCreate.sdwAgentAttr = 0;
    ret=CreateSvol(&tSvolCreate);
    if (-E_VDS_OBJECT_EXIST==ret)
    {
        T_VolInfo  tVolInfo;
        memset(&tVolInfo,0,sizeof(T_VolInfo));
        strncpy(tVolInfo.cVdName,req._vgname.c_str(),MAX_BLK_NAME_LEN);
        strncpy(tVolInfo.cVolName,tSvolCreate.cSvolName,MAX_BLK_NAME_LEN);
        ret=GetVolInfo(&tVolInfo);
        if (ret==0)
        {
            SWORD64 sqwCapacity = (req._volsize % 1024 ==0) ?(req._volsize / 1024) :(req._volsize / 1024 + 1);
            if (tVolInfo.sqwCapacity != sqwCapacity)
            {
                OutPut(SYS_WARNING,"IpsanAdaptor: EV_SA_CREATE_SNAPSHOT exist size is %ld ,createsize is %ld  del  ret is %d\n",tVolInfo.sqwCapacity,sqwCapacity,ret);
                ret = ERROR;    
            }
            else
            {
                ret=0;
            }
        }
        else
        {
            OutPut(SYS_ERROR,"IpsanAdaptor: EV_SA_CREATE_SNAPSHOT GetVolInfo ret is %d\n",ret);
        }
    }
    if(ret==0)
    {
        T_VolInfo  tVolInfo;        
        memset(&tVolInfo,0,sizeof(T_VolInfo));
        strncpy(tVolInfo.cVdName,req._vgname.c_str(),MAX_BLK_NAME_LEN);
        strncpy(tVolInfo.cVolName,tSvolCreate.cSvolName,MAX_BLK_NAME_LEN);        
        ret=GetVolInfo(&tVolInfo);        
        ProcessCapiRet(ret);
        string  tmp;
        if(ret==0)
        {
            char buf[32];
            sprintf(buf,"%02x%02x%02x%02x%02x%02x%02x%02x",tVolInfo.ucVolUuid[0],tVolInfo.ucVolUuid[1],
                tVolInfo.ucVolUuid[2],tVolInfo.ucVolUuid[3],tVolInfo.ucVolUuid[4],tVolInfo.ucVolUuid[5],
                tVolInfo.ucVolUuid[6],tVolInfo.ucVolUuid[7]);
            printf("buf is %s\n",buf);
            vol_uuid.assign((const char *)buf);
        }
        
    }
    ProcessCapiRet(ret);
    
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_CREATE_SNAPSHOT name %s   ret is %d  uuid is %s\n",req._volname.c_str(),ret,vol_uuid.c_str());
    return ret;
}

STATUS IpsanAdaptor::DeleteSnapshot(const StDelVolReq & req)
{
    int        ret=-1;
    T_SvolName  tvolname;
    memset(tvolname.cSvolName,0,MAX_BLK_NAME_LEN);
    strncpy(tvolname.cSvolName,req._volname.c_str(),MAX_BLK_NAME_LEN);
    ret=DelSvol(&tvolname);
    if(-E_VDS_NO_SNAP==ret)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_DELETE_SNAPSHOT name %s   ret is %d \n",req._volname.c_str(),ret);
    return ret;
}

STATUS IpsanAdaptor::CreateTarget(const StCreateTargetReq & req)
{
    int           ret=-1;
    T_MapGrpName  tMap;
    memset(tMap.cMapGrpName,0,MAX_MAPGRP_NAME_LEN);
    strncpy(tMap.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    ret=CreateMapGrp(&tMap);
    /*映射组已经存在，则返回成功*/
    if (ret==-E_SCS_API_MAPGRP_EXIST)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_CREATE_TARGET target name is %s  ret is %d\n",req._targetname.c_str(),ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::DelTarget(const StDelTargetReq & req)
{
    int           ret=-1;
    T_MapGrpName  tMap;

    memset(tMap.cMapGrpName,0,MAX_MAPGRP_NAME_LEN);

    strncpy(tMap.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    ret=DelMapGrp(&tMap);
    if(-E_SCS_API_MAPGRP_NOEXIST==ret)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_DELETE_TARGET target name is %s  ret is %d\n",req._targetname.c_str(),ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::AddVol2Target(const StAddToTarReq& req)
{
    int           ret=-1;
    T_AddVolToGrp tVol;
    T_HostInGrp   tHost;

    list<int> lunidlist;
    memset(&tHost,0,sizeof(T_HostInGrp));
    memset(&tVol,0,sizeof(T_AddVolToGrp));
#if 0
    T_MapGrpInfo  tMapinfo;
    int      found=0;
    int      loop,freeid=0;
    memset(&tMapinfo,0,sizeof(T_MapGrpInfo));

    strncpy(tMapinfo.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    ret=GetMapGrpInfo(&tMapinfo);
    ProcessCapiRet(ret);
    OutPut(SYS_DEBUG,"IpsanAdaptor:GetMapGrpInfo  ret is %d\n",ret);
    if (0!=ret)
    {
        return ret;
    }


    for (loop=0;loop<tMapinfo.sdwLunNum;loop++)
    {
        lunidlist.push_back(tMapinfo.tLunInfo[loop].sdwLunId);
    }
    lunidlist.sort();
    list <int>::iterator it;
    it=lunidlist.begin();
    for (loop=0;loop<tMapinfo.sdwLunNum;loop++)
    {
        if (*it!=loop)
        {
            freeid=loop;
            found=1;
            break;
        }
        it++;
    }

    if (!found)
        freeid=loop;
#endif
    strncpy(tHost.ucInitName,req._iscsiname.c_str(),MAX_INI_NAME_LEN);
    strncpy(tHost.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    tVol.tLunInfo.sdwLunId=req._lunid;
    strncpy(tVol.tLunInfo.cVolName,req._volname.c_str(),MAX_BLK_NAME_LEN);
    strncpy(tVol.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    ret=AddVolToGrp(&tVol);
    if(-E_SCS_API_LUN_ID_EXIST==ret)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_ADDVOLTO_TARGET target name is %s lunid is %d  ret is %d \n",req._targetname.c_str(),req._lunid,ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::AddHost2Target(const StAddToTarReq& req)
{
    int           ret=-1;
    T_HostInGrp   tHost;

    memset(&tHost,0,sizeof(T_HostInGrp));

    strncpy(tHost.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    strncpy(tHost.ucInitName,req._iscsiname.c_str(),MAX_INI_NAME_LEN);

    ret=AddHostToGrp(&tHost);
    if (ret==-E_SCS_API_MAPGRP_HOST_EXIST)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_ADDHOSTTO_TARGET target name is %s iscsiname is %s ret is %d\n",req._targetname.c_str(),req._iscsiname.c_str(),ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;

}

STATUS IpsanAdaptor::DelVolFromTarget(const StDelFromTarReq & req)
{
    int            ret=-1;
    T_DelVolFrmGrp tDelVol;
    T_HostInGrp    tHost;

    memset(&tDelVol,0,sizeof(T_DelVolFrmGrp));
    memset(&tHost,0,sizeof(T_HostInGrp));

    tDelVol.sdwLunId=req._lunid;
    strncpy(tDelVol.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    ret=DelVolFromGrp(&tDelVol);
    if (ret==-E_SCS_API_MAPGRP_LUN_NOEXIST || ret==-E_SCS_API_MAPGRP_NOEXIST)
    {
        ret=0;
    }
    ProcessCapiRet(ret);
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_DELVOLFROM_TARGET target name is %s  ret is %d\n",req._targetname.c_str(),ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;

}

STATUS IpsanAdaptor::DelHostFromTarget(const StDelFromTarReq & req)
{
    int           ret=-1;
    T_HostInGrp    tHost;

    memset(&tHost,0,sizeof(T_HostInGrp));   
    strncpy(tHost.ucInitName,req._iscsiname.c_str(),MAX_INI_NAME_LEN);
    strncpy(tHost.cMapGrpName,req._targetname.c_str(),MAX_MAPGRP_NAME_LEN);
    ret=DelHostFromGrp(&tHost);
    if (ret==-E_SCS_API_MAPGRP_HOST_NOEXIST || ret==-E_SCS_API_MAPGRP_NOEXIST)
    {
        ret=0;
    }  
    ProcessCapiRet(ret);
   
    OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_SA_DELHOSTFROM_TARGET target name is %s  ret is %d \n",req._targetname.c_str(),ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}


STATUS IpsanAdaptor::GetStorageInfo(capacity & info)
{
    int           ret=-1;
    T_VdInfo tinfo;
    T_AllVdNames  allvd;
    int64     size;
    memset(&tinfo,0,sizeof(T_VdInfo));
    memset(&allvd,0,sizeof(T_VdInfo));

    ret=GetAllVdNames(&allvd);
    if (ret==0)
    {
        for (int loop=0;loop<allvd.sdwVdNum;loop++)
        {
            strncpy(tinfo.cVdName,allvd.cVdNames[loop],MAX_BLK_NAME_LEN);
            ret=GetVdInfo(&tinfo);
            ProcessCapiRet(ret);
            if (0!=ret)
            {
                break;
            }
            if(!(tinfo.sdwState==(1<<VD_GOOD) || tinfo.sdwState==(1<<VD_DEGRADE)))
            {
                continue;
            }
            if(tinfo.local!=ISLOCAL)
            {
                continue;
            }

            info.iTotalCapacity+=tinfo.sqwTotalCapacity;            
            size=tinfo.sqwFreeCapacity;
            if (size>info.iAllocMaxCapacity)
            {
                info.iAllocMaxCapacity=size;
            }
        }
    }
    else
    {
        ProcessCapiRet(ret);
    }
    info.iTotalCapacity=info.iTotalCapacity/1024;
    info.iAllocMaxCapacity= info.iAllocMaxCapacity/1024;
    //OutPut(SYS_NOTICE,"IpsanAdaptor:rcv EV_STORAGE_GETINFO total %lld alloc %lld free  %lld   ret is %d \n",info.iTotalCapacity,info.iAllocMaxCapacity,info.iFreeCapacity,ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::KeepAlive(void)
{
    int           ret=-1;
    ret=RefreshConTime();
    ProcessCapiRet(ret);
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::GetDataIp(vector<string>  &dataip)
{
    T_IscsiPortInfoList tPortInfo;
    int      ret;
    unsigned int      loop1;
    int               loop2,loop3;
    memset(&tPortInfo,0,sizeof(T_IscsiPortInfoList));
    ret=GetIscsiPortInfo(&tPortInfo);
    if (0!=ret)
    {
        OutPut(SYS_ERROR,"IpsanAdaptor:GetIscsiPortInfo err ,ret is %d\n",ret);
        ProcessCapiRet(ret);
        return ret ;
    }

    T_SystemNetCfgList tNet;
    memset(&tNet,0,sizeof(T_SystemNetCfgList));
    ret=GetSystemNetCfg(&tNet);
    if (0!=ret)
    {
        OutPut(SYS_ERROR,"IpsanAdaptor:GetSystemNetCfg err ,ret is %d\n",ret);
        ProcessCapiRet(ret);
        return ret;
    }
    for (loop1=0;loop1<tPortInfo.udwCtrlNum;loop1++)
    {
        for (loop2=0;loop2<SPR10_ISCSI_PORT_NUM;loop2++)
        {
            //if (tPortInfo.tSingleIscsiPortInfo[loop1].tIscsiPortInfo[loop2].tIscsiPortInfo.udwPortState==PORT_STATE_UP)
            {
                for (loop3=0;loop3<tNet.sdwDeviceNum;loop3++)
                {
                    if (tPortInfo.tSingleIscsiPortInfo[loop1].tIscsiPortInfo[loop2].dwPortNo==tNet.tSystemNetCfg[loop3].udwDeviceId 
                        &&tPortInfo.tSingleIscsiPortInfo[loop1].udwCtrlId==tNet.tSystemNetCfg[loop3].udwCtrlId)
                    {
                        if(strlen(tNet.tSystemNetCfg[loop3].cIpAddr)!=0)
                        {
                            dataip.push_back(tNet.tSystemNetCfg[loop3].cIpAddr);
                        }
                        break;
                    }
                }
            }
        }
    }
    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

STATUS IpsanAdaptor::GetVgInfo(vector<Vginfo> &vginfo)
{
    int      ret;
    T_VdInfo  tVdino;
    T_AllVdNames  allvd;
    memset(&tVdino,0,sizeof(T_VdInfo));
    memset(&allvd,0,sizeof(T_AllVdNames));
    int    loop;
    ret=GetAllVdNames(&allvd);
    Vginfo info;
    if (ret!=0)
    {
        OutPut(SYS_ERROR,"IpsanAdaptor:GetAllVdNames err ,ret is %d\n",ret);
        ProcessCapiRet(ret);
        return ret;
    }
    for (loop=0;loop<allvd.sdwVdNum;loop++)
    {
        strncpy(tVdino.cVdName,allvd.cVdNames[loop],MAX_BLK_NAME_LEN);
        ret=GetVdInfo(&tVdino);
        if (ret!=0)
        {
            OutPut(SYS_ERROR,"IpsanAdaptor:GetVdInfo err ,ret is %d\n",ret);
            ProcessCapiRet(ret);
            return ret;
        }
        if(!(tVdino.sdwState==(1<<VD_GOOD) || tVdino.sdwState==(1<<VD_DEGRADE)))
        {
            continue;
        }
        if(tVdino.local!=ISLOCAL)
        {
            continue;
        }
        info._vgname.assign(tVdino.cVdName);
        info._total_size=tVdino.sqwTotalCapacity/1024;
        info._max_size  =tVdino.sqwTotalCapacity/1024;
        vginfo.push_back(info);
    }

    return ret==0?SUCCESS:ERROR_SA_OPERATE_ERR;
}

