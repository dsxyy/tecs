#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
#
# �ļ����ƣ�user.py
# �������ݣ�userģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
#*******************************************************************************/
import hashlib
#****************************************************************************#
#                             ��������                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
#�û���ɫ
CLOUDADMIN = 1
ACCOUNTADMIN = 2
ACCOUNTUSER = 3
USER_ENABLE = 1
USER_DISABLE = 2
NO_DESCRIPTION = ""
NO_PHONE = ""
NO_EMAIL = ""
NO_ADDR = ""
#****************************************************************************#

def login(server,username,passwd):
    '''��¼tecs����ȡ���ܺ��session'''
    session = '%s:%s' % (username,passwd)
	
	#�˴�����tecs.session.login���������¼ӵĽӿ�
    result = server.tecs.session.login(session)
    if result[0] == 0:
        encryped_session = result[1]
        #print "login session = %s" % encryped_session
        return encryped_session
    else:
        print "login session error info: %s" % result[1]
        return None

def logout(server,session):   	
	#�˴�����tecs.session.login���������¼ӵĽӿ�
    result = server.tecs.session.logout(session)
    
    if result[0] == 0:
        #print "login session = %s" % encryped_session
        return True
    else:	    
        return False		
		
def logqurey(server,session):   	
	#�˴�����tecs.session.login���������¼ӵĽӿ�
    result = server.tecs.session.query(session)
    #����ʧ��������������Ϊsessionû�м��ܣ�������20��ר�����ڷ��ؼ���session�ַ�����
    if result[0] != 0:
        print "result = %s" % result[1]
        return None
    else:	    
        return len(result[1])	

class user:
    ''' tecs user methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session

    def allocate(self,username,password,confirm_pass,role,groupname):
        result = self.apisvr.tecs.user.allocate(self.session,username,password,confirm_pass,role,groupname)
        if result[0] != 0:
            print "failed to create user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create user %s!" % username
            return True

    def delete(self,username):
        result = self.apisvr.tecs.user.delete(self.session,username)
        if result[0] != 0:
            print "failed to delete user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete user %s!" % username
            return True

    def query(self,start_index,query_count,username):
        result = self.apisvr.tecs.user.query(self.session,start_index,query_count,username)
        if result[0] != 0:
            print "failed to query user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query user group %s!" % username
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            userinfo = result[3]
            print "---------show user---------"
            for user in userinfo:
                print "user %s" % username
                for k in user.keys():
                    print "  %s = %s" % (k,user[k])
            return True

    def query_phone(self,start_index,query_count,username):
        result = self.apisvr.tecs.user.query(self.session,start_index,query_count,username)
        if result[0] != 0:
            print "failed to query user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            userinfo = result[3]
            for user in userinfo:
                print "phone = %s" % user["phone"]
                return user["phone"]

    def query_email(self,start_index,query_count,username):
        result = self.apisvr.tecs.user.query(self.session,start_index,query_count,username)
        if result[0] != 0:
            print "failed to query user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            userinfo = result[3]
            for user in userinfo:
                print "email = %s" % user["email"]
                return user["email"]

    def query_location(self,start_index,query_count,username):
        result = self.apisvr.tecs.user.query(self.session,start_index,query_count,username)
        if result[0] != 0:
            print "failed to query user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            userinfo = result[3]
            for user in userinfo:
                print "location = %s" % user["address"]
                return user["address"]

    def query_description(self,start_index,query_count,username):
        result = self.apisvr.tecs.user.query(self.session,start_index,query_count,username)
        if result[0] != 0:
            print "failed to query user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            userinfo = result[3]
            for user in userinfo:
                print "description = %s" % user["description"]
                return user["description"]
            return None

    def query_create_time(self,start_index,query_count,username):
        result = self.apisvr.tecs.user.query(self.session,start_index,query_count,username)
        if result[0] != 0:
            print "failed to query user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            userinfo = result[3]
            for user in userinfo:
                print "create_time = %s" % user["create_time"]
                return user["create_time"]
            return None

    def fullquery(self,start_index,query_count,query_type,query_user):
        '''��ʾ�û�������Ϣ  '''
        result = self.apisvr.tecs.user.full.query(self.session,start_index,query_count,query_type,query_user)
        if result[0] != 0:
            print "failed to query full information of user %s! error code =%d" % (query_user,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            if len(result[3]) == 0:
                print "no user %s!" % query_user
                return True;

            print "user information:"
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            print "---------user list---------"
            user_list = result[3]
            for u in user_list:
                print "user %d:" % u["user_id"]
                for k in u.keys():
                    print "  %s = %s" % (k,u[k])
            return True

    def set(self,username,password,confirm_pass,role,groupname,enable,phone,email,location,description):
        result = self.apisvr.tecs.user.set(self.session,username,password,confirm_pass,role,groupname,enable,1,phone,email,location,description)
        if result[0] != 0:
            print "failed to set user %s! error code = %d" % (username,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set user %s!" % username
            return True        