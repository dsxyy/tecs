#ifndef _OAM_CRC32_H_
#define _OAM_CRC32_H_

/*
#ifdef  __cplusplus
extern "C" {
#endif
*/

#include "oam_pub_type.h"
extern WORD32  g_adwCRC32Table[256]; 
#if 0
void InitCRC32Table(void);
DWORD Reflect(DWORD dwRef, BYTE ucCount);
#endif

WORD32 Oam_CountCRC32(BYTE *pucData , WORD32 dwLen, WORD32 dwPreCRC);
/*
#ifdef  __cplusplus
}
#endif
*/

#endif
