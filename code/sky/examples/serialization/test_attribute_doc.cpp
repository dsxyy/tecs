#include "sky.h"

int main(int argc, char ** argv)
{
    AttributeDoc *t1;
    string template_str;
    string   test_ok;
    string   test_ok_marshall;
    string   test_ok_xml;
    string   test_ok_str;
    string tmp_xml;
    string tmp_str;
    string tmp_marshall;
    string    tmp1;

    vector<Attribute*> attrs;

        test_ok_xml="<VMCONFIG><CPU><![CDATA[4]]></CPU><DISK><EXTRA>"
            "<![CDATA[disk attribute ]]></EXTRA><FILE><![CDATA[path1]]></FILE>"
            "</DISK><DISK><EXTRA><![CDATA[str]]></EXTRA><FILE><![CDATA[path2]]>"
            "</FILE><TYPE><![CDATA[disk]]></TYPE></DISK><EMPTY_VAR><![CDATA[]]>"
            "</EMPTY_VAR><GRAPHICS><PORT><![CDATA[12]]></PORT><VNC>"
            "<![CDATA[127.0.0.1]]></VNC></GRAPHICS><MEMORY><![CDATA[345]]>"
            "</MEMORY><REQUIREMENTS><![CDATA[HOSTNAME = \"host*.com\"]]>"
            "</REQUIREMENTS></VMCONFIG>";
	

        test_ok_marshall="CPU=4\nDISK=EXTRA=disk attribute @^_^@FILE=path1\n"
            "DISK=EXTRA=str@^_^@FILE=path2@^_^@TYPE=disk\nEMPTY_VAR=\n"
            "GRAPHICS=PORT=12@^_^@VNC=127.0.0.1\nMEMORY=345\n"
            "REQUIREMENTS=HOSTNAME = \"host*.com\"\n";



        test_ok_str=
            "CPU=4\n"
            "DISK=EXTRA=disk attribute ,FILE=path1\n"
            "DISK=EXTRA=str,FILE=path2,TYPE=disk\n"
            "EMPTY_VAR=\n"
            "GRAPHICS=PORT=12,VNC=127.0.0.1\n"
            "MEMORY=345\n"
            "REQUIREMENTS=HOSTNAME = \"host*.com\"\n";

   
    //创建 AttributeDoc对象
    t1 = new AttributeDoc(false,"VMCONFIG");

	cout<<endl<<endl<<"----------------------------------------------------"<<endl;
    cout<<"create AttributeDoc success!"<<endl;

    //从xml字符串进行反序列化，生成AttributeDoc对象里的属性
	bool success = t1->deserialize(test_ok_xml);  
	if (success)
	{
      cout<<"deserialize success\n"<<endl;
	}

	 //把AttributeDoc对象反序列化生成 xml字符串
	 tmp_xml = t1->serialize();
	 cout<<"\n serialize result:"<<endl<<tmp_xml <<endl;
	 
	if (tmp_xml == test_ok_xml)
    {
	   cout<<"\n serialize success!\n"<<endl;
	}
    else
    {
	    cout<<"serialize failed!\n"<<endl;
	}

    multimap<string, Attribute *> m_values;

    int num = t1->get_attributes(m_values);

	cout <<"There are "<<num <<" attributes in m_values" <<endl <<endl;
	
    multimap<string, Attribute *>::const_iterator  iter;
	Attribute                          *pattr;
	map<string,string>                  map_value;	
	map<string,string>::const_iterator  itmap; 
	
	
   for (iter = m_values.begin(); iter !=m_values.end(); iter++)
   {
      pattr = iter->second;
	  if (Attribute::SIMPLE == pattr->get_type())
	  {
        cout << pattr->get_attribute_name()<<": " <<
			((SingleAttribute *)pattr)->get_attribute_value()<< endl;
	  }
	  else
	  {
         cout << pattr->get_attribute_name()<<": ";
         map_value = ((VectorAttribute *)pattr)->get_attribute_value();		  
		 for(itmap = map_value.begin(); itmap !=  map_value.end(); itmap++)
		 {
           cout <<"["<< itmap->first <<
		   	      ", "<< itmap->second << "]"<<" " ;
		 }
		 cout <<endl;
	  }
   }
	
   for (iter = m_values.begin(); iter !=m_values.end(); iter++)
   {
       delete iter->second;
   }
   cout << "delete mvalues !" <<endl;
   
  /*  
	cout<<endl<<endl<<"----------------------------------------------------"<<endl;
	t1->to_str(tmp_str);
	//将錋ttributeDoc对象 以 key = value形式的字符串输出
    cout<<"STR result：\n"<<tmp_str<<endl;
	if (tmp_str == test_ok_str)
    {
	   cout<<"str success!\n"<<endl;
	}
    else
    {
	    cout<<"str failed!\n"<<endl;
	}
	 cout<<endl<<endl<<"----------------------------------------------------"<<endl;
    t1->Marshall(tmp_marshall);
    cout<<"MARSHALL ：\n"<<tmp_marshall<<endl;
	if (tmp_marshall == test_ok_marshall)
    {
	   cout<<"marshall success!\n"<<endl;
	}
    else
    {
	    cout<<"marshall failed!\n"<<endl;
	}
	 */
	 
	cout<<endl<<endl<<"----------------------------------------------------"<<endl;
	//获取 DISK 字段的值
	if (t1->Get("DISK",attrs) == 2) 
	{
	  cout<<"Get disk success!\n"<<endl;
	}
     
    tmp1 = attrs[0]->Serialize();
    if( tmp1 ==
            "<DISK><EXTRA><![CDATA[disk attribute ]]></EXTRA><FILE>"
            "<![CDATA[path1]]></FILE></DISK>")
	{
	   cout<<"Get disk 1 success!\n"<<endl;
	};
	
    string sval;
    string sname = "REQUIREMENTS";
	//获取 REQUIREMENTS 字段的值
	t1->Get(sname,sval);

    if( sval == "HOSTNAME = \"host*.com\"")
	{
	  cout<<"Get REQUIREMENTS success!\n"<<endl;
	}
	
	cout<<endl<<endl<<"----------------------------------------------------"<<endl;
	//删除 AttributeDoc对象里的 DISK 字段
	if(2 == t1->Erase("DISK"))
	{
        cout<<"Erase disk success! \n"<<endl;
	};

    //把AttributeDoc对象反序列化生成 xml字符串
    tmp_xml = t1->serialize();
	 cout<<"Serialize after erase disk:\n "<<tmp_xml <<endl;

	/* t1->to_str(tmp_str);	
    cout<<endl<<"STR after erase ：\n"<<tmp_str<<endl; */
		
    delete t1;
}

