/******************************************************************************************
* ��Ȩ���� (C)2011, ����ͨѶ�ɷ����޹�˾��
*
*   All rights reserved
*
* �ļ����ƣ�storage.cpp
* �ļ���ʶ��
* ����ժҪ��storage ��������Դ�ļ�
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� �����о�Ժ-- ����洢ƽ̨--  ������
* ������ڣ� 2011-08-12
*
*    �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
* �� �� �ˣ�
* �޸����ڣ�
* �޸����ݣ�
****************************************************************************************/

/****************************************************************************************
*                                   ͷ�ļ�                                             *
****************************************************************************************/
#include "tecs_pub.h"
#include "log_agent.h"
#include "storage.h"
#include <mntent.h>
#include <sys/stat.h>
#include "../../api/include/api_const.h"

using namespace zte_tecs;

/****************************************************************************************
*                                 ��ֵ                                                  *
****************************************************************************************/
#define MOUNT_FILENAME           "/proc/mounts"
#define BUF_SIZE_FOR_GETMNTENT   4096
#define MAX_FILENAME_LEN         160

#define IMAGE_VHD_KEYWORD       "conectix"
#define IMAGE_QCOW2_KEYWORD     "QFI"

/****************************************************************************************
*                                  ȫ�ֱ���                                            *
****************************************************************************************/

/****************************************************************************************
*                                  ���ر���                                            *
****************************************************************************************/
static int storage_cmd_print_on = 0;  
DEFINE_DEBUG_VAR(storage_cmd_print_on);
/****************************************************************************************
*                                  ��������                                            *
****************************************************************************************/


/****************************************************************************************
*                                  �ӿ�ʵ��                                            *
****************************************************************************************/
void storage_print(const string &cmd,int ret,const string &res)
{
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"%s failed,ret:%d,res:%s\n",cmd.c_str(),ret,res.c_str());
    }
    else
    {
        if (storage_cmd_print_on)
        {
            if (res.empty())
            {
                OutPut(SYS_DEBUG,"%s success\n",cmd.c_str());
            }
            else
            {
                OutPut(SYS_DEBUG,"%s success,result:%s\n",cmd.c_str(),res.c_str()); 
            }
        }
    }
}
/******************************************************************************/ 
int storage_command(const string &cmd,string &res)
{
    int ret = ERROR;

    ret = RunCmd(cmd,res);
    storage_print(cmd,ret,res);
    return ret;
}
/******************************************************************************/ 
int get_filetype(const string& name,int &type)
{
    struct stat64 tStat;

    if (name.empty())
    {
        return ERROR;
    }
    
    if (-1 == lstat64(name.c_str(),&tStat))
    {
        return ERROR;
    }

    type = FILETYPE_UNKNOWN;
    if (S_ISLNK(tStat.st_mode))
    {
        type = FILETYPE_LNK;
    }
    if (S_ISREG(tStat.st_mode))
    {
        type = FILETYPE_REG;
    }
    if (S_ISDIR(tStat.st_mode))
    {
        type = FILETYPE_DIR;
    }
    if (S_ISCHR(tStat.st_mode))
    {
        type = FILETYPE_CHR;
    }
    if (S_ISBLK(tStat.st_mode))
    {
        type = FILETYPE_BLK;
    }
    if (S_ISFIFO(tStat.st_mode))
    {
        type = FILETYPE_FIFO;
    }
    if (S_ISSOCK(tStat.st_mode))
    {
        type = FILETYPE_SOCK;
    }
    return SUCCESS;
}
/******************************************************************************/ 
bool check_fileexist(const string& name)
{
    if (name.empty())
    {
        return false;
    }
    
    if (0 == access(name.c_str(),F_OK))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************/ 
bool is_regular(const string& name)
{
    int type = FILETYPE_UNKNOWN;
    if (SUCCESS != get_filetype(name,type))    
    {
        return false;
    }
    return FILETYPE_REG == type;
}
/******************************************************************************/ 
bool is_directory(const string& name)
{
    int type = FILETYPE_UNKNOWN;
    if (SUCCESS != get_filetype(name,type))    
    {
        return false;
    }
    return FILETYPE_DIR == type;
}
/******************************************************************************/ 
bool check_mount(const string &name)
{
    bool ret = false;
    struct mntent tMnt;
    FILE *fp = NULL;
    char acBuf[BUF_SIZE_FOR_GETMNTENT];

    if (name.empty())
    {
        return ret;
    }

    fp = setmntent(MOUNT_FILENAME, "r");
    if (NULL == fp)
    {
        return ret;
    }

    while ((NULL != getmntent_r(fp, &tMnt, acBuf, BUF_SIZE_FOR_GETMNTENT)))
    {
        if (0 == strncmp(name.c_str(),tMnt.mnt_dir,MAX_FILENAME_LEN))   
        {
            ret = true;
            break;
        }
        if (NULL != strstr(tMnt.mnt_fsname,name.c_str()))   
        {
            ret = true;
            break;
        }
    }

    endmntent(fp);
    return ret;
}
/******************************************************************************/ 
int get_mount(const string &fsname, string &mountdir)
{
    int ret = ERROR;
    struct mntent tMnt;
    FILE *fp = NULL;
    char acBuf[BUF_SIZE_FOR_GETMNTENT];

    if (fsname.empty())
    {
        return ret;
    }

    fp = setmntent(MOUNT_FILENAME, "r");
    if (NULL == fp)
    {
        return ret;
    }

    while ((NULL != getmntent_r(fp, &tMnt, acBuf, BUF_SIZE_FOR_GETMNTENT)))
    {
        if (NULL != strstr(tMnt.mnt_fsname,fsname.c_str()))   
        {
            mountdir = tMnt.mnt_dir;
            ret = SUCCESS;
            break;
        }
    }

    endmntent(fp);
    return ret;
}
/******************************************************************************/ 
bool findsubstr(const string &str,const string &sub)
{
    if (str.empty() || sub.empty())
    {
        return false;
    }
    string::size_type find_pose = str.find(sub);
    if (find_pose == string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}
/******************************************************************************/ 
bool runfind(const string &cmd,const string &find,string &res)
{
    if (cmd.empty() || find.empty())
    {
        return false;
    }
    if (SUCCESS != storage_command(cmd,res))
    {
        return false;
    }
    return findsubstr(res,find);
}
/******************************************************************************/ 
bool runfind(const string &cmd,const string &find)
{
    string res;
    
    if (cmd.empty() || find.empty())
    {
        return false;
    }
    if (SUCCESS != storage_command(cmd,res))
    {
        return false;
    }
    return findsubstr(res,find);
}
/******************************************************************************/  
bool check_filesystem(const string &fsname, const string &fsprefix)
{
    string cmd,sub;

    cmd = "fsadm -v -n -y check "+fsname+" 2>/dev/null";
    if (fsprefix.empty())
    {
        sub = "found";
    }
    else
    {
        sub = fsprefix;
    }

    return runfind(cmd,sub);
}
/******************************************************************************/  
int make_filesystem(const string &fsname, const string &fstype)
{
    string cmd,res;

    if (fsname.empty())
    {
        return SUCCESS;
    }
    cmd = "mkfs."+fstype+" -m 0 "+fsname+" 2> /dev/null 1> /dev/null";
    return storage_command(cmd,res);
}
/******************************************************************************/  
void gen_mkfsscript(void)
{
    ofstream outfile;
    string  sStr ;
    outfile.open(STORAGE_MKFS_SHELL.c_str(),ios::out); 
    if(outfile.is_open())
    {
        sStr = "#!/bin/bash";
        outfile<<sStr<<endl;
        sStr = "mkfs.$1 -m 0 $2 <<EOF"; 
        outfile<<sStr<<endl; 
        sStr = "y <<EOF";       
        outfile<<sStr<<endl; 
        outfile.close(); 
    }
}
/******************************************************************************/ 
int sys_md5(const string &filename,string &md5)
{
    int ret = ERROR;
    if (filename.empty())
    {
        return ret;
    }
    string cmd,res;
    cmd = "md5sum "+filename;
    ret = storage_command(cmd,res);
    if (SUCCESS == ret)
    {
        md5 = res.substr(0,32);
    }
    return ret;
}
/******************************************************************************/  
MID GetSC_MID()
{    
   string  reg_system ;
   GetRegisteredSystem(reg_system);   
   MID mid(reg_system,PROC_SC,MU_SC_CONTROL);
   return mid;
}
/******************************************************************************/ 
int GetTailBytes(const string &name, char *buf, size_t count)
{
    int ret = ERROR;
    
    if (name.empty() || NULL == buf || 1024 < count || count < 0)
    {
        return ret;
    }

    FILE *fp = fopen(name.c_str(),"r");
    if (NULL == fp)
    {
        OutPut(SYS_ERROR,"GetTailBytes fopen Failed for %s\n",strerror(errno));
        goto RET;
    }

    if (0 != fseek(fp,-count,SEEK_END))
    {
        OutPut(SYS_ERROR,"GetTailBytes fseek Failed for %s\n",strerror(errno));
        goto RET;
    }

    if (1 != fread(buf,count,1,fp))
    {
        OutPut(SYS_ERROR,"GetTailBytes fread Failed for %s\n",strerror(errno));
        goto RET;
    }

    ret = SUCCESS;
RET:
    if (NULL != fp)
    {
        fclose(fp);
    }
    return ret;
    
}
/******************************************************************************/ 
int SetTailBytes(const string &name, const char *buf, size_t count)
{
    int ret = ERROR;
    
    if (name.empty() || NULL == buf || 1024 < count || count < 0)
    {
        return ret;
    }

    FILE *fp = fopen(name.c_str(),"w");
    if (NULL == fp)
    {
        OutPut(SYS_ERROR,"SetTailBytes fopen Failed for %s\n",strerror(errno));
        goto RET;
    }

    if (0 != fseek(fp,-count,SEEK_END))
    {
        OutPut(SYS_ERROR,"SetTailBytes fseek Failed for %s\n",strerror(errno));
        goto RET;
    }

    if (1 != fwrite(buf,count,1,fp))
    {
        OutPut(SYS_ERROR,"SetTailBytes fwrite Failed for %s\n",strerror(errno));
        goto RET;
    }

    ret = SUCCESS;
RET:
    if (NULL != fp)
    {
        fclose(fp);
    }
    return ret;
    
}
/**********************************************************************
* �������ƣ�LoadShareDisk
* �������������ع�����
* ���ʵı���
* �޸ĵı���
* ���������vid ӳ����id,���Ϊ-1,�����hostip����ӳ������
            target �̷�
            access_option �����̷�����Ϣ
            fstype ��ʽ���ļ�ϵͳ����
* ���������device �����̼��غ󱾵��豸��
* �� �� ֵ��SUCCESS-�ɹ�,����-ʧ��
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/11/16   V1.0    ��ΰ      ����
************************************************************************/
STATUS LoadShareDisk(int64 vid, 
                         const string &target, 
                         const string &access_option, 
                         const string &fstype, 
                         string &device)
{

    int ret = ERROR;
    int load_num = 0;
    iSCSIAccessOption option;
    T_LOAD_BLOCK_INFO info;
    vector<T_LOAD_BLOCK_INFO> loadinfo;
    
    option.deserialize(access_option);
    string  iscsiname;
    GetIscsiName(iscsiname);
    LunAddr  addr;
    string   addrstr;
    addrstr=option._auth_tbl._table[iscsiname];
	if(addrstr.empty())
	{
		OutPut(SYS_ERROR,"LoadShareDisk _auth_tbl not include :%s\n",iscsiname.c_str());
        return ERROR;
	}
    addr._target=addrstr.substr(0,addrstr.find_last_of(':'));
    addr._lunid=atoi(addrstr.substr(addrstr.find_last_of(':')+1).c_str());
    info.lunid = addr._lunid;
    info.sTarget = target;
    loadinfo.push_back(info); 
    string target_name = addr._target;
    BlockAdaptorDataPlane *padaptor = BlockAdaptorDataPlane::GetInstance();
    OutPut(SYS_DEBUG,"LoadShareDisk for target:%s,lun:%ld!\n",info.sTarget.c_str(),info.lunid);
    ret = padaptor->LoadBlocks(option._type,option._dataip_array,vid,target_name,loadinfo,load_num);
    if (SUCCESS != ret && ret!= ERROR_LOAD_NEED_REFRESH)
    {                       
        OutPut(SYS_ALERT,"LoadShareDisk LoadBlocks ret:%d\n",ret);
        return ret;
    }

    device = loadinfo.begin()->sDeviceName;
    
    if(!fstype.empty() && !check_filesystem(loadinfo.begin()->sDeviceName, fstype))
    {
        string cmd,res;
        gen_mkfsscript();
        int  ret_cmd;
        cmd = "bash "+STORAGE_MKFS_SHELL+" " + fstype + " " + loadinfo.begin()->sDeviceName;
        ret_cmd = storage_command(cmd, res);
        if (SUCCESS != ret_cmd)
        {
            OutPut(SYS_ERROR,"LoadShareDisk mkfs failed cmd:%s,res:%s\n",cmd.c_str(),res.c_str());
            return ret_cmd;
        }
    }
    
    return ret;
}


STATUS RefreshLoadShareDisk(const string &target, 
                         const string &access_option)
{

    int ret = ERROR;   
    iSCSIAccessOption option;   
    T_LOAD_BLOCK_INFO info;
    vector<T_LOAD_BLOCK_INFO> loadinfo;
    
    option.deserialize(access_option);
    string  iscsiname;
    GetIscsiName(iscsiname);
    LunAddr  addr;
    string   addrstr;
    addrstr=option._auth_tbl._table[iscsiname];
	if(addrstr.empty())
	{
		OutPut(SYS_ERROR,"RefreshLoadShareDisk _auth_tbl not include :%s\n",iscsiname.c_str());
        return ERROR;
	}
    addr._target=addrstr.substr(0,addrstr.find_last_of(':'));
    addr._lunid=atoi(addrstr.substr(addrstr.find_last_of(':')+1).c_str());
    info.lunid = addr._lunid;
    info.sTarget = target;
    loadinfo.push_back(info); 
    string target_name = addr._target;
    BlockAdaptorDataPlane *padaptor = BlockAdaptorDataPlane::GetInstance();
    OutPut(SYS_DEBUG,"LoadShareDisk for target:%s,lun:%ld!\n",info.sTarget.c_str(),info.lunid);
    ret = padaptor->RefreshLoadBlocks(option._type,option._dataip_array,target_name,loadinfo);    
    return ret;
}
/**********************************************************************
* �������ƣ�UnLoadShareDisk
* ����������ж�ع����
* ���ʵı���
* �޸ĵı���
* ���������vid ӳ����id,���Ϊ-1,�����hostip����ӳ������
            target �̷�
            access_option �����̷�����Ϣ
* �����������
* �� �� ֵ��SUCCESS-�ɹ�,����-ʧ��
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/11/16   V1.0    ��ΰ      ����
************************************************************************/
STATUS UnLoadShareDisk(int64 vid, 
                            const string &target, 
                            const string &access_option)
{
    iSCSIAccessOption option;    
    T_UNLOAD_BLOCK_INFO  info;
    vector<T_UNLOAD_BLOCK_INFO>  unloadinfo;   
    option.deserialize(access_option);
    string  iscsiname;
    GetIscsiName(iscsiname);
    LunAddr  addr;
    string   addrstr;
    addrstr=option._auth_tbl._table[iscsiname];
	if(addrstr.empty())
	{
		OutPut(SYS_ERROR,"UnLoadShareDisk _auth_tbl not include :%s\n",iscsiname.c_str());
        return ERROR;
	}
    addr._target=addrstr.substr(0,addrstr.find_last_of(':'));
    addr._lunid=atoi(addrstr.substr(addrstr.find_last_of(':')+1).c_str());
    info.sTarget = target;
    info.lunid   = addr._lunid;    
    unloadinfo.push_back(info);
    string targetname = addr._target;
    BlockAdaptorDataPlane *padaptor = BlockAdaptorDataPlane::GetInstance();
    
    OutPut(SYS_DEBUG,"UnLoadShareDisk targetname:%s!\n",targetname.c_str());
    return padaptor->UnloadBlocks(option._type,option._dataip_array,vid,targetname,unloadinfo); 
}

void ReclaimDevice()
{
    BlockAdaptorDataPlane *padaptor = BlockAdaptorDataPlane::GetInstance();
    padaptor->ReclaimDev();
}

STATUS CheckImgFormatAndSize(const string& img, string& format, int64& size)
{
    string cmd;
    string res;
    stringstream ss;

    if (img.empty())
    {
        return ERROR;
    }

    cmd = "hexdump -C -n 1024 "+img;
    if (SUCCESS != storage_command(cmd,res))
    {
        return ERROR;
    }

    //  ����Ƿ���vhd
    if(res.find(IMAGE_VHD_KEYWORD) != res.npos)
    {
        //��ѯ�����̴�С,��λMB
        cmd = " vhd-util query -v -n  "+img;
        res = "";
        if (SUCCESS != storage_command(cmd,res))
        {
            return ERROR;
        }
        else
        {
            format = IMAGE_FORMAT_VHD;
            ss << res;
            ss >> size;
            size = size*1024*1024;
        }
    }
    else if(res.find(IMAGE_QCOW2_KEYWORD)!= res.npos)
    {//����Ƿ���qcow2��ʽ����        
        //��ѯ�����̴�С��ֱ��ȡ��bytes
        cmd = " qemu-img info "+img+" | grep 'virtual size' |cut -d ' ' -f 4 |cut -d '(' -f 2";
        res = "";
        if (SUCCESS != storage_command(cmd,res))
        {
            return ERROR;
        }
        else
        {
            format = IMAGE_FORMAT_QCOW2;
            ss << res;
            ss >> size;
            size = size;
        }
    }
    else
    {
        //���ļ������С����
        if(access(img.c_str(), F_OK) != 0)
        {
            return ERROR;
        }

        struct stat64 s;
        if (0 != stat64(img.c_str(),&s))
        {
            return ERROR;
        }
        format = IMAGE_FORMAT_RAW;
        size = s.st_size;

    }
    return SUCCESS;
}


/******************************************************************************/ 
STATUS CreateShareDisk(MessageUnit *mu, const string& request_id, const string& base_id, int64 size)
{
    return SUCCESS;
}
/******************************************************************************/ 
STATUS AuthShareDisk(MessageUnit *mu, const auth_info& auth)
{
    return SUCCESS;
}
/******************************************************************************/ 
STATUS DeAuthShareDisk(MessageUnit *mu, const auth_info& auth)
{
    return SUCCESS;
}
/******************************************************************************/ 
STATUS DeleteShareDisk(MessageUnit *mu, const string& request_id)
{
    return SUCCESS;
}

