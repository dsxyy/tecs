/*
xmlrpc命令入参和出参预处理程序，用于入参、出参复杂的命令进行特殊处理
*/
#include "xmlrpc_preprocess.h"
#include <map>
#include <vector>
#include <iostream>
#include "includes.h"
#include "vnet_api.h"
#include "vmcfg_api.h"
#include "cluster_manager_api.h"
#include "host_api.h" 
#include "vmtpl_api.h"
#include "image_api.h"
#include "project_api.h"
#include "version_api.h"
#include "file_api.h"

using namespace std;
using namespace xmlrpc_c;

BYTE g_aucParaDataBuf[1024];

typedef vector<int> PARASET;
typedef void (*FUNC_PARAIN_PREPROCESS)(MSG_COMM_OAM *, xmlrpc_c::paramList &);
typedef void (*FUNC_PARAOUT_PREPROCESS)( xmlrpc_c::value, MSG_COMM_OAM *);
typedef map<DWORD, FUNC_PARAIN_PREPROCESS> T_MAP_PARAIN_FUNC;
typedef map<DWORD, FUNC_PARAOUT_PREPROCESS> T_MAP_PARAOUT_FUNC;
static T_MAP_PARAIN_FUNC    g_map_ParaInFunc;
static T_MAP_PARAOUT_FUNC g_map_ParaOutFunc;
static BOOLEAN AddDataToXmlParamList(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &paramlist, PARASET vCliParas);

typedef enum Type_Query_Return {
        FIRST_PKG_FINISH        = 0, /*没有分包，第一包，也是最后一包*/
        FIRST_PKG_NOTFINISH,        /*分包，第一包*/
        MIDDLE_PKG_NOTFINISH,     /*分包，中间包*/
        LAST_PKG_FINISH                 /*分包，最后一包*/
}T_QueryRetType;
static T_QueryRetType GetQueryReturnType(MSG_COMM_OAM *pCliMsg);
    
extern BOOLEAN GetParaByNo(MSG_COMM_OAM *pCliMsg, int iParaNo, OPR_DATA *pData, int index = 0);
extern void AddDataToParamList(OPR_DATA *pData, TYPE_CMD_PARA *pCurParaInfo, xmlrpc_c::paramList &paramlist);

/* CLI上对应的MAP定义 */
#define CLUSTER_REGISTER_STATE_MAP 501
#define CLUSTER_RUN_STATE_MAP      502

/* 镜像上使用到的MAP定义 */
#define IMAGE_TYPE_MAP             701
#define IMAGE_ARCH_MAP             702
#define IMAGE_PUBLIC_MAP           704
#define IMAGE_STATE_MAP            705
#define IMAGE_YES_OR_NO_MAP        706

/*用户管理用到的MAP*/
#define USER_ROLE_MAP              401
#define USER_ENABLE_MAP            402

#define HOST_IS_DISABLED_MAP        601
#define HOST_REGISTER_STATE_MAP     602
#define HOST_RUN_STATE_MAP          603
#define HOST_HARDWARE_STATE_MAP     604
#define HOST_PORT_USED_MAP          605
#define HOST_PORT_NEGOTITATE_MAP    606
#define HOST_PORT_SPEED_MAP         607
#define HOST_PORT_DUPLEX_MAP        608
#define HOST_PORT_MASTER_MAP        609
#define HOST_PORT_STATE_MAP         610
#define HOST_TRUNK_TYPE_MAP         611
#define HOST_IS_SUPPORT_HVM_MAP     612
#define HOST_PORT_LINKED_MAP        613

/* 虚拟机模板MAP */
#define VT_MANAGER_T_OR_FALSE_MAP   1
#define VT_MANAGER_VIRT_TYPE_MAP    901
//#define VT_MANAGER_SYS_POLICY_MAP   902
//#define VT_MANAGER_HOST_POLICY_MAP  903
#define VT_MANAGER_VDISK_TYPE_MAP   904
#define VT_MANAGER_VDISK_BUS_MAP    905
#define VT_MANAGER_DHCP_STATE_MAP   906
#define VT_MANAGER_SRIOV_MAP        907

/* 虚拟机管理MAP */
#define VM_MANAGER_VIRT_TYPE_MAP    1001
//#define VM_MANAGER_SYS_POLICY_MAP   1002
//#define VM_MANAGER_HOST_POLICY_MAP  1003

#define VM_MANAGER_VDISK_TYPE_MAP   1004
#define VM_MANAGER_VDISK_BUS_MAP    1005  
#define VM_MANAGER_OPERATE_MAP      1006  
#define VM_MANAGER_VM_STATE_MAP     1007 
#define VM_MANAGER_DHCP_STATE_MAP   1008
#define VM_MANAGER_SRIOV_MAP        1009

#ifndef MAPTYPE_RETURNCODE
#define MAPTYPE_RETURNCODE 400

/*告警管理MAP*/
#define ALARM_QUERY_MAP             1301
#endif

#define CLI_RETURN_FIRST_PAGE       0
#define CLI_RETURN_SECOND_PAGE      1
#define CLI_RETURN_ERR_FLAG         2

inline float BytesToKB(long long bytes)
{
    return bytes/((float)1024);
}

inline float BytesToMB(long long bytes)
{
    return bytes/((float)1024*1024);
}

inline float BytesToGB(long long bytes)
{
    return bytes/((float)1024*1024*1024);
}

/**************************************************
钩子函数添加方法:
1、按照FUNC_PARAIN_PREPROCESS/FUNC_PARAOUT_PREPROCESS定义预处理函数，
       如果没有定义，cli按照默认方式自动转换
2、在LoadPreprocessFuncs 函数中注册定义的钩子函数
3、输入预处理函数例子func_sample_in
       输出预处理函数例子func_sample_out
**************************************************/

/*预处理函数定义*/
void func_sample_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    //逐个给paramlist 中的参数赋值

    if (pCliMsg->bIfNo)
    {
        //如果是no命令，按照no命令的参数进行解析
    }
    else
    {
        /*设定xmlrpc的第一个参数值*/
        /*首先设定cli参数编号和应用定义的参数对应关系*/
        vParaSet.push_back(0);
        vParaSet.push_back(1);
        vParaSet.push_back(5);
        vParaSet.push_back(7);    
        //从配置的一个或多个cli参数中，读取第一个已经输入的赋值给xmlrpc参数
        AddDataToXmlParamList(pCliMsg, list, vParaSet);

        /*设定xmlrpc的第二个参数值*/
        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(2);
        vParaSet.push_back(3);
        vParaSet.push_back(4);    
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    
        /*设定xmlrpc的第三个参数值*/
        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(6);    
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    
    return;
}

void func_sample_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    //根据xmlrpc返回的数组value，生成和回显脚本匹配的cli结构   
    
    /*1、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*2、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/

    /*第一个参数是统一的返回码，因此这里从1开始*/
    int iTmp = xmlrpc_c::value_int(rs[1]);
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
            
    string strTmp = xmlrpc_c::value_string(rs[2]);
    AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
            
    WORD64 qdwTmp = xmlrpc_c::value_i8(rs[3]);
    AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&qdwTmp, sizeof(WORD64));

    int iMap = xmlrpc_c::value_int(rs[4]);
    AddXmlRpcMapParaToCliMsg(pCliMsg, iMap);
    
    return;
}

void image_enable_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
     {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }  

    {
        xmlrpc_c::paramList listtemp;
        bool flag;
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        flag = xmlrpc_c::value_boolean(listtemp.getInt(0));   
        list.add(value_boolean(flag));       
    }   
    
}

/* image_publish 把第2个参数修改为bool型 */
void image_publish_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
     {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }  

    {
        xmlrpc_c::paramList listtemp;
        bool flag;
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        flag = xmlrpc_c::value_boolean(listtemp.getInt(0));   
        list.add(value_boolean(flag));       
    }   
    
}
    
/* 镜像注册的输入钩子 */
void image_register_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    int     type;
    int     arch;
    ApiImageReg Data;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    
    if (pCliMsg->bIfNo)
    {
        //如果是no命令，按照no命令的参数进行解析    
        /*设定xmlrpc的第一个参数值 regiseter_data._id*/
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
        vParaSet.clear();//首先清空前一个参数的内容
        return;
    }

    /* 下面是非NO命令解析 */
    /*设定xmlrpc的第一个参数值 regiseter_data._id*/
     {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.id = xmlrpc_c::value_i8(listtemp.getI8(0));    
    }   
    
     {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.name = xmlrpc_c::value_string(listtemp.getString(0));    
    }   
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        type = xmlrpc_c::value_int(listtemp.getInt(0));
        Data.type = xmlrpc_c::value_string(GetMapString(IMAGE_TYPE_MAP,type));    
    }   
   
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        arch = xmlrpc_c::value_int(listtemp.getInt(0));
        Data.arch = xmlrpc_c::value_string(GetMapString(IMAGE_ARCH_MAP,arch));    
    }   

    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(4);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.is_public = xmlrpc_c::value_int(listtemp.getInt(0));    
    }   
 
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(5);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.is_available = xmlrpc_c::value_int(listtemp.getInt(0));    
    }  

    if (true == GetParaByNo(pCliMsg, 6, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(6);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.description = xmlrpc_c::value_string(listtemp.getString(0));   
    }
      
    list.add(Data.to_rpc());
    
    return;
}

/* 镜像修改的输入钩子 */
void project_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;


    /*设定xmlrpc的第一个参数值 regiseter_data._id*/
    if (true == GetParaByNo(pCliMsg, 0, ptData))
    {
        list.add(value_i8(-5));
        list.add(value_string(""));
        list.add(value_string(""));
    }
    else
    {
        list.add(value_i8(0));
        
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
        if (true == GetParaByNo(pCliMsg, 2, ptData))
        {
            vParaSet.clear();
            vParaSet.push_back(2);
            AddDataToXmlParamList(pCliMsg, list, vParaSet);
        }
        else
        {
            list.add(value_string(""));
        }
    }

}

/* 镜像修改的输入钩子 */
void image_set_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    xmlrpc_c::paramList listtemp1;
    xmlrpc_c::paramList listtemp2;
    int     type;
    int     arch;

    //逐个给paramlist 中的参数赋值

    /*设定xmlrpc的第一个参数值 image_id*/
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);
    vParaSet.clear();//首先清空前一个参数的内容
    
    /* modify_data._name */
    vParaSet.push_back(1);
    if (false == AddDataToXmlParamList(pCliMsg, list, vParaSet))
    {
        list.add(value_string(""));
    }
    vParaSet.clear();//首先清空前一个参数的内容
    
    /* modify_data._type */
    vParaSet.push_back(2);
    if (true == AddDataToXmlParamList(pCliMsg, listtemp1, vParaSet))
    {
        type = xmlrpc_c::value_int(listtemp1.getInt(0));
        list.add(value_string(GetMapString(IMAGE_TYPE_MAP,type)));   
    }
    else
    {
        list.add(value_string(""));
    }
    vParaSet.clear();//首先清空前一个参数的内容
 
    /* modify_data._arch */
    vParaSet.push_back(3);
    if (true == AddDataToXmlParamList(pCliMsg, listtemp2, vParaSet))
    {
        arch = xmlrpc_c::value_int(listtemp2.getInt(0));
        list.add(value_string(GetMapString(IMAGE_ARCH_MAP,arch)));
    }
    else
    {
        list.add(value_string(""));
    }
    vParaSet.clear();//首先清空前一个参数的内容

    vParaSet.push_back(4);
    if (false == AddDataToXmlParamList(pCliMsg, list, vParaSet))
    {
        list.add(value_string(""));
    }
    
    return;
}

/* 镜像显示输入钩子 */
void image_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;


    /*设定xmlrpc的第一个参数值 regiseter_data._id*/
    if (true == GetParaByNo(pCliMsg, 0, ptData))
    {
        list.add(value_int(-4));
        list.add(value_string(""));
        list.add(value_i8(-1));
    }
    else if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
        list.add(value_int(-2));
        list.add(value_string(""));
        list.add(value_i8(-1));
    }
    else if (true == GetParaByNo(pCliMsg, 2, ptData))
    {
        list.add(value_int(0));
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
        list.add(value_i8(-1));
    }
    else
    {
        list.add(value_int(1));
        list.add(value_string(""));
        vParaSet.clear();
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);  
    }
    
    return;
}

/* 文件显示输入钩子 */
void image_file_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;


    /*设定xmlrpc的第一个参数值 regiseter_data._id*/
    if (true == GetParaByNo(pCliMsg, 0, ptData))
    {
        list.add(value_i8(-4));
    }
    else
    {
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    
    return;
}


/* 虚拟机显示输入钩子 */
void vm_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;


    /*设定xmlrpc的第一个参数值 regiseter_data._id*/
    if (true == GetParaByNo(pCliMsg, 0, ptData))
    {
        list.add(value_i8(-5));
    }
    else
    {
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    
    return;
}


/* 虚拟机热迁移钩子 */
void vm_live_migrate(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
     PARASET vParaSet;
     OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
     ApiVmmLiveMigrate Data;

    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vid = xmlrpc_c::value_i8(listtemp.getI8(0));
    }

    if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
        {
            xmlrpc_c::paramList listtemp;
            vParaSet.clear();
            vParaSet.push_back(1);
            AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
            Data.hid = xmlrpc_c::value_int(listtemp.getInt(0));
        }

        {
            xmlrpc_c::paramList listtemp;
            vParaSet.clear();
            vParaSet.push_back(2);
            AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
            Data.bForced = xmlrpc_c::value_int(listtemp.getInt(0));
        }
    }
    else
    {
        Data.hid = -1;
        Data.bForced = false;
    }

    list.add(Data.to_rpc());

    return;
}


/* 虚拟机模板显示输入钩子 */
void vt_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    string   tmp_str;


    /*设定xmlrpc的第一个参数值 regiseter_data._id*/
    if (true == GetParaByNo(pCliMsg, 0, ptData))
    {
        list.add(value_int(1));
        list.add(value_string(tmp_str));
    }
    else if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
        list.add(value_int(2));
        list.add(value_string(tmp_str));
    }
    else
    {
        list.add(value_int(3));
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    
    return;
}


/* 虚拟机模板配置输入钩子 */
void vmtemplate_manager_create_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    VtQuickAllocateData Data;
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vt_name = xmlrpc_c::value_string(listtemp.getString(0));    
    }

    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.is_public = xmlrpc_c::value_int(listtemp.getInt(0));
    }   

    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vcpu = xmlrpc_c::value_int(listtemp.getInt(0));
    }  

    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.tcu = xmlrpc_c::value_int(listtemp.getInt(0));
    }    
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(4);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.memory = xmlrpc_c::value_i8(listtemp.getI8(0)) * (1024*1024);
    } 
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(5);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.machine.id = xmlrpc_c::value_i8(listtemp.getI8(0));
    }   
    
    if (true == GetParaByNo(pCliMsg, 6, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(6);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        int type = xmlrpc_c::value_int(listtemp.getInt(0));
        Data.machine.bus = GetMapString(VT_MANAGER_VDISK_BUS_MAP,type);
    }
    else
    {
        Data.machine.bus = DISK_BUS_SCSI;
    }
      
    if (true == GetParaByNo(pCliMsg, 7, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(7);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);      
        Data.root_device = xmlrpc_c::value_string(listtemp.getString(0));
    }
    
    
    if (true == GetParaByNo(pCliMsg, 8, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(8);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.kernel_command = xmlrpc_c::value_string(listtemp.getString(0));
    }   

    
    if (true == GetParaByNo(pCliMsg, 9, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(9);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.bootloader = xmlrpc_c::value_string(listtemp.getString(0));
    }   
   
    
    if (true == GetParaByNo(pCliMsg, 10, ptData))
    {
        xmlrpc_c::paramList listtemp;
        int       type;
        vParaSet.clear();
        vParaSet.push_back(10);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        type = xmlrpc_c::value_int(listtemp.getInt(0));
        Data.virt_type = GetMapString(VT_MANAGER_VIRT_TYPE_MAP,type);
    }
    else
    {
        Data.virt_type = VIRT_TYPE_HVM;
    }   
   
    
    if (true == GetParaByNo(pCliMsg, 11, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(11);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.rawdata = xmlrpc_c::value_string(listtemp.getString(0));
    }   

    list.add(Data.to_rpc());
    
    return;
}

/* 虚拟硬盘配置输入 */
void vmcfg_vt_disk_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    vector<xmlrpc_c::value> disk;
    int i;
    
    {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
    }

    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (1+2*i), ptData))
        { 
            ApiVmDisk Data;
            int type;
            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.size = xmlrpc_c::value_i8(listtemp.getI8(0))*1024*1024*1024;
            }
            
            if (true == GetParaByNo(pCliMsg, (2+2*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2+2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                type = xmlrpc_c::value_int(listtemp.getInt(0));
                Data.bus = GetMapString(VT_MANAGER_VDISK_BUS_MAP,type); 
            }
            else
            {
                Data.bus = DISK_BUS_SCSI ;
            }
            
            Data.type = DISK_TYPE_DISK;

            disk.push_back(Data.to_rpc());    
        }   
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(value_array(disk));
    
    return;
}

/* 虚拟机配置输入钩子 */
void vmcfg_manager_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    VcQuickAllocateData Data;
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vm_name = xmlrpc_c::value_string(listtemp.getString(0));    
    }

   // if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.project_name = xmlrpc_c::value_string(listtemp.getString(0));
    }   

    
    if (true == GetParaByNo(pCliMsg, 2, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vm_num = xmlrpc_c::value_int(listtemp.getInt(0));
    }
    else
    {
        Data.vm_num = 1;
    }

  
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vcpu = xmlrpc_c::value_int(listtemp.getInt(0));
    }  

    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(4);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.tcu = xmlrpc_c::value_int(listtemp.getInt(0));
    }    
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(5);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.memory = xmlrpc_c::value_i8(listtemp.getI8(0)) * (1024*1024);
    } 
    
    
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(6);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.machine.id = xmlrpc_c::value_i8(listtemp.getI8(0));
        Data.machine.type = DISK_TYPE_MACHINE;
    }   
    
    if (true == GetParaByNo(pCliMsg, 7, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(7);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
      
        int type = xmlrpc_c::value_int(listtemp.getInt(0));
        Data.machine.bus = GetMapString(VM_MANAGER_VDISK_BUS_MAP,type);
    }
    else
    {
       Data.machine.bus = DISK_BUS_SCSI; //默认选SCSI
    }
    
    
    if (true == GetParaByNo(pCliMsg, 8, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(8);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);      
        Data.root_device = xmlrpc_c::value_string(listtemp.getString(0));
    }
    
    
    if (true == GetParaByNo(pCliMsg, 9, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(9);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.kernel_command = xmlrpc_c::value_string(listtemp.getString(0));
    }   

    
    if (true == GetParaByNo(pCliMsg, 10, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(10);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.bootloader = xmlrpc_c::value_string(listtemp.getString(0));
    }   
   
    
    if (true == GetParaByNo(pCliMsg, 11, ptData))
    {
        xmlrpc_c::paramList listtemp;
        int       type;
        vParaSet.clear();
        vParaSet.push_back(11);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        type = xmlrpc_c::value_int(listtemp.getInt(0));
        Data.virt_type = GetMapString(VM_MANAGER_VIRT_TYPE_MAP,type);
    } 
    else
    {
        Data.virt_type = VIRT_TYPE_HVM;
    }   
   
    
    if (true == GetParaByNo(pCliMsg, 12, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(12);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.rawdata = xmlrpc_c::value_string(listtemp.getString(0));
    }   

    if (true == GetParaByNo(pCliMsg, 12, ptData))
    {
        xmlrpc_c::paramList listtemp;
        vParaSet.clear();
        vParaSet.push_back(12);
        AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
        Data.vnc_passwd = xmlrpc_c::value_string(listtemp.getString(0));
    }
    else
    {
        Data.vnc_passwd = "";
    }

    list.add(Data.to_rpc());
    
    return;
}

/* 虚拟硬盘配置输入 */
void vmcfg_vm_disk_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    vector<xmlrpc_c::value> disk;
    int i;
    
    {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
    }

    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (1+2*i), ptData))
        { 
            ApiVmDisk Data;
            int type;
            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.size = xmlrpc_c::value_i8(listtemp.getI8(0))*1024*1024*1024;
            }
            
            if (true == GetParaByNo(pCliMsg, (2+2*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2+2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                type = xmlrpc_c::value_int(listtemp.getInt(0));
                Data.bus = GetMapString(VM_MANAGER_VDISK_BUS_MAP,type); 
            }
            else
            {
                Data.bus = DISK_BUS_SCSI ;
            }        

            Data.type = DISK_TYPE_DISK;
            disk.push_back(Data.to_rpc());    
        }   
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(value_array(disk));
    
    return;
}

/* 虚拟硬盘配置输入 */
void vmcfg_vm_image_disk_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    vector<xmlrpc_c::value> disk;
    int i;
    
    {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
    }

    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (1+3*i), ptData))
        { 
            ApiVmDisk Data;
            int type;
            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+3*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.id = xmlrpc_c::value_i8(listtemp.getI8(0));
            }
            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2+3*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                type = xmlrpc_c::value_int(listtemp.getInt(0));
                Data.type = GetMapString(VM_MANAGER_VDISK_TYPE_MAP,type); 
            }
            
            if (true == GetParaByNo(pCliMsg, (3+3*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(3+3*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                type = xmlrpc_c::value_int(listtemp.getInt(0));
                Data.bus = GetMapString(VM_MANAGER_VDISK_BUS_MAP,type); 
            }
            else
            {
                Data.bus = DISK_BUS_SCSI;
            }        
            disk.push_back(Data.to_rpc());    
        }   
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(value_array(disk));
    
    return;
}

/* 虚拟机模板硬盘配置输入 */
void vmcfg_vt_image_disk_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    vector<xmlrpc_c::value> disk;
    int i;
    
    {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
    }

    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (1+3*i), ptData))
        { 
            ApiVmDisk Data;
            int type;
            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+3*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.id = xmlrpc_c::value_i8(listtemp.getI8(0));
            }
            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2+3*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                type = xmlrpc_c::value_int(listtemp.getInt(0));
                Data.type = GetMapString(VT_MANAGER_VDISK_TYPE_MAP,type); 
            }
            
            if (true == GetParaByNo(pCliMsg, (3+3*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(3+3*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                type = xmlrpc_c::value_int(listtemp.getInt(0));
                Data.bus = GetMapString(VT_MANAGER_VDISK_BUS_MAP,type); 
            }
            else
            {
                Data.bus = DISK_BUS_SCSI;
            }        
            disk.push_back(Data.to_rpc());    
        }   
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(value_array(disk));
    
    return;
}


// ipv4_to_string 
#define DIGEST_COUNT                17
#define IPV4_ADDR_STRING_LEN sizeof("255.255.255.255")
#define IPV4_ADDR_LEN           4
#define MAX_IP_ADDR_LEN         IPV4_ADDR_LEN

int local_is_bigendian()
{
    typedef union 
    {
     char c[4];
     int i;
    } testEndian;

    testEndian _t;
    _t.c[0]=1;
    _t.c[1]=0;
    _t.c[2]=0;
    _t.c[3]=0;

    // little endian
    if ( _t.i == 1)
    {
        return 0;
    }

    return 1;
}

int  vnet_ntohl(int x)
{
    if(local_is_bigendian())
    {
        return x;
    }

    return (int)( \
                        ((((int)(x)) & 0x000000ff) << 24) | \
                        ((((int)(x)) & 0x0000ff00) <<  8) | \
                        ((((int)(x)) & 0x00ff0000) >>  8) | \
                        ((((int)(x)) & 0xff000000) >> 24));
}


char * ipv4_to_string(int addr)
{
    static char str[16];
    static char digits[DIGEST_COUNT] = "0123456789ABCDEF";
    char *tmp, punctuation, stack[4];
    unsigned int  i, j, chunk, radix, size;
    unsigned bits;
    unsigned char temp_ip[IPV4_ADDR_LEN] = {0};  
    unsigned int len = sizeof(str);

    int l_addr = addr;
    
    // ---> 转换一下 
    l_addr = vnet_ntohl(addr);

    memcpy(temp_ip, &l_addr, IPV4_ADDR_LEN);
    
    punctuation = '.'; 
    size = IPV4_ADDR_STRING_LEN; 
    chunk = 1; 
    radix = 10;
   
    if (len < size)
    {        
        *str = '\0'; 
        return(str);
    }

    tmp = str;
    size = IPV4_ADDR_LEN;
    for (i = 0; i < size; i += chunk)
    {
        bits = 0;
        for (j = 0; j < chunk; j++)
        {
            //VNET_ASSERT((i+j)<MAX_IP_ADDR_LEN);
            bits = (bits << 8) | temp_ip[i + j];
        }
        if (i > 0)
        {
            *tmp++ = punctuation;
        }
        j = 0;
        do {
            //VNET_ASSERT(j<4);
            //VNET_ASSERT((bits % radix)<DIGEST_COUNT);
            stack[j++] = digits[bits % radix];
            bits /= radix;
        } while (bits);
        while (j > 0)
        {
            //VNET_ASSERT((j-1)<4); 
            *tmp++ = stack[--j];
        }
    }
    *tmp = '\0';

    return(str);
}

/* 虚拟网卡配置输入 */
void vmcfg_vm_Nic_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    vector<xmlrpc_c::value> Nic;
    int i;
    
    {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
    }

    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (1+7*i), ptData))
        {
            ApiVmNic Data;
            
            //if (true == GetParaByNo(pCliMsg, (1+5*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.sriov = xmlrpc_c::value_int(listtemp.getInt(0));
            }
            
           // if (true == GetParaByNo(pCliMsg, (2+5*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.plane = xmlrpc_c::value_string(listtemp.getString(0));
            }
            
           // if (true == GetParaByNo(pCliMsg, (3+6*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(3+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.vlan = xmlrpc_c::value_int(listtemp.getInt(0)); 
            }

           // if (true == GetParaByNo(pCliMsg, (4+5*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(4+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.dhcp = xmlrpc_c::value_int(listtemp.getInt(0)); 
            }
            
            if (true == GetParaByNo(pCliMsg, (5+7*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(5+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.ip = ipv4_to_string(xmlrpc_c::value_int(listtemp.getInt(0)));
            }

            if (true == GetParaByNo(pCliMsg, (6+7*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(6+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.netmask = ipv4_to_string(xmlrpc_c::value_int(listtemp.getInt(0)));
            }
            
            if (true == GetParaByNo(pCliMsg, (7+7*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(7+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.gateway = ipv4_to_string(xmlrpc_c::value_int(listtemp.getInt(0)));
            }
          /*  if (true == GetParaByNo(pCliMsg, (6+6*i), ptData))
            {
                char temp[32];
                memset(temp, 0, sizeof(temp));
                sprintf(temp,"%2x:%2x:%2x:%2x:%2x:%2x",
                        ptData->Data[0],
                        ptData->Data[1],
                        ptData->Data[2],
                        ptData->Data[3],
                        ptData->Data[4],
                        ptData->Data[5]);
                printf(temp);
                Data.mac = temp;     
            }       */ 

            Nic.push_back(Data.to_rpc()); 
        }        
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(value_array(Nic));
    
    return;
}


/* 虚拟机模板网卡配置输入 */
void vmcfg_vt_Nic_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    vector<xmlrpc_c::value> Nic;
    int i;
    
    {
        vParaSet.clear();
        vParaSet.push_back(0);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
    }

    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (1+7*i), ptData))
        {
            ApiVmNic Data;
            
            //if (true == GetParaByNo(pCliMsg, (1+6*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.sriov = xmlrpc_c::value_int(listtemp.getInt(0));
            }
            
            //if (true == GetParaByNo(pCliMsg, (2+6*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.plane = xmlrpc_c::value_string(listtemp.getString(0));
            }
            
            //if (true == GetParaByNo(pCliMsg, (3+6*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(3+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.vlan = xmlrpc_c::value_int(listtemp.getInt(0)); 
            }

            //if (true == GetParaByNo(pCliMsg, (4+6*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(4+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.dhcp = xmlrpc_c::value_int(listtemp.getInt(0)); 
            }
            
            if (true == GetParaByNo(pCliMsg, (5+7*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(5+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.ip = ipv4_to_string(xmlrpc_c::value_int(listtemp.getInt(0)));
            }

            if (true == GetParaByNo(pCliMsg, (6+7*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(6+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.netmask = ipv4_to_string(xmlrpc_c::value_int(listtemp.getInt(0)));
            }
            
            if (true == GetParaByNo(pCliMsg, (7+7*i), ptData))
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(7+7*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                Data.gateway = ipv4_to_string(xmlrpc_c::value_int(listtemp.getInt(0)));
            }
            
            
           /* if (true == GetParaByNo(pCliMsg, (6+6*i), ptData))
            {
                char temp[32];
                memset(temp, 0, sizeof(temp));
                sprintf(temp,"%2x:%2x:%2x:%2x:%2x:%2x",
                        ptData->Data[0],
                        ptData->Data[1],
                        ptData->Data[2],
                        ptData->Data[3],
                        ptData->Data[4],
                        ptData->Data[5]);
                printf(temp);
                Data.mac = temp;     
            }      */  

            Nic.push_back(Data.to_rpc()); 
        }        
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(value_array(Nic));
    
    return;
}

void vmcfg_vm_create_indirect_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    
    // vt_name
    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet); 

    // project_name
    {
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
   

    { //vm_name
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }

    if (true == GetParaByNo(pCliMsg, 3, ptData))
    {
        vParaSet.clear();
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    else
    {
        list.add(value_int(1));
    }    

      
}


void vmcfg_vm_operation_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    xmlrpc_c::paramList listtemp;
    int type;
    
    
    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);  

    vParaSet.clear();
    vParaSet.push_back(1);
    AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
    type = xmlrpc_c::value_int(listtemp.getInt(0));
    list.add(value_string(GetMapString(VM_MANAGER_OPERATE_MAP,type)));     
}

void vmcfg_vm_del_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    xmlrpc_c::paramList listtemp;
    
    
    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);  

    list.add(value_string(VM_ACTION_DELETE));     
}

/* 配置虚拟机互斥关系输入 */
void vmcfg_set_relation_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    int i;
    
    ApiVmCfgRelationData  relation_data;
    relation_data.type = static_cast<int >(ApiVmCfgRelationData::RELATION_ADD);
    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (2*i), ptData))
        { 
            ApiVmCfgMap vm_map;            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                vm_map.src = xmlrpc_c::value_i8(listtemp.getI8(0));
            }
            
            GetParaByNo(pCliMsg, (1+2*i), ptData);
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                vm_map.des = xmlrpc_c::value_i8(listtemp.getI8(0));; 
            }            

            relation_data.vec_vm.push_back(vm_map);    
        }   
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(relation_data.to_rpc());
    
    return;
}



/* 删除虚拟机互斥关系输入 */
void vmcfg_del_relation_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    int i;
    
    ApiVmCfgRelationData  relation_data;
    relation_data.type = static_cast<int >(ApiVmCfgRelationData::RELATION_DEL);
    for (i = 0; i < 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, (2*i), ptData))
        { 
            ApiVmCfgMap vm_map;            
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                vm_map.src = xmlrpc_c::value_i8(listtemp.getI8(0));
            }
            
            GetParaByNo(pCliMsg, (1+2*i), ptData);
            {
                xmlrpc_c::paramList listtemp;
                vParaSet.clear();
                vParaSet.push_back(1+2*i);
                AddDataToXmlParamList(pCliMsg, listtemp, vParaSet);
                vm_map.des = xmlrpc_c::value_i8(listtemp.getI8(0));; 
            }            

            relation_data.vec_vm.push_back(vm_map);    
        }   
    }
     
    /* 把所有的输入结构都输入到一个Vector上 */
    list.add(relation_data.to_rpc());
    
    return;
}

/* 虚拟机互斥关系显示输入钩子 */
void vmcfg_relation_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;


    /*设定xmlrpc的第一个参数值 */
    if (true == GetParaByNo(pCliMsg, 0, ptData))
    {
        list.add(value_i8(0));
    }
    else
    {
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    
    return;
}

/* 虚拟机互斥关系查询输出 */
void vmcfg_relation_show_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    else
    {
        iTmp = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    
    /* 因为下面这个VECTOR就一个成员，所以不需要压Vector数据，就读取一个成员就好了 */
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[1]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiVmCfgMap Data;
         
        Data.from_rpc(value_struct(*it_struct));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.src, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.des, sizeof(WORD64));
    }
    
    return;
}

void project_vm_show_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    
    
    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet); 

    if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    else
    {
        list.add(value_string(""));
    }
}

void project_delete_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;


    
    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet); 

    if (!pCliMsg->bIfNo)
    {   /* 不是no命令，读取描述字符串 */
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet); 
        return;
    }
    
    if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
        vParaSet.clear();
        vParaSet.push_back(1);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }
    else
    {
        list.add(value_string(""));
    }
}


void host_register_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;

    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    vParaSet.clear();
    vParaSet.push_back(1);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    bool is_forced = false;
    if (true == GetParaByNo(pCliMsg, 2, ptData))
    {
        is_forced = (ptData->Data[0] != 0);
    }
    list.add(value_boolean(is_forced));

    if (true == GetParaByNo(pCliMsg, 3, ptData))
    {
         AddDataToParamList(ptData, NULL, list);
    }
    else
    {
        list.add(value_string(string()));
    }    
}

void host_disable_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;

    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    vParaSet.clear();
    vParaSet.push_back(1);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    bool is_forced = false;
    if (true == GetParaByNo(pCliMsg, 2, ptData))
    {
        is_forced = (ptData->Data[0] != 0);
    }
    list.add(value_boolean(is_forced));
}

void port_config_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    int i;

    for (i = 0; i < 3; i++)
    {
        vParaSet.clear();
        vParaSet.push_back(i);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }

    if (pCliMsg->bIfNo)
    {
        return;
    }

    xmlrpc_c::paramList temp_list;
    for (i = 3; i <= 8; i++)
    {
        if (true == GetParaByNo(pCliMsg, i, ptData))
        {
             AddDataToParamList(ptData, NULL, temp_list);
        }
        else
        {
            temp_list.add(value_int(-1));
        }
    }

    ApiPortSet config;
    config.used = temp_list.getInt(0);
    config.negotiate = temp_list.getInt(1);
    config.speed = temp_list.getInt(2);
    config.duplex = temp_list.getInt(3);
    config.master = temp_list.getInt(4);
    config.delay_up_time = temp_list.getInt(5);
    list.add(config.to_rpc());
}

void trunk_config_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    unsigned int i;

    for (i = 0; i < 3; i++)
    {
        vParaSet.clear();
        vParaSet.push_back(i);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
    }

    if (!pCliMsg->bIfNo)
    {
        vParaSet.clear();
        vParaSet.push_back(i);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);
        i++;
    }

    xmlrpc_c::paramList temp_list;
    int index = i;
    for (; i < pCliMsg->Number; i++)
    {
        if (true == GetParaByNo(pCliMsg, index, ptData, i-index))
        {
             AddDataToParamList(ptData, NULL, temp_list);
        }
        else
        {
            temp_list.add(value_string(string()));
        }
    }

    vector<xmlrpc_c::value> ports;
    for (i = 0; i < temp_list.size(); i++)
    {
        ports.push_back(value_string(temp_list.getString(i)));
    }
    list.add(value_array(ports));
}

void svcpu_config_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;    

    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    list.add(value_string(string()));

    vParaSet.clear();
    vParaSet.push_back(1);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    if (!pCliMsg->bIfNo)
    {
        vParaSet.clear();
        vParaSet.push_back(2);
        AddDataToXmlParamList(pCliMsg, list, vParaSet);

        if (true == GetParaByNo(pCliMsg, 3, ptData))
        {
             AddDataToParamList(ptData, NULL, list);
        }
        else
        {
            list.add(value_string(string()));
        }
    }
}

void svcpu_query_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;

    vParaSet.clear();
    vParaSet.push_back(0);
    AddDataToXmlParamList(pCliMsg, list, vParaSet);

    list.add(value_string(string()));

    if (true == GetParaByNo(pCliMsg, 1, ptData))
    {
         AddDataToParamList(ptData, NULL, list);
    }
    else
    {
        list.add(value_i8(-1));
    }
}


/******************************************************************************
                       输出到CLI上的钩子在下面定义
******************************************************************************/
/* 集群管理查询 */
void func_cluster_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    //根据xmlrpc返回的数组value，生成和回显脚本匹配的cli结构   
    
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);
    
    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
          
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = xmlrpc_c::value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    int iTmp1 = array_struct.size();

    /* 写入第3个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp1, sizeof(int));
       
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiClusterManger Data;
        
        Data.from_rpc(value_struct(*it_struct));
        
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.cluster_name.c_str(), Data.cluster_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.ip.c_str(), Data.ip.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.cluster_append.c_str(), Data.cluster_append.size());
        AddMapParaToMsg(pCliMsg, CLUSTER_REGISTER_STATE_MAP, Data.register_State);
        AddMapParaToMsg(pCliMsg, CLUSTER_RUN_STATE_MAP, Data.run_state);
        
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.core_free_max, sizeof(int));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.tcu_unit_free_max, sizeof(int));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.tcu_max_total, sizeof(int));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.tcu_free_total, sizeof(int));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.tcu_free_max, sizeof(int));
        
        float temp;
        temp = BytesToGB(Data.disk_max_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToGB(Data.disk_free_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToGB(Data.disk_free_max);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        
        temp = BytesToGB(Data.share_disk_max_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToGB(Data.share_disk_free_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp)); 
        
        temp = BytesToMB(Data.mem_max_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToMB(Data.mem_free_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToMB(Data.mem_free_max);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
    }    
    return;
}


void image_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    //根据xmlrpc返回的数组value，生成和回显脚本匹配的cli结构   
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);
    
    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
    
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = xmlrpc_c::value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiImageInfo Data;
        string temp;
        char   tempbuf[64];
        
        memset(tempbuf, 0, sizeof(tempbuf));
        Data.from_rpc(value_struct(*it_struct));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)Data.user_name.c_str(), Data.user_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.name.c_str(), Data.name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.type.c_str(), Data.type.size());
        AddMapParaToMsg(pCliMsg, IMAGE_PUBLIC_MAP, Data.is_public);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.stored_time.c_str(), Data.stored_time.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.registered_time.c_str(), Data.registered_time.size());
        AddMapParaToMsg(pCliMsg, IMAGE_STATE_MAP, Data.state);
        if (Data.size < 1024)
        {
            sprintf(tempbuf, "%lu B", Data.size);
        }
        else if (Data.size < 1024*1024)
        {
            sprintf(tempbuf, "%.0f KB", BytesToKB(Data.size));
        }
        else if (Data.size < 1024*1024*1024)
        {
            sprintf(tempbuf, "%.1f MB", BytesToMB(Data.size));
        }
        else
        {
           sprintf(tempbuf, "%.2f GB", BytesToGB(Data.size)); 
        }
        temp = tempbuf;
        
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)temp.c_str(), temp.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.running_vms, sizeof(int));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.arch.c_str(), Data.arch.size());  
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.description.c_str(), Data.description.size());  
    }    
    return;
}



void image_file_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    //根据xmlrpc返回的数组value，生成和回显脚本匹配的cli结构   
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
    
    
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    
    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiFileInfo Data;
        
        string temp;
        char   tempbuf[64];
        memset(tempbuf, 0, sizeof(tempbuf));
        Data.from_rpc(value_struct(*it_struct));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.name.c_str(), Data.name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.owner.c_str(), Data.owner.size());
        AddMapParaToMsg(pCliMsg, IMAGE_YES_OR_NO_MAP, Data.lock);
        if (Data.size < 1024)
        {
            sprintf(tempbuf, "%lu B", Data.size);
        }
        else if (Data.size < 1024*1024)
        {
            sprintf(tempbuf, "%.0f KB", BytesToKB(Data.size));
        }
        else if (Data.size < 1024*1024*1024)
        {
            sprintf(tempbuf, "%.1f MB", BytesToMB(Data.size));
        }
        else
        {
           sprintf(tempbuf, "%.2f GB", BytesToGB(Data.size)); 
        }
        temp = tempbuf;
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)temp.c_str(), temp.size());        
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.stored_time.c_str(), Data.stored_time.size());     
    }    
    return;
}

/* 虚拟机查询输出 */
void vm_show_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
    
    /* 因为下面这个VECTOR就一个成员，所以不需要压Vector数据，就读取一个成员就好了 */
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiVmCfgInfo Data;
        int i,j;
        map<string,string>::iterator it_context;
        
        Data.from_rpc(value_struct(*it_struct));
        
        float temp;
        /* base info */
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.base_info.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.vm_name.c_str(), Data.vm_name.size());
        AddMapParaToMsg(pCliMsg, VM_MANAGER_VM_STATE_MAP, Data.state);    

        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)Data.base_info.user_name.c_str(), Data.base_info.user_name.size()); 
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.project_name.c_str(), Data.project_name.size());
        
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.cluster_name.c_str(), Data.cluster_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.hc_name.c_str(), Data.hc_name.size());        
          
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.create_time.c_str(), Data.create_time.size());
        
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.base_info.vcpu, sizeof(int)); 
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.base_info.tcu, sizeof(int)); 
        temp = BytesToMB(Data.base_info.memory);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));

        if ( Data.vnc_ip.empty() )
        {
            Data.vnc_ip = "0.0.0.0";
        }
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.vnc_ip.c_str(), Data.vnc_ip.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&Data.vnc_port, sizeof(int));
      
        /* 传递 machine image 的信息 */
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE*)&Data.base_info.machine.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.machine.bus.c_str(), Data.base_info.machine.bus.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.machine.target.c_str(), Data.base_info.machine.target.size());

        /* 传给xen的参数 */
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.root_device.c_str(), Data.base_info.root_device.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.kernel_command.c_str(), Data.base_info.kernel_command.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.bootloader.c_str(), Data.base_info.bootloader.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.virt_type.c_str(), Data.base_info.virt_type.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.rawdata.c_str(), Data.base_info.rawdata.size());
        j = Data.base_info.context.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (it_context = Data.base_info.context.begin(); it_context != Data.base_info.context.end(); it_context++)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)it_context->first.c_str(), it_context->first.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)it_context->second.c_str(), it_context->second.size());
        }
        
        /* 虚拟机调度策略 --  物理机应该满足的表达式条件 */
        j = Data.base_info.qualifications.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (i = 0; i < j; i++)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.qualifications.at(i).key.c_str(), Data.base_info.qualifications.at(i).key.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.qualifications.at(i).op.c_str(), Data.base_info.qualifications.at(i).op.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.qualifications.at(i).value.c_str(), Data.base_info.qualifications.at(i).value.size());
        } 

        /* 虚拟机磁盘列表 */
        j = Data.base_info.disks.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (i = 0; i < j; i++)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.base_info.disks.at(i).id, sizeof(WORD64));
            temp = BytesToGB(Data.base_info.disks.at(i).size);
            AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.disks.at(i).bus.c_str(), Data.base_info.disks.at(i).bus.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.disks.at(i).type.c_str(), Data.base_info.disks.at(i).type.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.disks.at(i).target.c_str(), Data.base_info.disks.at(i).target.size());
        }        

        /* 虚拟网卡 */
        j = Data.base_info.nics.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (i = 0; i < j; i++)
        {
            AddMapParaToMsg(pCliMsg, VM_MANAGER_SRIOV_MAP, Data.base_info.nics.at(i).sriov);
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.base_info.nics.at(i).vlan, sizeof(int));
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).plane.c_str(), Data.base_info.nics.at(i).plane.size());
            AddMapParaToMsg(pCliMsg, VM_MANAGER_DHCP_STATE_MAP, Data.base_info.nics.at(i).dhcp);
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).ip.c_str(), Data.base_info.nics.at(i).ip.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).netmask.c_str(), Data.base_info.nics.at(i).netmask.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).gateway.c_str(), Data.base_info.nics.at(i).gateway.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).mac.c_str(), Data.base_info.nics.at(i).mac.size());
        }       
    }   
    return;
}

/* 虚拟机模板查询输出 */
void vt_show_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
  
    /* 因为下面这个VECTOR就一个成员，所以不需要压Vector数据，就读取一个成员就好了 */
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiVtInfo Data;
        int i,j;
        map<string,string>::iterator it_context;
        float temp;
        
        Data.from_rpc(value_struct(*it_struct));
        
        /* base info */
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.vt_name.c_str(), Data.vt_name.size());
        AddMapParaToMsg(pCliMsg, VT_MANAGER_T_OR_FALSE_MAP, Data.is_public);
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.base_info.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)Data.base_info.user_name.c_str(), Data.base_info.user_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.base_info.vcpu, sizeof(int));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.base_info.tcu, sizeof(int));
        temp = BytesToMB(Data.base_info.memory);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE*)&Data.base_info.machine.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.machine.bus.c_str(), Data.base_info.machine.bus.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.machine.target.c_str(), Data.base_info.machine.target.size());

        
        /* 传给xen的参数 */
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.root_device.c_str(), Data.base_info.root_device.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.kernel_command.c_str(), Data.base_info.kernel_command.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.bootloader.c_str(), Data.base_info.bootloader.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.virt_type.c_str(), Data.base_info.virt_type.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.rawdata.c_str(), Data.base_info.rawdata.size());
        j = Data.base_info.context.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (it_context = Data.base_info.context.begin(); it_context != Data.base_info.context.end(); it_context++)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)it_context->first.c_str(), it_context->first.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)it_context->second.c_str(), it_context->second.size());
        }
        
        /* 虚拟机调度策略 --  物理机应该满足的表达式条件 */
        j = Data.base_info.qualifications.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (i = 0; i < j; i++)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.qualifications.at(i).key.c_str(), Data.base_info.qualifications.at(i).key.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.qualifications.at(i).op.c_str(), Data.base_info.qualifications.at(i).op.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.qualifications.at(i).value.c_str(), Data.base_info.qualifications.at(i).value.size());
        } 

        /* 虚拟机磁盘列表 */
        j = Data.base_info.disks.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (i = 0; i < j; i++)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.base_info.disks.at(i).id, sizeof(WORD64));
            temp = BytesToGB(Data.base_info.disks.at(i).size);
            AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.disks.at(i).bus.c_str(), Data.base_info.disks.at(i).bus.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.disks.at(i).type.c_str(), Data.base_info.disks.at(i).type.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.disks.at(i).target.c_str(), Data.base_info.disks.at(i).target.size());
        }        

        /* 虚拟网卡 */
        j = Data.base_info.nics.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&j, sizeof(int));
        for (i = 0; i < j; i++)
        {
            AddMapParaToMsg(pCliMsg, VT_MANAGER_SRIOV_MAP, Data.base_info.nics.at(i).sriov );
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&Data.base_info.nics.at(i).vlan, sizeof(int));
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).plane.c_str(), Data.base_info.nics.at(i).plane.size());
            AddMapParaToMsg(pCliMsg, VT_MANAGER_DHCP_STATE_MAP, Data.base_info.nics.at(i).dhcp);
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).ip.c_str(), Data.base_info.nics.at(i).ip.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).netmask.c_str(), Data.base_info.nics.at(i).netmask.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).gateway.c_str(), Data.base_info.nics.at(i).gateway.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.base_info.nics.at(i).mac.c_str(), Data.base_info.nics.at(i).mac.size());
        }        
    }    
    return;
}

/* 虚拟机工程创建输出，主要解决NO命令和正常命令返回格式不一致的情况 */
void project_create_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /* 如果是no命令，则只返回一个错误码，错误码已经加CLImsg中去了 */
    if (pCliMsg->bIfNo == true)
    {
        return;
    }
    
    /* 如果不是no命令，那么把附加信息带上传到CLI上 */
    string strTmp = xmlrpc_c::value_string(rs[1]);
    AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());        
    
}


/* 虚拟机工程查询输出 */
void project_show_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
    
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    
    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiProjectInfo Data;
        
        Data.from_rpc(value_struct(*it_struct));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.id, sizeof(WORD64));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)Data.uname.c_str(), Data.uname.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.name.c_str(), Data.name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.des.c_str(), Data.des.size());    
    }    
    return;
}

/* 虚拟机查询输出 */
void project_vm_show_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }    
    
    /* 因为下面这个VECTOR就一个成员，所以不需要压Vector数据，就读取一个成员就好了 */
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    iTmp = array_struct.size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiProShVmInfo Data;
        map<string,string>::iterator it_context;
        
        Data.from_rpc(value_struct(*it_struct));
        
        /* base info */
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data.vm_name.c_str(), Data.vm_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&Data.vm_id, sizeof(WORD64));
    }    
    return;
}



/*****************************************************************************/
/* 用户管理输出*/
void user_info_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{    
        int iTmp = 0;
    
    /*1、设置消息返回码*/
    //pCliMsg->Number = 0;
    //pCliMsg->DataLen = 0;
    //pCliMsg->ReturnCode = SUCC_AND_HAVEPARA;//应用根据执行结果自行赋值，成功或失败
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    iTmp = xmlrpc_c::value_int(rs[0]);
    
    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }          
    
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    iTmp = 0;
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        iTmp ++;
    }
    
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    it_struct = array_struct.begin();
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        map<string, xmlrpc_c::value> p = value_struct(*it_struct); 
        map<string, xmlrpc_c::value>::iterator it;
            
        if(p.end() != (it = p.find("username")))
        {
             string username = xmlrpc_c::value_string(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)username.c_str(), username.size());                     
        }
        /*if(p.end() != (it = p.find("password")))
        {
             string pass = xmlrpc_c::value_string(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)pass.c_str(), pass.size());                                          
        }*/        
        if(p.end() != (it = p.find("enabled")))
        {
               int enabled = xmlrpc_c::value_int(it->second);
               AddMapParaToMsg(pCliMsg, USER_ENABLE_MAP, enabled); 
        }
        if(p.end() != (it = p.find("role")))
        {
               int role = xmlrpc_c::value_int(it->second);
             AddMapParaToMsg(pCliMsg, USER_ROLE_MAP, role);                   
        }                
        if(p.end() != (it = p.find("group")))
        {
             //int group = xmlrpc_c::value_int(it->second);
             string groupname = xmlrpc_c::value_string(it->second);    
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)groupname.c_str(), groupname.size());               
        }        
        if(p.end() != (it = p.find("phone")))
        {
               string phone = xmlrpc_c::value_string(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)phone.c_str(), phone.size());                                            
        }        
        if(p.end() != (it = p.find("email")))
        {
               string email = xmlrpc_c::value_string(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)email.c_str(), email.size());                                            
        }        
        if(p.end() != (it = p.find("address")))
        {
               string addr = xmlrpc_c::value_string(it->second);    
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)addr.c_str(), addr.size());                                        
        }        
    }   
}

void usergroup_info_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
        int iTmp = 0;
    
    /*1、设置消息返回码*/
    //pCliMsg->Number = 0;
    //pCliMsg->DataLen = 0;
    //pCliMsg->ReturnCode = SUCC_AND_HAVEPARA;//应用根据执行结果自行赋值，成功或失败
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    iTmp = xmlrpc_c::value_int(rs[0]);
    
    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
    
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }          
    
    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    iTmp = 0;
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        iTmp ++;
    }
    
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    it_struct = array_struct.begin();
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        map<string, xmlrpc_c::value> p = value_struct(*it_struct); 
        map<string, xmlrpc_c::value>::iterator it;
            
        if(p.end() != (it = p.find("groupname")))
        {
             string groupname = xmlrpc_c::value_string(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)groupname.c_str(), groupname.size());                     
        }
        if(p.end() != (it = p.find("grouptype")))
        {
             int type = xmlrpc_c::value_int(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&type, sizeof(int));                                          
        }        
        if(p.end() != (it = p.find("enabled")))
        {
             int enabled = xmlrpc_c::value_int(it->second);
             AddMapParaToMsg(pCliMsg, USER_ENABLE_MAP, enabled); 
        }
        if(p.end() != (it = p.find("description")))
        {
             string desc = xmlrpc_c::value_string(it->second);
             AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)desc.c_str(), desc.size());                     
        }        
    }   
}

#if 0
void host_command_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    string s = xmlrpc_c::value_string(rs[1]);
    AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)s.c_str(), s.size());    

    int temp = 1;
    if (0 == xmlrpc_c::value_int(rs[0]))
    {
        temp = 0;
    }
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&temp, sizeof(temp)); 
}
#endif

void host_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp;
    if (0 != xmlrpc_c::value_int(rs[0]))
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        return;
    }
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }

    vector<xmlrpc_c::value> array = xmlrpc_c::value_array(rs[3]).vectorValueValue();
    int num = array.size();
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
    
    vector<xmlrpc_c::value>::iterator it;    
    for (it = array.begin(); it != array.end(); ++it)
    {
        ApiHostInfo h;
        h.from_rpc(value_struct(*it));        
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&h.oid, sizeof(h.oid));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)h.host_name.c_str(), h.host_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)h.ip_address.c_str(), h.ip_address.size());
        AddMapParaToMsg(pCliMsg, HOST_REGISTER_STATE_MAP , h.register_state);
        AddMapParaToMsg(pCliMsg, HOST_RUN_STATE_MAP, h.run_state);
        AddMapParaToMsg(pCliMsg, HOST_IS_DISABLED_MAP, h.is_disabled);
        AddParaToMsg(pCliMsg, DATA_TYPE_TIME, (BYTE *)&h.create_time, sizeof(h.create_time));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&h.cpu_info_id, sizeof(h.cpu_info_id));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)h.vmm_info.c_str(), h.vmm_info.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)h.os_info.c_str(), h.os_info.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&h.running_vms, sizeof(h.running_vms));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&h.tcu_max, sizeof(h.tcu_max));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&h.tcu_free, sizeof(h.tcu_free));

        float temp;
        temp = BytesToMB(h.mem_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToMB(h.mem_max);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToMB(h.mem_free);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToGB(h.disk_total);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToGB(h.disk_max);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        temp = BytesToGB(h.disk_free);
        AddParaToMsg(pCliMsg, DATA_TYPE_FLOAT, (BYTE *)&temp, sizeof(temp));
        
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&h.cpu_usage_1m, sizeof(h.cpu_usage_1m));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&h.cpu_usage_5m, sizeof(h.cpu_usage_5m));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&h.cpu_usage_30m, sizeof(h.cpu_usage_30m));
        AddMapParaToMsg(pCliMsg, HOST_HARDWARE_STATE_MAP, h.hardware_state);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)h.description.c_str(), h.description.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)h.location.c_str(), h.location.size());
        AddMapParaToMsg(pCliMsg, HOST_IS_SUPPORT_HVM_MAP, h.is_support_hvm);

    }    
}

void port_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int num = 0;
    if (0 != xmlrpc_c::value_int(rs[0]))
    {
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
        return;
    }   

    vector<xmlrpc_c::value> array = xmlrpc_c::value_array(rs[1]).vectorValueValue();
    num = array.size();
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
    
    vector<xmlrpc_c::value>::iterator it;    
    for (it = array.begin(); it != array.end(); ++it)
    {
        ApiPortInfo p;
        p.from_rpc(value_struct(*it));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)p.name.c_str(), p.name.size());
        AddMapParaToMsg(pCliMsg, HOST_PORT_USED_MAP , p.used);
        AddMapParaToMsg(pCliMsg, HOST_PORT_NEGOTITATE_MAP, p.negotiate);        
        AddMapParaToMsg(pCliMsg, HOST_PORT_SPEED_MAP, p.speed);
        AddMapParaToMsg(pCliMsg, HOST_PORT_DUPLEX_MAP, p.duplex);
        AddMapParaToMsg(pCliMsg, HOST_PORT_MASTER_MAP, p.master);
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&p.delay_up_time, sizeof(p.delay_up_time));
        AddMapParaToMsg(pCliMsg, HOST_PORT_STATE_MAP, p.state);
        AddMapParaToMsg(pCliMsg, HOST_PORT_LINKED_MAP, p.linked); // TODO add by xuemzh
        AddMapParaToMsg(pCliMsg, HOST_PORT_SPEED_MAP, p.work_speed);
        AddMapParaToMsg(pCliMsg, HOST_PORT_DUPLEX_MAP, p.work_duplex);
    }
}

void trunk_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int num = 0;
    if (0 != xmlrpc_c::value_int(rs[0]))
    {
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
        return;
    }
    
    vector<xmlrpc_c::value> array = xmlrpc_c::value_array(rs[1]).vectorValueValue();
    num = array.size();
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
    
    vector<xmlrpc_c::value>::iterator it;    
    for (it = array.begin(); it != array.end(); ++it)
    {
        ApiTrunkInfo t;
        t.from_rpc(value_struct(*it));    

        string port_names = "";
        vector<string>::const_iterator it_port = t.trunk_ports.begin();
        for (; it_port != t.trunk_ports.end(); ++it_port)
        {
            port_names += *it_port;
            port_names += " ";
        }
        
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)t.trunk_name.c_str(), t.trunk_name.size());
        AddMapParaToMsg(pCliMsg, HOST_TRUNK_TYPE_MAP, t.trunk_type);
        AddMapParaToMsg(pCliMsg, HOST_PORT_STATE_MAP, t.trunk_state); 
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)port_names.c_str(), port_names.size());
    }
}

void clustercfg_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int num = 0;
    if (0 != xmlrpc_c::value_int(rs[0]))
    {
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
        return;
    }
    
    vector<xmlrpc_c::value> array = xmlrpc_c::value_array(rs[1]).vectorValueValue();
    num = array.size();
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
    
    vector<xmlrpc_c::value>::iterator it;    
    for (it = array.begin(); it != array.end(); ++it)
    {
        ApiClusterConfigInfo c;
        c.from_rpc(value_struct(*it));     
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)c.config_name.c_str(), c.config_name.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)c.config_value.c_str(), c.config_value.size());
    }
}

void svcpu_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp;
    if (0 != xmlrpc_c::value_int(rs[0]))
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        return;
    }
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }

    vector<xmlrpc_c::value> array = xmlrpc_c::value_array(rs[3]).vectorValueValue();
    int num = array.size();
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
    
    vector<xmlrpc_c::value>::iterator it;    
    for (it = array.begin(); it != array.end(); ++it)
    {
        ApiSvcpuInfo s;
        s.from_rpc(value_struct(*it));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&s.cpu_info_id, sizeof(s.cpu_info_id));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&s.tcu_num, sizeof(s.tcu_num));    
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&s.commend_tcu_num, sizeof(s.commend_tcu_num));   
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)s.cpu_info.c_str(), s.cpu_info.size());
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)s.description.c_str(), s.description.size());
    }
}

#if 0
void cpuinfo_query_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int num = 0;
    if (0 != xmlrpc_c::value_int(rs[0]))
    {
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
        return;
    }    
    
    vector<xmlrpc_c::value> array = xmlrpc_c::value_array(rs[1]).vectorValueValue();
    num = array.size();
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&num, sizeof(int));
    
    vector<xmlrpc_c::value>::iterator it;    
    for (it = array.begin(); it != array.end(); ++it)
    {
        ApiCpuInfo c;
        c.from_rpc(value_struct(*it));
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&c.cpu_info_id, sizeof(c.cpu_info_id));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)c.cpu_info.c_str(), c.cpu_info.size());
    }
}
#endif
/* 版本查询入向钩子 */
//lixiaocheng

void version_query_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    string   tmp_str;
    ApiVersionReq Data;
    xmlrpc_c::paramList   listcc;
    xmlrpc_c::paramList   listhc;
    xmlrpc_c::paramList   listrepo;



    /* modify_data._type */
    vParaSet.push_back(0);
    if (true == AddDataToXmlParamList(pCliMsg, listcc, vParaSet))
    {
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "query";
        Data._targe_type ="cc";
        Data._targe_postion = "";
        Data._targe_name.push_back (xmlrpc_c::value_string(listcc.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(1);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(listcc.getString(0)));
        }
        else
        {
//            Data._package_name = "*";
        }
    }

    vParaSet.clear();//首先清空前一个参数的内容
    vParaSet.push_back(2);
    if (true == AddDataToXmlParamList(pCliMsg, listhc, vParaSet))
    {
        xmlrpc_c::paramList   listname;
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "query";
        Data._targe_type ="hc";
        Data._targe_postion = xmlrpc_c::value_string(listhc.getString(0));


        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listname, vParaSet);
        Data._targe_name.push_back (xmlrpc_c::value_string(listname.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(4);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
       //     Data._package_name = "*";
        }
    }

    vParaSet.clear();//首先清空前一个参数的内容
    vParaSet.push_back(5);
    if (true == AddDataToXmlParamList(pCliMsg, listrepo, vParaSet))
    {
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "query";
        Data._targe_type ="repo";
        Data._targe_postion = "";
        Data._targe_name.push_back (xmlrpc_c::value_string(listrepo.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(6);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
       //     Data._package_name = "*";
        }
    }

    list.add(Data.to_rpc());

    return;
}

void version_install_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    string   tmp_str;
    ApiVersionReq Data;
    xmlrpc_c::paramList   listcc;
    xmlrpc_c::paramList   listhc;
    xmlrpc_c::paramList   listrepo;



    /* modify_data._type */
    vParaSet.push_back(0);
    if (true == AddDataToXmlParamList(pCliMsg, listcc, vParaSet))
    {
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "install";
        Data._targe_type ="cc";
        Data._targe_postion = "";
        Data._targe_name.push_back (xmlrpc_c::value_string(listcc.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(1);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
       //     Data._package_name = "*";
        }
    }

    vParaSet.clear();//首先清空前一个参数的内容
    vParaSet.push_back(2);
    if (true == AddDataToXmlParamList(pCliMsg, listhc, vParaSet))
    {
        xmlrpc_c::paramList   listtemp1;
        xmlrpc_c::paramList   listname;
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "install";
        Data._targe_type ="hc";
        Data._targe_postion = xmlrpc_c::value_string(listhc.getString(0));


        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listname, vParaSet);
        Data._targe_name.push_back (xmlrpc_c::value_string(listname.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(4);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
       //     Data._package_name = "*";
        }
    }

    list.add(Data.to_rpc());

    return;
}

void version_uninstall_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    string   tmp_str;
    ApiVersionReq Data;
    xmlrpc_c::paramList   listcc;
    xmlrpc_c::paramList   listhc;
    xmlrpc_c::paramList   listrepo;



    /* modify_data._type */
    vParaSet.push_back(0);
    if (true == AddDataToXmlParamList(pCliMsg, listcc, vParaSet))
    {
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "uninstall";
        Data._targe_type ="cc";
        Data._targe_postion = "";
        Data._targe_name.push_back (xmlrpc_c::value_string(listcc.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(1);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
//            Data._package_name = "*";
        }
    }

    vParaSet.clear();//首先清空前一个参数的内容
    vParaSet.push_back(2);
    if (true == AddDataToXmlParamList(pCliMsg, listhc, vParaSet))
    {
        xmlrpc_c::paramList   listtemp1;
        xmlrpc_c::paramList   listname;
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "uninstall";
        Data._targe_type ="hc";
        Data._targe_postion = xmlrpc_c::value_string(listhc.getString(0));


        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listname, vParaSet);
        Data._targe_name.push_back (xmlrpc_c::value_string(listname.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(4);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
//            Data._package_name = "*";
        }
    }

    list.add(Data.to_rpc());

    return;
}

void version_update_in(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &list)
{
    PARASET vParaSet;
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    string   tmp_str;
    ApiVersionReq Data;
    xmlrpc_c::paramList   listcc;
    xmlrpc_c::paramList   listhc;
    xmlrpc_c::paramList   listrepo;



    /* modify_data._type */
    vParaSet.push_back(0);
    if (true == AddDataToXmlParamList(pCliMsg, listcc, vParaSet))
    {
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "update";
        Data._targe_type ="cc";
        Data._targe_postion = "";
        Data._targe_name.push_back (xmlrpc_c::value_string(listcc.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(1);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
//            Data._package_name = "*";
        }
    }

    vParaSet.clear();//首先清空前一个参数的内容
    vParaSet.push_back(2);
    if (true == AddDataToXmlParamList(pCliMsg, listhc, vParaSet))
    {
        xmlrpc_c::paramList   listtemp1;
        xmlrpc_c::paramList   listname;
        xmlrpc_c::paramList   list_pakage_name;
        Data._opt_type = "update";
        Data._targe_type ="hc";
        Data._targe_postion = xmlrpc_c::value_string(listhc.getString(0));


        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(3);
        AddDataToXmlParamList(pCliMsg, listname, vParaSet);
        Data._targe_name.push_back (xmlrpc_c::value_string(listname.getString(0)));

        vParaSet.clear();//首先清空前一个参数的内容
        vParaSet.push_back(4);
        if (true == AddDataToXmlParamList(pCliMsg, list_pakage_name, vParaSet))
        {
            Data._package_name.push_back (xmlrpc_c::value_string(list_pakage_name.getString(0)));
        }
        else
        {
//            Data._package_name = "*";
        }
    }

    list.add(Data.to_rpc());

    return;
}


/*版本管理查询 */
//lixiaocheng

void version_getresult_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    //根据xmlrpc返回的数组value，生成和回显脚本匹配的cli结构   
    
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /*例如下面是回显脚本为 %v%u  %v%s  %v%w 的数据转换*/
    /*也可以在转换过程中进行业务逻辑的判断处理*/
    int iTmp = xmlrpc_c::value_int(rs[0]);
    
    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

        string strTmp = xmlrpc_c::value_string(rs[1]);
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
        return;
    }
          
    /* 写入第2个参数当前是否是第一包,如果是第一包，在报文中传个TRUE过去 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }

    /* 下面是返回正确的时候返回的打印 */
    vector<xmlrpc_c::value> array_struct = xmlrpc_c::value_array(rs[3]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();

    int iTmp1 = array_struct.size();

    /* 写入第3个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp1, sizeof(int));
       
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        ApiPackageVerInfo Data;
        
        Data.from_rpc(value_struct(*it_struct));
        

        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)Data._target_name.c_str(), Data._target_name.size());
        if (Data._result == 0)
        {
            string temp = "Success";
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)temp.c_str(), temp.size());
        }
        else
        {
            string temp = "Unkown";
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)temp.c_str(), temp.size());
        }

        int packnum, i;

        packnum = Data._info.size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&packnum, sizeof(int));
        for (i = 0; i < packnum; i++)
        {
            string packname = Data._info[i]._package_name;
            string packver = Data._info[i]._package_ver;
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)packname.c_str(), packname.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)packver.c_str(), packver.size());
        }
    }    
    return;
}



///////////////////////////////////////////////////////////////////////////////
///   虚拟网络管理  CLI 回调处理
/////////////////////////////////////////////////////////////////////////////// 
/* 虚拟网络管理MAP */
#define VM_VNET_EVB_MOD_TYPE_MAP    1101
#define VM_VNET_DHCP_STATE_MAP      1102
#define VM_VNET_NETWORK_MAP         1103


// VNET  CLI SHOW COMMAND CALL FUNCTION START
void vnet_show_net_plane_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /* 读取返回数据*/    
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        int iCtl = CLI_RETURN_ERR_FLAG;

        //pCliMsg->Number = 0;
        //pCliMsg->DataLen = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&iCtl, sizeof(iCtl));
        AddMapParaToMsg(pCliMsg, MAPTYPE_RETURNCODE, iTmp);

        return ;
    }

    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }

    VnetNetplaneList  m_cNetplaneList;
    m_cNetplaneList.from_rpc(xmlrpc_c::value_struct(rs[3]));    
    
    /* 下面是返回正确的时候返回的打印 */
    iTmp = m_cNetplaneList.GetData().size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

    /* 将获取的结果返回 */
    vector <VnetNetplane>::iterator itrVecNPData = m_cNetplaneList.GetData().begin();
    for( ; itrVecNPData != m_cNetplaneList.GetData().end(); ++itrVecNPData)
    {
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&itrVecNPData->netplane_id, sizeof(itrVecNPData->netplane_id));
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)itrVecNPData->netplane_name.c_str(), itrVecNPData->netplane_name.size());
    }
    
    return ;
}
void vnet_show_net_plane_map_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /* 读取返回数据*/    
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        int iCtl = CLI_RETURN_ERR_FLAG;

        //pCliMsg->Number = 0;
        //pCliMsg->DataLen = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&iCtl, sizeof(iCtl));
        AddMapParaToMsg(pCliMsg, MAPTYPE_RETURNCODE, iTmp);

        return ;
    }

    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }

    VnetNetplaneMapLists cListDatas; 
    cListDatas.from_rpc(xmlrpc_c::value_struct(rs[3]));    

    /* 下面是返回正确的时候返回的打印 */
    iTmp = cListDatas.GetData().size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

    vector <VnetNetplaneMapList>::iterator itrListDatas = cListDatas.GetData().begin();
    for( ; itrListDatas != cListDatas.GetData().end(); ++itrListDatas)
    {
        // host name
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)itrListDatas->host_name.c_str(), itrListDatas->host_name.size());

        iTmp = itrListDatas->GetData().size();
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        /* 将获取的结果返回 */
        vector <VnetNetplaneMap>::iterator itrListData = itrListDatas->GetData().begin();
        for( ; itrListData != itrListDatas->GetData().end(); ++itrListData)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)itrListData->phynic_name.c_str(), itrListData->phynic_name.size());        
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)itrListData->netplane_name.c_str(), itrListData->netplane_name.size());        
            // AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&itrListData->evb_mod, sizeof(itrListData->evb_mod));
            AddMapParaToMsg(pCliMsg, DATA_TYPE_INT, itrListData->priority);
        }
    }
    
    return ;
}

void vnet_show_vlan_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /* 读取返回数据*/    
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp = xmlrpc_c::value_int(rs[0]);
    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        int iCtl = CLI_RETURN_ERR_FLAG;

        //pCliMsg->Number = 0;
        //pCliMsg->DataLen = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&iCtl, sizeof(iCtl));
        AddMapParaToMsg(pCliMsg, MAPTYPE_RETURNCODE, iTmp);
        return ;
    }

    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));     
    }

    VnetVlanMapList cVlanListData;
    cVlanListData.from_rpc(xmlrpc_c::value_struct(rs[3]));    

    // netplane_name
    AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)cVlanListData.netplane_name.c_str(), cVlanListData.netplane_name.size());

    /* 下面是返回正确的时候返回的打印 */
    iTmp = cVlanListData.GetData().size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

    /* 将获取的结果返回 */
    vector <VnetVlanMap>::iterator itrListData = cVlanListData.GetData().begin();
    for( ; itrListData != cVlanListData.GetData().end(); ++itrListData)
    {
        WORD64 qdwTmp = (WORD64)itrListData->project_id;
        AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&qdwTmp, sizeof(qdwTmp));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrListData->user_vid, sizeof(itrListData->user_vid));        
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&itrListData->cvid, sizeof(itrListData->cvid));
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&itrListData->svid, sizeof(itrListData->svid));
    }
    
    return ;
}

void vnet_show_dhcp_ip_mac_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /* 读取返回数据*/    
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        int iCtl = CLI_RETURN_ERR_FLAG;

        //pCliMsg->Number = 0;
        //pCliMsg->DataLen = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&iCtl, sizeof(iCtl));
        AddMapParaToMsg(pCliMsg, MAPTYPE_RETURNCODE, iTmp);

        return ;
    }

    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }

    VnetIpMacList cIpMacListData;
    cIpMacListData.from_rpc(xmlrpc_c::value_struct(rs[3]));    

    /* 下面是返回正确的时候返回的打印 */
    iTmp = cIpMacListData.GetData().size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

    /* 将获取的结果返回 */
    vector <VnetIpMac>::iterator itrListData = cIpMacListData.GetData().begin();
    for( ; itrListData != cIpMacListData.GetData().end(); ++itrListData)
    {
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrListData->vm_ip, sizeof(itrListData->vm_ip));        
        AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)itrListData->vm_mac.c_str(), itrListData->vm_mac.size());        
    }
    
    return ;
}

void vnet_show_dhcp_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /* 读取返回数据*/    
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        int iCtl = CLI_RETURN_ERR_FLAG;

        pCliMsg->Number = 0;
        pCliMsg->DataLen = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&iCtl, sizeof(iCtl));
        AddMapParaToMsg(pCliMsg, MAPTYPE_RETURNCODE, iTmp);

        return ;
    }

    VnetDhcp cDhcpData;
    cDhcpData.from_rpc(xmlrpc_c::value_struct(rs[3]));    

    // shell_path
    // AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)cDhcpData.shell_path.c_str(), cDhcpData.shell_path.size());
    // user_name
    // AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)cDhcpData.user_name.c_str(), cDhcpData.user_name.size());
    // port_name
    AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)cDhcpData.port_name.c_str(), cDhcpData.port_name.size());

    AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE *)cDhcpData.pxe_file.c_str(), cDhcpData.pxe_file.size());

    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&cDhcpData.pxe_server_address, sizeof(cDhcpData.pxe_server_address));
 

    /* 下面是返回正确的时候返回的打印 */
    iTmp = cDhcpData.GetVlanData().size();
    /* 写入第2个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));

    /* 将获取的结果返回 */
    vector <VnetDhcpVlan>::iterator itrListData = cDhcpData.GetVlanData().begin();
    for( ; itrListData != cDhcpData.GetVlanData().end(); ++itrListData)
    {
        // AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrListData->cvid, sizeof(itrListData->cvid));         
        AddMapParaToMsg(pCliMsg, VM_VNET_NETWORK_MAP, itrListData->cvid);
        
        /* 下面是返回正确的时候返回的打印 */
        iTmp = itrListData->GetSubnetData().size();
        /* 写入第2个参数（数组的数量） */
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        vector<VnetDhcpSubnet>::iterator itrVecSubnet = itrListData->GetSubnetData().begin();
        for( ; itrVecSubnet != itrListData->GetSubnetData().end(); ++itrVecSubnet)
        {            
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecSubnet->subnet, sizeof(itrVecSubnet->subnet)); 
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecSubnet->subnet_mask, sizeof(itrVecSubnet->subnet_mask));
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecSubnet->gateway, sizeof(itrVecSubnet->gateway));
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecSubnet->client_mask, sizeof(itrVecSubnet->client_mask)); 
            
            /* 下面是返回正确的时候返回的打印 */
            iTmp = itrVecSubnet->GetRangeData().size();
            /* 写入第2个参数（数组的数量） */
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
            
            vector<VnetDhcpRange>::iterator itrVecRng = itrVecSubnet->GetRangeData().begin();
            for( ; itrVecRng != itrVecSubnet->GetRangeData().end(); ++itrVecRng)
            {
                AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecRng->ip_start, sizeof(itrVecRng->ip_start));
                AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecRng->ip_end, sizeof(itrVecRng->ip_end)); 
            }
        }
        
        /* 下面是返回正确的时候返回的打印 */
        iTmp = itrListData->GetHostData().size();
        /* 写入第2个参数（数组的数量） */
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        vector<VnetDhcpHost>::iterator itrVecHost = itrListData->GetHostData().begin();
        for( ; itrVecHost != itrListData->GetHostData().end(); ++itrVecHost)
        {
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecHost->vm_ip, sizeof(itrVecHost->vm_ip));
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)itrVecHost->vm_mac.c_str(), itrVecHost->vm_mac.size());
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecHost->gateway, sizeof(itrVecHost->gateway));
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&itrVecHost->client_mask, sizeof(itrVecHost->client_mask)); 
        }        
    }
    
    return ;
}


void vnet_show_dhcp_state_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    /* 读取返回数据*/    
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    int iTmp = xmlrpc_c::value_int(rs[0]);

    /* 错误返回的时候提取的内容 */
    if (iTmp != 0)
    {
        int iCtl = CLI_RETURN_ERR_FLAG;

        pCliMsg->Number = 0;
        pCliMsg->DataLen = 0;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&iCtl, sizeof(iCtl));
        AddMapParaToMsg(pCliMsg, MAPTYPE_RETURNCODE, iTmp);

        return ;
    }

    iTmp = xmlrpc_c::value_int(rs[3]);
    AddMapParaToMsg(pCliMsg, VM_VNET_DHCP_STATE_MAP, iTmp);

    return ;
}
// VNET  CLI SHOW COMMAND CALL FUNCTION END

void alarm_show_current_alarm_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    int iTmp = 0;
    
    /*1、设置消息返回码*/
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/    
    /* 错误返回的时候提取的内容 */
    if (xmlrpc_c::value_int(rs[0]) != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        iTmp = xmlrpc_c::value_int(rs[0]);
        AddMapParaToMsg(pCliMsg, ALARM_QUERY_MAP, iTmp);
        return;
    }
    
    /* 下面是返回正确的时候返回的打印 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }
    
    vector<xmlrpc_c::value> array_struct = value_array(rs[4]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    iTmp = xmlrpc_c::value_int(rs[3]);
    
    /* 写入第3个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    it_struct = array_struct.begin();
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        map<string, xmlrpc_c::value> p = value_struct(*it_struct); 
        map<string, xmlrpc_c::value>::iterator it;
            
        if(p.end() != (it = p.find("alarmcode")))
        {
            unsigned int alarmcode = xmlrpc_c::value_int(it->second);
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&alarmcode, sizeof(unsigned int));                     
        }
        if(p.end() != (it = p.find("alarmtime")))
        {
            string alarmtime = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)alarmtime.c_str(), alarmtime.size());                
        }
        if(p.end() != (it = p.find("alarmaddr")))
        {
            string alarmaddr = xmlrpc_c::value_string(it->second);	
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)alarmaddr.c_str(), alarmaddr.size());        		
        }
        if(p.end() != (it = p.find("tcname")))
        {
            string tcname = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)tcname.c_str(), tcname.size());                
        }
        if(p.end() != (it = p.find("ccname")))
        {
            string ccname = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)ccname.c_str(), ccname.size());                
        }
        if(p.end() != (it = p.find("hcname")))
        {
            string hcname = xmlrpc_c::value_string(it->second);	
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)hcname.c_str(), hcname.size());        		
        }
        if(p.end() != (it = p.find("location")))
        {
            string location = xmlrpc_c::value_string(it->second);	
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)location.c_str(), location.size());        		
        }
        if(p.end() != (it = p.find("alarmaddinfo")))
        {
            string alarmaddinfo = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)alarmaddinfo.c_str(), alarmaddinfo.size());                                           
        }        
    }   
}

void alarm_show_history_alarm_out(xmlrpc_c::value value, MSG_COMM_OAM *pCliMsg)
{
    int iTmp = 0;
    
    /*2、读取返回数据*/
    xmlrpc_c::value_array const arrayValue(value);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    
    /*3、按照命令回显脚本从返回数据转成cli结构数据*/
    /* 错误返回的时候提取的内容 */
    if (xmlrpc_c::value_int(rs[0]) != 0)
    {
        iTmp = CLI_RETURN_ERR_FLAG;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
        
        iTmp = xmlrpc_c::value_int(rs[0]);
        AddMapParaToMsg(pCliMsg, ALARM_QUERY_MAP, iTmp);
        return;
    }
    
    /* 下面是返回正确的时候返回的打印 */
    if ((FIRST_PKG_FINISH == GetQueryReturnType(pCliMsg)) || (FIRST_PKG_NOTFINISH == GetQueryReturnType(pCliMsg)))
    {
        iTmp = CLI_RETURN_FIRST_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    }
    else
    {
        iTmp = CLI_RETURN_SECOND_PAGE;
        AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));      
    }
    
    vector<xmlrpc_c::value> array_struct = value_array(rs[4]).vectorValueValue();
    vector<xmlrpc_c::value>::iterator it_struct = array_struct.begin();
    iTmp = xmlrpc_c::value_int(rs[3]);
    
    /* 写入第3个参数（数组的数量） */
    AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
    
    it_struct = array_struct.begin();
    for (; it_struct != array_struct.end(); ++it_struct)
    {
        map<string, xmlrpc_c::value> p = value_struct(*it_struct); 
        map<string, xmlrpc_c::value>::iterator it;
            
        if(p.end() != (it = p.find("alarmcode")))
        {
            unsigned int alarmcode = xmlrpc_c::value_int(it->second);
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&alarmcode, sizeof(unsigned int));                     
        }
        if(p.end() != (it = p.find("alarmtime")))
        {
            string alarmtime = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)alarmtime.c_str(), alarmtime.size());                
        }
        if(p.end() != (it = p.find("restoretime")))
        {
            string restoretime = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)restoretime.c_str(), restoretime.size());                
        }
        if(p.end() != (it = p.find("restoretype")))
        {
            unsigned int restoretype = xmlrpc_c::value_int(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE*)&restoretype, sizeof(unsigned int));                
        }
        if(p.end() != (it = p.find("alarmaddr")))
        {
            string alarmaddr = xmlrpc_c::value_string(it->second);	
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)alarmaddr.c_str(), alarmaddr.size());        		
        }
        if(p.end() != (it = p.find("tcname")))
        {
            string tcname = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)tcname.c_str(), tcname.size());                
        }
        if(p.end() != (it = p.find("ccname")))
        {
            string ccname = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)ccname.c_str(), ccname.size());                
        }
        if(p.end() != (it = p.find("hcname")))
        {
            string hcname = xmlrpc_c::value_string(it->second);	
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)hcname.c_str(), hcname.size());        		
        }
        if(p.end() != (it = p.find("location")))
        {
            string location = xmlrpc_c::value_string(it->second);	
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)location.c_str(), location.size());        		
        }
        if(p.end() != (it = p.find("alarmaddinfo")))
        {
            string alarmaddinfo = xmlrpc_c::value_string(it->second);    
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)alarmaddinfo.c_str(), alarmaddinfo.size());                                           
        }        
    }   
}

/*****************************************************
                       钩子函数请在下面定义
****************************************************/


/*
预处理函数注册
*/
void LoadPreprocessFuncs()
{
    //入参预处理函数注册 
    g_map_ParaInFunc[0x82010058] = func_sample_in;
    g_map_ParaInFunc[0xB5010002] = image_enable_in;
    g_map_ParaInFunc[0xB5010003] = image_publish_in;
    g_map_ParaInFunc[0xB5010008] = image_register_in;
    g_map_ParaInFunc[0xB5010009] = image_set_in;
    g_map_ParaInFunc[0xB5000004] = image_show_in;
    g_map_ParaInFunc[0xB5000005] = image_file_show_in;
    g_map_ParaInFunc[0xAB040065] = host_register_in;
    g_map_ParaInFunc[0xAB040068] = host_disable_in;
    g_map_ParaInFunc[0xAB0100C9] = port_config_in;
    g_map_ParaInFunc[0xAB01012D] = trunk_config_in;
    g_map_ParaInFunc[0xAC0100C9] = svcpu_config_in;
    g_map_ParaInFunc[0xAC00012E] = svcpu_query_in;

    /* 
        project的创建和删除使用project_delete_in这一个钩子，
        这个钩子中有对no命令的判 断 
     */
    g_map_ParaInFunc[0xBF010001] = project_delete_in;
    g_map_ParaInFunc[0xBF000003] = project_show_in;
    g_map_ParaInFunc[0xBF000004] = project_vm_show_in;
    
    g_map_ParaInFunc[0xC9010001] = vmtemplate_manager_create_in;
    g_map_ParaInFunc[0xC9010002] = vmcfg_vt_disk_in;
    g_map_ParaInFunc[0xC9010003] = vmcfg_vt_Nic_in;
    g_map_ParaInFunc[0xC9010004] = vmcfg_vt_image_disk_in;
    g_map_ParaInFunc[0xC9000006] = vt_show_in;
    
    g_map_ParaInFunc[0xD3010001] = vmcfg_manager_in;
    g_map_ParaInFunc[0xD3010002] = vmcfg_vm_disk_in;
    g_map_ParaInFunc[0xD3010003] = vmcfg_vm_Nic_in;
    g_map_ParaInFunc[0xD3010004] = vmcfg_vm_image_disk_in;
    g_map_ParaInFunc[0xD3010005] = vmcfg_vm_create_indirect_in;
    g_map_ParaInFunc[0xD3010006] = vmcfg_vm_del_in;
    g_map_ParaInFunc[0xD3010007] = vmcfg_vm_operation_in;
    g_map_ParaInFunc[0xD3000008] = vm_show_in;
    g_map_ParaInFunc[0xD3010009] = vmcfg_set_relation_in;
    g_map_ParaInFunc[0xD301000A] = vmcfg_del_relation_in;
    g_map_ParaInFunc[0xD301000B] = vmcfg_relation_show_in;
    g_map_ParaInFunc[0xD301000C] = vm_live_migrate;
    g_map_ParaInFunc[0xE7010003] = version_query_in;
    g_map_ParaInFunc[0xe7010002] = version_install_in;
    g_map_ParaInFunc[0xe7010005] = version_update_in;
    g_map_ParaInFunc[0xe7010004] = version_uninstall_in;

    //出参预处理函数注册
    g_map_ParaOutFunc[0x82010058] = func_sample_out;
    g_map_ParaOutFunc[0xA1000004] = func_cluster_query_out;
    g_map_ParaOutFunc[0xB5000004] = image_query_out;
    g_map_ParaOutFunc[0xB5000005] = image_file_query_out;
    g_map_ParaOutFunc[0x97000003] = user_info_out;
    g_map_ParaOutFunc[0x98000003] = usergroup_info_out;    
//  g_map_ParaOutFunc[0xAB04006C] = host_command_out;    
    g_map_ParaOutFunc[0xAB000191] = host_query_out;
    g_map_ParaOutFunc[0xAB040192] = port_query_out;
    g_map_ParaOutFunc[0xAB040193] = trunk_query_out;
    g_map_ParaOutFunc[0xAC04012D] = clustercfg_query_out;
    g_map_ParaOutFunc[0xAC00012E] = svcpu_query_out;
//  g_map_ParaOutFunc[0xAC00012F] = cpuinfo_query_out;

    g_map_ParaOutFunc[0xBF010001] = project_create_out;
    g_map_ParaOutFunc[0xBF000003] = project_show_out;
    g_map_ParaOutFunc[0xBF000004] = project_vm_show_out;
    
    g_map_ParaOutFunc[0xC9000006] = vt_show_out;
    
    g_map_ParaOutFunc[0xD3000008] = vm_show_out;
    g_map_ParaOutFunc[0xD301000B] = vmcfg_relation_show_out;
    
    g_map_ParaOutFunc[0xE6000001] = vnet_show_dhcp_out;
    g_map_ParaOutFunc[0xE6000002] = vnet_show_net_plane_out;
    g_map_ParaOutFunc[0xE6000003] = vnet_show_net_plane_map_out;
    g_map_ParaOutFunc[0xE6000004] = vnet_show_vlan_out;
    g_map_ParaOutFunc[0xE6000005] = vnet_show_dhcp_ip_mac_out;
    g_map_ParaOutFunc[0xE6000006] = vnet_show_dhcp_state_out;
    
    g_map_ParaOutFunc[0xF1000001] = alarm_show_current_alarm_out;
    g_map_ParaOutFunc[0xF1000002] = alarm_show_history_alarm_out;

    g_map_ParaOutFunc[0xE7000001] = version_getresult_out;



}

/*
功能: 根据cli 输入的参数给xmlrpc的一个参数赋值
参数: pCliMsg   --入参，cli界面输入的参数
             vCliParas --入参，cli参数对应该xmlrpc参数的参数编号(界面会有一个或多个对应xmlrpc的一个参数，取第一个有效的输入)
             paramlist -- 出参，xmlrpc的参数列表
*/
BOOLEAN AddDataToXmlParamList(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &paramlist, PARASET vCliParas)
{
    OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
    int i;
    for (i = 0; i < (int)vCliParas.size(); i++)
    {
        //逐个给paramlist 中的参数赋值
        BOOLEAN bRet = GetParaByNo(pCliMsg, vCliParas[i], ptData);
        if (bRet)
        {
             AddDataToParamList(ptData, NULL, paramlist);
             return TRUE;
        }
    }

    return FALSE;
}
/*
功能：把输入的CLI命令参数转成xmlrpc远程方法需要的格式
参数：pCliMsg   -- 入参，cli输出的tlv结构，包含命令参数信息
      paramlist -- 出参，xmlrpc远程方法需要的参数集合
返回值：
      TRUE -- 该命令入参需要特殊处理
      FALSE -- 该命令入参不需要特殊处理
*/
bool ParamInPreProcess(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &paramlist)
{
       T_MAP_PARAIN_FUNC::iterator it = g_map_ParaInFunc.find(pCliMsg->CmdID);
    if (it == g_map_ParaInFunc.end())
    {
        return false;
    }
    
       it->second(pCliMsg, paramlist);
       
    return true;
}

/*
功能：把xmlrpc远程方法返回的数据转成CLI显示脚本匹配的TLV结构数据
参数：dwCmdId   -- 入参，命令id
                result     -- 入参，xmlrpc远程方法返回的参数数组
                pCliMsg   -- 出参，cli的tlv结构数据，包含命令参数信息
返回值：
      TRUE -- 该命令出参需要特殊处理
      FALSE -- 该命令出参不需要特殊处理
*/
bool ParamOutPreProcess(DWORD dwCmdId, xmlrpc_c::value result, MSG_COMM_OAM *pCliMsg)
{
       T_MAP_PARAOUT_FUNC::iterator it = g_map_ParaOutFunc.find(dwCmdId);
    if (it == g_map_ParaOutFunc.end())
    {
        return false;
    }
    
       //调用钩子函数
       it->second(result, pCliMsg);
       
    return true;
}

/*
查询命令的多包情况下，判断返回的数据包类型
具体返回值参见T_QueryRetType 的说明
*/
static T_QueryRetType GetQueryReturnType(MSG_COMM_OAM *pCliMsg)
{
    if (pCliMsg->CmdRestartFlag == TRUE)
    {
        if (pCliMsg->ReturnCode != SUCC_CMD_NOT_FINISHED)
        {
            return FIRST_PKG_FINISH;
        }
        else
        {
            return FIRST_PKG_NOTFINISH;
        }
    }
    else
    {
        if (pCliMsg->ReturnCode != SUCC_CMD_NOT_FINISHED)
        {
            return LAST_PKG_FINISH;
        }
        else
        {
            return MIDDLE_PKG_NOTFINISH;
        }
    }
}


