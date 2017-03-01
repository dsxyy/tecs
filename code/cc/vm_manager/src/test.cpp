#include "modify_vmcfg.h"
#include "alarm_agent.h"

namespace zte_tecs
{

extern MsgFrmHandlCreaterBoundle<> vmm_msgfrmhandl_creaters;

class TestVmm
{
public:
    TestVmm()
    {
        //TestVmmMsgHandle();
        //TestRegex();
    }

    void TestAction()
    {
        VmAction deauth_block(STORAGE_CATEGORY, VM_ACT_DEAUTH_BLOCK, EV_STORAGE_DEAUTH_BLOCK_REQ, MID("LCM"), MID("LCM"));

        VmAction test = deauth_block;

        vector<VmAction> actions;
        //actions.push_back(test);

        cout << deauth_block.get_id() << "-----------------" << test.get_id() << endl;
    }

    void TestVmmMsgHandle()
    {
        MessageOption option("vm_manager", EV_VM_OP, 0, 0);
        VmModifyReq req;
        req._operation = MODIFY_VMCFG;
        req._modification._element = VM_CFG_MACHINE;

        MessageFrame test_msg(req, option);

        MessageFrameHandling *handle = vmm_msgfrmhandl_creaters.CreateHandling(test_msg, 0);
        handle->Handling();
        delete handle;
    }

    bool checkEmail(const string& email){
        regex reg("\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*");
     
        return regex_match(email.c_str(),reg);
    }
     
    string getHost(const string& url){
        regex reg("https?://([^/]+).*");
        cmatch what;
     
        if (regex_match(url.c_str(),what,reg)){
            return what[1];
        }
         
        return "";
    }

    string getATag(const string& str){
        regex reg("<a.*?title='([^']+)'[^>]+>\\1</a>");
        cmatch what;
     
        if (regex_match(str.c_str(),what,reg)){
            return what[1];
        }
         
        return "";
    }
     
    string searchUsername(const string& str){
        regex reg("username=([^&]+)");
        string::const_iterator start(str.begin()),
                                end(str.end());
        match_results<string::const_iterator> what;
     
        if (regex_search(start,end,what,reg,match_default)){
            return what[1];
        }
     
        return "";
    }
    string searchargs(const string& str){
        regex reg("<id>.*</id>");
        string::const_iterator start(str.begin()),
                                end(str.end());
        match_results<string::const_iterator> what;
     
        if (regex_search(start,end,what,reg,match_default)){
            return what[1];
        }
     
        return "";
    }
    string searchAllArgs(const string& str){
        regex reg("([^&]+)=([^&]+)");
        string::const_iterator start(str.begin()),
                                end(str.end());
        match_results<string::const_iterator> what;
        string res("");
     
        while (regex_search(start,end,what,reg,match_default)){
            res+=what[1] + " => " + what[2] + '\n';
            start=what[0].second;
        }
     
        return res;
    }
     
    string replaceHello(const string& str){
        regex reg("(?<![a-zA-Z])hello(?![a-zA-Z])");
        string target("olleh");
     
        string res=regex_replace(str,reg,target);
     
        return res;
    }
     
    string replaceTo3(const string& str){
        regex reg("([a-zA-Z]+)2");
        string target("\\13");
     
        string res=regex_replace(str,reg,target);
         
        return res;
    }
    

    void TestRegex()
    {
        //http://www.cppblog.com/ming81/archive/2011/05/04/145686.html
#if 0
        std::string phone =  
                "(\\()?(\\d{3})(\\))?([-. ])?(\\d{3})([-. ]?)(\\d{4})";  
        boost::regex r(phone);  
        //代替  
        std::string fmt("$2.$5.$7");  
        std::string number("(908)555-1800");  
        std::cout << regex_replace(number, r, fmt) << std::endl;  
        
        std::string file("Caroline (201)555-2368 862-55-0123");  
        std::cout << boost::regex_replace(file, r, fmt) << std::endl;  
        std::cout << boost::regex_replace(file, r, fmt,  
                boost::regex_constants::format_no_copy) << std::endl;  
#endif
        VmCfgModifyIndividualItem modify;
        modify._args["id"] = "-1";
        modify._args["type"] = "cdrom";

        string args = modify.serialize();

        const char *szReg = "\\s*<_args>[\\s\\S]*</_args>";
        const char *szStr = args.c_str();

        boost::cmatch mat;
        boost::regex reg( szReg );
        bool r=boost::regex_search( szStr , mat, reg);
        cout << "result = " << r << endl;
        for(boost::cmatch::iterator itr=mat.begin(); itr!=mat.end(); ++itr)
        {
            //       指向子串对应首位置        指向子串对应尾位置          子串内容
            //cout << itr->first-szStr << ' ' << itr->second-szStr << ' ' << *itr << endl;
        }

        args = mat[0];
        cout << args << endl;

        args = boost::regex_replace( args, (boost::regex)"(</?)(.+?)", "\\1_\\2");
        cout << "4   " << args << endl;;

        args = boost::regex_replace( args, (boost::regex)"__args", "VmDiskConfig");
        cout << "1   " << args << endl;;


        VmDiskConfig disk;
        bool result = disk.deserialize(args);
        cout << "deserialize result = " << result << endl;

        szStr = disk.serialize().c_str();
        boost::regex reg1( "(?<=<).+?(?=>)" );
        r=boost::regex_search( szStr , mat, reg1);
        cout << "result = " << r << endl;
        cout << mat[0] << endl;

    }

};

TestVmm test;

}

