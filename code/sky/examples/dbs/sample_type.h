/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sample_type.h
* 文件标识：
* 内容摘要： SampleType 类定义文件
* 其它说明：
* 当前版本： 1.0
* 作    者： KIMI
* 完成日期： 2011-06-21
*
*    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
* 修 改 人：
* 修改日期：
* 修改内容：
**********************************************************************/

#ifndef DBS_SAMPLE_TYPE_SQL_H_
#define DBS_SAMPLE_TYPE_SQL_H_

#include "object_sql.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

/**
 @brief SampleType 类. 数据类型测试类
 @li @b 其它说明：无
 */ 
class  SampleType: public ObjectSQL
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    SampleType();

    virtual ~SampleType();
    
    //根据需要组织查询结果
    //资源池dump回调函数中调用
    static int Dump(ostringstream& oss, SqlColumn * columns);
    
    //此处还应该定义一堆的属性的set get函数
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
    //测试代码把它们定义为public
    virtual int Select(SqlDB * db);

    virtual int Insert(SqlDB * db, string& error_str);
  
    virtual int Update(SqlDB * db);

    virtual int Drop(SqlDB * db);
    
    //创建表
    static int BootStrap(SqlDB *db)
    {
        ostringstream oss(SampleType::db_bootstrap);
        
        return db->Execute(oss);
    }
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

    enum ColNames {
    FLD_INT2 = 0,
    FLD_INT4 = 1,
    FLD_INT8 = 2,
    FLD_DEC  = 3,
    FLD_NUM  = 4,
    FLD_REAL = 5,
    FLD_DOUBLE = 6,
    FLD_FLOAT  = 7,
    FLD_CHAR   = 8,
    FLD_VAR    = 9,
    FLD_TEXT   = 10,
    FLD_BYTEA  = 11,
    FLD_TS     = 12,
    FLD_IV     = 13,
    FLD_DATE   = 14,
    FLD_TIME   = 15,
    FLD_BOOL   = 16,
    LIMIT      = 17    
    };
    DISALLOW_COPY_AND_ASSIGN(SampleType);
    
    //查询回调函数
    int SelectCallback(void *nil, SqlColumn * columns);

    //表名
    static const char * table;

    //查询的列
    static const char * db_names;
    
    //创建表的SQL
    static const char * db_bootstrap;

    //各个字段
    int _fld_int2;
    int _fld_int4;
    int64 _fld_int8;
    int64 _fld_dec;
    int64 _fld_num;
    double _fld_real;
    double _fld_double;
    double _fld_float;
    string _fld_char;
    string _fld_var;
    string _fld_text;
    string _fld_bytea;
    string _fld_ts;
    string _fld_iv;
    string _fld_date;
    string _fld_time;
    int    _fld_bool;    
};

}  //namespace zte_tecs
#endif /*DBS_SAMPLE_TYPE_SQL_H_*/

