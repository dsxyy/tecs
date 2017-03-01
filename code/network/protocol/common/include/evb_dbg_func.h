#if !defined(EVB_DBG_FUNC_INCLUDE_H__)
#define EVB_DBG_FUNC_INCLUDE_H__

#ifdef __cplusplus
extern "C"{
#endif 

    /* EVB Timer; */
    void DbgAccTimerTestStart(void);
    void DbgAccTimerTestStop(void);

    /* EVB entry debug functions; */
    void DbgShowEvbSysState(void);
    void DbgSetMsgPrint(u8 ucOpen); /* 0:关闭, 其他打开; */

    /* EVB port debug functions; */
    void DbgShowEvbPortInfo(u32 uiPortNo);
    void DbgShowEvbPortPktStt(u32 uiPortNo);

    /* CDCP debug functions; */
    void DbgShowCdcpConf(u32 uiLocalPort);
    void DbgOpenSndCdcpPktPrint(void);
    void DbgCloseSndCdcpPktPrint(void);
    void DbgOpenRcvCdcpPktPrint(void);
    void DbgCloseRcvCdcpPktPrint(void);
    void DbgSetStationCdcpData(u32 uiLocalPort);
    void DbgSetBridgeCdcpData(u32 uiLocalPort);
#ifdef __cplusplus
}
#endif 

#endif
