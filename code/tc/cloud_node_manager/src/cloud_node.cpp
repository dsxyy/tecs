/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cloud_node.cpp
* �ļ���ʶ��
* ����ժҪ��cloud�ڵ�����ݿ���ز���
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��8��23��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��8��23��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
******************************************************************************/

#include "cloud_node.h"
#include "db_config.h"


namespace zte_tecs{

const char * CloudNodePool::_table = "cloud_node";

const char * CloudNodePool::_col_names = "ip_address,is_master";
       
CloudNodePool *CloudNodePool::_instance = NULL;

/******************************************************************************/
int CloudNodePool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    CloudNode  node;
    columns->GetValue(CloudNodePool::IP_ADDRESS,   node._ip_address);
    columns->GetValue(CloudNodePool::IS_MASTER,    node._is_master);
    
    vector<CloudNode>* p = static_cast<vector<CloudNode> *>(nil);
    p->push_back(node);
    return 0;
}

/******************************************************************************/
int CloudNodePool::SelectNodes(vector<CloudNode>& nodes)
{
    ostringstream   oss;
    int             rc;


    oss << "SELECT " << _col_names<< 
           " FROM " << _table;


    SetCallback(static_cast<Callbackable::Callback>(&CloudNodePool::SelectCallback),
                &nodes);
    rc = _db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int CloudNodePool::UpdateNodes(vector<CloudNode>& nodes)
{
    int rc = -1;
    
    SqlCommand sql(_db,_table);

    rc = sql.Delete();    
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    /*vector<CloudNode>::const_iterator  it;
    for(it = nodes.begin(); it != nodes.end(); it++)
    {
        sql.Clear();
        sql.Add("ip_address",   it->_ip_address);
        sql.Add("is_master",   it->_is_master);
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }*/
    return InsertNodes(nodes);
}

/******************************************************************************/
int CloudNodePool::DropNodes()
{
    ostringstream oss;
    int rc;

    oss << "DELETE FROM " << _table ;

    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = _db->Execute(oss);
    }
    return rc;
}


/******************************************************************************/
int CloudNodePool::InsertNodes(const vector<CloudNode>& nodes)
{
    int rc = 0;

    SqlCommand sql(_db,_table);

    vector<CloudNode>::const_iterator it;
    for(it = nodes.begin(); it != nodes.end(); it++)
    {
        sql.Clear();
        sql.Add("ip_address",   it->_ip_address);
        sql.Add("is_master",     it->_is_master);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

}

