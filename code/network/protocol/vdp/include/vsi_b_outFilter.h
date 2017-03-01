
#ifndef _EVB_VSI_B_OUTFILTER_H_
#define _EVB_VSI_B_OUTFILTER_H_

#ifdef __cplusplus
extern "C"{
#endif 

#include "vdp.h"
#include "vsi_profile.h"

/*********************************************
  VSI bridge out filter struct define 
  ∫Õvsi profile ¿‡À∆;
*********************************************/

typedef struct tagVsiBOutFilter
{
    BYTE vsiIDFormat;
    BYTE vsiID[VDPTLV_VSIID_LEN]; 

    u32 port;
    LIST_ENTRY(tagVsiBOutFilter) lstEntry;
    BYTE tlvType;
    BYTE vsiMgrID[VDPTLV_VSIMGRID_LEN];
    BYTE vsitypeID[VDPTLV_VSITYPEID_LEN];
    BYTE vsitypeVer;
    
    BYTE fltFormat; 
    UINT16 fltNumber;
    struct tagVsiGMVInfo_head lstFlt;

}T_VSIBOUTFILTER;
LIST_HEAD(tagVsiBOutFilter_head,tagVsiBOutFilter);

/*********************************************
  vsiBOutFilter global
*********************************************/
extern struct tagVsiBOutFilter_head g_lstVsiBOutFilter;

/*********************************************
  vsiBOutFilter function
*********************************************/
void vsiBOutFlt_init();
void vsiBOutFlt_shutdown();

struct tagVsiBOutFilter* vsiBOutFlt_getVsi(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN]);

int32 vsiBOutFlt_addvsiID(u32 port,BYTE mgrID[VDPTLV_VSIID_LEN],struct tagVdpTlv_VDPAss * vdpass);
int32 vsiBOutFlt_delvsiID(u32 port,BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN]);

int32 DbgShowVsiOutFlt(u32 port);
#ifdef __cplusplus
}
#endif 

#endif 
