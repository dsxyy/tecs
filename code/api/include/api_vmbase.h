/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�api_vmbase.h
* �ļ���ʶ��
* ����ժҪ��vmcfg��vmtemplate������api�ӿ��ļ���һ���������ļ�������xml-rpc�� ! !
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2012��10��29��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/10/29
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
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
       ������������뷽��
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
@brief ��������: �޸�������̲������� \n
@li @b ����˵��: ��
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
    @brief ��������: ���̲���λ��
    @li @b �Ƿ��û�����: �ǣ�Ĭ��ֵΪ0
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ����0��ʾ��ccѡ�����ڱ��ػ��ǹ���λ��
    */
    int         position;

    /**
    @brief ��������: �����ļ���С
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    long long    size;

    /**
    @brief ��������: domU�����Ĵ�����������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ide
    @li @b ����˵��: ȡֵΪide����scsi��
    */ 
    string   bus; 

    /**
    @brief ��������: �����ļ�����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: �μ� DISK_TYPE_*��Ķ���
    */  
    string   type; 

     /**
    @brief ��������: domU�����Ĵ�����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: hda
    @li @b ����˵��: 
    @li @b 1.  ���⻯����ΪHVM������������ΪIDE�Ĵ洢�豸��Ϊhdx��xΪa~z ����ĸ����
    @li @b                      ��������ΪSCSI�Ĵ洢�豸��Ϊsdx��xΪa~z ����ĸ����
    @li @b                      �������Ͳ�ͬ���豸��������ַ��
    @li @b 2.  ���⻯����ΪPVM����ͳһ�� xvdx��xΪa~z ����ĸ����ַ���������������ͣ�
    */ 
    string   target;

    /**
    @brief ��������: �����ļ�ϵͳ��Ҫ��ʽ��������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ŀǰ��֧��ext2,ext3,ext4
    */
    string   fstype;

    /**
    @brief ��������: ����Ԥ�����ݿռ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��: ��
    */
    int   reserved_backup;

};

/**
@brief ��������: �޸����������������� \n
@li @b ����˵��: ��
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
    // 1. ���²����û�������
    int      nic_index;                     //NicIndex,�������
    bool     sriov;                          //IsSriov,�������
    bool     loop;                         //IsLoop,�������
    string   logic_network_id;        //LogicNetwork UUID,�������
    string   model;                   //�����ͺ�,�������

    // 2. ���²����û��������ã�����ѯ
    string   vsi_profile_id;             //VSIProfile UUID,�������
    string   pci_order;                  //SRIOVPCI,�����
    string   bridge_name;
    string   ip;
    string   netmask ;
    string   gateway;
    string   mac;
    int      c_vlan;
};


/**
@brief ��������: ����������豸���� \n
@li @b ����˵��: ��
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
    @brief ��������: �豸����
    @li @b �Ƿ��û�����: �ǣ�Ĭ��ֵΪ0
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    int             type;

    /**
    @brief ��������: �����ʹ����Դ����
    @li @b �Ƿ��û�����: �ǣ�Ĭ��ֵΪ0
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: �������0
    */    
    int             count;    
};

/**
@brief ��������: ����������ӿڵĲ������� \n
@li @b ����˵��: ��
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

         // 1. У���豸����
        if (disks.size() > CFG_DISK_MAX_NUM)
        {
            oss<<"disk num > "<< CFG_DISK_MAX_NUM <<", fail!";
            err_str = oss.str();
            return false;
        }


        /* ������target�ǿ�ֵ�����ظ�����ʽ�Ƿ�Ϸ�
           1.�����pvm����Ӧ����xvdb~xvdy  (xvdz������contextʹ��)
           2.�����hvm��
              ����Ϊideʱ����Ϊ [hdb,hds] (hdd������context��hdt֮���xen��֧��. ��ӳ�������hdc,����Ϊ�ǹ��� )
              ����Ϊscsiʱ��[sda,sby]   */

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

         // 2. ����target��Ч��
        vector<char>* pchars = NULL;
        vector<ApiVmDisk>::iterator itdisk;
        for(itdisk = disks.begin(); itdisk!= disks.end(); itdisk++)
        {    
            if(!itdisk->Validate(err_str))
            {
               return false;
            }

            // targetĬ��Ϊ�յ� �����Ჹ��
            if((itdisk->target).empty())
            {
                continue;
            }

            // 2.1 ȡ����Ч���̷���?
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

            // 2.2 ���ظ��Լ��
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
                //�Ѿ��ù�����Ҫ���
                (*pchars)[order-'a'] = ' '; 
            }
            else
            {
                // �ظ���
                oss << "target order = " << itdisk->target << " is already used!" << endl;
                err_str = oss.str();
                return false;
            }
        }

         // 3. ��targetΪ�յĲ�������
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
                // hda ,hdc,hdd�������̷��Զ������û���趨�̷������ !
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
                //target��������
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
           success = itnic->Validate(tmp_str);//����ƽ��, vlan ,dhcp
           if (!success)
           {
              oss<<"error nic config : "<<tmp_str ;
              err_str = oss.str();
              return false;
           }  

           if (itnic->sriov) //SR-IOV�����ݲ�֧�������ͺ�ѡ��
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
           else //��ʹ��SR-IOV����
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
                   //������ѡ�������ͺ�Ϊ���ַ���
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

        //��� nic_index�ֶ�
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
                //nic_index�ظ���
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

        // ��������indexΪ0���������
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
        
        //���⻯����
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

        // У�������Ĵ�С,���ܴ���100K
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
    @brief ��������: �û����������ģ�������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string    description;
    
    //Capacity Section
    /**
    @brief ��������: �����vcpu����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    int        vcpu;  

    /**
    @brief ��������: ��������˼�������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    int        tcu;  

     /**
    @brief ��������: ������ڴ��С
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��λ �ֽ�
    */
    long long  memory;

    //OS and Boot Options Section

    /**
    @brief ��������: machine����ӳ��ID
    @li @b �Ƿ��û�����: ����
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: Ҫôʹ��machine���� Ҫôʹ��(kernel + ramdik)����ӳ��
    */
    ApiVmDisk  machine;

   // ApiVmDisk  kernel;

   // ApiVmDisk  ramdisk;
    /**
    @brief ��������: ������ĸ�����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string     root_device;

    /**
    @brief ��������: ����������ں˵�������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string     kernel_command;

    /**
    @brief ��������: �����bootloader������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string     bootloader;

     /**
    @brief ��������: ���⻯����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: VIRT_TYPE_HVM
    @li @b ����˵��: ֻ����VIRT_TYPE_HVM �� VIRT_TYPE_PVM
    */
    string     virt_type;

     /**
    @brief ��������: hypervisor ����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: HYPERVISOR_XEN
    @li @b ����˵��: ֻ����HYPERVISOR_XEN �� HYPERVISOR_KVM
    */
    string     hypervisor;

     /**
    @brief ��������: ֱ�Ӵ��ݸ�hypervisor��ԭʼ������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string     rawdata;    

   /**
    @brief ��������:��������Ϣ
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: 
    @li @b Ĭ��ֵ���⻧�ɲ��������£�:��
    @li @b ����˵��: ͨ��iso����
    */
    map<string,string>       context;

    /**
    @brief ��������: �����Ӧ������ı��ʽ����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    vector<ApiVmExpression>  qualifications;  

    /**
    @brief ��������: ����������б�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    vector<ApiVmDisk>        disks;  

    /**
    @brief ��������: ӳ�������б�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    vector<ApiVmNic>         nics; 

    /**
    @brief ��������: ����������ĸ���Ⱥ
    @li @b �Ƿ��û�����: ��ֻ�й���Ա��Ȩ��ʹ�ñ��ֶ�
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    string                   cluster;

    /**
    @brief ��������: ����������ĸ�����
    @li @b �Ƿ��û�����: ��ֻ�й���Ա��Ȩ��ʹ�ñ��ֶ�
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    string                   host;

    /**
    @brief ��������: ���Ź�ʱ������С1200�루20���ӣ�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0, ��ʾ������
    @li @b ����˵��: ��
    */
    int            vm_wdtime;

    /**
    @brief ��������: �����������VNC��������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ""
    @li @b ����˵��: ��
    */
    string                  vnc_passwd;
    
    /**
    @brief ��������: ʹ��coredump����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��: ��
    */
    bool                     enable_coredump;
    bool                     enable_serial;
    
    /**
    @brief ��������: ����������豸�б�
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: ��
    */
    vector<ApiVmDevice>        devices;  

    /**
    @brief ��������: vm�Ƿ�������Ǩ��
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 1
    @li @b ����˵��: ��
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

   //��������е��ֶ�
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
       ����������������
*******************************************************************************/


/*****************************************************************************/
/* �����ģ��/�������Ϣ����    */
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
    
    long long     id; //������Ϣ
    string        description;
    string        user_name;
    int           vcpu;
    int           tcu;
    long long     memory;
    ApiVmDisk     machine;
 //   ApiVmDisk     kernel;
 //   ApiVmDisk     ramdisk; 
    long long     disk_size;
    string        root_device;//����xen�Ĳ���
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
    
    map<string,string>       context;  //�û��ṩ����������Ϣ��ͨ��iso����
    vector<ApiVmExpression>  qualifications;  //��������Ȳ��� --  �����Ӧ������ı��ʽ����
    vector<ApiVmDisk>      disks;   //����������б�
    vector<ApiVmNic>       nics;  //��������
    vector<ApiVmDevice>       devices;  //���������豸
};
#endif
#endif  /* TECS_API_VMBASE_H  */

