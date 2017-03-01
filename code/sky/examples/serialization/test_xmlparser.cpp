#include "sky.h"

/*
<domain type='kvm'>
    <name>i-510D098A</name>
    <os>
        <type>hvm</type>
        <kernel>/var/lib/tecs/instances/admin/i-510D098A/kernel</kernel>
        <initrd>/var/lib/tecs/instances/admin/i-510D098A/ramdisk</initrd>
        <cmdline>root=/dev/sda1 console=ttyS0</cmdline>
    </os>
    <features>
        <acpi/>
    </features>
    <memory>102400</memory>
    <vcpu>1</vcpu>
    <devices>
        <disk type='file'>
            <source file='/var/lib/tecs/instances/admin/i-510D098A/disk'/>
            <target dev='sda'/>
        </disk>
        <interface type='bridge'>
            <source bridge='br0'/>
            <mac address='d0:0d:51:0D:09:8A'/>
            <model type='e1000'/>
        </interface>
        <serial type="file">
            <source path='/var/lib/tecs/instances/admin/i-510D098A/console.log'/>
            <target port='1'/>
        </serial>
    </devices>
</domain>
*/

//本例演示生成xml文档的方式，适用于创建xml格式的配置文件等场合
void test1()
{
    XmlParser xml;
    xml.Create("domain");
    xml.SetAttr("type","kvm");
    xml.Write("name","i-510D098A");
    
    xml.Write("os","");
    xml.Enter();
    xml.Write("type","hvm");
    xml.Write("kernel","/var/lib/tecs/instances/admin/i-510D098A/kernel");
    xml.Write("initrd","/var/lib/tecs/instances/admin/i-510D098A/ramdisk");
    xml.Write("cmdline","/dev/sda1 console=ttyS0");
    xml.Exit();

    xml.Write("features","");
    xml.Enter();
    xml.Write("acpi","");
    xml.Exit();

    xml.Write("memory","102400");
    xml.Write("vcpu","1");

    xml.Write("devices","");
    xml.Enter();
    
    xml.Write("disk","");
    xml.SetAttr("type","file");
    xml.Enter();
    xml.Write("source","");
    xml.SetAttr("file","/var/lib/tecs/instances/admin/i-510D098A/disk");
    xml.Write("target","");
    xml.SetAttr("dev","sda");
    xml.Exit();

    xml.Write("interface","");
    xml.SetAttr("type","bridge");
    xml.Enter();
    xml.Write("source","");
    xml.SetAttr("bridge","br0");
    xml.Write("mac","");
    xml.SetAttr("address","d0:0d:51:0D:09:8A");
    xml.Write("model","");
    xml.SetAttr("type","e1000");
    xml.Exit();

    xml.Write("serial","");
    xml.SetAttr("type","file");
    xml.Enter();
    xml.Write("source","");
    xml.SetAttr("path","/var/lib/tecs/instances/admin/i-510D098A/console.log");
    xml.Write("target","");
    xml.SetAttr("port","1");
    xml.Exit();
    
    xml.Exit();
    
    string s = xml.DocToString();
    cout << s << endl;
}

//本例演示读取xml文档的方式
void test2()
{
    string xml_str = " \
<SNMP> \
    <oid property='scalar' value='1.3.6.1.4.1.3902.6051.19.1.1.1' get='yes' modify='yes' add_del='no'> \
        <member property='read_write' key='no' type='Displaystring' max_value='' min_value='' rowstate='no'>1</member> \
        <member property='read_write' key='no' type='Displaystring' max_value='' min_value='' rowstate='no'>2</member> \
        <member property='read_write' key='no' type='Displaystring' max_value='2' min_value='1' rowstate='no'>3</member> \
    </oid> \
    <oid property='table' value='1.3.6.1.4.1.3902.6051.19.1.1.2' get='yes' modify='yes' add_del='yes'> \
        <member property='read_write' key='yes' type='ipaddress' max_value='' min_value='' rowstate='no'>1</member> \
        <member property='read_write' key='yes' type='int' max_value='65535' min_value='0' rowstate='no'>2</member> \
        <member property='read_write' key='no' type='int' max_value='6' min_value='1' rowstate='yes'>3</member> \
    </oid> \
</SNMP> \
";
    XmlParser xml;
    xml.Locate(xml_str,"SNMP");
}

int main()
{
    test2();
    test1();
    return 0;
}

