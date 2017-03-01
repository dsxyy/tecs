/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：api_vmbase.h
* 文件标识：
* 内容摘要：vmcfg和vmtemplate公共的api接口文件，一旦包含该文件将链接xml-rpc库 ! !
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2012年10月29日
*
* 修改记录1：
*    修改日期：2012/10/29
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#ifndef TECS_API_VMBASE_H
#define TECS_API_VMBASE_H
#include "rpcable.h"
#include "api_error.h"
#include "api_const.h"
#include "api_pub.h"
#include <boost/regex.hpp>

using namespace boost;

class  VmParaCheck
{
public:

    static bool VirtType(const string &str)
    {
        if ((VIRT_TYPE_HVM!= str)
            &&(VIRT_TYPE_PVM!= str))
        {
            return false;
        }        
        return true;
    };
    
    static bool DiskBusType(const string &str)
    {
        if ((DISK_BUS_IDE != str)
            &&(DISK_BUS_SCSI != str))
        {
            return false;
        }        
        return true;
    };

    static bool DiskPosition(int position)
    {
        if (position != DISK_POSITION_LOCAL && 
            position != DISK_POSITION_SHARE)
        {
            return false;
        }
        return true;
    };

    static bool DiskFsType(const string &fstype)
    {
        if(fstype.empty())
        {
            return true;
        }
        
        if (fstype != "ext2" && 
            fstype != "ext3" && 
            fstype != "ext4")
        {
            return false;
        }
        return true;
    };
};

/*******************************************************************************
       虚拟机配置输入方向
*******************************************************************************/
class ApiVmExpression: public Rpcable
{
public:
    ApiVmExpression() {};
    ApiVmExpression(const string& _key,const string& _op, const string& _value):
    key(_key),op(_op),value(_value)
    {};
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(key);
        TO_VALUE(op);
        TO_VALUE(value);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(key);
        FROM_VALUE(op);
        FROM_VALUE(value);
        FROM_RPC_END();
    }
    string key;
    string op;
    string value;
};

/**
@brief 功能描述: 修改虚拟磁盘参数定义 \n
@li @b 其它说明: 无
*/
class ApiVmDisk : public Rpcable
{
public:
    ApiVmDisk()
    {
        id    = -1;
        position = 0;
        size    = 0;
        reserved_backup = 0;
    };
    
    ApiVmDisk(long long     _id,
                   int           _position,
                   long long     _size,
                   const string &_bus,
                   const string &_type,
                   const string &_target,
                   const string &_fstype)
    {
        id       = _id;
        position = _position;
        size     = _size;
        bus      = _bus;
        type     = _type;
        target   = _target;
        fstype   = _fstype;
    };
    
    ApiVmDisk(const string &disk_type)
    {
        id    = -1;
        position = 0;
        size  = 0;
        type  = disk_type;
    };

    bool Validate(string &err_str)
    {
        ostringstream   oss;
        long long  max_size = DISK_MAX_SIZE;
        if ((DISK_TYPE_MACHINE != type)
             &&(DISK_TYPE_CDROM != type)
             &&(DISK_TYPE_DATABLOCK != type)
             &&(DISK_TYPE_DISK  != type))
        {
           oss<<"error disk type: "<<type ;
           err_str = oss.str();
           return false;
        }

        if ((DISK_TYPE_DISK == type)
             &&((size <= 0)||(size > max_size*1024*1024*1024)))
        {
           oss<<"error disk size: "<<size <<
                ", valid range is (0,"<<max_size<<"] GB." ;
           err_str = oss.str();
           return false;  
        }

        if (! VmParaCheck::DiskBusType(bus))
        {
           oss<<"error disk bus type : "<<bus ;
           err_str = oss.str();
           return false;
        }

        if (! VmParaCheck::DiskPosition(position))
        {
            oss<<"error disk position : "<<position ;
            err_str = oss.str();
            return false;
        }
        
        if (! VmParaCheck::DiskFsType(fstype))
        {
            oss<<"error disk fs type : "<<fstype ;
            err_str = oss.str();
            return false;
        }
        
        return true;
    };
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(position);
        TO_I8(size);
        TO_VALUE(bus);
        TO_VALUE(type);
        TO_VALUE(fstype);
        TO_VALUE(target);
        TO_VALUE(reserved_backup);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(position);
        FROM_I8(size);
        FROM_VALUE(bus);
        FROM_VALUE(type);
        FROM_VALUE(fstype);
        FROM_VALUE(target);
        FROM_VALUE(reserved_backup);
        FROM_RPC_END();
    };

    long long   id;

    /**
    @brief 参数描述: 磁盘部署位置
    @li @b 是否用户必填: 是，默认值为0
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 等于0表示由cc选择存放在本地还是共享位置
    */
    int         position;

    /**
    @brief 参数描述: 磁盘文件大小
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    long long    size;

    /**
    @brief 参数描述: domU看到的磁盘总线类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: ide
    @li @b 其它说明: 取值为ide或者scsi。
    */ 
    string   bus; 

    /**
    @brief 参数描述: 磁盘文件类型
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 参见 DISK_TYPE_*宏的定义
    */  
    string   type; 

     /**
    @brief 参数描述: domU看到的磁盘名
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: hda
    @li @b 其它说明: 
    @li @b 1.  虚拟化类型为HVM，则总线类型为IDE的存储设备名为hdx（x为a~z 的字母），
    @li @b                      总线类型为SCSI的存储设备名为sdx（x为a~z 的字母），
    @li @b                      总线类型不同的设备名单独编址；
    @li @b 2.  虚拟化类型为PVM，则统一按 xvdx（x为a~z 的字母）编址，不区分总线类型，
    */ 
    string   target;

    /**
    @brief 参数描述: 磁盘文件系统需要格式化的类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 目前仅支持ext2,ext3,ext4
    */
    string   fstype;

    /**
    @brief 参数描述: 镜像预留备份空间数
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明: 无
    */
    int   reserved_backup;

};

/**
@brief 功能描述: 修改虚拟网卡参数定义 \n
@li @b 其它说明: 无
*/
class  ApiVmNic: public Rpcable
{
public:
    
     ApiVmNic()
    {
        sriov   = false;
        loop    = false;
        model   = "";
        c_vlan  = -1;
    };

    ApiVmNic(int           _nic_index,
                 bool          _sriov,
                 bool          _loop,
                 const string &_logic_network_id,
                 const string &_model,
                 const string &_vsi_profile_id)
    {
        nic_index = _nic_index;
        sriov     = _sriov;
        loop      = _loop;
        logic_network_id = _logic_network_id;
        model   = _model;
        vsi_profile_id = _vsi_profile_id;
        
    };
    ApiVmNic(int           _nic_index,
                 bool          _sriov,
                 bool          _loop,
                 const string &_logic_network_id,
                 const string &_model,
                 const string &_vsi_profile_id,
                 const string &_ip,
                 const string &_netmask,
                 const string &_gateway)
    {
        nic_index = _nic_index;
        sriov     = _sriov;
        loop      = _loop;
        logic_network_id = _logic_network_id;
        model   = _model;
        vsi_profile_id = _vsi_profile_id;
        ip = _ip;
        netmask = _netmask;
        gateway = _gateway;        
    };  
    ApiVmNic(    int           _nic_index,
                 bool          _sriov,
                 bool          _loop,
                 const string &_logic_network_id,
                 const string &_model,
                 const string &_vsi_profile_id,
                 const string &_ip,
                 const string &_netmask,
                 const string &_gateway,
                 const string &_mac)
    {
        nic_index = _nic_index;
        sriov     = _sriov;
        loop      = _loop;
        logic_network_id = _logic_network_id;
        model   = _model;
        vsi_profile_id = _vsi_profile_id;
        ip = _ip;
        netmask = _netmask;
        gateway = _gateway;  
        mac     = _mac;        
    };    
    bool Validate(string &err_str) const
    {
        ostringstream   oss;

        if (logic_network_id.empty())
        {
            oss <<" logicnet para is null!";
            err_str = oss.str();
            return false;
        }
        
        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(nic_index);
        TO_VALUE(sriov);
        TO_VALUE(loop);
        TO_VALUE(logic_network_id);
        TO_VALUE(model);
        TO_VALUE(vsi_profile_id);
        TO_VALUE(pci_order);
        TO_VALUE(bridge_name);
        TO_VALUE(ip);
        TO_VALUE(netmask);
        TO_VALUE(gateway);
        TO_VALUE(mac);
        TO_VALUE(c_vlan);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(nic_index);
        FROM_VALUE(sriov);
        FROM_VALUE(loop);
        FROM_VALUE(logic_network_id);
        FROM_VALUE(model);
        FROM_VALUE(vsi_profile_id);
        FROM_VALUE(pci_order);
        FROM_VALUE(bridge_name);
        FROM_VALUE(ip);
        FROM_VALUE(netmask);
        FROM_VALUE(gateway);
        FROM_VALUE(mac);
        FROM_VALUE(c_vlan);
        FROM_RPC_END();
    };
    // 1. 以下参数用户可配置
    int      nic_index;                     //NicIndex,输入参数
    bool     sriov;                          //IsSriov,输入参数
    bool     loop;                         //IsLoop,输入参数
    string   logic_network_id;        //LogicNetwork UUID,输入参数
    string   model;                   //网卡型号,输入参数

    // 2. 以下参数用户不可配置，仅查询
    string   vsi_profile_id;             //VSIProfile UUID,输出参数
    string   pci_order;                  //SRIOVPCI,不相关
    string   bridge_name;
    string   ip;
    string   netmask ;
    string   gateway;
    string   mac;
    int      c_vlan;
};


/**
@brief 功能描述: 虚拟机特殊设备定义 \n
@li @b 其它说明: 无
*/
class ApiVmDevice : public Rpcable
{
public:
    ApiVmDevice()
    {
        type    = 0;
        count   = 0;
    }
    
    bool Validate(string &err_str) const
    {
        ostringstream   oss;
        
        if (type != 1)
        {
           oss<< "error device type: "<< type << endl;
           err_str = oss.str();
           return false;
        }

        if (count < 1)
        {
           oss<< "error device count: "<< count << endl;
           err_str = oss.str();
           return false;
        }

        return true;
    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(type);
        TO_VALUE(count);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(type);
        FROM_VALUE(count);
        FROM_RPC_END();
    };
    
    /**
    @brief 参数描述: 设备类型
    @li @b 是否用户必填: 是，默认值为0
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    int             type;

    /**
    @brief 参数描述: 虚拟机使用资源数量
    @li @b 是否用户必填: 是，默认值为0
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 必须大于0
    */    
    int             count;    
};

/**
@brief 功能描述: 创建虚拟机接口的参数定义 \n
@li @b 其它说明: 无
*/
class ApiVmBase: public Rpcable
{
public:

    ApiVmBase()
    {
        vcpu            = 0;
        tcu             = 1;
        memory          = 0;
        machine.type    = DISK_TYPE_MACHINE ;
        virt_type       = VIRT_TYPE_HVM ;
        vm_wdtime       = 0;
        enable_coredump = false;
        enable_serial = false;
        enable_livemigrate = true;
    };
    
    ~ApiVmBase() {};

    bool CheckMachine(string &err_str)
    {
        if(DISK_TYPE_MACHINE != machine.type) 
        {
            err_str = "error machine image type: " + machine.type ;
            return false;
        }

        if (! VmParaCheck::DiskBusType(machine.bus))
        {
           err_str ="error machine image bus type: " + machine.bus ;
           return false;
        }

        if (! VmParaCheck::DiskPosition(machine.position))
        {
            ostringstream    oss;
            oss << "error machine image position : "<< machine.position ;
            err_str = oss.str();
            return false;
        }
        
        return true;
    }

    bool CheckDisks(string &err_str)
    {
       ostringstream    oss;

         // 1. 校验设备数量
        if (disks.size() > CFG_DISK_MAX_NUM)
        {
            oss<<"disk num > "<< CFG_DISK_MAX_NUM <<", fail!";
            err_str = oss.str();
            return false;
        }


        /* 检查磁盘target非空值有无重复，格式是否合法
           1.如果是pvm，就应该是xvdb~xvdy  (xvdz保留给context使用)
           2.如果是hvm，
              总线为ide时，则为 [hdb,hds] (hdd保留给context，hdt之后的xen不支持. 如映像盘填成hdc,则认为是光驱 )
              总线为scsi时，[sda,sby]   */

        regex  pvm("xvd[b-y]");
        regex  ide("hd[b-s]");
        regex scsi("sd[a-y]"); 
        
        vector<char> ide_chars;
        vector<char> scsi_chars; 
        vector<char> xvd_chars; 
            
        for(char ch='a'; ch <= 'y'; ch++)
        {
            xvd_chars.push_back(ch); 
            
            if (ch <= 's' )
            {
                ide_chars.push_back(ch);
            }
            
            scsi_chars.push_back(ch);               
        }    

         // 2. 检验target有效性
        vector<char>* pchars = NULL;
        vector<ApiVmDisk>::iterator itdisk;
        for(itdisk = disks.begin(); itdisk!= disks.end(); itdisk++)
        {    
            if(!itdisk->Validate(err_str))
            {
               return false;
            }

            // target默认为空的 后续会补上
            if((itdisk->target).empty())
            {
                continue;
            }

            // 2.1 取自有效的盘符名?
            if(virt_type == VIRT_TYPE_PVM)
            {
                if(!regex_match(itdisk->target,pvm))
                {
                    /*oss << "error disk target: " << itdisk->target;
                    err_str = oss.str();
                    return false;*/
                }
            }
            else // hvm
            {
                if (itdisk->bus == DISK_BUS_IDE) // ide
                {
                    if((!regex_match(itdisk->target,ide))
                        ||(itdisk->target == "hdd")) // hdd
                    {
                        oss << "error disk target: " << itdisk->target;
                        err_str = oss.str();
                        return false;
                    }
                }
                else // scsi 
                {
                    if(!regex_match(itdisk->target,scsi))
                    {
                        oss << "error disk target: " << itdisk->target;
                        err_str = oss.str();
                        return false;
                    }
                }
            }

            // 2.2 的重复性检查
/*            if(virt_type == VIRT_TYPE_PVM)
            {
                pchars = &xvd_chars;
            }
            else 
            { */
                if(itdisk->bus == DISK_BUS_IDE)
               {
                   pchars = &ide_chars;
               }
               else
               {
                   pchars = &scsi_chars;
               }
/*            }*/
            
            char order = itdisk->target[itdisk->target.length()-1];
            if((*pchars)[order-'a'] == order)            
            {               
                //已经用过的需要清空
                (*pchars)[order-'a'] = ' '; 
            }
            else
            {
                // 重复了
                oss << "target order = " << itdisk->target << " is already used!" << endl;
                err_str = oss.str();
                return false;
            }
        }

         // 3. 将target为空的补充完整
        for(itdisk=disks.begin();itdisk!=disks.end();itdisk++)
        {
            if(!itdisk->target.empty())
            {
                continue;
            }
                
            string target_prefix;
/*            if(virt_type == VIRT_TYPE_PVM)
            {
                target_prefix="xvd";
                pchars = &xvd_chars;
            }
            else 
            {*/
                if(itdisk->bus == DISK_BUS_IDE)
                {
                    target_prefix="hd";
                    pchars = &ide_chars;
                }
                else
                {
                    target_prefix="sd";
                    pchars = &scsi_chars;
                } 
/*            }*/
            
            for(unsigned int i=0; i<pchars->size(); i++)
            {
                // hda ,hdc,hdd不再做盘符自动分配给没有设定盘符的情况 !
                if ((virt_type == VIRT_TYPE_HVM)  
                     &&(itdisk->bus == DISK_BUS_IDE)
                     &&(((*pchars)[i] == 'a')
                         ||((*pchars)[i] == 'c') 
                         ||((*pchars)[i] == 'd') ) )
                {
                    continue;  
                }
                
                if ((*pchars)[i] != ' ')
                {
                    itdisk->target = target_prefix.append(1,(*pchars)[i]);
                    (*pchars)[i] =' ';
                    break;
                }
            }

            if(itdisk->target.empty())
            {
                //target不够用了
                oss << "target order is used out!" << endl;
                err_str = oss.str();
                return false;
            }
        }
        return true;
    }; 

    bool CheckNics(string &err_str)
    {
        bool             success       = false;
        bool             is_conflic    = false;
        bool             netfront_type = false;
        bool             non_netfront_type = false;
        string           tmp_str;
        ostringstream    oss;
        list<int>             list_index;
        list<int>::iterator   it_list;
        int              max_index = 0;
        vector<ApiVmNic>::iterator itnic;
        int              sriov_nics_num = 0;
        int              nonsriov_nics_num = 0;
               
        for(itnic = nics.begin(); itnic!= nics.end(); itnic++)
        {
           success = itnic->Validate(tmp_str);//网络平面, vlan ,dhcp
           if (!success)
           {
              oss<<"error nic config : "<<tmp_str ;
              err_str = oss.str();
              return false;
           }  

           if (itnic->sriov) //SR-IOV网卡暂不支持网卡型号选择
           {
              sriov_nics_num++;
              if (sriov_nics_num > CFG_SRIOV_NIC_MAX_NUM)
              {
                  oss<<"sriov nic num > "<< CFG_SRIOV_NIC_MAX_NUM <<", fail!";
                  err_str = oss.str();
                  return false;
              } 
              if (!(itnic->model.empty()))
              {
                  oss<<"SR-IOV nic model should be empty!" ;
                  err_str = oss.str();
                  return false;
              }
           }           
           else //不使用SR-IOV功能
           {      
               nonsriov_nics_num++;
               if (nonsriov_nics_num > CFG_NONSRIOV_NIC_MAX_NUM)
               {
                   oss<<"non-sriov nic num > "<< CFG_NONSRIOV_NIC_MAX_NUM <<", fail!";
                   err_str = oss.str();
                   return false;
               } 
               if (virt_type == VIRT_TYPE_PVM) //PVM
               {   
                   //不允许选择网卡型号为空字符串
                   if (!(itnic->model.empty()))
                   {
                       oss<<"PVM nic model should be empty!" ;
                       err_str = oss.str();
                       return false;
                   }                   
               }
               else  //HVM
               {                  
                   if (itnic->model == NETFRONT)
                   {
                       netfront_type = true;
                       if (non_netfront_type == true)
                       {
                           oss<<"HVM nic model should all be netfront!" ;
                           err_str = oss.str();
                           return false;
                       }
                        
                   }
                   else if (itnic->model == E1000\
                            || itnic->model == RTL8139\
                            || itnic->model == NE2K_PCI\
                            || itnic->model == PCNET\
                            || itnic->model.empty())
                   {
                       non_netfront_type = true;
                       if (netfront_type == true)
                       {
                           oss<<"HVM nic model should all be non-netfront (including unspecified) !" ;
                           err_str = oss.str();
                           return false;
                       }
                   }
                   else
                   {
                       oss<<"HVM nic model is unknown!" ;
                       err_str = oss.str();
                       return false;
                   }
               }
           }         
        }

        //检查 nic_index字段
        for(itnic = nics.begin(); itnic != nics.end(); itnic++)
        {
            if (0 == itnic->nic_index)
            {
                continue;
            }
            is_conflic = false;
            for (it_list = list_index.begin();
                 it_list != list_index.end();                
                 it_list++)
            {
                if (*it_list == itnic->nic_index)
                { 
                   is_conflic = true;
                   break;
                }
            }
            
            if(is_conflic)
            {
                //nic_index重复了
                oss << "nic_index = " << itnic->nic_index<< " is already used!" << endl;
                err_str = oss.str();
                return false;
            }
            
            list_index.push_back(itnic->nic_index); 
            
            if(itnic->nic_index >max_index)
            {
                max_index = itnic->nic_index;
            }
            
        }

        // 重新生成index为0的网卡序号
        for(itnic = nics.begin(); itnic!= nics.end(); itnic++)
        {
           if (0 != itnic->nic_index)
            {
                continue;
            }
            max_index++;
            itnic->nic_index = max_index;
        }
        return true;
    }; 

    bool CheckContexts(string &err_str)
    {
        ostringstream    oss;
        map<string,string>::const_iterator iter_context;
        unsigned int  max_size = 0;

        for (iter_context = context.begin(); 
             iter_context != context.end(); 
             iter_context++)
        {
            if (false == StringCheck::CheckComplexName(iter_context->first, 
                                                       0, 
                                                       STR_LEN_256))
            {
               oss <<  "Error, invalide file name: " << iter_context->first;
               err_str = oss.str();
               return false;
            }

            if (true == StringCheck::CheckIsAllSpace(iter_context->second))
            {
               oss << "Error, file content are blank spaces.";
               err_str = oss.str();
               return false;
            }
            max_size += iter_context->second.size();
            max_size += iter_context->first.size();
        }

        if(max_size > CFG_CONTEXT_MAX_SIZE)
        {
            oss<<"Error, context size > "<< CFG_CONTEXT_MAX_SIZE << " Bytes.";
            err_str = oss.str();
            return false;
        }
        
        return true;
    }

    bool CheckQualifications(string &err_str)
    {
        if (0 != qualifications.size())
        {
             err_str = "unsupported function, qualifications must null." ;
             return false;
        }

        return true;
    }

    bool CheckWdtime(string &err_str)
    {
        if ((0 < vm_wdtime) &&(vm_wdtime < 600 ))
        {
            err_str = "error watchdog time, should equal or longer than 600s" ;
            return false;
        }  
        
        return true;
    }
    
    bool CheckDevices(string &err_str) const
    {
        bool             success;
        string           tmp_str;
        ostringstream    oss;
        
        vector<ApiVmDevice>::const_iterator it;
        for(it = devices.begin(); it!= devices.end(); it++)
        {
           success = it->Validate(tmp_str);
           if (!success)
           {
              oss<<"error device config : "<<tmp_str ;
              err_str = oss.str();
              return false;
           } 
        }
        
        return true;
    }

    bool CheckHypervisor(string &err_str) const
    {
        if((HYPERVISOR_XEN   != hypervisor) 
           &&(HYPERVISOR_KVM != hypervisor)
           &&(HYPERVISOR_ANY != hypervisor))
        {
            err_str = "error hypervisor type: " + hypervisor;
            return false;
        }         
        return true;
    }

    
    bool Validate(string &err_str)// const
    {
        vector<ApiVmExpression>::iterator  iter;
        
        //虚拟化类型
        if(! VmParaCheck::VirtType(virt_type))
        {
            err_str =  "Invalide virt type ( " + virt_type +" )";
            return false;
        }   
        
        if(false == CheckHypervisor(err_str))
        {
           return false;
        }
        
        if(false == CheckMachine(err_str))
        {
           return false;
        }
        
        if(false == CheckDisks(err_str))
        {
           return false;
        }
        
        if(false == CheckNics(err_str))
        {
           return false;
        } 

        if(false == CheckDevices(err_str))
        {
            return false;
        }

        // 校验上下文大小,不能大于100K
        if(false == CheckContexts(err_str))
        {
            return false;
        }        
        
        if(false == CheckQualifications(err_str))
        {
            return false;
        }        

        if(false == StringCheck::CheckNormalName(cluster,0,STR_LEN_64))
        {
            err_str = "Error, invalid cluster name :" + cluster + " .";
            return false;
        }

        if(false == StringCheck::CheckNormalName(host, 0, STR_LEN_64))
        {
            err_str = "Error, invalid host name :" + host + " .";
            return false;
        }

        if(false == CheckWdtime(err_str))
        {
             return false;
        }
        
        if(true == StringCheck::CheckIsAllSpace(vnc_passwd))
        {
            err_str = "Error, vnc password consist of all space char." ;
            return false;
        }
        
        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(description);
        TO_VALUE(vcpu);
        TO_VALUE(tcu);
        TO_I8(memory);
        TO_STRUCT(machine);
        TO_VALUE(root_device);
        TO_VALUE(kernel_command);
        TO_VALUE(bootloader);
        TO_VALUE(virt_type);
        TO_VALUE(hypervisor);
        TO_VALUE(rawdata);
        
        TO_VALUE(context);
        
        TO_STRUCT_ARRAY(qualifications);
        TO_STRUCT_ARRAY(disks);
        TO_STRUCT_ARRAY(nics);
        TO_VALUE(cluster);
        TO_VALUE(host);
        TO_VALUE(vm_wdtime);
        TO_VALUE(vnc_passwd);
        TO_VALUE(enable_coredump);
        TO_VALUE(enable_serial);
        TO_STRUCT_ARRAY(devices);        
        TO_VALUE(enable_livemigrate);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(description);
        FROM_VALUE(vcpu);
        FROM_VALUE(tcu);
        FROM_I8(memory);
        FROM_STRUCT(machine);
        FROM_VALUE(root_device);
        FROM_VALUE(kernel_command);
        FROM_VALUE(bootloader);
        FROM_VALUE(virt_type);
        FROM_VALUE(hypervisor);
        FROM_VALUE(rawdata);
        
        FROM_VALUE(context);
        FROM_STRUCT_ARRAY(qualifications);
        
        FROM_STRUCT_ARRAY(disks);
        FROM_STRUCT_ARRAY(nics);
        FROM_VALUE(cluster);
        FROM_VALUE(host);
        FROM_VALUE(vm_wdtime);
        FROM_VALUE(vnc_passwd);
        FROM_VALUE(enable_coredump);
        FROM_VALUE(enable_serial);
        FROM_STRUCT_ARRAY(devices);        
        FROM_VALUE(enable_livemigrate);
        FROM_RPC_END();
    };

    /**
    @brief 参数描述: 用户对虚拟机或模板的描述
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 否
    @li @b 其它说明: 
    */
    string    description;
    
    //Capacity Section
    /**
    @brief 参数描述: 虚拟机vcpu数量
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 否
    @li @b 其它说明: 
    */
    int        vcpu;  

    /**
    @brief 参数描述: 虚拟机单核计算能力
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 否
    @li @b 其它说明: 
    */
    int        tcu;  

     /**
    @brief 参数描述: 虚拟机内存大小
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 单位 字节
    */
    long long  memory;

    //OS and Boot Options Section

    /**
    @brief 参数描述: machine类型映像ID
    @li @b 是否用户必填: 其他
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 要么使用machine类型 要么使用(kernel + ramdik)类型映像
    */
    ApiVmDisk  machine;

   // ApiVmDisk  kernel;

   // ApiVmDisk  ramdisk;
    /**
    @brief 参数描述: 虚拟机的根分区
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string     root_device;

    /**
    @brief 参数描述: 传给虚拟机内核的启动项
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string     kernel_command;

    /**
    @brief 参数描述: 虚拟机bootloader配置项
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string     bootloader;

     /**
    @brief 参数描述: 虚拟化类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: VIRT_TYPE_HVM
    @li @b 其它说明: 只能是VIRT_TYPE_HVM 或 VIRT_TYPE_PVM
    */
    string     virt_type;

     /**
    @brief 参数描述: hypervisor 类型
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: HYPERVISOR_XEN
    @li @b 其它说明: 只能是HYPERVISOR_XEN 或 HYPERVISOR_KVM
    */
    string     hypervisor;

     /**
    @brief 参数描述: 直接传递给hypervisor的原始配置项
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 
    */
    string     rawdata;    

   /**
    @brief 参数描述:上下文信息
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 
    @li @b 默认值（租户可不填的情况下）:无
    @li @b 其它说明: 通过iso传递
    */
    map<string,string>       context;

    /**
    @brief 参数描述: 物理机应该满足的表达式条件
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    vector<ApiVmExpression>  qualifications;  

    /**
    @brief 参数描述: 虚拟机磁盘列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    vector<ApiVmDisk>        disks;  

    /**
    @brief 参数描述: 映像网卡列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    vector<ApiVmNic>         nics; 

    /**
    @brief 参数描述: 虚拟机部署到哪个集群
    @li @b 是否用户必填: 否，只有管理员有权限使用本字段
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    string                   cluster;

    /**
    @brief 参数描述: 虚拟机部署到哪个主机
    @li @b 是否用户必填: 否，只有管理员有权限使用本字段
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    string                   host;

    /**
    @brief 参数描述: 看门狗时长，最小1200秒（20分钟）
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0, 表示不启用
    @li @b 其它说明: 无
    */
    int            vm_wdtime;

    /**
    @brief 参数描述: 创建的虚拟机VNC访问密码
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: ""
    @li @b 其它说明: 无
    */
    string                  vnc_passwd;
    
    /**
    @brief 参数描述: 使能coredump功能
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明: 无
    */
    bool                     enable_coredump;
    bool                     enable_serial;
    
    /**
    @brief 参数描述: 虚拟机特殊设备列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 是
    @li @b 默认值（租户可不填的情况下）: 无
    @li @b 其它说明: 无
    */
    vector<ApiVmDevice>        devices;  

    /**
    @brief 参数描述: vm是否允许热迁移
    @li @b 是否用户必填: 是
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 1
    @li @b 其它说明: 无
    */
    bool                     enable_livemigrate;
};

class ApiVmCfgInfo : public Rpcable
{
public:
    ApiVmCfgInfo()
    {};

    ~ApiVmCfgInfo()
    {};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(vm_name);
        TO_VALUE(project_name);
        TO_STRUCT(base_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(vm_name);
        FROM_VALUE(project_name);
        FROM_STRUCT(base_info);
        FROM_RPC_END();
    };

   //虚拟机独有的字段
    string    vm_name;
    string    project_name;

    ApiVmBase base_info;

};

class ApiVtCfgInfo : public Rpcable
{
public:
    ApiVtCfgInfo()
    {};

    ~ApiVtCfgInfo()
    {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(base_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(base_info);
        FROM_RPC_END();
    };

    ApiVmBase base_info;

};


/*******************************************************************************
       虚拟机配置输出方向
*******************************************************************************/


/*****************************************************************************/
/* 虚拟机模板/虚拟机信息基类    */
/*****************************************************************************/
#if 0
class ApiVmBaseInfo : public Rpcable
{
public:
    ApiVmBaseInfo() 
    {
        id              = -1;
        vcpu            = 0;
        memory          = 0;
        disk_size       = 0;
        machine.type    = DISK_TYPE_MACHINE;
        machine.id      =  -1;
        vm_wdtime       = 0;
        enable_coredump = false;
        enable_serial = false;
    };
    
    ~ApiVmBaseInfo() {};
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(id);
        TO_VALUE(description);
        TO_VALUE(user_name);
        TO_I8(memory);
        TO_VALUE(vcpu);
        TO_VALUE(tcu);
        TO_STRUCT(machine);
     //   TO_STRUCT(kernel);
     //   TO_STRUCT(ramdisk);
        TO_I8(disk_size);
        TO_VALUE(root_device);
        TO_VALUE(kernel_command);
        TO_VALUE(bootloader);
        TO_VALUE(virt_type);
        TO_VALUE(hypervisor);
        TO_VALUE(rawdata);
        TO_VALUE(cluster);
        TO_VALUE(host);
        TO_VALUE(vm_wdtime);
        TO_VALUE(vnc_passwd);
        TO_VALUE(enable_coredump);
        TO_VALUE(enable_serial);
        TO_VALUE(context);
        TO_STRUCT_ARRAY(qualifications);
        TO_STRUCT_ARRAY(disks);
        TO_STRUCT_ARRAY(nics);
        TO_STRUCT_ARRAY(devices);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(id);
        FROM_VALUE(description);
        FROM_VALUE(user_name);
        FROM_I8(memory);
        FROM_VALUE(vcpu);
        FROM_VALUE(tcu);
        FROM_STRUCT(machine);
     //   FROM_STRUCT(kernel);
      //  FROM_STRUCT(ramdisk);
        FROM_I8(disk_size);
        FROM_VALUE(root_device);
        FROM_VALUE(kernel_command);
        FROM_VALUE(bootloader);
        FROM_VALUE(virt_type);
        FROM_VALUE(hypervisor);
        FROM_VALUE(rawdata);
        FROM_VALUE(cluster);
        FROM_VALUE(host);
        FROM_VALUE(vm_wdtime);
        FROM_VALUE(vnc_passwd);
        FROM_VALUE(enable_coredump);
        FROM_VALUE(enable_serial);
        FROM_VALUE(context);
        FROM_STRUCT_ARRAY(qualifications);
        FROM_STRUCT_ARRAY(disks);
        FROM_STRUCT_ARRAY(nics);
        FROM_STRUCT_ARRAY(devices);
        FROM_RPC_END();
    };
    
    long long     id; //基本信息
    string        description;
    string        user_name;
    int           vcpu;
    int           tcu;
    long long     memory;
    ApiVmDisk     machine;
 //   ApiVmDisk     kernel;
 //   ApiVmDisk     ramdisk; 
    long long     disk_size;
    string        root_device;//传给xen的参数
    string       kernel_command;
    string       bootloader;
    string       virt_type;
    string       hypervisor;
    string       rawdata;
    string       cluster;
    string       host;
    int           vm_wdtime;
    string       vnc_passwd;
    bool         enable_coredump;
    bool         enable_serial;
    
    map<string,string>       context;  //用户提供的上下文信息，通过iso传递
    vector<ApiVmExpression>  qualifications;  //虚拟机调度策略 --  物理机应该满足的表达式条件
    vector<ApiVmDisk>      disks;   //虚拟机磁盘列表
    vector<ApiVmNic>       nics;  //虚拟网卡
    vector<ApiVmDevice>       devices;  //虚拟特殊设备
};
#endif
#endif  /* TECS_API_VMBASE_H  */

