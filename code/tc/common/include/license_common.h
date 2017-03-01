/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�license_common.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��֤�������ģ��ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��10��11��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/10/11
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
******************************************************************************/
#include "sky.h"
#include "pool_sql.h"
#include "license.h"

enum LicensedObject
{
    LICOBJ_CREATED_VMS = 0,
    LICOBJ_DEPLOYED_VMS = 1,
    LICOBJ_IMAGES = 2,
    LICOBJ_USERS = 3,
    LICOBJ_CLUSTERS = 4
};

int64 GetObjectCount(SqlCallbackable& sqlcb, LicensedObject object);
STATUS GetRunningStat(RunningStat& stat);
int LicenseRemainingDays();
void StopAllVM();
STATUS ValidateLicense(LicensedObject object, int64 increment);


