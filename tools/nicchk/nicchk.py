#!/usr/bin/python
#  -*- mode: python; -*-

from datetime import *
import sys, os, time, atexit,commands,ConfigParser
from signal import SIGTERM


class Daemon:
    """
    A generic daemon class.
    
    Usage: subclass the Daemon class and override the _run() method
    """

    def __init__(self, pidfile, stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
        self.stdin = stdin
        self.stdout = stdout
        self.stderr = stderr
        self.pidfile = pidfile
        self.configfile = '/etc/nicchk/nicchk.conf'
        self.statusfile = '/dev/shm/nicchk.status'

    def _daemonize(self):
        """
        do the UNIX double-fork magic, see Stevens' "Advanced 
        Programming in the UNIX Environment" for details (ISBN 0201563177)
        http://www.erlenstar.demon.co.uk/unix/faq_2.html#SEC16
        """

        #desperate from parent process
        try:
            pid = os.fork()
            if pid > 0:
                sys.exit(0)
        except OSError, e:
            sys.stderr.write("fork #1 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1)

        #desperate from terminal
        os.setsid()
        #change current work path  
        os.chdir("/")
        #reset file mask
        os.umask(0)


        # fork again to avoid opening the control teminal
        try:
            pid = os.fork()
            if pid > 0:
                sys.exit(0)
        except OSError, e:
            sys.stderr.write("fork #2 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1)

        sys.stdout.flush()
        sys.stderr.flush()
        si = file(self.stdin, 'r')
        so = file(self.stdout, 'a+')
        se = file(self.stderr, 'a+', 0)
        
        os.dup2(si.fileno(), sys.stdin.fileno())
        os.dup2(so.fileno(), sys.stdout.fileno())
        os.dup2(se.fileno(), sys.stderr.fileno())


        #register the functon at exit
        atexit.register(self.delfile)
        pid = str(os.getpid())
        file(self.pidfile,'w+').write("%s\n" % pid)
        

    def delfile(self):
        if os.path.isfile(self.statusfile):
            os.remove(self.statusfile)  
        if os.path.isfile(self.pidfile):    
            os.remove(self.pidfile)


    def start(self):
        """
        Start the daemon
        """
        # Check for a pidfile to see if the daemon already runs
        try:
            pf = file(self.pidfile,'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None

        if pid:
            if (os.system('ps ' + str(pid)+'  >>/dev/null') == 0):
                #message = "pidfile %s already exist. Daemon is running!\n"
                #sys.stderr.write(message % self.pidfile)
                sys.exit(0)
            else:
                os.remove(self.pidfile)
        # check whether config file exists #    
        if not os.path.isfile(self.configfile):
            message = "config file  %s not exist. Please check the config file.\n"
            sys.stderr.write(message % self.configfile)
            sys.exit(1) 


        # Start the daemon
        self._daemonize()
        self._run()

    def stop(self):
        """
        Stop the daemon
        """
        if os.path.isfile(self.statusfile):
            os.remove(self.statusfile)        
        # Get the pid from the pidfile
        try:
            pf = file(self.pidfile,'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None

        if not pid:
            return # not an error in a restart
        # Try killing the daemon process    
        try:
            while 1:
                os.kill(pid, SIGTERM)
                time.sleep(0.1)
                if os.path.isfile(self.pidfile):
                    os.remove(self.pidfile)
        except OSError, err:
            err = str(err)
            if err.find("No such process") > 0:
                if os.path.isfile(self.pidfile):
                    os.remove(self.pidfile)
            else:
                print str(err)
                sys.exit(1)
    def restart(self):
        """
        Restart the daemon
        """
        self.stop()
        self.start()

    def _run(self):
        """
        You should override this method when you subclass Daemon. It will be called after the process has been
        daemonized by start() or restart().
        """
        nic_status = NicStatus()
        nic_status.CheckStatus()

    def probe(self):
        """
        probe the nic status
        """
        try:
            mtime = os.stat(self.statusfile).st_mtime
            if (time.time() - mtime) > 60 :
                sys.exit(2)
            sf = file(self.statusfile,'r')
            nic_status = sf.readline()
            sf.close
            sys.exit(int(nic_status))
        except Exception, err:
            err = str(err)
            print '%s'  % err
            sys.exit(2)

class NicStatus(object):
    """
    A generic nic status class.
    
    Usage: check configurated nic status
    """

    def __init__(self):
        self.configfile ='/etc/nicchk/nicchk.conf'
        self.statusfile ='/dev/shm/nicchk.status'
        self.logfile ='/var/log/nicchk/nicchk.log'
        self.nic_list=[]
        self.nic_statistics_list = []
        self.net_dev_info =[]
        self.nic_statistics_rec=[]
        self.nic_rec_date = ''
        self.chkperiod=300
        self.ModifyRecStatus(0)
        self.GetConfig()
        if not os.path.isdir('/var/log/nicchk') :
            os.mkdir('/var/log/nicchk')


    def ModifyRecStatus(self,status):
        sf = file(self.statusfile,'w')
        sf.write("%d" % status)
        sf.close
    
    def UpdateNicRecord(self):
        self.nic_statistics_rec = self.net_dev_info 
        self.nic_rec_date = datetime.now()

    def LogNicStatistics(self):
        # log statistics  #             
        sf = file(self.logfile,'a')
        sf.write("The nic error lasted for more than %d seconds.\n" % self.chkperiod)
        sf.write("Original statistics record at %s :\n" % self.nic_rec_date)
        for eachline in self.nic_statistics_rec:
            sf.write("%s" %eachline)  
        sf.write("Last statistics record at %s :\n" % datetime.now())
        for eachline in self.net_dev_info:
            sf.write("%s" %eachline) 
        sf.write("\n" )        
        sf.close

    def GetConfig(self):
        try:
            cf=ConfigParser.ConfigParser()
            cf.read(self.configfile)
        except IOError:
            message = "read config file  %s failed.\n"
            sys.stderr.write(message % self.configfile)
            lf = file(self.logfile,'a+')
            lf.write(message % self.configfile)
            lf.close()
            sys.exit(1)

        nic_name = cf.get("config","nic_list")
        self.nic_list += nic_name.strip().split()
        self.chkperiod = cf.getint("config","chk_period")

    def CheckStatus(self):
        count=0
        Status=0
        for nic_name in self.nic_list:
            self.nic_statistics_list.append(NicStatistics(nic_name))
        self.GetNicStatus()
        self.UpdateNicRecord()
        while True:
            time.sleep(10)
            Status = self.GetNicStatus()
            if Status == 0:
                count=0
                self.UpdateNicRecord()
                self.ModifyRecStatus(0)
            else:
                if (count <= (self.chkperiod/10)):
                    count += 1
                if count >= (self.chkperiod/10) :
                    self.ModifyRecStatus(1)
                    if count == (self.chkperiod/10) :
                        self.LogNicStatistics()
                else:
                    self.ModifyRecStatus(0) 
            
                    
    def GetNicStatus(self):
        try:
            nsf = file('/proc/net/dev','r')
            self.net_dev_info = nsf.readlines()
            nsf.close()
        except IOError:
            message = "read file  %s failed.\n"
            sys.stderr.write(message % '/proc/net/dev')
       
        for nic_obj in self.nic_statistics_list:
            nic_obj.CheckStatistics(self.net_dev_info)
        for nic_obj in self.nic_statistics_list:
            if nic_obj.nic_result == 1:
                return 1
        return 0            

class NicStatistics(object):
    """
    A generic nic statistics class.
    
    Usage: get nic statistics.
    """


    def __init__(self,name):
        self.nic_name = name
        self.nic_result=0
        self.RX=0
        self.TX=0
        self.net_item_dict = {'RX_bytes':0,
                              'RX_packets':1,
                              'RX_errs':2,
                              'RX_drop':3,
                              'RX_fifo':4,
                              'RX_frame':5, 
                              'RX_compressed':6,
                              'RX_multicast':7,
                              'TX_bytes':8,
                              'TX_packets':9,
                              'TX_errs':10, 
                              'TX_drop':11,
                              'TX_fifo':12,
                              'TX_colls':13,
                              'TX_carrier':14,
                              'TX_compressed':15
                   }

    def CheckStatistics(self,net_dev_info):
        # if not find nic name in net_dev_info, default nic result is 1 #
    	self.nic_result=1
        if len(net_dev_info) == 0:
            self.nic_result = 1
        else:
            for eachLine in  net_dev_info : 
                if  eachLine.find(self.nic_name) != -1:
                    tmpindex = len(self.nic_name) + len(':')
                    tmpstr = eachLine.lstrip()
                    nic_item = tmpstr[tmpindex:].split()

                    RX_ok = int(nic_item[self.net_item_dict['RX_packets']]) 
                    RX_errors = int(nic_item[self.net_item_dict['RX_errs']]) 
                    RX_dropped = int(nic_item[self.net_item_dict['RX_drop']])
                    TX_ok = int(nic_item[self.net_item_dict['TX_packets']]) 
                    TX_errors =  int(nic_item[self.net_item_dict['TX_errs']])
                    TX_dropped = int(nic_item[self.net_item_dict['TX_drop']])
                    
                    if (RX_ok > 0 ) and (TX_ok > 0):
                        if (RX_ok > self.RX) and (TX_ok > self.TX):
                            self.nic_result=0
                            self.RX = RX_ok
                            self.TX = TX_ok
                        else:
                            self.nic_result=1
                    elif (RX_ok == 0 ) and (TX_ok == 0) \
                        and (RX_errors == 0) and (RX_dropped == 0) \
                        and (TX_errors == 0) and (TX_dropped == 0): 
                        if self.IsNicDown() == True:
                            self.nic_result=0
                        else:
                            self.nic_result=1
                    else:
                        self.nic_result=1
                                         


    def IsNicDown(self):
        result = commands.getstatusoutput("ethtool  " + self.nic_name)
        if result[0] != 0:
            return False
        if len(result[1]) == 0:
            return False
        if  (result[1].find('Link detected: no') != -1) :
            return True
        else:   
            return False        
            
               

class MyDaemon(Daemon):
    def run(self):
        while True:
            time.sleep(1)

if __name__ == "__main__":
    daemon = MyDaemon('/var/run/nicchk.pid')
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        elif 'restart' == sys.argv[1]:
            daemon.restart()
        elif 'probe' == sys.argv[1]:
            daemon.probe()            
        else:
            print "Unknown command"
            sys.exit(2)
        sys.exit(0)
    elif len(sys.argv) == 1: 
        daemon.start()   
    else:
        print "usage: %s start|stop|restart|probe" % sys.argv[0]
        sys.exit(2)

