/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_api.h
* �ļ���ʶ��
* ����ժҪ��ӳ��ģ�����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��10��19��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#ifndef TECS_FILE_API_H
#define TECS_FILE_API_H
#include "rpcable.h"
#include "api_error.h"



/**
@brief ��������: �ļ���ѯ�ӿڵĲ������� \n
@li @b ����˵��: ��
*/
class ApiCoredumpUrlInfo : public Rpcable
{
public:
    ApiCoredumpUrlInfo(){};
    ~ApiCoredumpUrlInfo(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(access_type);
        TO_VALUE(url);
        TO_VALUE(path);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(access_type);
        FROM_VALUE(url);
        FROM_VALUE(path);
        FROM_RPC_END();
    };

    string            access_type;
    string            url;  
    string            path;
};


#endif

