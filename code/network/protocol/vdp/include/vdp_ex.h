#ifndef _VDP_EX_H
#define _VDP_EX_H

#ifdef __cplusplus
extern "C"{
#endif 
    
void vdp_vm_api(UINT32 ifIndex,BYTE vsiIDFormat, char vsiID[VDPTLV_VSIID_LEN], BYTE oper);

#ifdef __cplusplus
}
#endif 
#endif //_VDP_EX_H

