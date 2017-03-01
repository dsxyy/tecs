/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�clireglib.h
* �ļ���ʶ��
* ����ժҪ��OAM����ģ��CLI����ע�����ӿڽṹ�嶨��
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����
* ������ڣ�2008.9.18
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�2010��9��2��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ������ṩ��SBC��ҵ��ע��dat�ű��ĺ���OamCliRegFuncSpecial
* �޸ļ�¼2����
**********************************************************************/
/**
  @file clireglib.h
  @brief CLIע��ӿں�������
  @page CLIREGLIB OAM��ϵͳ��CLI����ע�ᣩ�ӿڹ淶
  @author  OAM����3Gƽ̨

���ĵ��������CLI����ע��Ľӿ����ݡ�\n

*/
#ifndef _OAM_CLI_REG_LIB_H_
#define _OAM_CLI_REG_LIB_H_

/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "oam_cfg.h"

/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/
/*---------------------------------------------------
                     ע�ắ������ֵ����
--------------------------------------------------*/
#define  OAM_CLI_SUCCESS  0 /*�ɹ�*/
#define  OAM_CLI_FAIL     1 /*ʧ��*/

/**
  @brief ƽ̨Ӧ����OAM����ע����Ҫ�������õ���Ϣ
  @param tJobID ��OAM�������ý�����JID
  @param dwDatFuncType ��JOB��OAM�������ý���ʱ��ʹ�õ���DAT�ű�����  
  @return  OAM_CLI_SUCCESS(0)���ɹ�    OAM_CLI_FAIL(1)��   ʧ��
  <HR>
  @b �޸ļ�¼��
*/
BYTE  OamCliRegFunc(JID tJobID, WORD32 dwDatFuncType);

/**
  @brief ƽ̨Ӧ����OAM����ע���������ù�����Ϣ����OAM���յ�OSS
  @param tJobID  ��OAM�������ý�����JID  
  @return  OAM_CLI_SUCCESS(0)���ɹ�    OAM_CLI_FAIL(1)��   ʧ��
  <HR>
  @b �޸ļ�¼��
*/
BYTE  OamCliUnRegFunc(JID tJobID);

/**
  @brief ��ƽ̨��Ӧ��/ҵ�������OAM����ע����Ҫ�������õ���Ϣ
  @param tJobID  ��OAM�������ý�����JID  
  @param dwNetId  LOMP���ڵ���Ԫ��ʶ
  @return  OAM_CLI_SUCCESS(0)���ɹ�    OAM_CLI_FAIL(1)��   ʧ��
  <HR>
  @b �޸ļ�¼��
*/
BYTE  OamCliRegFuncEx(JID tJobID, WORD32 dwNetId);

/**
  @brief SBC��ҵ�������OAM����ע����Ҫ�������õ���Ϣ
  @param tJobID  ��OAM�������ý�����JID  
  @param dwDatFuncType ��JOB��OAM�������ý���ʱ��ʹ�õ���DAT�ű�����
  @return  OAM_CLI_SUCCESS(0)���ɹ�    OAM_CLI_FAIL(1)��   ʧ��
  <HR>
  @b �޸ļ�¼��
*/
BYTE  OamCliRegFuncSpecial(JID tJobID, WORD32 dwDatFuncType);

#endif

