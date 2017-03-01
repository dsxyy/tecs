#!/usr/bin/python
# -*- coding: utf-8 -*-
# 作者： 张文剑
# 创建日期：2012.08.04
# 描述：对每次集成测试产生的数据库日志文件进行解析，过滤掉已知的、忽略的错误
# 提取新的未知错误，并打印出来

import sys,os,getopt
import re

def usage():
    print "Usage: pglog.py [OPTION]..."
    print "-h,--help     show this help"
    print "-l,--log        the postgresql log file path"
    print "-i,--ignores    the ignored errors file path"

def parse_args():
    # 解析处理命令行参数
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:l:i:", ["help", "log=","ignores="])
    except getopt.GetoptError:
        # print help information and exit:
        usage()
        sys.exit(-2)
        
    for o, v in opts:
        #用-h或--help打印帮助
        if o in ("-h", "--help"):
            usage()
            
        #用-l或--log可以指定要解析的postgres日志文件
        if o in ("-l", "--log"):
            global log_file 
            log_file = v
            if os.path.isfile(log_file) is not True:
                print ("postgres log file %s does not exist!" % log_file)
                exit(-2)

        #用-e或--except表示特例错误列表文件
        if o in ("-i", "--ignores"):
            global ignored_errors_file 
            ignored_errors_file = v
            if os.path.isfile(ignored_errors_file) is not True:
                print ("ignored errors file %s does not exist!" % ignored_errors_file)
                exit(-2)
       
class PglogParser:
    def __init__(self, log_file=None,ignored_errors_file=None):
        self.__log_file = log_file
        self.__ignored_errors_file= ignored_errors_file
        self.__ignored_error_list = []
        self.__new_error_list=[]
        self.__logfd=None
        self.__cached_line=None
        
    def __read_ignored_errors(self):
        '''从指定的文件中读取已经作为例外处理的忽略的错误日志内容'''
        if self.__ignored_errors_file is None:
            return

        file = open(self.__ignored_errors_file)
        while 1:
            line = file.readline().strip()
            if not line:
                break
            #print("except:%s" % line)
            self.__ignored_error_list.append(line)
        return

    def __filter(self,log):
        '''搜集数据库日志文件中出现的除了已忽视的错误日志之外的错误日志'''
#        print("log = %s" % log["content"])
        if(log["level"] != 'ERROR'):
            return
        #目前只是处理一下ERROR级别的日志，以后如果需要处理其它日志，可以在这里补充    
        #print("error = %s" % log["content"])
        if log["content"] in self.__ignored_error_list:
#            print("ignored error:\n%s" % log["raw"])
            return
        #如果不是已经设置为忽略的日志，记录起来    
        self.__new_error_list.append(log["raw"])

    def __read_log(self):
        '''每次调用时读一条日志，每条日志可能有多行'''
        if self.__logfd is None:
            self.__logfd=open(self.__log_file)
        
        loglines = []
        if self.__cached_line is not None:
            #如果存在上次读取残留的缓存行，这次读取肯定是要优先返回的
            loglines.append(self.__cached_line)
            self.__cached_line = None
      
        #逐行读取，直到读完文件或读到非tab键开头的行
        while True:
            newline = self.__logfd.readline()
            if newline is None:
                #读操作失败了
                break

            if newline == "":
                #文件已经读完了
                break;

            if newline.strip() == "":
                #读到空行了
                #print("==blank line==")
                continue;
                
            if newline[0] == '\t' or not loglines:
                #以tab开头，或者还是第一次读时，都要追加到loglines
                loglines.append(newline)
            else:
                #既不是tab开头，也不是第一次读，就缓存下来，并返回
                self.__cached_line = newline
                break

        return "".join(loglines)
        
    def parse(self):
        '''
        读取postgresql数据库日志文件，逐行进行解析，解析时使用python的正则表达式机制。
        format正则表达式的定义前提是postgres.conf中的日志格式必须这样定义：log_line_prefix = '%t [%p]: [%l-1] user=%u,db=%d '
        例如下面是定义后的日志打印示例：
        2012-08-04 00:08:06 CST [11708]: [1-1] user=tecs,db=tecscloud LOG:  connection received: host=10.43.164.31 port=4753
        '''
        if self.__log_file is None:
            return
            
        self.__read_ignored_errors()
        format = '(?P<time>.+CST)\\s+\\[(?P<pid>\\d+)\\]:\\s+\\[(?P<session>.+)\\]\\s+user=(?P<user>\\S*),db=(?P<db>\\S*)\\s+(?P<level>\\w+):\\s+(?P<content>.*)'
        #使用re.DOTALL是为了让正则表达式的星号元字符(*)也包含换行，因为一条日志可能有多行，content中是可能有换行的
        pattern = re.compile(format,re.DOTALL)

        while True:
            line=self.__read_log()
            line = line.strip()
            if line == "" or line is None:
                break
            #print("log: %s" % line);
            m = pattern.match(line)
            if(m is not None):
                log=m.groupdict()
                #日志解析成功后，进行过滤处理
                log["raw"] = line
                self.__filter(log)
#            else:
#                print("warning! failed to parse postgres log line: \n%s" % line);
            
    def get_new_errors(self):
        return self.__new_error_list
        
    def get_ignored_errors(self):
        return self.__ignored_error_list
        
log_file=None
ignored_errors_file=None

if __name__ == "__main__":
    parse_args()
    if log_file is None:
        print "no log file assigned!"
        usage()
        exit(0)
        
    p=PglogParser(log_file,ignored_errors_file)
    p.parse()
    #解析完之后，打印新发现的错误
    new_errors=p.get_new_errors()
    if not new_errors:
        print "no new errors!"
        exit(0)
    else:        
        print("=============tecs has new database errors=============")
        for e in new_errors:
            print e
        exit(-1)
