#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：postgresql.py
# 测试内容：数据库接口的封装
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/12/01
#*******************************************************************************/
import psycopg2 
import sys

class postgresql_db:
    ''' tecs postgresql_db methods '''
    def __init__(self,host,port,user,passwd,db_name):
        try:
            self.dsn = "host=" + host + " port=" + port  + " dbname=" + db_name + " user=" + user + " password=" + passwd
            self.db_client = psycopg2.connect(self.dsn)  
            self.cursor = self.db_client.cursor()
            self.tc_db_name=db_name;
        except Exception, e:
            print "connect except: %s!!!" % e
            sys.exit(0)
        
    def __del__(self):
        self.cursor.close()
        self.db_client.close()
        
    
    def query_from_tc(self, table_name, clum, where):
        results=[]
        try:
            self.cursor.execute('select %s from %s where %s' % (clum, table_name, where))
            rows = self.cursor.fetchall ()
            for row in rows:
                results.append('%s' %(row)) 
        except Exception, e:
            print "execute db except: %s!!!" % e
        return results
        
    # 在storage_cluster表中插入数据
    def add_to_storage_cluster(self, clum1, clum2):
        try:
            self.cursor.execute("insert into storage_cluster(sid,cluster_name) values(%ld,'%s')" % (clum1, clum2))
            self.cursor.execute("commit")
        except Exception, e:
            print "execute db except: %s!!!" % e
            return False
        return True

    def query_from_cc(self, cc_name, table_name, clum, where):
        results=[]
        cc_host = self.query_from_tc("cluster_pool", "db_server_url", "name='"+cc_name+"'")
        cc_port = self.query_from_tc("cluster_pool", "db_server_port", "name='"+cc_name+"'")
        cc_user = self.query_from_tc("cluster_pool", "db_user", "name='"+cc_name+"'")
        cc_pwd  = self.query_from_tc("cluster_pool", "db_passwd", "name='"+cc_name+"'")
        cc_dbname = self.query_from_tc("cluster_pool", "db_name", "name='"+cc_name+"'")
        try:
            dsn_format = 'host=%s port=%s dbname=%s user=%s password=%s' %(cc_host[0], cc_port[0], cc_dbname[0], cc_user[0], cc_pwd[0])
            self.cc_dsn = dsn_format
            self.cc_db_client = psycopg2.connect(self.cc_dsn)  
            self.cc_cursor = self.cc_db_client.cursor()
        except Exception, e:
            print "connect except: %s!!!" % e
            sys.exit(0)
        
        cmd = 'select %s from %s where %s' % (clum, table_name, where)
        print "query cc db:",;print cmd
        self.cc_cursor.execute(cmd)
        rows = self.cc_cursor.fetchall ()
        self.cc_cursor.close()
        self.cc_db_client.close()
        for row in rows:
            results.append('%s' %(row)) 
        return results
        
    def query_from_cc_ex(self, cc_name, table_name, columns, where):
        results=[]
        cc_host = self.query_from_tc("cluster_pool", "db_server_url", "name='"+cc_name+"'")
        cc_port = self.query_from_tc("cluster_pool", "db_server_port", "name='"+cc_name+"'")
        cc_user = self.query_from_tc("cluster_pool", "db_user", "name='"+cc_name+"'")
        cc_pwd  = self.query_from_tc("cluster_pool", "db_passwd", "name='"+cc_name+"'")
        cc_dbname = self.query_from_tc("cluster_pool", "db_name", "name='"+cc_name+"'")
        try:
            dsn_format = 'host=%s port=%s dbname=%s user=%s password=%s' %(cc_host[0], cc_port[0], cc_dbname[0], cc_user[0], cc_pwd[0])
            self.cc_dsn = dsn_format
            self.cc_db_client = psycopg2.connect(self.cc_dsn)  
            self.cc_cursor = self.cc_db_client.cursor()
        except Exception, e:
            print "connect except: %s!!!" % e
            sys.exit(0)
        
        cmd = 'select %s from %s where %s' % (columns, table_name, where)
        print "query cc db:",;print cmd
        self.cc_cursor.execute(cmd)
        rows = self.cc_cursor.fetchall ()
        self.cc_cursor.close()
        self.cc_db_client.close()
        return rows


    
