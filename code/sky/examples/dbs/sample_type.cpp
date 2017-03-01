/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sample_type.cpp
* 文件标识：
* 内容摘要： SampleType 类实现文件
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
#include <iostream>
#include "sample_type.h"

namespace zte_tecs
{

//表名
const char * SampleType::table = "sample_type";

//查询SQL的列
const char * SampleType::db_names = "fld_int2,fld_int4,fld_int8,fld_dec,fld_num,fld_real,"
                                    "fld_double,fld_float,fld_char,fld_var,fld_text,fld_bytea,"
                                    "fld_ts,fld_iv,fld_date,fld_time,fld_bool";
//表的创建SQL，尽量使用SQL标准的数据类型，日期类型尽量不使用
const char * SampleType::db_bootstrap = "create table sample_type ("
    "fld_int2 smallint,             /* SQL标准 */"
    "fld_int4 integer,              /* SQL标准 */"
    "fld_int8 bigint,               /* 扩展非标准，可以使用numeric */"
    "fld_dec  decimal,              /* SQL标准 */" 
    "fld_num  numeric,              /* SQL标准 */"
    "fld_real real,                 /* 依赖下层处理器，操作系统和编译器对它的支持，推荐使用numeric */"
    "fld_double double precision,   /* 依赖下层处理器，操作系统和编译器对它的支持，推荐使用numeric */"
    "fld_float  float,              /* 依赖下层处理器，操作系统和编译器对它的支持，推荐使用numeric */"
    "fld_char   char(32),           /* SQL标准推荐使用varchar */"
    "fld_var    varchar(32),        /* SQL标准 */"
    "fld_text   text,               /* SQL扩展非标准不推荐使用 */"
    "fld_bytea  bytea,              /* SQL扩展非标准不推荐使用 */"
    "fld_ts     timestamp,          /* SQL标准包括日期和时间 */"
    "fld_iv     interval,           /* SQL标准时间间隔 */"
    "fld_date   date,               /* SQL标准只用于日期 */"
    "fld_time   time,               /* SQL标准只用于一日内时间 */"
    "fld_bool   boolean             /* SQL标准 */"
    ");";

/* ************************************************************************** */
/* SampleType constructor/destructor                                             */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
SampleType::SampleType()
{
    _fld_int2   = -1;
    _fld_int4   = -1;
    _fld_int8   = -1;
    _fld_dec    = -1;
    _fld_num    = -1;
    _fld_real   = 0.0;
    _fld_double = 0.0;
    _fld_float  = 0.0;
    _fld_char   = "";
    _fld_var    = "";
    _fld_text   = "";
    _fld_bytea  = "";
    _fld_ts     = "";
    _fld_iv     = "";
    _fld_date   = "";
    _fld_time   = "";
    _fld_bool   = 0;    
}

SampleType::~SampleType()
{
}

/* -------------------------------------------------------------------------- */
int SampleType::Select(SqlDB * db)
{
    ostringstream   oss;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&SampleType::SelectCallback));

    //避免使用SELECT *，应该明确要查询的列，这样在表结构升级时才不会有问题
    oss << "SELECT "<< db_names << " FROM " << table;

    rc = db->Execute(oss,this);

    UnsetCallback();

    return rc;
}

/* -------------------------------------------------------------------------- */
int SampleType::Insert(SqlDB * db, string& error_str)
{
    ostringstream   oss;
    
    //此处的处理比较简单，应用应该设置属性，再调用insert
    oss << "insert into sample_type values ";
    oss <<"(255, 65535, -1, 123456, 654321, 1.0, 2.0, 3.0, 'char', 'varchar', 'text', 'abcd', ";
    oss << "CURRENT_TIMESTAMP,CURRENT_TIMESTAMP-CURRENT_TIMESTAMP,CURRENT_DATE,CURRENT_TIME,'true')";
    
    int rc = db->Execute(oss);
    
    return rc;
}
/* -------------------------------------------------------------------------- */
int SampleType::Update(SqlDB * db)
{
    ostringstream   oss;
    
    //此处的处理比较简单，应用应该首先select，然后修改某些属性，最后再调用update
    oss << "update " << table << " set fld_bytea = 'hello world'";
    
    int rc = db->Execute(oss);
    
    return rc;    
}
/* -------------------------------------------------------------------------- */
int SampleType::Drop(SqlDB * db)        
{
    ostringstream   oss;
    
    //此处的处理比较简单，应用应该设置某些属性，再调用delete
    oss << "delete from " << table;

    int rc = db->Execute(oss);
    
    return rc;    
}

/* -------------------------------------------------------------------------- */
int SampleType::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns ||
         LIMIT != columns->get_column_num()
       )
        return -1;
    
    _fld_int2   = columns->GetValueInt(FLD_INT2);
    _fld_int4   = columns->GetValueInt(FLD_INT4);
    _fld_int8   = columns->GetValueInt64(FLD_INT8);
    _fld_dec    = columns->GetValueInt64(FLD_DEC);
    _fld_num    = columns->GetValueInt64(FLD_NUM);
    _fld_real   = columns->GetValueDouble(FLD_REAL);
    _fld_double = columns->GetValueDouble(FLD_DOUBLE);
    _fld_float  = columns->GetValueDouble(FLD_FLOAT);
    _fld_char   = columns->GetValueText(FLD_CHAR);
    _fld_var    = columns->GetValueText(FLD_VAR);
    _fld_text   = columns->GetValueText(FLD_TEXT);
    _fld_bytea  = columns->GetValueText(FLD_BYTEA);
    _fld_ts     = columns->GetValueText(FLD_TS);
    _fld_iv     = columns->GetValueText(FLD_IV);
    _fld_date   = columns->GetValueText(FLD_DATE);
    _fld_time   = columns->GetValueText(FLD_TIME);
    _fld_bool   = columns->GetValueInt(FLD_BOOL);
	
    ostringstream   oss_dump;
    SampleType::Dump(oss_dump, columns);

    cout << oss_dump.str() << endl;	
    
    return 0;
}


int SampleType::Dump(ostringstream& oss, SqlColumn * columns)
{
    //根据需要组织查询结果
    //资源池dump回调函数中调用
	
    oss <<"int2:"  << columns->GetValueInt(FLD_INT2);
    oss <<",int4:" << columns->GetValueInt(FLD_INT4); 
    oss <<",int8:" <<columns->GetValueInt64(FLD_INT8); 
    oss <<",decimal:" << columns->GetValueInt64(FLD_DEC);
    oss <<",numebic:" <<columns->GetValueInt64(FLD_NUM); 
    oss <<",real:" <<columns->GetValueDouble(FLD_REAL); 
    oss <<",double:" <<columns->GetValueDouble(FLD_DOUBLE);
    oss <<",float:" <<columns->GetValueDouble(FLD_FLOAT); 
    oss <<",char:" <<columns->GetValueText(FLD_CHAR); 
    oss <<",varchar:" <<columns->GetValueText(FLD_VAR);
    oss <<",text:" <<columns->GetValueText(FLD_TEXT); 
    oss <<",bytea:" <<columns->GetValueText(FLD_BYTEA);
    oss <<",timestamp:" <<columns->GetValueText(FLD_TS); 
    oss <<",interval:" <<columns->GetValueText(FLD_IV); 
    oss <<",date:" <<columns->GetValueText(FLD_DATE); 
    oss <<",time:" <<columns->GetValueText(FLD_TIME); 
    oss <<",bool:" << columns->GetValueInt(FLD_BOOL);
	
    return 0;
}
/* -------------------------------------------------------------------------- */
}  //namespace zte_tecs
