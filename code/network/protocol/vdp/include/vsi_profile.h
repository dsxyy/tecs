
#ifndef _EVB_VSI_PROFILE_H_
#define _EVB_VSI_PROFILE_H_

#ifdef __cplusplus
extern "C"{
#endif 
	
#include "vdp.h"

/*********************************************
  profile struct define 
  1 vsi profile
  2 group mac vlan 
*********************************************/
typedef struct tagVsiGMVInfo
{
    LIST_ENTRY(tagVsiGMVInfo) lstEntry;
    struct tagVDP_FLT_GROUPMACVID gmv;
}T_VSIGMVINFO;

LIST_HEAD(tagVsiGMVInfo_head,tagVsiGMVInfo);

typedef struct tagVsiProfile
{
    BYTE vsiIDFormat;
    BYTE vsiID[VDPTLV_VSIID_LEN]; 

    LIST_ENTRY(tagVsiProfile) lstEntry;

    BYTE vsiMgrID[VDPTLV_VSIMGRID_LEN];
    BYTE vsitypeID[VDPTLV_VSITYPEID_LEN];
    BYTE vsitypeVer;
    
    BYTE fltFormat; 
    UINT16 fltNumber;
    struct tagVsiGMVInfo_head lstFlt;

}T_VSIPROFILE;
LIST_HEAD(tagVsiProfile_head,tagVsiProfile);

/*********************************************
  vsi profile global
*********************************************/
extern struct tagVsiProfile_head g_lstVsiProfile;

/*********************************************
  vsi profile function
*********************************************/
void vsiProfile_init();
void vsiProfile_shutdown();

struct tagVsiProfile* vsiProfile_getVsi(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN]);

int32 vsiProfile_addvsiID(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE mgrid[VDPTLV_VSIMGRID_LEN],BYTE typeID[VDPTLV_VSIMGRID_LEN],BYTE typeVer);
int32 vsiProfile_delvsiID(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN]);

int32 vsiProfile_addflt(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE fltFormat,struct tagVDP_FLT_GROUPMACVID* pGMV );
int32 vsiProfile_delflt(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE fltFormat,struct tagVDP_FLT_GROUPMACVID* pGMV );

int32 vsiProfile_addvsivlanmac(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE mgrid[VDPTLV_VSIMGRID_LEN],BYTE typeID[VDPTLV_VSIMGRID_LEN],BYTE typeVer,BYTE groupid[EVB_VDP_GROUPID_LEN],u16 vlanid,BYTE mac[EVB_ECP_MAC_LEN] );

int32 DbgShowVsiProfile();

#ifdef __cplusplus
}
#endif 

#endif 
