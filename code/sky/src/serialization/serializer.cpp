/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：serializer.cpp
* 文件标识：见配置管理计划书
* 内容摘要：XML序列化解析器实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "common.h"
#include "serialization.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
using namespace rapidxml;
typedef xml_node<> Node;
typedef xml_document<> Document;

class XmlParserImpl
{
public:
    XmlParserImpl()
    {
        pCurRoot = NULL;
        pCurNode = NULL;
        pSavedCur = NULL;
        pSavedRoot = NULL;
    };
    
    ~XmlParserImpl()
    {
        //cout << "rapid document clear!" << endl;
        doc.clear();
    };

    bool LoadDoc(const char *xmlstring);
    void EnterNode(); //进入当前节点的内一层
    void ExitNode();  //返回当前节点的外一层
    bool GotoFirstNode(const char *name); //移动到当前节点的第一个名为name的兄弟节点
    bool GotoNextNode(const char *name); //移动到当前节点的下一个名为name的兄弟节点
    bool WriteNode(const char *name,const char *value); //在当前父节点的下面增加一个新节点
    bool WriteNodeTree(const char *name,const char *tree); //在当前父节点的下面增加一个新节点树
    bool ReadNode(const char *name,string& s); //在当前父节点下查找名为name的兄弟节点，获得其value
    bool ReadNodeTree(const char *name,string& s); //在当前节点下查找名为name的子节点，将其内容树存入s

    bool SetAttr(const char *name,const char *value);//给当前节点设置属性
    const char *GetAttr(const char *name); //获取当前节点指定属性的值
    const char *GetName(); //获得当前节点的名称
    const char *GetValue(); //获得当前节点的值
    void SaveState();//保存当前状态
    void RestoreState();//恢复当前状态
    void Print();
    string ToString();
private:    
    Document doc;
    Node *pCurRoot;
    Node *pCurNode;
    Node *pSavedCur;
    Node *pSavedRoot;
};

bool XmlParserImpl::LoadDoc(const char *xmlstring)
{
    char *pxmlstring = doc.allocate_string(xmlstring);
    try
    {
        doc.parse<parse_declaration_node | parse_no_data_nodes>(pxmlstring);
    }
    catch(const parse_error &err)
    {
        cout << "failed to load xml document! error is " << err.what() << endl;
        return false;
    }
    pCurNode = doc.first_node();
    pCurRoot = &doc;
    return true;
}

//保存当前状态
void XmlParserImpl::SaveState()
{
    pSavedCur = pCurNode;
    pSavedRoot = pCurRoot;
}

//恢复当前状态
void XmlParserImpl::RestoreState()
{
    pCurNode = pSavedCur;
    pCurRoot = pSavedRoot;
}

//进入当前节点的下一层
void XmlParserImpl::EnterNode()
{
    if (pCurNode)
    {
        pCurRoot = pCurNode;
        pCurNode = pCurRoot->first_node();
    }
}

//返回当前节点的上一层
void XmlParserImpl::ExitNode()
{
    if (pCurRoot)
    {
        pCurRoot = pCurRoot->parent();
        pCurNode = pCurRoot->first_node();
    }
}

//查找当前节点的第一个名为name的兄弟节点
bool XmlParserImpl::GotoFirstNode(const char* name)
{
    Node *tmp = NULL;
    if (!pCurRoot)
        tmp = doc.first_node(name);
    else
        tmp = pCurRoot->first_node(name);

    if (!tmp)
        return false;

    pCurNode = tmp;
    //cout << "FindNode a node = " << tmp->value() << "\n" << endl;
    return true;
}

//查找当前节点的下一个名为name的兄弟节点
bool XmlParserImpl::GotoNextNode(const char* name)
{  
    if(!pCurNode)
        return false;
    
    Node *tmp = pCurNode->next_sibling(name);

    if(tmp)
    {
        pCurNode = tmp;     
        return true;
    }
    else
    {
        return false;
    }
}

const char* XmlParserImpl::GetName()
{
    if (!pCurNode)
        return NULL;

    return pCurNode->name();
}

const char* XmlParserImpl::GetValue()
{
    if (!pCurNode)
        return NULL;

    return pCurNode->value();
}

bool XmlParserImpl::SetAttr(const char *name,const char *value)
{
    if (pCurNode)
    {
        const char *pname = NULL;
        const char *pvalue = NULL;
        try
        {
            pname = doc.allocate_string(name);
            pvalue = doc.allocate_string(value);
        }
        catch(const parse_error &err)
        {
            cout << "failed to set attribute! error is " << err.what() << endl;
            return false;
        }
        pCurNode->append_attribute(doc.allocate_attribute(pname, pvalue));
    }
    return true;
}

const char* XmlParserImpl::GetAttr(const char* name)
{
    if (!pCurNode)
        return NULL;

    xml_attribute<> *attr = pCurNode->first_attribute(name);
    if (!attr)
        return NULL;

    return attr->value();
}

//获取当前根节点的名为name的子节点树的整个字符串
bool XmlParserImpl::ReadNodeTree(const char *name,string& s)
{
    if(NULL == pCurRoot)
        return false;
    
    //如果name为空，则读取第一个子子节点树
    Node *child = pCurRoot->first_node(name);
    if(!child)
    {
        //如果没有指定name，将本节点的value返回
        //因为不是所有的object序列化结果都是xml tree
        if(!name)
            s.assign(pCurRoot->value());
        else
            return false;
    }
    else
    {
        print(back_inserter(s), *child);
    }
    
    return true;
}

//在当前根节点下增加一个名为name的子节点树
bool XmlParserImpl::WriteNodeTree(const char *name,const char *tree)
{
    const char *pname = NULL;
    char *ptree = NULL;
    try
    {
        pname = doc.allocate_string(name);
        ptree = doc.allocate_string(tree);
    }
    catch(const parse_error &err)
    {
        cout << "failed to set attribute! error is " << err.what() << endl;
        return false;
    }
            
    if(!pname || !ptree)
    {
        SkyAssert(false);
        return false;
    }
    
    Node* newnode = NULL;
    if(doc.is_xml_str<parse_declaration_node | parse_no_data_nodes>(ptree))
    {
        Node* treenode = NULL;
        try
        {
            treenode = doc.create_node_tree<parse_declaration_node | parse_no_data_nodes>(ptree);
        }
        catch(const parse_error &err)
        {
            cout << "failed to create node tree! error is " << err.what() << endl;
            return false;
        }
        
        if(!treenode)
        {
            SkyAssert(false);
            return false;
        }
        newnode = doc.allocate_node(node_element,pname," ");
        newnode->append_node(treenode);
    }
    else
    {
        //可序列化对象的序列化结果未必都是xml字符串!
        //如果是普通字符串，直接利用它创建节点即可
        newnode = doc.allocate_node(node_element,pname,ptree);
    }
    
    pCurNode = newnode;
    if (!pCurRoot)
    {
        //还没有root节点时，将当前的新节点作为根节点
        doc.append_node(pCurNode);
        pCurRoot = pCurNode;
    }
    else
    {
        //已经有root节点时，将当前的新节点作为根节点的子节点
        pCurRoot->append_node(pCurNode);
    }

    return true;
}

bool XmlParserImpl::WriteNode(const char* name,const char* value)
{
    const char *pname = NULL;
    const char *pvalue = NULL;
    try
    {
        pname = doc.allocate_string(name);
        pvalue = doc.allocate_string(value);
    }
    catch(const parse_error &err)
    {
        return false;        
    }
    
    if(!pname || !pvalue)
    {
        SkyAssert(false);
        return false;
    }
        
    Node* newnode = doc.allocate_node(node_element,pname,pvalue);
    if(!newnode)
    {
        SkyAssert(false);
        return false;
    }
    
    pCurNode = newnode;
    if (!pCurRoot)
    {
        //还没有root节点时，将当前的新节点作为根节点
        doc.append_node(pCurNode);
        pCurRoot = pCurNode;
    }
    else
    {
        //已经有root节点时，将当前的新节点作为根节点的子节点
        pCurRoot->append_node(pCurNode);
    }
    return true;
}

bool XmlParserImpl::ReadNode(const char *name,string& s)
{
    Node *tmp = NULL;
    if (!pCurRoot)
        tmp = doc.first_node(name);
    else
        tmp = pCurRoot->first_node(name);

    if (!tmp)
        return false;
    
    s.assign(tmp->value());
    return true;
}

void XmlParserImpl::Print()
{
    string s;
    print(back_inserter(s), doc);
    cout << s << endl;
}

string XmlParserImpl::ToString()
{
    string s;
    //print(back_inserter(s), *this,rapidxml::print_no_indenting);
    print(back_inserter(s), doc);
    return s;
}

XmlParser::XmlParser()
{
    impl = new XmlParserImpl();
}

XmlParser::~XmlParser()
{
    delete impl;
}

bool XmlParser::Create(const char *title)
{
    return impl->WriteNode(title,title); 
}

bool XmlParser::SetAttr(const char *name,const string& value)
{
    return impl->SetAttr(name,value.c_str()); 
}

string XmlParser::GetAttr(const char *name)
{
    return impl->GetAttr(name); 
}

bool XmlParser::Locate(const string& xmlstring,const char*name)
{
    impl->LoadDoc(xmlstring.c_str());
    if(!impl->GotoFirstNode(name))
    {
        return false;
    }
    
    impl->EnterNode();
    return true;
}

bool XmlParser::Locate(const char*name)
{
    if(!impl->GotoFirstNode(name))
    {
        return false;
    }
    
    impl->EnterNode();
    return true;
}

bool XmlParser::WriteString(const char *name,const string& s)
{
    return impl->WriteNode(name,s.c_str());
}

bool XmlParser::WriteObject(const char *name,const Serializable& object)
{  
    string s = object.serialize();
    return impl->WriteNodeTree(name, s.c_str());
}

bool XmlParser::ReadString(const char *name,string& s)
{
    return impl->ReadNode(name,s);
}

bool XmlParser::ReadObject(const char *name,Serializable& object)
{
    string s;
    if(false == Locate(name))
        return false;
        
    if(!impl->ReadNodeTree(NULL,s))
    {
        return false;
    }
    bool ret = object.deserialize(s.c_str());
    Exit();
    return ret;
}

string XmlParser::DocToString()
{
    return impl->ToString();
}

void XmlParser::Enter()
{
    impl->EnterNode();
}

void XmlParser::Exit()
{
    impl->ExitNode();
}

bool XmlParser::FirstItem()
{
    if(impl->GotoFirstNode("item"))
        return true;
    else
        return false;
}

bool XmlParser::NextItem()
{
    if(impl->GotoNextNode("item"))
        return true;
    else
        return false;
}

string XmlParser::ChildToString(const char *name)
{
    //将当前节点的名为name的子节点树转换为字符串返回
    string s;
    impl->SaveState();
    impl->EnterNode();  //首先要进入当前节点
    impl->ReadNodeTree(name,s); //在当前节点下读取名为name的子节点树
    impl->ExitNode(); //从当前节点的下一层返回到当前节点
    impl->RestoreState();
    return s;
}

bool XmlParser::GotoFirst(const char* name)
{
    return impl->GotoFirstNode(name);
}

bool XmlParser::GotoNext(const char* name)
{
    return impl->GotoNextNode(name);
}

string XmlParser::GetName()
{
    return impl->GetName();
}
    
string XmlParser::GetValue()
{
    return impl->GetValue();
}


