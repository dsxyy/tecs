({
    xid:{
        html:{
            saRefresh:"刷新",
            saDisable:"开始维护",
            saEnable:"结束维护",
            saForget:"清理",
            salistSummaryNav:"磁阵列表",

            saSummaryNav:"摘要",

            saVgNav:"卷组",
            vg_t1:"名称",
            vg_t2:"空闲空间",
            vg_t3:"总空间",
            vg_t4:"卷数量",
            vgRefresh:"刷新",

            saVolumeNav:"卷",
            volume_t1:"卷组名称",
            volume_t2:"卷名称",
            volume_t3:"大小",
            volume_t4:"请求ID",
            volume_t5:"虚拟机",
            volume_t6:"盘符",
            volumeRefresh:"刷新",

            saUseage:"使用情况",
            saInfo:"信息",
            saDesc:"描述"        
        },
        title:{},
        val:{
            forget_sa_confirm:"确定要清理选中的磁阵吗？",
            disable_sa_confirm:"确定要维护选中的磁阵吗？"
        },
        dialog:{
            add_cluster_to_sa_dlg:"配置集群",
            forget_sa_dlg:"清理磁阵",
            disable_sa_dlg:"开始维护"            
        }
    },
    type:{
        SaRunState:{
            0: "离线",
            1: "在线",
            2: "维护"},    
        SaState:{
            0: "维护",
            1: "运行"},
        SaStateOpr:{
            0: "结束维护",
            1: "开始维护"},
        SaOnline:{
            0: "离线",
            1: "在线"},
        usage:{
            used:"已用量",
            max:"单主机最大可用量",
            free:"可用量",
            total:"总量"},            
        field_name:{
            "sa.sid":"ID",
            "sa.name":"名称",
            "sa.sa_name":"SA名称",
            "sa.state":"维护状态",
            "sa.online":"联网状态",
            "sa.type":"磁阵类型",
            "sa.ctrl_addr":"管理地址",
            "sa.register_time":"注册时间",            
            "sa.cluster":"集群",
            "sa.desc":"描述",
            "sa.maped":"使用该磁阵的集群",
            "sa.unmaped":"未使用该磁阵的集群",
            "select_vg":"请选择卷组"
        },
        opr:{
            setting:"设置"        
        }
    }
})