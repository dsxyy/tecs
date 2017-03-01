#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "evb_dbg_func.h"
#include "evb_entry.h"
    extern void DbgShowEvbThrdInfo(void);   
    extern u8 g_ucPrintMsg;

    void DbgShowEvbSysState(void)
    {
        PRINTF("==========================================================================\n");
        if(EvbMainThreadIsWorking())
        {
            PRINTF("EVB module is working!\n");
        } 
        else
        {
            PRINTF("EVB module is power on!\n");
        }
        DbgShowEvbThrdInfo();
        PRINTF("==========================================================================\n");

        return ;
    }

    void DbgSetMsgPrint(u8 ucOpen)
    {
        g_ucPrintMsg = ucOpen;
        return ;
    }

#ifdef __cplusplus
}
#endif 
