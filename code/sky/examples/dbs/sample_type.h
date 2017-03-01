/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sample_type.h
* �ļ���ʶ��
* ����ժҪ�� SampleType �ඨ���ļ�
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

#ifndef DBS_SAMPLE_TYPE_SQL_H_
#define DBS_SAMPLE_TYPE_SQL_H_

#include "object_sql.h"

using namespace std;

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

/**
 @brief SampleType ��. �������Ͳ�����
 @li @b ����˵������
 */ 
class  SampleType: public ObjectSQL
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    SampleType();

    virtual ~SampleType();
    
    //������Ҫ��֯��ѯ���
    //��Դ��dump�ص������е���
    static int Dump(ostringstream& oss, SqlColumn * columns);
    
    //�˴���Ӧ�ö���һ�ѵ����Ե�set get����
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
    //���Դ�������Ƕ���Ϊpublic
    virtual int Select(SqlDB * db);

    virtual int Insert(SqlDB * db, string& error_str);
  
    virtual int Update(SqlDB * db);

    virtual int Drop(SqlDB * db);
    
    //������
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
    
    //��ѯ�ص�����
    int SelectCallback(void *nil, SqlColumn * columns);

    //����
    static const char * table;

    //��ѯ����
    static const char * db_names;
    
    //�������SQL
    static const char * db_bootstrap;

    //�����ֶ�
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

