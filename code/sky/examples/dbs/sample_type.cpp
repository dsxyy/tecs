/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sample_type.cpp
* �ļ���ʶ��
* ����ժҪ�� SampleType ��ʵ���ļ�
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� KIMI
* ������ڣ� 2011-06-21
*
*    �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
* �� �� �ˣ�
* �޸����ڣ�
* �޸����ݣ�
**********************************************************************/
#include <iostream>
#include "sample_type.h"

namespace zte_tecs
{

//����
const char * SampleType::table = "sample_type";

//��ѯSQL����
const char * SampleType::db_names = "fld_int2,fld_int4,fld_int8,fld_dec,fld_num,fld_real,"
                                    "fld_double,fld_float,fld_char,fld_var,fld_text,fld_bytea,"
                                    "fld_ts,fld_iv,fld_date,fld_time,fld_bool";
//��Ĵ���SQL������ʹ��SQL��׼���������ͣ��������;�����ʹ��
const char * SampleType::db_bootstrap = "create table sample_type ("
    "fld_int2 smallint,             /* SQL��׼ */"
    "fld_int4 integer,              /* SQL��׼ */"
    "fld_int8 bigint,               /* ��չ�Ǳ�׼������ʹ��numeric */"
    "fld_dec  decimal,              /* SQL��׼ */" 
    "fld_num  numeric,              /* SQL��׼ */"
    "fld_real real,                 /* �����²㴦����������ϵͳ�ͱ�����������֧�֣��Ƽ�ʹ��numeric */"
    "fld_double double precision,   /* �����²㴦����������ϵͳ�ͱ�����������֧�֣��Ƽ�ʹ��numeric */"
    "fld_float  float,              /* �����²㴦����������ϵͳ�ͱ�����������֧�֣��Ƽ�ʹ��numeric */"
    "fld_char   char(32),           /* SQL��׼�Ƽ�ʹ��varchar */"
    "fld_var    varchar(32),        /* SQL��׼ */"
    "fld_text   text,               /* SQL��չ�Ǳ�׼���Ƽ�ʹ�� */"
    "fld_bytea  bytea,              /* SQL��չ�Ǳ�׼���Ƽ�ʹ�� */"
    "fld_ts     timestamp,          /* SQL��׼�������ں�ʱ�� */"
    "fld_iv     interval,           /* SQL��׼ʱ���� */"
    "fld_date   date,               /* SQL��׼ֻ�������� */"
    "fld_time   time,               /* SQL��׼ֻ����һ����ʱ�� */"
    "fld_bool   boolean             /* SQL��׼ */"
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

    //����ʹ��SELECT *��Ӧ����ȷҪ��ѯ���У������ڱ�ṹ����ʱ�Ų���������
    oss << "SELECT "<< db_names << " FROM " << table;

    rc = db->Execute(oss,this);

    UnsetCallback();

    return rc;
}

/* -------------------------------------------------------------------------- */
int SampleType::Insert(SqlDB * db, string& error_str)
{
    ostringstream   oss;
    
    //�˴��Ĵ���Ƚϼ򵥣�Ӧ��Ӧ���������ԣ��ٵ���insert
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
    
    //�˴��Ĵ���Ƚϼ򵥣�Ӧ��Ӧ������select��Ȼ���޸�ĳЩ���ԣ�����ٵ���update
    oss << "update " << table << " set fld_bytea = 'hello world'";
    
    int rc = db->Execute(oss);
    
    return rc;    
}
/* -------------------------------------------------------------------------- */
int SampleType::Drop(SqlDB * db)        
{
    ostringstream   oss;
    
    //�˴��Ĵ���Ƚϼ򵥣�Ӧ��Ӧ������ĳЩ���ԣ��ٵ���delete
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
    //������Ҫ��֯��ѯ���
    //��Դ��dump�ص������е���
	
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
