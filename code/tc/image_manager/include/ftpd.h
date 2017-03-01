/*********************************************************************
 * 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
 *
 * 文件名称： ossserv_dhcp.c
 * 文件标识：
 * 内容摘要： 实现DHCP协议的ip管理功能
 * 其它说明： 无
 * 当前版本： V1.0
 * 作    者： 李忠雷——3G平台
 * 完成日期：20080918
 *
 **********************************************************************/
#ifndef TECS_FTPD_H_
#define TECS_FTPD_H_

#define ERR_OSSSVR_START            0x890 /*  在TULIP中使用OSS_VOS的最后错误号码段    */
#define ERR_OSSSVR_PARAM_ERROR      (ERR_OSSSVR_START + 0) /*  参数出错         */
#define ERR_OSSSVR_THREAD           (ERR_OSSSVR_START + 1) /*  创建线程失败     */
#define ERR_OSSSVR_SOCKET           (ERR_OSSSVR_START + 2) /*  创建socket失败   */
#define ERR_OSSSVR_MEM              (ERR_OSSSVR_START + 3) /*  内存不够         */
#define ERR_OSSSVR_NOT_FOUND        (ERR_OSSSVR_START + 4) /*  文件或目录或指定目标不存在 */
#define ERR_OSSSVR_EXIST            (ERR_OSSSVR_START + 5) /*  文件或目录或其它目标已存在 */
#define ERR_OSSSVR_FULL             (ERR_OSSSVR_START + 6)  /*  已满            */
#define ERR_OSSSVR_NULL             (ERR_OSSSVR_START + 7)  /*  空              */
#define ERR_OSSSVR_OTHER            (ERR_OSSSVR_START + 9)  /*  其它错误        */

#include "image_db.h"

STATUS FtpdInit();
STATUS EnableFtpdOptions();
STATUS GetUpLoadUrl(UploadUrl &url);
STATUS GetFtpSvrInfo(int& port,string& ip);

#endif

