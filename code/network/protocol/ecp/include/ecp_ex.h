#ifndef _ECP_EX_H
#define _ECP_EX_H

#ifdef __cplusplus
extern "C"{
#endif 
    
// ecp port
int list_EcpULPDUtx_insert(DWORD32 ifIndex,struct ecp_ecpdu * pIn, DWORD32 len);
int list_EcpULPDUrx_remove(DWORD32 ifIndex,struct ecp_ecpdu * pIn);

#ifdef __cplusplus
}
#endif 
#endif //_ECP_EX_H

