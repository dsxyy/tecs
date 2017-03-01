/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cloud_node.h
* 文件标识：
* 内容摘要：cloud节点的头文件
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年8月23日
*
* 修改记录1：
*     修改日期：2012年8月23日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/

#ifndef TC_CLOUD_NODE_H
#define TC_CLOUD_NODE_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "pool_object_sql.h"
#include "tecs_pub.h"

namespace zte_tecs
{

class CloudNode: public Serializable
{        
public:
    enum MsState
    {
        MASTER = 0,
        NO_MASTER   = 1
    };
    CloudNode(){_is_master=MASTER;};
    CloudNode(const string & ip_address,int is_master)
    {
       _ip_address      = ip_address;
       _is_master       = is_master;
    }        
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(CloudNode);
        WRITE_VALUE(_ip_address);
        WRITE_VALUE(_is_master);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(CloudNode);
        READ_VALUE(_ip_address);
        READ_VALUE(_is_master);
        DESERIALIZE_END();    
    };

    string   _ip_address;
    int      _is_master;
};


class CloudNodePool: public Callbackable
{
public:
    static CloudNodePool* GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
      
    static CloudNodePool* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new CloudNodePool(db); 
        }
        
        return _instance;
    };
    
    int SelectNodes(vector<CloudNode>& nodes);
    int UpdateNodes(vector<CloudNode>& nodes);
    int DropNodes();
    int InsertNodes(const vector<CloudNode> &nodes);
private:
    enum ColNames 
    {
        IP_ADDRESS      = 0,
        IS_MASTER       = 1,
        LIMIT           = 2
    };
    
    CloudNodePool(SqlDB *pDb)
    {
        _db = pDb;
    };
    int SelectCallback(void *nil, SqlColumn * columns);

    DISALLOW_COPY_AND_ASSIGN(CloudNodePool);
    static CloudNodePool    *_instance;
    SqlDB                   *_db;
    static const char       *_table;    
    static const char       *_col_names;    
};

}

#endif /* end TC_CLOUD_NODE_H */

