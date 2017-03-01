#if !defined(EVB_ENTRY_INCLUDE_H__)
#define EVB_ENTRY_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif
    /* 外部接口; */
    int32 StartEvbModule(u8 ucAdminRole);           /* 启动Evb模块; */
    int32 ShutdownEvbModule(void);                  /* 关闭Evb模块; */
    int32 EvbMainThreadIsWorking(void);             /* EVB主控是否进入工作态; */
    int32 Evb20msTimerLoop(void);                   /* EVB协议定时器心跳; */
    int32 AddEvbIssPort(u32 uiPortNo, const char *pName, u8 *aucMac, u8 ucState, u8 ucEvbPortType); /* 创建EvbIssPort; */
    int32 DelEvbIssPort(u32 uiPortNo);              /* 删除EvbIssPort; */
    int32 ChgEvbIssPortState(u32 uiPortNo, u8 ucState);/* 设置端口状态; state: 1表示work，0表示down*/
    int32 ConfCdcpCmd(void);                        /* 模拟配置; */
    int32 ConfEvbIssPort(u32 uiIssPortNo, u8 ucPortType); /* 配置ISS Port为UAP or URP; */
    int32 ChgEvbIssPortMode(u32 uiPortNo, u8 ucMode); /* 配置ISS Port为VEPA or VEB; */

    /* 主控模块内部接口; */   
    void EvbMainEntry(u32 dwMessage, void *pMsg, u32 dwMsgLen);  /* 主控消息入口; */   

#ifdef __cplusplus
}
#endif

#endif

