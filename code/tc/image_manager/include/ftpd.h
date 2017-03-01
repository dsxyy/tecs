/*********************************************************************
 * ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
 *
 * �ļ����ƣ� ossserv_dhcp.c
 * �ļ���ʶ��
 * ����ժҪ�� ʵ��DHCPЭ���ip������
 * ����˵���� ��
 * ��ǰ�汾�� V1.0
 * ��    �ߣ� �����ס���3Gƽ̨
 * ������ڣ�20080918
 *
 **********************************************************************/
#ifndef TECS_FTPD_H_
#define TECS_FTPD_H_

#define ERR_OSSSVR_START            0x890 /*  ��TULIP��ʹ��OSS_VOS������������    */
#define ERR_OSSSVR_PARAM_ERROR      (ERR_OSSSVR_START + 0) /*  ��������         */
#define ERR_OSSSVR_THREAD           (ERR_OSSSVR_START + 1) /*  �����߳�ʧ��     */
#define ERR_OSSSVR_SOCKET           (ERR_OSSSVR_START + 2) /*  ����socketʧ��   */
#define ERR_OSSSVR_MEM              (ERR_OSSSVR_START + 3) /*  �ڴ治��         */
#define ERR_OSSSVR_NOT_FOUND        (ERR_OSSSVR_START + 4) /*  �ļ���Ŀ¼��ָ��Ŀ�겻���� */
#define ERR_OSSSVR_EXIST            (ERR_OSSSVR_START + 5) /*  �ļ���Ŀ¼������Ŀ���Ѵ��� */
#define ERR_OSSSVR_FULL             (ERR_OSSSVR_START + 6)  /*  ����            */
#define ERR_OSSSVR_NULL             (ERR_OSSSVR_START + 7)  /*  ��              */
#define ERR_OSSSVR_OTHER            (ERR_OSSSVR_START + 9)  /*  ��������        */

#include "image_db.h"

STATUS FtpdInit();
STATUS EnableFtpdOptions();
STATUS GetUpLoadUrl(UploadUrl &url);
STATUS GetFtpSvrInfo(int& port,string& ip);

#endif

