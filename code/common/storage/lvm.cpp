/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�lvm_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��lvm�洢�����÷������ʵ���ļ�
* ��ǰ�汾��1.0

* ��    �ߣ����¶�
* ������ڣ�2011.08.05
*
* �޸ļ�¼1��
*    �޸����ڣ�2011.08.05
*    �� �� �ţ�V1.0
*    �� �� �ˣ����¶�
*    �޸����ݣ�����
* �޸ļ�¼2��
*    �޸����ڣ�2012.06.11
*    �� �� �ţ�V2.0
*    �� �� �ˣ�yanwei
*    �޸����ݣ�����
*******************************************************************************/
#include <sys/mount.h>
#include "lvm.h"
#include "tecs_pub.h"
#include "log_agent.h"

using namespace zte_tecs;

/****************************************************************************************
*                                  ���ر���                                             *
****************************************************************************************/
static Mutex *s_ptLvMapLock = NULL;
static map<string,  Mutex *>   s_tLvMutex;

/****************************************************************************************
*                                  lv�������ӿ�                                        *
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
*                                  ����ʹ�õĺ�����װ                                   *
****************************************************************************************/

/****************************************************************************************
*                                  ����ӿڷ�װ                                        *
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
            {/*û��snapshot �Ҳ����Լ���fileid*/
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
    string cmd="vgcfgbackup"; /*������ /etc/lvm/backup ����*/
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
//�����߼���LV ָ����LVNAME
int createLV(const string &vg, const string &lv,const string &base, int64 size, string &res)
{
    bool ret = false;
    string cmd ;
    //uint32  mfree = 0;//���пռ� ��λMB
    uint32  msize = 0;//��λMB (���i64LVCapacity �ֽ�ת��ΪM)

    if (vg.empty() || lv.empty() || (0 == size))
    {
        OutPut(SYS_WARNING," createLV failed for Invalid Args!");
        return ERROR_INVALID_ARGUMENT;
    }
    //init
    res = "";

    //��λת������������λBתΪMB.
    msize = (size % LVM_MBYTE ==0)?(size / LVM_MBYTE):(size / LVM_MBYTE + 1);
    if (true == isExistLVName(vg,lv))
    {
        OutPut(SYS_ERROR,"createLV vg:%s,lv:%s already exists!\n",vg.c_str(),lv.c_str());
        res ="/dev/"+vg+"/"+lv;
        return ERR_EXIST;
    }
    /*�����ղ����Ƶ����õĵط� ���ڼ���*/
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
    /*tag����Ϊ��*/
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
//�ж��Ǵ���VG���µ�LV����
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
//�ж��Ǵ���VG���µ�LV����
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
//�жϷ����Ƿ��Ѿ�����PV
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
//�ж�PV��VG �Ƿ����,true ��ʾ���ڣ�false��ʾ������ ,����������
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
//��չ�߼���LV����
int extendLVCapacity(int extendType,
                     const string & sVGName ,
                     const string & sLVName ,
                     int64 i64LVCapacity)
{
    bool ret = false;
    string sCmd ;
    string sUnit = "M"; //��λ��MB
    string sResult ;
    string sExtendType;
    uint32  iLVCapacity = 0;//��λMB (���i64LVCapacity �ֽ�ת��ΪM)

    if ((LVM_EXTENDADD != extendType && LVM_EXTENDTO != extendType)
        || sVGName.empty() || sLVName.empty() || (0 == i64LVCapacity))
    {
        OutPut(SYS_WARNING,"extendLVCapacity failed for Invalid Args!");
        return ERROR_INVALID_ARGUMENT;
    }

    //��λת������������λBתΪMB.
    iLVCapacity = (i64LVCapacity % LVM_MBYTE ==0) ?(i64LVCapacity / LVM_MBYTE) :(i64LVCapacity / LVM_MBYTE + 1);

    //�ж�sLVName�Ƿ����߼����д�����ͬ��������
    if (false == isExistLVName(sVGName,sLVName))
    {//�����ڴ����Ƶ�LV
        OutPut(SYS_WARNING,"extendLVCapacity failed for %s not exist\n",sLVName.c_str());
        return ERROR_DEVICE_NOT_EXIST ;
    }

    //��չ����
    sExtendType = extendType==LVM_EXTENDADD?"+":"";
    sCmd = "lvextend -L"+sExtendType+to_string<uint32>(iLVCapacity,dec)+sUnit+" /dev/"+sVGName+"/"+sLVName;
    ret = runfind(sCmd,"successfully");
    return (true != ret)?(ERROR):(SUCCESS);
}
/******************************************************************************/ 
//��ȡLVM�����ܿռ���Ϣ
bool getVGTotalInfo(TVGSpaceTotalInfo &tResVGSpaceTotalInfo)
{
    int64  uiTotalCapacity = 0;    //�߼��������ܿռ��С��ֵ=lTotalPENum*fPESize,
    int64  uiAllocCapacity = 0;    //�߼��������Ѿ�ʹ�ÿռ䣬ֵ=lAllocPENum*fPESize,
    int64  uiFreeCapacity  = 0;     //�߼�������ʣ����ÿռ䣬ֵ=lFreePENum*fPESize,
    list<TLvmVgInfo>     lvmVGList;

    //init
    tResVGSpaceTotalInfo.iVGCountNum = 0;
    tResVGSpaceTotalInfo.uiTotalCapacity = 0;
    tResVGSpaceTotalInfo.uiAllocCapacity = 0;
    tResVGSpaceTotalInfo.uiFreeCapacity = 0;

    //generateDiskTotalInfoĿǰ�ȼӵ����λ�ã��Ժ���Ըĵ������ط����ã�����Ͳ�����
    generateDiskInfo();//������Ϣ ����Ӳ�� ������Ϣ

    //��ȡ�߼�����vg��Ϣ
    getVGListInfo(lvmVGList);

    //lvm������ϢlvmDiskDevList
    for (list<TLvmVgInfo>::iterator itDev=lvmVGList.begin();itDev!=lvmVGList.end();++itDev)
    {/* ���̿ռ���Ϣ,Ŀǰtecsֻ��Ҫtecs_vg1�Ĵ�С*/
    	if(itDev->sVGName==LVM_TECS_VGNAME) 
    	{
	        uiTotalCapacity  += itDev->uiTotalPENum * itDev->uiPESize;
	        uiAllocCapacity  += itDev->uiAllocPENum * itDev->uiPESize;
	        uiFreeCapacity   += itDev->uiFreePENum * itDev->uiPESize;
			break;
    	}
    }

    tResVGSpaceTotalInfo.iVGCountNum = lvmVGList.size();
    tResVGSpaceTotalInfo.uiTotalCapacity  = uiTotalCapacity * LVM_MBYTE; //��λתΪB
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
        //����Ƿ��и��ַ��� ����ļ���ĳһ�д�����ͬ���ַ��򲻼��뵽�ļ��� 
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
                //�Ѿ����ڣ�����Ҫ���
                sStrPrint = "'"+sInsertStr+"' is exist in line "+to_string<int>(iLen,dec)+".";
                return true;
            }
        }
    } 
    
    //�ļ��򿪷�ʽѡ��
    //ios::in   0x01 �����������򴴽�.ifstreamĬ�ϴ򿪷�ʽ
    //ios::out  0x02 ��д �ļ��������򴴽� ���ļ��Ѿ����������ԭ������.ofstreamĬ�ϴ򿪷���.
    //ios::app  0x08 ��д �ļ��������򴴽� ���ļ��Ѿ���������ԭ�����ݺ�д���µ�����.ָ��λ���������.  
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
        /*����ִ�г��� ��Ϊ����unknown device*/ 
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
    #if 0 /*ipsan�ļܹ����в���io������������б�*/
    if (string::npos != res.find("Input/output error"))
    {
        OutPut(SYS_ERROR,"doVgchange find Input/output error");
        return false;
    } 
    #endif

    return true;
}
/******************************************************************************/ 
//����Ӳ�� ������Ϣ
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
    list<TDiskTotalInfo>  diskTotalList;     //Ӳ����Ϣ
    list<TDiskDevInfo>    lvmDiskDevList;    //lvm��Ӧ���д��̷���

    if (isPVExistUnknownDevice())
    {
        OutPut(SYS_ERROR,"PV Exist Unknown Device!");
        return false;
    }


    insertStrToFile("/etc/profile","export LVM_SUPPRESS_FD_WARNINGS=1",true);
    //��Ч/etc/profile
    RunCmd("source /etc/profile" , sRes);
    if (getenv("LVM_SUPPRESS_FD_WARNINGS"))
    {/* ��ȡ���������ɹ� */
        OutPut(SYS_DEBUG,"getenv LVM_SUPPRESS_FD_WARNINGS SUCCESS!");
    }
    else
    {/* ��ȡ��������ʧ�� */
        //RunCmd("export LVM_SUPPRESS_FD_WARNINGS=1" , sRes);
        setenv("LVM_SUPPRESS_FD_WARNINGS","1",1);
        //unsetenv("LVM_SUPPRESS_FD_WARNINGS");
        OutPut(SYS_DEBUG,"getenv FAILED.set LVM_SUPPRESS_FD_WARNINGS=1!");
    }


    /*����һ �ж��Ƿ���tecs_vg1�飬���򷵻�*/
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
    {/*����� û��LVM����������LVM������������HC */
        OutPut(SYS_WARNING,"no lvm disk,do fdisk lvm!\n");
        bRes = createLVMDisk();
    }
    else
    {
        /*����������lvm������������vg��*/
        for (list<TDiskDevInfo>::iterator itDev=lvmDiskDevList.begin();itDev!=lvmDiskDevList.end();++itDev)
        {
            //�ж�PV���Ƿ��Ѿ��ж�Ӧ�ķ���
            bisExistPV = isExistPVName(itDev->sDevice);
            if (bisExistPV == true)
            {
                bisPVExistVG = isPVExistVGName(itDev->sDevice,sPVVGName);
                if ((bisPVExistVG == true) && (!sPVVGName.empty()))
                {
                    /*����vg���lvm��ȥ���*/
                    continue;
                }

            }

            /*����������κ�vg�飬�����tecs_vg1*/
            sTmp = "pvcreate "+itDev->sDevice;
            RunCmd(sTmp , sRes);
            if (bisExistVG == false)
            {
                //��ʼ��LVM
                //system("vgscan"); //���� ���д�ӡReading all physical volumes.  This may take a while..
                //#��������
                sTmp = "vgcreate "+LVM_TECS_VGNAME+" "+itDev->sDevice;
                RunCmd(sTmp , sRes);
                str_pose = sRes.find("successfully");
                if (str_pose != string::npos)
                {//�����ɹ�
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
                //#��ӵ���
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
//��ȡӲ�����ƽӿ�
bool getDiskName(list<string> &diskListInfo)
{
    string sDiskNum;
    list<TDiskTotalInfo>  diskTotalList;     //Ӳ����Ϣ
    list<TDiskDevInfo>    lvmDiskDevList;    //lvm��Ӧ���д��̷���


    diskListInfo.clear();
    getDiskInfo(diskTotalList , lvmDiskDevList);

    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {// �ܵĴ�����Ϣ
        sDiskNum = it->sDiskName;
        diskListInfo.push_back(sDiskNum);
    }

    return true;
}
/******************************************************************************/ 
bool  getVgFreeSpace(const string &sVGName,uint32 &iFreeSpace)
{
    uint32  uiFreeCapacity  = 0;     //�߼�������ʣ����ÿռ䣬ֵ=lFreePENum*fPESize
    list<TLvmVgInfo>     lvmVGList;
    //��ȡ�߼�����vg��Ϣ
    getVGListInfo(lvmVGList);
    //lvm������ϢlvmDiskDevList
    for (list<TLvmVgInfo>::iterator itDev=lvmVGList.begin();itDev!=lvmVGList.end();++itDev)
    {/* ���̿ռ���Ϣ*/
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
//��ȡ������Ϣ(����LVM����������������)
bool getDiskInfo(list<TDiskTotalInfo>  &diskTotalList , list<TDiskDevInfo>  &lvmDiskDevList)
{
    string  sCmd = "fdisk -l 2> /dev/null";
    bool    bRes = false;
    TDiskDevInfo   tmpDiskDevInfo;

    //ִ��fdisk -l�����list
    diskTotalList.clear();
    lvmDiskDevList.clear();
    runCommandGetList(sCmd , diskTotalList);  //ִ�С�fdisk -l������ �����������ɻ�����Ӳ�̺ͷ�����Ϣ

    //��ȡLVM������Ϣ
    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {// �ܵĴ�����Ϣ
        for (list<TDiskDevInfo>::iterator itDev=(it->diskDevList).begin();itDev!=(it->diskDevList).end();++itDev)
        {//�������з���
            if (itDev ->iId == 0x8e)
            {//lvm����
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
//��ȡĳ�����̷�����ϢsDevice��������
bool getDiskDevInfo(const string & sDevice , list<TDiskDevInfo>  &diskDevList,int &iUsedCylinder)
{
    string                sCmd = "fdisk -l 2> /dev/null";
    bool                  bRes = false;
    TDiskDevInfo          tmpDiskDevInfo;
    list<TDiskTotalInfo>  diskTotalList;

    //ִ��fdisk -l�����list

    diskDevList.clear();
    runCommandGetList(sCmd , diskTotalList);  //ִ�С�fdisk -l������ �����������ɻ�����Ӳ�̺ͷ�����Ϣ
    //��ȡ������Ϣ
    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {// �ܵĴ�����Ϣ
        if (it ->sDiskName == sDevice)
        {//����Ҫ���ҵķ���
            iUsedCylinder = it->iUsedCylinder;
            for (list<TDiskDevInfo>::iterator itDev=(it->diskDevList).begin();itDev!=(it->diskDevList).end();++itDev)
            {//�������з���
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
//��ȡ�߼�����VG��Ϣ
bool getVGListInfo(list<TLvmVgInfo>    &lvmVGList)
{
    string sCmd = "vgs --noheadings -o vg_name,vg_uuid,lv_count,pv_count,vg_extent_count,vg_free_count 2>/dev/null";
    bool    bRes = false;
    TDiskDevInfo   tmpDiskDevInfo;

    lvmVGList.clear();//Lvm�߼�����vg�б� ���
    runCommandGetList(sCmd , lvmVGList); //�����߼�����vg��Ϣ
    bRes = true;
    return bRes;
}
/******************************************************************************/ 
//��ȡ�߼�LV��Ϣ
bool getLVListInfo(list<string>    &lvmLVNameList,list<TLvmVgInfo>    &lvmVGList)
{
    string sCmd = "lvs --noheadings -o lv_path,lv_uuid 2>/dev/null";
    bool    bRes = false;
    TDiskDevInfo   tmpDiskDevInfo;

    lvmLVNameList.clear();//Lvm�߼�����vg�б� ���
    getVGListInfo(lvmVGList);
    runCommandGetList(sCmd , lvmLVNameList , lvmVGList); //�����߼�����vg��Ϣ
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

//private ˽�з���

//�ԡ�fdisk -l������صĽ�����н��������н�����
void  analyseFdiskList(const char *str , string *sDiskName , list<TDiskTotalInfo> &diskResTotalList)
{
    char  cDevice[100]="";
    char  cBoot[100]="";
    int   iStart = 0;
    int   iEnd = 0;
    int   iBlocks = 0; /* ��λKB*/
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
    {/* ��ȡ������Ϣ */
        sscanf( str, "%99s%99s%f%99s%lld%99s",cDevice,cDiskNameTmp,&fCapacity,cCapacityUnitTmp,&lCapacityByte,cCapacityUnitBytes);
        /* ȥ����ȡ�Ĵ�����Ϣ�е�ð�� :*/
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
        /* ȥ����ȡ�ĵ�λ�еĶ��ţ�*/
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
        tmpDiskInfo.iUsedCylinder = 0; /* ��ʼ���̴ŵ�ʹ����ϢΪ0 */
        find_pose = tmpDiskInfo.sDiskName.find("/");
        if (find_pose == string::npos)
        {
            //���������в�����/ ����
            return;
        }
        find_pose = tmpDiskInfo.sDiskName.find("mapper");
        if (find_pose != string::npos) /*�߼���ֱ�ӹ���*/
        {
            *sDiskName="lvm";
            //���������в�����/ ����
            return;
        }
        //sDiskName �������ں����ж��ĸ�������Ҫ�����
        *sDiskName = cDiskName;
        find_pose = tmpDiskInfo.sDiskName.find("/dm-");
        if (find_pose != string::npos)
        {
            //���������к���/dm-,��ʾ������̵Ĳ���Ҫ����
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
        //���������к���/dm-,��ʾ������̵Ĳ���Ҫ����
        return;
    }
    find_pose = sDiskName->find("lvm");
    if (find_pose != string::npos)
    {
        //���������к���lvm,��ʾ���߼�������Ҫ����
        return;
    }
    //��ȡ�����ܷ���������Ϣ
    if (!strcmp(cBoot,"heads,"))
    {
        sscanf( str, "%99s%99s%99s%99s%d", cStrTmp,cStrTmp,cStrTmp,cStrTmp,&iCylinder);
        for (list<TDiskTotalInfo>::iterator it=diskResTotalList.begin();it!=diskResTotalList.end();++it)
        {/* �ܵĴ�����Ϣ*/
            if (it->sDiskName == *sDiskName)
            {
                it->iCylinder = iCylinder;
                break;
            }
        }
    }
    /* �ж϶�ȡ��boot�Ƿ�Ϊ*��Ȼ���ж�����������û�мӺ� */
    if (!strcmp(cBoot,"*"))
    {/* ��ȡ��BootΪ*,�м��и�*����Ҫ��ȡ */
        sscanf( str, "%99s%99s%d%d%d%9s", cDevice,cBoot,&iStart,&iEnd,&iBlocks,cPlus);
        if ((!strcmp(cPlus,"+")) || (!strcmp(cPlus,"-")))
        {/* BootΪ* blocks �����мӺ�ʱ ����ʽ
                      * �� /dev/sda1   *           1         127     1020096   83  Linux
                      */
            sscanf( str, "%99s%99s%d%d%d%s%x%99s", cDevice,cBoot,&iStart,&iEnd,&iBlocks,cPlus,&iId,cSystem);
        }
        else
        {/* BootΪ* blocks �����мӺ�ʱ ����ʽ
                      * �� /dev/sdh1   *           1         127     1020096+   8e Linux LVM
                      */
            sscanf( str, "%99s%99s%d%d%d%x%99s", cDevice,cBoot ,&iStart,&iEnd,&iBlocks,&iId,cSystem);
            strcpy(cPlus,"");
        }
    }
    else
    {/* ��ȡ��BootΪ��Ч���Ͳ���Ҫ��ȡ* */
        strcpy(cBoot,"");
        sscanf( str, "%99s%d%d%d%9s", cDevice,&iStart,&iEnd,&iBlocks,cPlus);
        if ((!strcmp(cPlus,"+")) || (!strcmp(cPlus,"-")))
        {/* blocks �����мӺ�ʱ ����ʽ
                      * �� /dev/sda6           32125       32520     3180838+  8e  Linux LVM
                      */
            sscanf( str, "%99s%d%d%d%s%x%99s", cDevice,&iStart,&iEnd,&iBlocks,cPlus,&iId,cSystem);
        }
        else
        {/* blocks ����û�мӺ�ʱ ����ʽ
                      * �� /dev/sda3           25624       31997    51199155   8e  Linux LVM
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
    {/* �ܵĴ�����Ϣ*/
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
            it->diskDevList.push_back(tmpDiskDevInfo);  //���з���
            break;
        }
    }
}
/******************************************************************************/ 
//���� lvdisplay ������lv�����б�ȫ�ֱ���lvmLVNameList
//��ȡLV��NAME�б�
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

    //����lv_path
    sLvFullName = cLvName;
    last_pose = sLvFullName.rfind("/");//�������һ������
    sLVName = sLvFullName.substr(last_pose+1);//��ȡlvname
    lvmLVNameList.push_back(sLVName);
    sTmp = sLvFullName.substr(0,last_pose);//ȥ��lvname
    last2_pose = sTmp.rfind("/");//�������һ������(�Ѿ�ȥ����һ��)
    sTmpVGName = sTmp.substr(last2_pose+1);//��ȡvgname
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
//��vgdisplay ���н�����ȡ��Ϣ���浽lvmVGList
//��ȡVG����Ϣ
void analyseVgdisplay(const char *str, string *psVGName ,  list<TLvmVgInfo>    &lvmVGList)
{
    char cVGName[100] = "";
    int    iCurLVCnt = 0;         //�߼��������߼���LV����
    int    iCurPVCnt = 0;         //�߼��������������PV����
    uint32   uiTotalPENum = 0;       //�߼�������PE������
    uint32   uiFreePENum = 0;        //�߼�������ʣ����õ�PE����
    uint32   uiAllocPENum = 0;      //�߼��������ѷ����PE������
    uint32   uiPESize = 0;
    char   sVGUUID[100] = "";           //�߼������ʶ
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

    //��ȡPE��С
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
//���� lvdisplay | grep '/dev/����/lv_�����ID_'  | awk '{  print $3 }'�����������ID��DiskID��Ϣ
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
    last_pose = sLVSource.rfind("_");//�������һ�����ֵ�"_"
    if (last_pose != string::npos)
    {
        sVMDiskID = sLVSource.substr(last_pose+1);//��ȡVMDiskID
        from_string<uint32>(uiVMDiskID, sVMDiskID, dec);
        uiVMDiskIDList.push_back(uiVMDiskID);
    }
}
/******************************************************************************/ 
//��linux����(��cat��fdisk���������Ϣ��linux����)����ִ�У������List����
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
        //���н����ַ���
        analyseFdiskList(s, &sDiskName , diskTotalList); //�Զ�ȡ���ַ���ĳ�н��н���
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}
/******************************************************************************/ 
//��vgdisplay ���н�����ȡ��Ϣ���浽lvmVGList
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
        //���н����ַ���
        analyseVgdisplay(s , &sVGName , lvmVGList);
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}
/******************************************************************************/ 
//��lvdisplay�������ִ�У������lvmLVNameList
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
        //���н����ַ���
        analyseLvdisplayLVName(s,&sLVVGName,&sLvFullName,lvmLVNameList ,lvmVGList);
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}
/******************************************************************************/ 
//���� lvdisplay | grep '/dev/����/lv_�����ID_'  | awk '{  print $3 }'�����������ID��DiskID��Ϣ
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
        //���н����ַ���
        analyseDiskIDList(s,uiVMDiskIDList);
    }
    pclose(fp);

    delete[] s;
    return  bRes;

}

/******************************************************************************/ 
//����lvm����
bool createLVMDisk()
{
    bool    bRes = false;
    bool    bResLVMDiskScript = false;
    uint32  uiDiskFullNum =0;//��¼����û��ʣ����Ϣ����Ŀ
    list<TDiskTotalInfo>  diskTotalList;     //Ӳ����Ϣ
    list<TDiskDevInfo>    lvmDiskDevList;    //lvm��Ӧ���д��̷���
    string                sRes ;

    getDiskInfo(diskTotalList , lvmDiskDevList);

    ////����lvm�����ű��ļ�
    bResLVMDiskScript = createLVMDiskScript();
    if (!bResLVMDiskScript)
    {
        OutPut(SYS_ERROR,"createLVMDisk failed");
        return bRes;
    }

    //�����д��̽��з���
    for (list<TDiskTotalInfo>::iterator it=diskTotalList.begin();it!=diskTotalList.end();++it)
    {
        string bashstr;
        string sDiskDevNum;
        if (it->iUsedCylinder < it->iCylinder)
        {
            //����ʣ��ŵ����з���
            string sNumDev ;
            int   iWhileCnt = 0;
            bool bDiskDevIsExist = false;
            list<TDiskDevInfo>    diskDevListTmp;     //Ӳ����Ϣ
            int                   iUsedCylinder = 0;
            do
            {
                //���������ĿС�������ѭ������
                iWhileCnt++;
                if (iWhileCnt > LVM_MAXDEV_ONEDISK)
                {
                    //����Ԥ������������Ŀ
                    OutPut(SYS_ERROR,"find disk num error!iWhileCnt:%d\n",iWhileCnt);
                    break;
                }

                //fdisk ��������
                bashstr = "bash fdisknewdev.sh "+it->sDiskName;
                //system( bashstr.c_str() );//ִ�нű�fdisksh�ű���������
                RunCmd(bashstr, sRes);
                OutPut(SYS_NOTICE,"%s ,sRes %s\n",bashstr.c_str(),sRes.c_str());
                diskDevListTmp.clear();
                getDiskDevInfo(it->sDiskName , diskDevListTmp,iUsedCylinder);
            }while (iUsedCylinder < it->iCylinder);


            //�Ա��¾ɷ����б���Ϣ(diskTotalListTmp )���ҵ������ķ������
            for (list<TDiskDevInfo>::iterator itDevNew = diskDevListTmp.begin(); itDevNew!=diskDevListTmp.end(); ++itDevNew)
            {//�������з���
                bDiskDevIsExist = false;
                for (list<TDiskDevInfo>::iterator itDevOld = (it->diskDevList).begin(); itDevOld!=(it->diskDevList).end(); ++itDevOld)
                {//�������з���
                    if (itDevOld->sDevice == itDevNew->sDevice)
                    {
                        bDiskDevIsExist = true;
                        continue;
                    }
                }
                if (false == bDiskDevIsExist)
                {
                    //�������������.��ʾ�����ķ������޸�Ϊ8e LVM
                    //��ȡ���
                    sNumDev = (itDevNew->sDevice).substr((itDevNew->sDiskName).length());//��ȡVMDiskID
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
        //û��LVM���������̻���ʣ��Ĵŵ����з���
        bRes = true;
        OutPut(SYS_ERROR," do fdisk Lvm success,reboot.....!\n");
        system("reboot");//���÷�������Ҫ����HC��������Ч
    }
    else
    {
        //û��LVM����������û��ʣ��Ĵŵ����з���
        bRes = false;
        OutPut(SYS_ERROR,"do fdisk Lvm failed,Disk has no free space to lvm!\n");
    }
    return bRes;
}
/******************************************************************************/ 
//����lvm�����ű��ļ�
bool createLVMDiskScript()
{
    ofstream outfile;
    string  sStr;

    //�ļ��򿪷�ʽѡ��
    //ios::in   0x01 �����������򴴽�.ifstreamĬ�ϴ򿪷�ʽ
    //ios::out  0x02 ��д �ļ��������򴴽� ���ļ��Ѿ����������ԭ������.ofstreamĬ�ϴ򿪷���.
    //ios::app  0x08 ��д �ļ��������򴴽� ���ļ��Ѿ���������ԭ�����ݺ�д���µ�����.ָ��λ���������.
    outfile.open("fdisknewdev.sh",ios::out);
    if (outfile.is_open())
    {
        sStr = "#!/bin/bash";
        outfile<<sStr<<endl;
        sStr = "#filename:fdisknewdev.sh";
        outfile<<sStr<<endl;
        sStr = "fdisk $1 <<EOF";
        outfile<<sStr<<endl;
        /* n p 2 3 4Ϊ�˱���û����չ������ӵ�*/
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
//�������е�LV
bool ActivateAllLV()
{
    list<string>         lvmLVNameList;
    list<TLvmVgInfo>     lvmVGList;
    string               sTmp;
    string               sRes;


    getLVListInfo(lvmLVNameList , lvmVGList);
    for (list<string>::iterator itLvm=lvmLVNameList.begin(); itLvm!=lvmLVNameList.end(); ++itLvm)
    {/* ������Ϣ*/
        sTmp = "lvchange -ay /dev/"+LVM_TECS_VGNAME+"/"+*itLvm;
        RunCmd(sTmp , sRes);
        OutPut(SYS_DEBUG," ActivateAllLV %s,sRes:%s\n",sTmp.c_str(),sRes.c_str());
    }
    return true;
}
/****************************************************************************************
*                                  �м���ع�����                                       *
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

