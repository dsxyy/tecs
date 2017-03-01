/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：lvm_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：lvm存储代理常用方法类的实现文件
* 当前版本：1.0

* 作    者：王德恩
* 完成日期：2011.08.05
*
* 修改记录1：
*    修改日期：2011.08.05
*    版 本 号：V1.0
*    修 改 人：王德恩
*    修改内容：创建
* 修改记录2：
*    修改日期：2012.06.11
*    版 本 号：V2.0
*    修 改 人：yanwei
*    修改内容：整改
*******************************************************************************/
#include <sys/mount.h>
#include "lvm.h"
#include "tecs_pub.h"
#include "log_agent.h"

using namespace zte_tecs;

/****************************************************************************************
*                                  本地变量                                             *
****************************************************************************************/
static Mutex *s_ptLvMapLock = NULL;
static map<string,  Mutex *>   s_tLvMutex;

/****************************************************************************************
*                                  lv锁操作接口                                        *
****************************************************************************************/
int LvGetMapMutex(Mutex ** ppmutex)
{
    if (NULL == s_ptLvMapLock)
    {
        s_ptLvMapLock = new Mutex("LvMapLock");
        if (NULL == s_ptLvMapLock)
        {
            return ERROR;
        }
        else
        {
            if (SUCCESS != s_ptLvMapLock->Init())
            {
                delete s_ptLvMapLock;
                s_ptLvMapLock = NULL;
                return ERROR;
            }
        }
    }
    *ppmutex = s_ptLvMapLock;
    return SUCCESS;
}
/******************************************************************************/ 
int getLvMutex(const string vgname,Mutex ** ppmutex)
{
    Mutex *pMutex = NULL;
    Mutex *pMapMutex = NULL;

    if (SUCCESS != LvGetMapMutex(&pMapMutex))
    {
        return ERROR;
    }
    pMapMutex->Lock();
    map<string, Mutex *>::iterator it = s_tLvMutex.find(vgname);
    if (it != s_tLvMutex.end())
    {
        *ppmutex=it->second;
        pMapMutex->Unlock();
        return SUCCESS;
    }
    pMutex=new Mutex(vgname.c_str());
    if (NULL != pMutex)
    {
        if (SUCCESS != pMutex->Init())
        {
            delete pMutex;
        }
        *ppmutex=pMutex;
        s_tLvMutex.insert(make_pair(vgname,pMutex));
    }
    pMapMutex->Unlock();
    return SUCCESS;
}
/******************************************************************************/ 
int  removeLvMutex(const string &vgname)
{
    int ret=ERROR;
    Mutex *pMapMutex;

    if (SUCCESS != LvGetMapMutex(&pMapMutex))
    {
        return ret;
    }
    pMapMutex->Lock();
    map<string, Mutex *>::iterator it = s_tLvMutex.find(vgname);
    if (it != s_tLvMutex.end())
    {
        s_tLvMutex.erase(it);
        ret = SUCCESS;
    }
    pMapMutex->Unlock();
    return ret;
}
/****************************************************************************************
*                                  本地使用的函数封装                                   *
****************************************************************************************/

/****************************************************************************************
*                                  对外接口封装                                        *
****************************************************************************************/
void fullnameofLV(const string &vg,const string &name,string &fullname)
{
    if (vg.empty() || name.empty())
    {
        return;
    }
    fullname = "/dev/"+vg+"/"+name;
}
/******************************************************************************/ 
void alignsizeofLV(int64 &size)
{
    if (0 != size%LVM_ALIGN_SIZE)
    {
        size = ((size/LVM_ALIGN_SIZE) + 1)*LVM_ALIGN_SIZE;
    }
}
/******************************************************************************/ 
bool recycleTmpLv(void)
{
    string sResult ;
    string sCmd;
    string lvname;

    sCmd = "lvs --noheadings -o lv_path 2> /dev/null| grep "+LVM_TMP_PREFIX;
    if (SUCCESS != storage_command(sCmd ,sResult))
    {
        return false;
    }
    stringstream iss(sResult);
    while (getline(iss,lvname))
    {
        deleteLV(lvname);
        OutPut(SYS_NOTICE,"deleteLV %s\n",lvname.c_str());
    }
    return true;
}
/******************************************************************************/ 
bool recycleBaseLV(const string &vgname,uint32 needspace,int64 fileid)
{
    bool findfreebase = false;
    uint32 freespace = 0;
    string cmd,res;
    string lvname;

    if ((vgname.empty()) || (0 == needspace))
    {
        return true;
    }
    getVgFreeSpace(vgname, freespace);
    if (needspace <= freespace)
    {
        return true;
    }
    cmd  = "lvs "+vgname+" --noheadings -o lv_path 2> /dev/null| grep "+LVM_FREE_PREFIX;
    RunCmd(cmd ,res);
    stringstream iss(res);
    while (freespace < needspace)
    {
        findfreebase = false;
        while (getline(iss,lvname))
        {
            if (false == isExistSnapShot(lvname))
            {/*没有snapshot 且不是自己的fileid*/
                OutPut(SYS_DEBUG,"recycleBaseLV %s has no snapshot\n",lvname.c_str());
                if (INVALID_FILEID != fileid)
                {
                    if (string::npos == lvname.find(to_string<int64>(fileid,dec)))
                    {
                        OutPut(SYS_DEBUG,"recycleBaseLV %s is not itself\n",lvname.c_str());
                        findfreebase = true;
                        break;
                    }
                    else
                    {
                        OutPut(SYS_DEBUG,"recycleBaseLV %s is itself\n",lvname.c_str());
                    }
                }
                else
                {
                    findfreebase = true;
                    break;
                }
            }
            OutPut(SYS_DEBUG,"recycleBaseLV %s has snapshot\n",lvname.c_str());
        }
        if (true == findfreebase)
        {
            OutPut(SYS_NOTICE,"recycleBaseLV %s no snapshot",lvname.c_str());
            deleteLV(lvname);
        }
        else
        {
            OutPut(SYS_WARNING,"recycleBaseLV failed for no free baselv\n");
            return false;
        }
        getVgFreeSpace(vgname, freespace);
    }
    OutPut(SYS_DEBUG,"recycleBaseLV success\n");
    return true;
}
/******************************************************************************/ 
void backVgCfg()
{
    string  sRes;
    string cmd="vgcfgbackup"; /*保存在 /etc/lvm/backup 下面*/
    RunCmd(cmd, sRes);
}
/******************************************************************************/ 
void getsizeofLV(int64 &size,const string &vg,const string &lv)
{
    string cmd,res;
    string longname;
    stringstream ss;

    fullnameofLV(vg,lv,longname);
    cmd = "lvs --noheadings --unit B -o lv_size "+longname+" 2>/dev/null |cut -d 'B' -f 1";
    RunCmd(cmd ,res);
    if (res.empty())
    {        
        size = 0;
    }
    else
    {
        ss << res;
        ss >> size;
    }
}
/******************************************************************************/ 
bool renameLV(const string & vg,const string &source,const string &target)
{
    bool ret = false;
    string src,tgt;
    string cmd ;

    if (source.empty() || target.empty())
    {
        return false;
    }
    fullnameofLV(vg,source,src);
    fullnameofLV(vg,target,tgt);
    cmd = "lvrename "+src+" "+tgt;
    ret = runfind(cmd,"Renamed");
    backVgCfg();
    return ret;
}
/******************************************************************************/ 
//创建逻辑卷LV 指定了LVNAME
int createLV(const string &vg, const string &lv,const string &base, int64 size, string &res)
{
    bool ret = false;
    string cmd ;
    //uint32  mfree = 0;//空闲空间 单位MB
    uint32  msize = 0;//单位MB (存放i64LVCapacity 字节转换为M)

    if (vg.empty() || lv.empty() || (0 == size))
    {
        OutPut(SYS_WARNING," createLV failed for Invalid Args!");
        return ERROR_INVALID_ARGUMENT;
    }
    //init
    res = "";

    //单位转换，将容量单位B转为MB.
    msize = (size % LVM_MBYTE ==0)?(size / LVM_MBYTE):(size / LVM_MBYTE + 1);
    if (true == isExistLVName(vg,lv))
    {
        OutPut(SYS_ERROR,"createLV vg:%s,lv:%s already exists!\n",vg.c_str(),lv.c_str());
        res ="/dev/"+vg+"/"+lv;
        return ERR_EXIST;
    }
    /*将回收操作移到调用的地方 便于加锁*/
    #if 0
    getVgFreeSpace(vg,mfree);
    if (msize > mfree)
    {
        if (!recycleBaseLV(vg,msize,INVALID_FILEID))
        {
            OutPut(SYS_ERROR,"createLV failed no free space,size:%dMB,free:%dMB !\n",msize,mfree);
            return ERROR_NO_SPACE;
        }
    }
    #endif 
    if (base.empty())
    {
        cmd = "lvcreate -L "+to_string<uint32>(msize,dec)+"M -n "+lv+" "+vg;
    }
    else
    {
        cmd = "lvcreate -L "+to_string<uint32>(msize,dec)+"M -s "+" -n "+lv+" /dev/"+vg+"/"+base;
    }
    ret = runfind(cmd,"created");
    backVgCfg();
    if (true == ret)
    {
        res = "/dev/"+vg+"/"+lv;
        return SUCCESS; 
    }
    else
    {
        return ERROR;
    }
}
/******************************************************************************/ 
int deleteDmPartition(const string& lv)
{
    char buf[256] = {'\0'};
    readlink(lv.c_str(),buf,sizeof(buf));
    string dm_name_path = buf;
    string dm_name = dm_name_path.substr(dm_name_path.find_last_of('/')+1);
    string holders_path = "/sys/block/"+dm_name+"/holders/";
    DIR *pDir = opendir(holders_path.c_str());
    if (NULL == pDir)
    {
        return ERROR;
    }
    string cmd,res;
    string part_name,part_name_path,lv_part_name;
    struct dirent *pDirent;
    for(;;)
    {
        pDirent = readdir(pDir);
        if (NULL == pDirent)
        {
            break;
        }
        
        if ((0 == strcmp(pDirent->d_name, "."))
             || (0 == strcmp(pDirent->d_name, "..")))
        {
            continue;
        }

        part_name = pDirent->d_name;
        part_name_path = holders_path+part_name+"/dm/name";
        cmd = "cat "+part_name_path;
        RunCmd(cmd,res);
        stringstream iss(res);
        while (getline(iss,lv_part_name))
        {
            cmd = "dmsetup remove "+lv_part_name;
            RunCmd(cmd,res);
        }
    }
    closedir(pDir);
    return SUCCESS;
}
/******************************************************************************/ 
int deleteLV(const string &lv)
{
    bool ret = false;
    string cmd,res;

    if (lv.empty())
    {
        OutPut(SYS_WARNING,"deleteLV failed for Invalid Args!");
        return ERROR_INVALID_ARGUMENT;
    }
    if (false == isExistLVName(lv))
    {
        OutPut(SYS_WARNING,"deleteLV failed for %s not exist\n",lv.c_str());
        return ERROR_DEVICE_NOT_EXIST;
    }

    string tmplv = lv;
    tmplv.erase(0,5);
    replace(tmplv.begin(),tmplv.end(), '/', '-');
    if (check_mount(tmplv))
    {
        string mountdir;
        if (SUCCESS == get_mount(tmplv,mountdir))
        {
            if (0 != umount(mountdir.c_str()))
            {
                OutPut(SYS_ERROR,"deleteLV failed for umount %s failed!\n",mountdir.c_str());
                return ERROR;
            }
            else
            {
                OutPut(SYS_DEBUG,"deleteLV umount %s success!\n",mountdir.c_str());
            }
        }
    }
#if 0
    cmd = "umount "+lv+" 2>&1";
    storage_command(cmd,res);
    if (true == findsubstr(res,"busy"))
    {
        return ERROR_DEVICE_BUSY;
    }
#endif
    cmd ="kpartx -d " +lv;
    storage_command(cmd,res);
    deleteDmPartition(lv);
    cmd = "lvremove -f "+lv;
    ret = runfind(cmd,"successfully");
    backVgCfg();
    return (true != ret)?(ERROR):(SUCCESS);
}
/******************************************************************************/ 
int deleteLV(const string &vg , const string &lv)
{
    bool ret = false;
    string cmd,res;

    if (vg.empty() || lv.empty())
    {
        OutPut(SYS_WARNING,"deleteLV failed for Invalid Args!\n");
        return ERROR_INVALID_ARGUMENT;
    }

    if (false == isExistLVName(vg,lv))
    {
        OutPut(SYS_WARNING,"deleteLV failed for /dev/%s/%s not exist\n",vg.c_str(),lv.c_str());
        return ERROR_DEVICE_NOT_EXIST;
    }

    string tag = vg+"-"+lv;
    if (check_mount(tag))
    {
        string mountdir;
        if (SUCCESS == get_mount(tag,mountdir))
        {
            if (0 != umount(mountdir.c_str()))
            {
                OutPut(SYS_ERROR,"deleteLV failed for umount %s failed!\n",mountdir.c_str());
                return ERROR;
            }
            else
            {
                OutPut(SYS_DEBUG,"deleteLV umount %s success!\n",mountdir.c_str());
            }
        }
    }
#if 0
    cmd = "umount /dev/"+vg+"/"+lv+" 2>&1";
    storage_command(cmd,res);
    if (true == findsubstr(res,"busy"))
    {
        return ERROR_DEVICE_BUSY;
    }
#endif
    cmd ="kpartx -d /dev/" +vg+"/"+lv;
    storage_command(cmd,res);
    deleteDmPartition("/dev/"+vg+"/"+lv);
    cmd = "lvremove -f /dev/"+vg+"/"+lv;
    ret = runfind(cmd,"successfully");
    backVgCfg();
    return (true != ret)?(ERROR):(SUCCESS);
}
/******************************************************************************/ 
int findLV(const string &tag,string &res)
{
    string cmd;
    /*tag允许为空*/
    cmd  = "lvs "+LVM_TECS_VGNAME+" --noheadings -o lv_name 2> /dev/null | grep "+tag;

    return storage_command(cmd,res);
}
/******************************************************************************/ 
bool isExistSnapShot(const string &lv)
{
    string cmd,vgname,lvname;    

    if (lv.empty())
    {
        return false;
    }

    string tmplv = lv;

    lvname=tmplv.substr(tmplv.find_last_of('/')+1);
    tmplv.erase(tmplv.find_last_of('/'));
    vgname=tmplv.substr(tmplv.find_last_of('/')+1);

    if(vgname.empty() || lvname.empty())
    {
        OutPut(SYS_ERROR,"isExistLVName lv name %s get err!\n",lv.c_str()); 
        return false;
    }

    cmd = "lvs "+vgname+" --noheadings -o origin 2> /dev/null |grep "+lvname;
    return runfind(cmd,lvname);
}
/******************************************************************************/ 
bool isExistSnapShot(const string &vg,const string &lv)
{
    string cmd;
    

    if (lv.empty())
    {
        return false;
    }
    cmd = "lvs "+vg+" --noheadings -o origin 2> /dev/null |grep "+lv;
    return runfind(cmd,lv);
}
/******************************************************************************/ 
int getSnapShotCount(const string &lv)
{
    int count = 0;
    string cmd,res,vgname,lvname;    

    if (lv.empty())
    {
        return count;
    }

    string tmplv = lv;

    lvname=tmplv.substr(tmplv.find_last_of('/')+1);
    tmplv.erase(tmplv.find_last_of('/'));
    vgname=tmplv.substr(tmplv.find_last_of('/')+1);

    if(vgname.empty() || lvname.empty())
    {
        OutPut(SYS_ERROR,"isExistLVName lv name %s get err!\n",lv.c_str()); 
        return count;
    }

    cmd = "lvs "+vgname+" --noheadings -o origin 2> /dev/null |grep "+lvname+" |wc -l";
    if(SUCCESS == RunCmd(cmd,res))
    {
        from_string<int>(count,res,dec);        
    }
    return count;
}
/******************************************************************************/ 
int getSnapShotCount(const string &vg,const string &lv)
{
    int count = 0;
    string cmd,res;

    if (vg.empty() || lv.empty())
    {
        return count;
    }
    cmd = "lvs "+vg+" --noheadings -o origin 2> /dev/null |grep "+lv+" |wc -l";
    if(SUCCESS == RunCmd(cmd,res))
    {
        from_string<int>(count,res,dec);
    }
    return count;
}
/******************************************************************************/ 
//判断是存在VG名下的LV名称
bool isExistLVName(const string &lv)
{
    string cmd;

    if (lv.empty())
    {
        return false;
    }

    cmd = "lvs --noheadings -o lv_path "+lv+" 2>/dev/null";
    return runfind(cmd,lv);
}
/******************************************************************************/ 
//判断是存在VG名下的LV名称
bool isExistLVName(const string &vg , const string &lv)
{
    string fullLVname;

    if (vg.empty() || lv.empty())
    {
        return false;
    }
    fullLVname = "/dev/"+vg+"/"+lv;
    return isExistLVName(fullLVname);
}
/******************************************************************************/ 
//判断分区是否已经创建PV
bool isExistPVName(const string &sPVName)
{
    string cmd;

    if (sPVName.empty())
    {
        return false;
    }
    cmd = "pvs --noheadings -o pv_name |grep " +sPVName+" 2>/dev/null";
    return runfind(cmd,sPVName);
}
/******************************************************************************/ 
//判断PV的VG 是否存在,true 表示存在，false表示不存在 ,并返回组名
bool isPVExistVGName(const string & sPVName , string &sVGName)
{
    bool isPVExistVG =false;
    string cmd,res;
    string vgnametag = "VG Name";

    if (sPVName.empty())
    {
        return isPVExistVG;
    }
    sVGName.clear();
    cmd = " pvs "+sPVName+" --noheadings -o vg_name 2>/dev/null" ;
    if (SUCCESS == RunCmd(cmd,res))
    {
        stringstream iss(res);
        if (getline(iss,sVGName))
        {
            isPVExistVG = true;
        }
    }
    return isPVExistVG;
}
/******************************************************************************/ 
//扩展逻辑卷LV容量
int extendLVCapacity(int extendType,
                     const string & sVGName ,
                     const string & sLVName ,
                     int64 i64LVCapacity)
{
    bool ret = false;
    string sCmd ;
    string sUnit = "M"; //单位是MB
    string sResult ;
    string sExtendType;
    uint32  iLVCapacity = 0;//单位MB (存放i64LVCapacity 字节转换为M)

    if ((LVM_EXTENDADD != extendType && LVM_EXTENDTO != extendType)
        || sVGName.empty() || sLVName.empty() || (0 == i64LVCapacity))
    {
        OutPut(SYS_WARNING,"extendLVCapacity failed for Invalid Args!");
        return ERROR_INVALID_ARGUMENT;
    }

    //单位转换，将容量单位B转为MB.
    iLVCapacity = (i64LVCapacity % LVM_MBYTE ==0) ?(i64LVCapacity / LVM_MBYTE) :(i64LVCapacity / LVM_MBYTE + 1);

    //判断sLVName是否在逻辑卷中存在相同的名名称
    if (false == isExistLVName(sVGName,sLVName))
    {//不存在此名称的LV
        OutPut(SYS_WARNING,"extendLVCapacity failed for %s not exist\n",sLVName.c_str());
        return ERROR_DEVICE_NOT_EXIST ;
    }

    //扩展类型
    sExtendType = extendType==LVM_EXTENDADD?"+":"";
    sCmd = "lvextend -L"+sExtendType+to_string<uint32>(iLVCapacity,dec)+sUnit+" /dev/"+sVGName+"/"+sLVName;
    ret = runfind(sCmd,"successfully");
    return (true != ret)?(ERROR):(SUCCESS);
}
/******************************************************************************/ 
//获取LVM分区总空间信息
bool getVGTotalInfo(TVGSpaceTotalInfo &tResVGSpaceTotalInfo)
{
    int64  uiTotalCapacity = 0;    //逻辑卷组中总空间大小，值=lTotalPENum*fPESize,
    int64  uiAllocCapacity = 0;    //逻辑卷组中已经使用空间，值=lAllocPENum*fPESize,
    int64  uiFreeCapacity  = 0;     //逻辑卷组中剩余可用空间，值=lFreePENum*fPESize,
    list<TLvmVgInfo>     lvmVGList;

    //init
    tResVGSpaceTotalInfo.iVGCountNum = 0;
    tResVGSpaceTotalInfo.uiTotalCapacity = 0;
    tResVGSpaceTotalInfo.uiAllocCapacity = 0;
    tResVGSpaceTotalInfo.uiFreeCapacity = 0;

    //generateDiskTotalInfo目前先加到这个位置，以后可以改到其他地方调用，这面就不用了
    generateDiskInfo();//汇总信息 生成硬盘 磁盘信息

    //获取逻辑卷组vg信息
    getVGListInfo(lvmVGList);

    //lvm分区信息lvmDiskDevList
    for (list<TLvmVgInfo>::iterator itDev=lvmVGList.begin();itDev!=lvmVGList.end();++itDev)
    {/* 磁盘空间信息,目前tecs只需要tecs_vg1的大小*/
    	if(itDev->sVGName==LVM_TECS_VGNAME) 
    	{
	        uiTotalCapacity  += itDev->uiTotalPENum * itDev->uiPESize;
	        uiAllocCapacity  += itDev->uiAllocPENum * itDev->uiPESize;
	        uiFreeCapacity   += itDev->uiFreePENum * itDev->uiPESize;
			break;
    	}
    }

    tResVGSpaceTotalInfo.iVGCountNum = lvmVGList.size();
    tResVGSpaceTotalInfo.uiTotalCapacity  = uiTotalCapacity * LVM_MBYTE; //单位转为B
    tResVGSpaceTotalInfo.uiAllocCapacity  = uiAllocCapacity * LVM_MBYTE;
    tResVGSpaceTotalInfo.uiFreeCapacity   = uiFreeCapacity * LVM_MBYTE;

    OutPut(SYS_DEBUG,"getVGTotalInfo VGNum:%lu,Total:%llu,Alloc:%llu,Free:%llu\n",
            tResVGSpaceTotalInfo.iVGCountNum,
            tResVGSpaceTotalInfo.uiTotalCapacity,
            tResVGSpaceTotalInfo.uiAllocCapacity,
            tResVGSpaceTotalInfo.uiFreeCapacity);
    return true;
}
/******************************************************************************/ 
bool insertStrToFile(const string &sStrFile,const string &sInsertStr ,bool isChecked)
{
    ofstream  outfile;
    string    sStrPrint ;
    
    if(isChecked)
    {
        //检查是否有该字符串 如果文件中某一行存在相同的字符则不加入到文件中 
        string::size_type str_pose;
        string sStrTmp ;
         const int LINE_LENGTH = 200; 
        char str[LINE_LENGTH];  
        int iLen = 0;
        ifstream fin(sStrFile.c_str()); 
        
        while( fin.getline(str,LINE_LENGTH) )
        {    
            iLen++;
            sStrTmp = str;
            str_pose = sStrTmp.find(sInsertStr);
            if(str_pose != string::npos)
            {
                //已经存在，不需要添加
                sStrPrint = "'"+sInsertStr+"' is exist in line "+to_string<int>(iLen,dec)+".";
                return true;
            }
        }
    } 
    
    //文件打开方式选项
    //ios::in   0x01 供读不存在则创建.ifstream默认打开方式
    //ios::out  0x02 供写 文件不存在则创建 若文件已经存在则清空原有内容.ofstream默认打开方向.
    //ios::app  0x08 供写 文件不存在则创建 若文件已经存在则在原有内容后写入新的内容.指针位置总在最后.  
    outfile.open(sStrFile.c_str(),ios::app); 
    if(outfile.is_open())
    {
         outfile<<sInsertStr<<endl; 
         sStrPrint = "'"+sInsertStr+"'\tsuccessed insert into file"+sStrFile;
         outfile.close(); 
    }
    else
    {
        sStrPrint = "failed to open file"+sStrFile;
        return false;
    }
    return true;
}
/******************************************************************************/ 
bool isPVExistUnknownDevice(void)
{
    string cmd,res;

    cmd = "pvs --noheadings -o pv_name 2>&1";
    if (SUCCESS != RunCmd(cmd, res))
    {
        OutPut(SYS_ERROR,"isPVExistUnknownDevice RunCmd %s failed",cmd.c_str());
        /*命令执行出错 认为存在unknown device*/ 
        return true;
    }

    if (string::npos != res.find("unknown device"))
    {
        OutPut(SYS_ERROR,"isPVExistUnknownDevice find unknown device");
        return true;
    }

    return false;
}
/******************************************************************************/ 
bool doVgchange(const string &vg)
{
    string cmd,res;

    cmd = "vgchange -ay "+vg+" 2>&1";

    if (SUCCESS != RunCmd(cmd,res))
    {
        OutPut(SYS_ERROR,"doVgchange RunCmd %s failed",cmd.c_str());
        return false;
    }

    if (string::npos != res.find("Refusing activation"))
    {
        OutPut(SYS_ERROR,"doVgchange find Refusing activation");
        return false;
    }
    if (string::npos != res.find("Inconsistent metadata found"))
    {
        OutPut(SYS_ERROR,"doVgchange find Inconsistent metadata found");
        return false;
    } 
    if (string::npos != res.find("Missing device"))
    {
        OutPut(SYS_ERROR,"doVgchange find Missing device");
        return false;
    } 
    if (string::npos != res.find("device not found"))
    {
        OutPut(SYS_ERROR,"doVgchange find device not found");
        return false;
    } 
    #if 0 /*ipsan的架构会有不少io错误，这个不能判别*/
    if (string::npos != res.find("Input/output error"))
    {
        OutPut(SYS_ERROR,"doVgchange find Input/output error");
        return false;
    } 
    #endif

    return true;
}
/******************************************************************************/ 
//生成硬盘 磁盘信息
bool  generateDiskInfo()
{
    bool    bRes = true;
    bool    bisExistVG = false;
    bool    bisExistPV = false;
    bool    bisPVExistVG = false;
    string  sCmd;
    string  sTmp;
    string  sRes ;
    string  sResTmp ;
    string  sPVVGName ;
    string::size_type     str_pose;
    list<TDiskTotalInfo>  diskTotalList;     //硬盘信息
    list<TDiskDevInfo>    lvmDiskDevList;    //lvm对应所有磁盘分区

    if (isPVExistUnknownDevice())
    {
        OutPut(SYS_ERROR,"PV Exist Unknown Device!");
        return false;
    }


    insertStrToFile("/etc/profile","export LVM_SUPPRESS_FD_WARNINGS=1",true);
    //生效/etc/profile
    RunCmd("source /etc/profile" , sRes);
    if (getenv("LVM_SUPPRESS_FD_WARNINGS"))
    {/* 获取环境变量成功 */
        OutPut(SYS_DEBUG,"getenv LVM_SUPPRESS_FD_WARNINGS SUCCESS!");
    }
    else
    {/* 获取环境变量失败 */
        //RunCmd("export LVM_SUPPRESS_FD_WARNINGS=1" , sRes);
        setenv("LVM_SUPPRESS_FD_WARNINGS","1",1);
        //unsetenv("LVM_SUPPRESS_FD_WARNINGS");
        OutPut(SYS_DEBUG,"getenv FAILED.set LVM_SUPPRESS_FD_WARNINGS=1!");
    }


    /*步骤一 判断是否有tecs_vg1组，有则返回*/
    sCmd = "vgs --noheadings -o vg_name 2>/dev/null | grep "+LVM_TECS_VGNAME;
    bisExistVG = runfind(sCmd,LVM_TECS_VGNAME);
    if (bisExistVG == true)
    {
        return doVgchange(LVM_TECS_VGNAME);
        #if 0
        sCmd = "vgchange -ay "+LVM_TECS_VGNAME;
        RunCmd(sCmd,sRes);
        return true;
        #endif
    }

    getDiskInfo(diskTotalList,lvmDiskDevList);

    if (lvmDiskDevList.size() == 0)
    {/*步骤二 没有LVM分区，生成LVM分区，并重启HC */
        OutPut(SYS_WARNING,"no lvm disk,do fdisk lvm!\n");
        bRes = createLVMDisk();
    }
    else
    {
        /*步骤三，有lvm分区，则生成vg组*/
        for (list<TDiskDevInfo>::iterator itDev=lvmDiskDevList.begin();itDev!=lvmDiskDevList.end();++itDev)
        {
            //判断PV中是否已经有对应的分区
            bisExistPV = isExistPVName(itDev->sDevice);
            if (bisExistPV == true)
            {
                bisPVExistVG = isPVExistVGName(itDev->sDevice,sPVVGName);
                if ((bisPVExistVG == true) && (!sPVVGName.empty()))
                {
                    /*其他vg组的lvm不去理会*/
                    continue;
                }

            }

            /*如果不属于任何vg组，则加入tecs_vg1*/
            sTmp = "pvcreate "+itDev->sDevice;
            RunCmd(sTmp , sRes);
            if (bisExistVG == false)
            {
                //初始化LVM
                //system("vgscan"); //不用 会有打印Reading all physical volumes.  This may take a while..
                //#建立卷组
                sTmp = "vgcreate "+LVM_TECS_VGNAME+" "+itDev->sDevice;
                RunCmd(sTmp , sRes);
                str_pose = sRes.find("successfully");
                if (str_pose != string::npos)
                {//创建成功
                    OutPut(SYS_NOTICE,"%s success!,sRes %s\n",sTmp.c_str(),sRes.c_str());
                    bisExistVG = true;
                }
                else
                {
                    OutPut(SYS_ERROR,"%s failed!,sRes %s\n",sTmp.c_str(),sRes.c_str());
                    bisExistVG = false;
                }
            }
            else
            {
                //#添加到组
                sTmp = "vgextend "+LVM_TECS_VGNAME+" "+itDev->sDevice;
                RunCmd(sTmp, sRes);
                OutPut(SYS_NOTICE,"%s !,sRes %s\n",sTmp.c_str(),sRes.c_str());
            }
        }
    }

    bRes = doVgchange(LVM_TECS_VGNAME);
    #if 0
    sTmp = "vgchange -ay "+LVM_TECS_VGNAME;
    RunCmd(sTmp , sRes);
    #endif
    return bRes;
}
/******************************************************************************/ 
//获取硬盘名称接口
bool getDiskName(list<string> &diskListInfo)
{
    string sDiskNum;
    list<TDiskTotalInfo>  diskTotalList;     //硬盘信息
    list<TDiskDevInfo>    lvmDiskDevList;    //lvm对应所有磁盘分区


    diskListInfo.clear();
    getDiskInfo(diskTotalList , lvmDiskDevList);

    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {// 总的磁盘信息
        sDiskNum = it->sDiskName;
        diskListInfo.push_back(sDiskNum);
    }

    return true;
}
/******************************************************************************/ 
bool  getVgFreeSpace(const string &sVGName,uint32 &iFreeSpace)
{
    uint32  uiFreeCapacity  = 0;     //逻辑卷组中剩余可用空间，值=lFreePENum*fPESize
    list<TLvmVgInfo>     lvmVGList;
    //获取逻辑卷组vg信息
    getVGListInfo(lvmVGList);
    //lvm分区信息lvmDiskDevList
    for (list<TLvmVgInfo>::iterator itDev=lvmVGList.begin();itDev!=lvmVGList.end();++itDev)
    {/* 磁盘空间信息*/
        if (itDev->sVGName == sVGName)
        {
            uiFreeCapacity = (itDev->uiFreePENum) * ( itDev->uiPESize);
            break;
        }
    }
    iFreeSpace = uiFreeCapacity;
    return true;
}
/******************************************************************************/ 
//获取磁盘信息(包含LVM分区、及其他分区)
bool getDiskInfo(list<TDiskTotalInfo>  &diskTotalList , list<TDiskDevInfo>  &lvmDiskDevList)
{
    string  sCmd = "fdisk -l 2> /dev/null";
    bool    bRes = false;
    TDiskDevInfo   tmpDiskDevInfo;

    //执行fdisk -l，清空list
    diskTotalList.clear();
    lvmDiskDevList.clear();
    runCommandGetList(sCmd , diskTotalList);  //执行“fdisk -l”命令 并解析，生成基本的硬盘和分区信息

    //获取LVM分区信息
    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {// 总的磁盘信息
        for (list<TDiskDevInfo>::iterator itDev=(it->diskDevList).begin();itDev!=(it->diskDevList).end();++itDev)
        {//遍历所有分区
            if (itDev ->iId == 0x8e)
            {//lvm分区
                tmpDiskDevInfo.sDiskName  = itDev ->sDiskName;
                tmpDiskDevInfo.sDevice    = itDev ->sDevice;
                tmpDiskDevInfo.sBoot      = itDev->sBoot;
                tmpDiskDevInfo.iStart     = itDev->iStart;
                tmpDiskDevInfo.iEnd       = itDev->iEnd;
                tmpDiskDevInfo.iBlocks    = itDev->iBlocks;
                tmpDiskDevInfo.sPlus      = itDev->sPlus;
                tmpDiskDevInfo.iId        = itDev->iId;
                tmpDiskDevInfo.sSystem    = itDev->sSystem;
                lvmDiskDevList.push_back(tmpDiskDevInfo);
            }
        }
    }
    bRes = true;
    return bRes;

}
/******************************************************************************/ 
//获取某个磁盘分区信息sDevice磁盘名称
bool getDiskDevInfo(const string & sDevice , list<TDiskDevInfo>  &diskDevList,int &iUsedCylinder)
{
    string                sCmd = "fdisk -l 2> /dev/null";
    bool                  bRes = false;
    TDiskDevInfo          tmpDiskDevInfo;
    list<TDiskTotalInfo>  diskTotalList;

    //执行fdisk -l，清空list

    diskDevList.clear();
    runCommandGetList(sCmd , diskTotalList);  //执行“fdisk -l”命令 并解析，生成基本的硬盘和分区信息
    //获取分区信息
    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {// 总的磁盘信息
        if (it ->sDiskName == sDevice)
        {//是需要查找的分区
            iUsedCylinder = it->iUsedCylinder;
            for (list<TDiskDevInfo>::iterator itDev=(it->diskDevList).begin();itDev!=(it->diskDevList).end();++itDev)
            {//遍历所有分区
                tmpDiskDevInfo.sDiskName  = itDev ->sDiskName;
                tmpDiskDevInfo.sDevice    = itDev ->sDevice;
                tmpDiskDevInfo.sBoot      = itDev->sBoot;
                tmpDiskDevInfo.iStart     = itDev->iStart;
                tmpDiskDevInfo.iEnd       = itDev->iEnd;
                tmpDiskDevInfo.iBlocks    = itDev->iBlocks;
                tmpDiskDevInfo.sPlus      = itDev->sPlus;
                tmpDiskDevInfo.iId        = itDev->iId;
                tmpDiskDevInfo.sSystem    = itDev->sSystem;
                diskDevList.push_back(tmpDiskDevInfo);
            }
            break;
        }
    }
    bRes = true;
    return bRes;

}
/******************************************************************************/ 
//获取逻辑卷组VG信息
bool getVGListInfo(list<TLvmVgInfo>    &lvmVGList)
{
    string sCmd = "vgs --noheadings -o vg_name,vg_uuid,lv_count,pv_count,vg_extent_count,vg_free_count 2>/dev/null";
    bool    bRes = false;
    TDiskDevInfo   tmpDiskDevInfo;

    lvmVGList.clear();//Lvm逻辑卷组vg列表 清空
    runCommandGetList(sCmd , lvmVGList); //生成逻辑卷组vg信息
    bRes = true;
    return bRes;
}
/******************************************************************************/ 
//获取逻辑LV信息
bool getLVListInfo(list<string>    &lvmLVNameList,list<TLvmVgInfo>    &lvmVGList)
{
    string sCmd = "lvs --noheadings -o lv_path,lv_uuid 2>/dev/null";
    bool    bRes = false;
    TDiskDevInfo   tmpDiskDevInfo;

    lvmLVNameList.clear();//Lvm逻辑卷组vg列表 清空
    getVGListInfo(lvmVGList);
    runCommandGetList(sCmd , lvmLVNameList , lvmVGList); //生成逻辑卷组vg信息
    bRes = true;
    return bRes;
}
/******************************************************************************/ 
STATUS getLVuuid(const string &vgname, const string &lvname,string &vol_uuid)
{
    string  result;
    int     ret;
    string sCmd = "lvs --noheadings -o lv_uuid /dev/"+vgname+"/"+lvname+" 2>/dev/null";
    ret=RunCmd(sCmd,result);
    if(ret!=SUCCESS)
    {
        return ret;
    }
    if (!result.empty())
    {    
        vol_uuid=result.substr(0,result.size()-1);
        return SUCCESS;
    }  
    return ERROR;
}

/******************************************************************************/ 
STATUS getLVNameByUuid(string &lvname,const string &vol_uuid)
{
    int     ret = ERROR;
    string  res;
    string  cmd;

    if(vol_uuid.empty())
    {
        return ret;
    }

    cmd = "lvs --noheadings -o lv_path,lv_uuid 2>/dev/null |grep "+vol_uuid+" |awk '{print $1}'";
    ret=RunCmd(cmd,res); 
    if(ret != SUCCESS)
    {
        return ret;
    }
    lvname = res;
    ret = SUCCESS;
    return ret;
}

//private 私有方法

//对“fdisk -l”命令返回的结果进行解析（按行解析）
void  analyseFdiskList(const char *str , string *sDiskName , list<TDiskTotalInfo> &diskResTotalList)
{
    char  cDevice[100]="";
    char  cBoot[100]="";
    int   iStart = 0;
    int   iEnd = 0;
    int   iBlocks = 0; /* 单位KB*/
    int   iId = 0;
    char  cSystem[100]="";
    char  cPlus[10]="" ;

    char  cStrTmp[100]="";
    int   iCylinder = 0;

    char  cDiskNameTmp[100]="";
    char  cDiskName[100]="";
    float  fCapacity = 0.0;
    char  cCapacityUnitTmp[100]="";
    char  cCapacityUnit[100]="";
    long  lCapacityByte = 0;
    char  cCapacityUnitBytes[100]="";
    int   iWhileCnt = 0;
    TDiskTotalInfo   tmpDiskInfo;
    TDiskDevInfo     tmpDiskDevInfo;
    string::size_type  find_pose;

    if (NULL == str)
    {
        return;
    }
    sscanf( str, "%99s%99s",cDevice,cBoot);
    if (!strcmp(cDevice,"Disk"))
    {/* 读取磁盘信息 */
        sscanf( str, "%99s%99s%f%99s%lld%99s",cDevice,cDiskNameTmp,&fCapacity,cCapacityUnitTmp,&lCapacityByte,cCapacityUnitBytes);
        /* 去除读取的磁盘信息中的冒号 :*/
        while (cDiskNameTmp[iWhileCnt]!='\0')
        {
            if (iWhileCnt >100)
            {
                break;
            }
            if (cDiskNameTmp[iWhileCnt]==':')
            {
                break;
            }
            cDiskName[iWhileCnt]=cDiskNameTmp[iWhileCnt];
            iWhileCnt++;
        }
        /* 去除读取的单位中的逗号，*/
        iWhileCnt = 0;
        while (cCapacityUnitTmp[iWhileCnt]!='\0')
        {
            if (iWhileCnt >100)
            {
                break;
            }
            if (cCapacityUnitTmp[iWhileCnt]==',')
            {
                break;
            }
            cCapacityUnit[iWhileCnt]=cCapacityUnitTmp[iWhileCnt];
            iWhileCnt++;
        }
        if ((0 == lCapacityByte) || (0 == fCapacity))
        {
            return;
        }
        tmpDiskInfo.sDiskName=cDiskName;
        tmpDiskInfo.iUsedCylinder = 0; /* 初始磁盘磁道使用信息为0 */
        find_pose = tmpDiskInfo.sDiskName.find("/");
        if (find_pose == string::npos)
        {
            //磁盘名称中不存在/ 返回
            return;
        }
        find_pose = tmpDiskInfo.sDiskName.find("mapper");
        if (find_pose != string::npos) /*逻辑卷，直接过滤*/
        {
            *sDiskName="lvm";
            //磁盘名称中不存在/ 返回
            return;
        }
        //sDiskName 出参用于后面判断哪个磁盘重要的输出
        *sDiskName = cDiskName;
        find_pose = tmpDiskInfo.sDiskName.find("/dm-");
        if (find_pose != string::npos)
        {
            //磁盘名称中含有/dm-,表示虚拟磁盘的不需要返回
            return;
        }
        tmpDiskInfo.fCapacity=fCapacity;
        tmpDiskInfo.sCapacityUnit=cCapacityUnit;
        tmpDiskInfo.lCapacityByte=lCapacityByte;
        tmpDiskInfo.sCapacityUnitBytes=cCapacityUnitBytes;
        diskResTotalList.push_back(tmpDiskInfo);
        return;
    }

    find_pose = sDiskName->find("/dm-");
    if (find_pose != string::npos)
    {
        //磁盘名称中含有/dm-,表示虚拟磁盘的不需要返回
        return;
    }
    find_pose = sDiskName->find("lvm");
    if (find_pose != string::npos)
    {
        //磁盘名称中含有lvm,表示是逻辑卷，不需要解析
        return;
    }
    //读取磁盘总分区磁柱信息
    if (!strcmp(cBoot,"heads,"))
    {
        sscanf( str, "%99s%99s%99s%99s%d", cStrTmp,cStrTmp,cStrTmp,cStrTmp,&iCylinder);
        for (list<TDiskTotalInfo>::iterator it=diskResTotalList.begin();it!=diskResTotalList.end();++it)
        {/* 总的磁盘信息*/
            if (it->sDiskName == *sDiskName)
            {
                it->iCylinder = iCylinder;
                break;
            }
        }
    }
    /* 判断读取的boot是否为*，然后判断容量后面有没有加号 */
    if (!strcmp(cBoot,"*"))
    {/* 读取的Boot为*,中间有个*，需要读取 */
        sscanf( str, "%99s%99s%d%d%d%9s", cDevice,cBoot,&iStart,&iEnd,&iBlocks,cPlus);
        if ((!strcmp(cPlus,"+")) || (!strcmp(cPlus,"-")))
        {/* Boot为* blocks 后面有加号时 处理方式
                      * 如 /dev/sda1   *           1         127     1020096   83  Linux
                      */
            sscanf( str, "%99s%99s%d%d%d%s%x%99s", cDevice,cBoot,&iStart,&iEnd,&iBlocks,cPlus,&iId,cSystem);
        }
        else
        {/* Boot为* blocks 后面有加号时 处理方式
                      * 如 /dev/sdh1   *           1         127     1020096+   8e Linux LVM
                      */
            sscanf( str, "%99s%99s%d%d%d%x%99s", cDevice,cBoot ,&iStart,&iEnd,&iBlocks,&iId,cSystem);
            strcpy(cPlus,"");
        }
    }
    else
    {/* 读取的Boot为无效，就不需要读取* */
        strcpy(cBoot,"");
        sscanf( str, "%99s%d%d%d%9s", cDevice,&iStart,&iEnd,&iBlocks,cPlus);
        if ((!strcmp(cPlus,"+")) || (!strcmp(cPlus,"-")))
        {/* blocks 后面有加号时 处理方式
                      * 如 /dev/sda6           32125       32520     3180838+  8e  Linux LVM
                      */
            sscanf( str, "%99s%d%d%d%s%x%99s", cDevice,&iStart,&iEnd,&iBlocks,cPlus,&iId,cSystem);
        }
        else
        {/* blocks 后面没有加号时 处理方式
                      * 如 /dev/sda3           25624       31997    51199155   8e  Linux LVM
                      */
            sscanf( str, "%99s%d%d%d%x%99s", cDevice,&iStart,&iEnd,&iBlocks,&iId,cSystem);
            strcpy(cPlus,"");
        }
    }
    /* Output the data read */
    if ((0 == iId) ||(0 == iEnd) || (0 == iStart))
    {
        return;
    }
    for (list<TDiskTotalInfo>::iterator it=diskResTotalList.begin();it!=diskResTotalList.end();++it)
    {/* 总的磁盘信息*/
        if (it->sDiskName == *sDiskName)
        {
            if ((iId !=0x5) && (iId !=0xf) )
            {
                it->iUsedCylinder += iEnd - iStart +1;
            }
            tmpDiskDevInfo.sDiskName=*sDiskName;
            tmpDiskDevInfo.sDevice=cDevice;
            tmpDiskDevInfo.sBoot=cBoot;
            tmpDiskDevInfo.iStart=iStart;
            tmpDiskDevInfo.iEnd=iEnd;
            tmpDiskDevInfo.iBlocks=iBlocks;
            tmpDiskDevInfo.sPlus=cPlus;
            tmpDiskDevInfo.iId=iId;
            tmpDiskDevInfo.sSystem=cSystem;
            it->diskDevList.push_back(tmpDiskDevInfo);  //所有分区
            break;
        }
    }
}
/******************************************************************************/ 
//解析 lvdisplay ，生成lv名称列表到全局变量lvmLVNameList
//获取LV的NAME列表
void analyseLvdisplayLVName(const char *str ,string *psVGName , string *psLvFullName,list<string> &lvmLVNameList,list<TLvmVgInfo>   &lvmVGList)
{
    char cLvName[100] = "";
    char cLVUUID[100] = "";
    string sLVName ;
    string sLvFullName ;
    string sTmpVGName;
    string sTmp ;
    string::size_type last_pose;
    string::size_type last2_pose;
    TLVMLVInfo     tmpTLVMLVInfo;

    if (NULL == str)
    {
        return;
    }
    sscanf( str, "%99s%99s",cLvName,cLVUUID);

    //处理lv_path
    sLvFullName = cLvName;
    last_pose = sLvFullName.rfind("/");//查找最后一个出现
    sLVName = sLvFullName.substr(last_pose+1);//获取lvname
    lvmLVNameList.push_back(sLVName);
    sTmp = sLvFullName.substr(0,last_pose);//去掉lvname
    last2_pose = sTmp.rfind("/");//查找最后一个出现(已经去掉了一个)
    sTmpVGName = sTmp.substr(last2_pose+1);//获取vgname
    tmpTLVMLVInfo.sLVName =sLVName;
    tmpTLVMLVInfo.sLVPathName = sLvFullName;
    tmpTLVMLVInfo.sLVUUID = cLVUUID;
    tmpTLVMLVInfo.uiCurrentLE = 0;
    *psVGName = sTmpVGName;
    *psLvFullName = sLvFullName;
    for (list<TLvmVgInfo>::iterator it=lvmVGList.begin();it!=lvmVGList.end();++it)
    {
        if (it->sVGName == sTmpVGName)
        {
            it->lvmLVList.push_back(tmpTLVMLVInfo);
        }
    }

    return;
}

/******************************************************************************/ 
//对vgdisplay 进行解析获取信息保存到lvmVGList
//获取VG组信息
void analyseVgdisplay(const char *str, string *psVGName ,  list<TLvmVgInfo>    &lvmVGList)
{
    char cVGName[100] = "";
    int    iCurLVCnt = 0;         //逻辑卷组中逻辑卷LV个数
    int    iCurPVCnt = 0;         //逻辑卷组中物理分区PV个数
    uint32   uiTotalPENum = 0;       //逻辑卷组中PE的数量
    uint32   uiFreePENum = 0;        //逻辑卷组中剩余可用的PE数量
    uint32   uiAllocPENum = 0;      //逻辑卷组中已分配的PE的数量
    uint32   uiPESize = 0;
    char   sVGUUID[100] = "";           //逻辑卷组标识
    TLvmVgInfo tmpTLvmVgInfo;
    string sCmd,res;
    stringstream ss;
    
    if (NULL == str)
    {
        return;
    }

    tmpTLvmVgInfo.iCurLVCnt = 0;
    tmpTLvmVgInfo.iCurPVCnt = 0;
    tmpTLvmVgInfo.uiPESize = 0;
    tmpTLvmVgInfo.uiTotalPENum = 0;
    tmpTLvmVgInfo.uiAllocPENum = 0;
    tmpTLvmVgInfo.uiFreePENum = 0;

    sscanf(str, "%99s%99s%d%d%d%d",cVGName,sVGUUID,&iCurLVCnt,&iCurPVCnt,&uiTotalPENum,&uiFreePENum);
    *psVGName = cVGName;
    uiAllocPENum = uiTotalPENum - uiFreePENum;
    tmpTLvmVgInfo.sVGName = cVGName;
    tmpTLvmVgInfo.sVGUUID = sVGUUID;
    tmpTLvmVgInfo.iCurLVCnt = iCurLVCnt;
    tmpTLvmVgInfo.iCurPVCnt = iCurPVCnt;
    tmpTLvmVgInfo.uiTotalPENum = uiTotalPENum;
    tmpTLvmVgInfo.uiFreePENum = uiFreePENum;
    tmpTLvmVgInfo.uiAllocPENum = uiAllocPENum;

    //获取PE大小
    sCmd = "vgs "+tmpTLvmVgInfo.sVGName+" --noheadings --unit m -o vg_extent_size 2>/dev/null "+"|cut -d '.' -f 1";
    storage_command(sCmd, res);
    ss << res;
    ss >> uiPESize;

    tmpTLvmVgInfo.uiPESize = uiPESize;
    tmpTLvmVgInfo.sPESizeUnit.assign("MiB");
    lvmVGList.push_back(tmpTLvmVgInfo);
    return;
}
/******************************************************************************/ 
//解析 lvdisplay | grep '/dev/组名/lv_虚拟机ID_'  | awk '{  print $3 }'，生成虚拟机ID的DiskID信息
void analyseDiskIDList(const char *str, list<uint32> &uiVMDiskIDList)
{
    char cLVSource[100] = "";
    string::size_type last_pose;
    string sLVSource ;
    string sVMDiskID ;
    uint32 uiVMDiskID = 0;
    
    if (NULL == str)
    {
        return;
    }
    sscanf( str, "%99s",cLVSource);
    sLVSource = cLVSource;
    last_pose = sLVSource.rfind("_");//查找最后一个出现的"_"
    if (last_pose != string::npos)
    {
        sVMDiskID = sLVSource.substr(last_pose+1);//获取VMDiskID
        from_string<uint32>(uiVMDiskID, sVMDiskID, dec);
        uiVMDiskIDList.push_back(uiVMDiskID);
    }
}
/******************************************************************************/ 
//对linux命令(如cat、fdisk等有输出信息的linux命令)进行执行，并输出List链表
bool runCommandGetList(const string& command, list<TDiskTotalInfo> &diskTotalList)
{
    bool    bRes = false;
    string  sResult ;
    char    *s  = new char[STORAGE_MAX_CMDRESLEN];
    FILE    *fp = NULL;
    string  sDiskName   ;

    bzero(s, 2047);
    diskTotalList.clear();

    if (command.empty())
    {
        delete[] s;
        return bRes;
    }
    if ((fp = popen(command.c_str(), "r")) == NULL)
    {
        OutPut(SYS_ERROR,"Failed to open pipe!");
        delete[] s;
        return bRes;
    }
    while (fgets(s, STORAGE_MAX_CMDRESLEN, fp) != NULL)
    {
        sResult += s;
        //按行解析字符串
        analyseFdiskList(s, &sDiskName , diskTotalList); //对读取的字符串某行进行解析
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}
/******************************************************************************/ 
//对vgdisplay 进行解析获取信息保存到lvmVGList
bool runCommandGetList(const string& command, list<TLvmVgInfo> &lvmVGList)
{
    bool    bRes = false;
    string  sResult ;
    char    *s  = new char[STORAGE_MAX_CMDRESLEN];
    FILE    *fp = NULL;
    string  sVGName ;


    bzero(s, 2047);
    lvmVGList.clear();
    if (command.empty())
    {
        delete[] s;
        return bRes;
    }
    if ((fp = popen(command.c_str(), "r")) == NULL)
    {
        OutPut(SYS_ERROR,"Failed to open pipe!" );
        delete[] s;
        return bRes;
    }
    while (fgets(s, STORAGE_MAX_CMDRESLEN, fp) != NULL)
    {
        sResult += s;
        //按行解析字符串
        analyseVgdisplay(s , &sVGName , lvmVGList);
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}
/******************************************************************************/ 
//对lvdisplay命令进行执行，并输出lvmLVNameList
bool runCommandGetList(const string& command,list<string> &lvmLVNameList , list<TLvmVgInfo>   &lvmVGList)
{
    bool    bRes = false;
    string  sResult ;
    char    *s  = new char[STORAGE_MAX_CMDRESLEN];
    FILE    *fp = NULL;
    string  sLVVGName;
    string  sLvFullName;

    bzero(s, 2047);
    lvmLVNameList.clear();

    if (command.empty())
    {
        delete[] s;
        return bRes;
    }
    if ((fp = popen(command.c_str(), "r")) == NULL)
    {
        OutPut(SYS_ERROR,"Failed to open pipe!" );
        delete[] s;
        return bRes;
    }
    while (fgets(s, STORAGE_MAX_CMDRESLEN, fp) != NULL)
    {
        sResult += s;
        //按行解析字符串
        analyseLvdisplayLVName(s,&sLVVGName,&sLvFullName,lvmLVNameList ,lvmVGList);
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}
/******************************************************************************/ 
//解析 lvdisplay | grep '/dev/组名/lv_虚拟机ID_'  | awk '{  print $3 }'，生成虚拟机ID的DiskID信息
bool runCommandGetList(const string& command,list<uint32> &uiVMDiskIDList)
{
    bool    bRes = false;
    string  sResult;
    char    *s  = new char[STORAGE_MAX_CMDRESLEN];
    FILE    *fp = NULL;
    string  sLVVGName;
    string  sLvFullName;

    bzero(s, 2047);
    uiVMDiskIDList.clear();


    if (command.empty())
    {
        delete[] s;
        return bRes;
    }
    if ((fp = popen(command.c_str(), "r")) == NULL)
    {
        OutPut(SYS_ERROR,"Failed to open pipe!" );
        delete[] s;
        return bRes;
    }
    while (fgets(s, STORAGE_MAX_CMDRESLEN, fp) != NULL)
    {
        sResult += s;
        //按行解析字符串
        analyseDiskIDList(s,uiVMDiskIDList);
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}

/******************************************************************************/ 
//创建lvm分区
bool createLVMDisk()
{
    bool    bRes = false;
    bool    bResLVMDiskScript = false;
    uint32  uiDiskFullNum =0;//记录磁盘没有剩余信息的数目
    list<TDiskTotalInfo>  diskTotalList;     //硬盘信息
    list<TDiskDevInfo>    lvmDiskDevList;    //lvm对应所有磁盘分区
    string                sRes ;

    getDiskInfo(diskTotalList , lvmDiskDevList);

    ////创建lvm分区脚本文件
    bResLVMDiskScript = createLVMDiskScript();
    if (!bResLVMDiskScript)
    {
        OutPut(SYS_ERROR,"createLVMDisk failed");
        return bRes;
    }

    //对所有磁盘进行分区
    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {
        string bashstr;
        string sDiskDevNum;
        if (it->iUsedCylinder < it->iCylinder)
        {
            //还有剩余磁道进行分区
            string sNumDev ;
            int   iWhileCnt = 0;
            bool bDiskDevIsExist = false;
            list<TDiskDevInfo>    diskDevListTmp;     //硬盘信息
            int                   iUsedCylinder = 0;
            do
            {
                //如果分区数目小于最大做循环处理
                iWhileCnt++;
                if (iWhileCnt > LVM_MAXDEV_ONEDISK)
                {
                    //超过预定的最大分区数目
                    OutPut(SYS_ERROR,"find disk num error!iWhileCnt:%d\n",iWhileCnt);
                    break;
                }

                //fdisk 创建分区
                bashstr = "bash fdisknewdev.sh "+it->sDiskName;
                //system( bashstr.c_str() );//执行脚本fdisksh脚本创建分区
                RunCmd(bashstr, sRes);
                OutPut(SYS_NOTICE,"%s ,sRes %s\n",bashstr.c_str(),sRes.c_str());
                diskDevListTmp.clear();
                getDiskDevInfo(it->sDiskName , diskDevListTmp,iUsedCylinder);
            }while (iUsedCylinder < it->iCylinder);


            //对比新旧分区列表信息(diskTotalListTmp )，找到新增的分区编号
            for (list<TDiskDevInfo>::iterator itDevNew = diskDevListTmp.begin(); itDevNew!=diskDevListTmp.end(); ++itDevNew)
            {//遍历所有分区
                bDiskDevIsExist = false;
                for (list<TDiskDevInfo>::iterator itDevOld = (it->diskDevList).begin(); itDevOld!=(it->diskDevList).end(); ++itDevOld)
                {//遍历所有分区
                    if (itDevOld->sDevice == itDevNew->sDevice)
                    {
                        bDiskDevIsExist = true;
                        continue;
                    }
                }
                if (false == bDiskDevIsExist)
                {
                    //这个分区不存在.表示新增的分区，修改为8e LVM
                    //获取编号
                    sNumDev = (itDevNew->sDevice).substr((itDevNew->sDiskName).length());//获取VMDiskID
                    bashstr = "bash fdisktolvm.sh "+it->sDiskName+" "+sNumDev;
                    RunCmd(bashstr, sRes);
                    OutPut(SYS_ERROR,"%s,sRes:%s",bashstr.c_str(),sRes.c_str());
                    // break;
                }
            }
        }
        else
        {
            OutPut(SYS_ERROR,"%s has no Cylinder to fdisk lvm!\n",it->sDiskName.c_str());
            ++uiDiskFullNum;
            continue;
        }
    }
    if (uiDiskFullNum < diskTotalList.size())
    {
        //没有LVM分区，磁盘还有剩余的磁道进行分区
        bRes = true;
        OutPut(SYS_ERROR," do fdisk Lvm success,reboot.....!\n");
        system("reboot");//设置分区，需要重启HC，才能生效
    }
    else
    {
        //没有LVM分区，磁盘没有剩余的磁道进行分区
        bRes = false;
        OutPut(SYS_ERROR,"do fdisk Lvm failed,Disk has no free space to lvm!\n");
    }
    return bRes;
}
/******************************************************************************/ 
//创建lvm分区脚本文件
bool createLVMDiskScript()
{
    ofstream outfile;
    string  sStr;

    //文件打开方式选项
    //ios::in   0x01 供读不存在则创建.ifstream默认打开方式
    //ios::out  0x02 供写 文件不存在则创建 若文件已经存在则清空原有内容.ofstream默认打开方向.
    //ios::app  0x08 供写 文件不存在则创建 若文件已经存在则在原有内容后写入新的内容.指针位置总在最后.
    outfile.open("fdisknewdev.sh",ios::out);
    if (outfile.is_open())
    {
        sStr = "#!/bin/bash";
        outfile<<sStr<<endl;
        sStr = "#filename:fdisknewdev.sh";
        outfile<<sStr<<endl;
        sStr = "fdisk $1 <<EOF";
        outfile<<sStr<<endl;
        /* n p 2 3 4为了避免没有扩展分区添加的*/
        sStr = "n";
        outfile<<sStr<<endl;
        sStr = "p";
        outfile<<sStr<<endl;
        sStr = "2";
        outfile<<sStr<<endl;
        sStr = "n";
        outfile<<sStr<<endl;
        sStr = "p";
        outfile<<sStr<<endl;
        sStr = "3";
        outfile<<sStr<<endl;
        sStr = "n";
        outfile<<sStr<<endl;
        sStr = "p";
        outfile<<sStr<<endl;
        sStr = "4";
        outfile<<sStr<<endl;
        sStr = "n";
        outfile<<sStr<<endl;
        sStr = "l";
        outfile<<sStr<<endl;
        sStr = "";
        outfile<<sStr<<endl;
        sStr = "";
        outfile<<sStr<<endl;
#if 0
        sStr = "t";
        outfile<<sStr<<endl;
        sStr = "$2";
        outfile<<sStr<<endl;
        sStr = "8e";
        outfile<<sStr<<endl;
#endif
        sStr = "w";
        outfile<<sStr<<endl;
        sStr = "EOF";
        outfile<<sStr<<endl;

        outfile.close();
    }
    else
    {
        OutPut(SYS_ERROR,"can't createLVMDiskScript fdisknewdev.sh\n");
    }
    outfile.open("fdisktolvm.sh",ios::out);
    if (outfile.is_open())
    {
        sStr = "#!/bin/bash";
        outfile<<sStr<<endl;
        sStr = "#filename:fdisktolvm.sh";
        outfile<<sStr<<endl;
        sStr = "fdisk $1 <<EOF";
        outfile<<sStr<<endl;
        sStr = "t";
        outfile<<sStr<<endl;
        sStr = "$2";
        outfile<<sStr<<endl;
        sStr = "8e";
        outfile<<sStr<<endl;
        sStr = "w";
        outfile<<sStr<<endl;
        sStr = "EOF";
        outfile<<sStr<<endl;

        outfile.close();
    }
    else
    {
        OutPut(SYS_ERROR,"can't createLVMDiskScript fdisktolvm.sh\n");
        return false;
    }
    return true;
}
/******************************************************************************/ 
//激活所有的LV
bool ActivateAllLV()
{
    list<string>         lvmLVNameList;
    list<TLvmVgInfo>     lvmVGList;
    string               sTmp;
    string               sRes;


    getLVListInfo(lvmLVNameList , lvmVGList);
    for (list<string>::iterator itLvm=lvmLVNameList.begin(); itLvm!=lvmLVNameList.end(); ++itLvm)
    {/* 磁盘信息*/
        sTmp = "lvchange -ay /dev/"+LVM_TECS_VGNAME+"/"+*itLvm;
        RunCmd(sTmp , sRes);
        OutPut(SYS_DEBUG," ActivateAllLV %s,sRes:%s\n",sTmp.c_str(),sRes.c_str());
    }
    return true;
}
/****************************************************************************************
*                                  中间层重构函数                                       *
****************************************************************************************/
int getLvMutex(Mutex ** ppmutex)
{
    return getLvMutex(LVM_TECS_VGNAME,ppmutex);
}
/******************************************************************************/ 
void fullnameofLV(const string &name,string &fullname)
{
    if (name.empty())
    {
        return;
    }
    fullnameofLV(LVM_TECS_VGNAME,name,fullname);
}
/******************************************************************************/ 
bool recycleBaseLV(uint32 needspace,int64 fileid)
{
    return recycleBaseLV(LVM_TECS_VGNAME,needspace,fileid);
}
/******************************************************************************/ 
void getsizeofLV(int64 &size,const string &lv)
{
    getsizeofLV(size,LVM_TECS_VGNAME,lv);
}
/******************************************************************************/ 
int createLV(const string &lv, const string &base,int64 size,string &res)
{
    return createLV(LVM_TECS_VGNAME,lv,base,size,res);
}
/******************************************************************************/ 
bool renameLV(const string &source,const string &target)
{
    return renameLV(LVM_TECS_VGNAME,source,target);
}
/******************************************************************************/ 
int deleteLV(const string &lv,bool isfullname)
{
    if (isfullname)
    {
        return deleteLV(lv);
    }
    else
    {
        return deleteLV(LVM_TECS_VGNAME,lv);
    }
}
/******************************************************************************/ 
bool isExistLVName(const string &lv,bool isfullname)
{
    if (isfullname)
    {
        return isExistLVName(lv);
    }
    else
    {
        return isExistLVName(LVM_TECS_VGNAME,lv);
    }
}
/******************************************************************************/ 
bool isExistSnapShot(const string &lv,bool isfullname)
{
    if (isfullname)
    {
        return isExistSnapShot(lv);
    }
    else
    {
        return isExistSnapShot(LVM_TECS_VGNAME,lv);
    }
}
/******************************************************************************/ 
int getSnapShotCount(const string &lv,bool isfullname)
{

    if (isfullname)
    {
        return getSnapShotCount(lv);
    }
    else
    {
        return getSnapShotCount(LVM_TECS_VGNAME,lv);
    }
}
/******************************************************************************/ 
bool getVgFreeSpace(uint32 &iFreeSpace)
{
    return getVgFreeSpace(LVM_TECS_VGNAME,iFreeSpace);
}
/* endof lvm_manager.cpp */

