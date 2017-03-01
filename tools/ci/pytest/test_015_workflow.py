#!/usr/bin/python
#-*- coding:utf-8 -*   
import psycopg2 
import sys
import sys,os,getopt,time,inspect
import hashlib
import unittest 
from tecs.image  import *
from tecs.project  import * 
from tecs.vmcfg  import * 
from tecs.workflow  import * 
from contrib  import * 
from contrib import xmlrpclibex
from ftplib import FTP
import string
import time
#from openpyxl.reader.excel import load_workbook
#from openpyxl.workbook import Workbook       
#from openpyxl.writer.excel import ExcelWriter     
#from openpyxl.cell import get_column_letter  
#from openpyxl.reader.excel import load_workbook  
#import xlrd
import random

server_addr = "10.43.179.144"
server_port = "8080"
test_cloud = "tecscloud"
test_cluster = "tecscluster"
test_root_user = "admin"
test_root_passwd = "admin"
operation = ""
ignorant_vm_list = [] 
#ignorant_vms=('2','3')
 
def login(server,username,passwd):
    '''µÇ¼tecs£¬»ñÓܺóession'''
    session = '%s:%s' % (username,passwd)
    
    result = server.tecs.session.login(session)
    if result[0] == 0:
        encryped_session = result[1]
        return encryped_session
    else:
        print "login session error info: %s" % result[1]
        return None

def logout(server,session):
    result = server.tecs.session.logout(session)
    
    if result[0] == 0:
        return True
    else:
        return False

def usage():
    ''' ´òïÐϢ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -p,--operation, start or stop"
    sys.exit(0)
    
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
            f.write("connect except: %s!!!\n" % e)
            sys.exit(0)
        
    def __del__(self):
        self.cursor.close()
        self.db_client.close()        

    def query(self, table_name, clum, where):
        results=[]
        try:
            self.cursor.execute('select %s from %s %s' % (clum, table_name, where))
            results = self.cursor.fetchall ()
        except Exception, e:
            print "execute db except: %s!!!" % e
        return results

def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        f.write(time.strftime('%Y-%m-%d-%H-%M-%S ',time.localtime(time.time()))+"tecs ci test exit code=%d" %value+",error string="+error+"\n")
        sys.exit(value)
        
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url,allow_none=True)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")
root_session = login(server,test_root_user,test_root_passwd)
if root_session is None:
    ci_exit(-1,"root_session is none") 
#w_addr["target_workflow"] = [{"target":"tecscluster.cc.workflow_engine_0","workflow_id":"w-3e16ad30-60aa-4aa3-ae14-5b69904b0209"},{"target":v_target,"workflow_id":v_workflow_id}]
#w_addr["target_workflow"] = [{"target":v_target,"workflow_id":v_workflow_id},{"target":v_target,"workflow_id":v_workflow_id}]

w = worflow(server,root_session)
p = project(server,root_session)
i =  image(server,root_session)
v = vmcfg(server,root_session)
w_addr = {}

def workflow_max_query(target,workflow_id):
    w_addr["workflows"] = [{"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id}]
    w_states = w.query(w_addr)
    
    w_addr["workflows"] = [{"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id},
                            {"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id}]
    w_states = w.query(w_addr)
    
def workflow_empty_query(target,workflow_id):
    v_target,v_workflow_id = v.image_back_create(long(2),"hda")  
    w_addr["workflows"] = []
    w_states = w.query(w_addr)
    w_addr["workflows"] = [{"engine":"","id":""}]
    w_states = w.query(w_addr)
    w_addr["workflows"] = [{"engine":"","id":"111"}]
    w_states = w.query(w_addr)
    w_addr["workflows"] = [{"engine":"111","id":""}]
    w_states = w.query(w_addr)

   
def workflow_query(target,workflow_id):
    w_addr["workflows"] = [{"engine":target,"id":workflow_id},{"engine":target,"id":workflow_id}]
    while 1:
        w_states = w.query(w_addr)
        if w_states[0]["state"] == 2:
            continue;
        if w_states[0]["state"] != 0 :
            print"worflow %s fail to query errstate.............................%d" % (w_states[0]["id"] ,w_states[0]["state"])
            break;
        time.sleep(20)
    
def workflow_project_action_query():
    p_target,p_workflow_id = p.action('worflow',"project_deploy")
    workflow_query(p_target,p_workflow_id)
    p_target,p_workflow_id = p.action("worflow","project_reset")
    workflow_query(p_target,p_workflow_id)
    p_target,p_workflow_id = p.action("worflow","project_reboot")
    workflow_query(p_target,p_workflow_id)
    p_target,p_workflow_id = p.action("worflow","project_cancel")
    workflow_query(p_target,p_workflow_id)
        
def workflow_vmcfg_action_query():
    v_target,v_workflow_id = v.action2(long(2),"deploy")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"stop")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id =  v.action2(long(2),"upload_img")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id =  v.action2(long(2),"cancel_upload_img")
    workflow_query(v_target,v_workflow_id)

    v_target,v_workflow_id =  v.action2(long(2),"recover_image")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id =  v.action2(long(2),"freeze")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id =  v.action2(long(2),"unfreeze")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"start")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"pause")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"resume")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"reset")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"destroy")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"start")
    workflow_query(v_target,v_workflow_id)
    time.sleep(20)
    v_target,v_workflow_id = v.action2(long(2),"reboot")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.action2(long(2),"cancel")
    workflow_query(v_target,v_workflow_id)

def workflow_portable_disk_action_query():
    v_target,v_workflow_id = v.portable_disk_attach("5e1c26e2-f144-4189-89ba-a76ae1bf1c3f",long(2),"sda","scsi")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.portable_disk_deattach(long(2),"5e1c26e2-f144-4189-89ba-a76ae1bf1c3f")
    workflow_query(v_target,v_workflow_id)

def workflow_image_action_query():
    i_target,i_workflow_id = i.deploy(long(2),test_cluster)
    workflow_query(i_target,i_workflow_id)
    i_target,i_workflow_id = i.cancel("1d7eba34-94fc-4525-9004-370a21c4e93f",test_cluster)
    workflow_query(i_target,i_workflow_id)
    
def workflow_image_back_action_query():
    v_target,v_workflow_id = v.image_back_create(long(2),"hda")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.image_back_restore(long(2),"2329020a-dd21-4efc-8daf-de2257fef7d3","hda")
    workflow_query(v_target,v_workflow_id)
    v_target,v_workflow_id = v.image_back_delete(long(2),"2329020a-dd21-4efc-8daf-de2257fef7d3")
    workflow_query(v_target,v_workflow_id)
    
v_target,v_workflow_id = v.action2(long(3),"deploy")
workflow_query(v_target,v_workflow_id)
workflow_empty_query("1","1")

workflow_max_query("1","1")

workflow_project_action_query()

workflow_vmcfg_action_query()

workflow_image_action_query()

workflow_portable_disk_action_query()


workflow_image_back_action_query()


        


