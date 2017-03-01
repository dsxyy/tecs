#if !defined(EVB_ENTRY_INCLUDE_H__)
#define EVB_ENTRY_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif
    /* �ⲿ�ӿ�; */
    int32 StartEvbModule(u8 ucAdminRole);           /* ����Evbģ��; */
    int32 ShutdownEvbModule(void);                  /* �ر�Evbģ��; */
    int32 EvbMainThreadIsWorking(void);             /* EVB�����Ƿ���빤��̬; */
    int32 Evb20msTimerLoop(void);                   /* EVBЭ�鶨ʱ������; */
    int32 AddEvbIssPort(u32 uiPortNo, const char *pName, u8 *aucMac, u8 ucState, u8 ucEvbPortType); /* ����EvbIssPort; */
    int32 DelEvbIssPort(u32 uiPortNo);              /* ɾ��EvbIssPort; */
    int32 ChgEvbIssPortState(u32 uiPortNo, u8 ucState);/* ���ö˿�״̬; state: 1��ʾwork��0��ʾdown*/
    int32 ConfCdcpCmd(void);                        /* ģ������; */
    int32 ConfEvbIssPort(u32 uiIssPortNo, u8 ucPortType); /* ����ISS PortΪUAP or URP; */
    int32 ChgEvbIssPortMode(u32 uiPortNo, u8 ucMode); /* ����ISS PortΪVEPA or VEB; */

    /* ����ģ���ڲ��ӿ�; */   
    void EvbMainEntry(u32 dwMessage, void *pMsg, u32 dwMsgLen);  /* ������Ϣ���; */   

#ifdef __cplusplus
}
#endif

#endif

