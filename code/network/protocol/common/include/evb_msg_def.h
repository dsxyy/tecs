#if !defined(EVB_MSG_DEF_INCLUDE_H__)
#define EVB_MSG_DEF_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  EVB_INVALID_MESSAGE    (0x0)

#ifdef _WIN32
#define  EVB_MSG_BEGIN          (WM_USER + 1)
#else
#define  EVB_MSG_BEGIN          (0x1000)
#endif

#define EVB_WORKING_MSG         (EVB_MSG_BEGIN + 1)
#define EVB_SHUTDOWN_MSG        (EVB_MSG_BEGIN + 2)
#define EVB_20MS_TIMER_MSG      (EVB_MSG_BEGIN + 3)
#define EVB_ADD_ISS_PORT_MSG    (EVB_MSG_BEGIN + 4)
#define EVB_DEL_ISS_PORT_MSG    (EVB_MSG_BEGIN + 5)
#define EVB_CHG_ISS_PORT_STATE_MSG  (EVB_MSG_BEGIN + 6)
#define EVB_CDCP_CMD            (EVB_MSG_BEGIN + 7)
#define EVB_CFG_ISS_PORT_MSG    (EVB_MSG_BEGIN + 8)
#define EVB_CHG_ISS_PORT_MODE   (EVB_MSG_BEGIN + 9)

    /* EVB消息结构体定义; */
    typedef struct tagEvbAddIssPortMsg
    {
        u32 uiPortNo;
        char acName[EVB_PORT_NAME_MAX_LEN];
        u8 aucMac[EVB_PORT_MAC_LEN];
        u8 ucState;
        u8 ucEvbPortType;
    }TEvbAddIssPortMsg;

    typedef struct tagEvbDelIssPortMsg
    {
        u32 uiPortNo;
    }TEvbDelIssPortMsg;

    typedef struct tagEvbChgIssPortEvbModeMsg
    {
        u32 uiPortNo;
        u8  ucMode;
    }TEvbChgIssPortEvbModeMsg;

    typedef struct tagEvbChgPortStatMsg
    {
        u32 uiPortNo;
        u8  ucState;
        u8  ucPad[3];
    }TEvbChgPortStatMsg;

    typedef struct tagEvbPortMsg
    {
        u32 uiPortNo;
        u32 uiIssPortNo;
        u8  ucPortType;
        u8  ucPad[3];
    }TEvbPortMsg;

#ifdef __cplusplus
}
#endif

#endif
