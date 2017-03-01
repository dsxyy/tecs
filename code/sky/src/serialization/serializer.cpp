/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�serializer.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��XML���л�������ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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
    void EnterNode(); //���뵱ǰ�ڵ����һ��
    void ExitNode();  //���ص�ǰ�ڵ����һ��
    bool GotoFirstNode(const char *name); //�ƶ�����ǰ�ڵ�ĵ�һ����Ϊname���ֵܽڵ�
    bool GotoNextNode(const char *name); //�ƶ�����ǰ�ڵ����һ����Ϊname���ֵܽڵ�
    bool WriteNode(const char *name,const char *value); //�ڵ�ǰ���ڵ����������һ���½ڵ�
    bool WriteNodeTree(const char *name,const char *tree); //�ڵ�ǰ���ڵ����������һ���½ڵ���
    bool ReadNode(const char *name,string& s); //�ڵ�ǰ���ڵ��²�����Ϊname���ֵܽڵ㣬�����value
    bool ReadNodeTree(const char *name,string& s); //�ڵ�ǰ�ڵ��²�����Ϊname���ӽڵ㣬��������������s

    bool SetAttr(const char *name,const char *value);//����ǰ�ڵ���������
    const char *GetAttr(const char *name); //��ȡ��ǰ�ڵ�ָ�����Ե�ֵ
    const char *GetName(); //��õ�ǰ�ڵ������
    const char *GetValue(); //��õ�ǰ�ڵ��ֵ
    void SaveState();//���浱ǰ״̬
    void RestoreState();//�ָ���ǰ״̬
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

//���浱ǰ״̬
void XmlParserImpl::SaveState()
{
    pSavedCur = pCurNode;
    pSavedRoot = pCurRoot;
}

//�ָ���ǰ״̬
void XmlParserImpl::RestoreState()
{
    pCurNode = pSavedCur;
    pCurRoot = pSavedRoot;
}

//���뵱ǰ�ڵ����һ��
void XmlParserImpl::EnterNode()
{
    if (pCurNode)
    {
        pCurRoot = pCurNode;
        pCurNode = pCurRoot->first_node();
    }
}

//���ص�ǰ�ڵ����һ��
void XmlParserImpl::ExitNode()
{
    if (pCurRoot)
    {
        pCurRoot = pCurRoot->parent();
        pCurNode = pCurRoot->first_node();
    }
}

//���ҵ�ǰ�ڵ�ĵ�һ����Ϊname���ֵܽڵ�
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

//���ҵ�ǰ�ڵ����һ����Ϊname���ֵܽڵ�
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

//��ȡ��ǰ���ڵ����Ϊname���ӽڵ����������ַ���
bool XmlParserImpl::ReadNodeTree(const char *name,string& s)
{
    if(NULL == pCurRoot)
        return false;
    
    //���nameΪ�գ����ȡ��һ�����ӽڵ���
    Node *child = pCurRoot->first_node(name);
    if(!child)
    {
        //���û��ָ��name�������ڵ��value����
        //��Ϊ�������е�object���л��������xml tree
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

//�ڵ�ǰ���ڵ�������һ����Ϊname���ӽڵ���
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
        //�����л���������л����δ�ض���xml�ַ���!
        //�������ͨ�ַ�����ֱ�������������ڵ㼴��
        newnode = doc.allocate_node(node_element,pname,ptree);
    }
    
    pCurNode = newnode;
    if (!pCurRoot)
    {
        //��û��root�ڵ�ʱ������ǰ���½ڵ���Ϊ���ڵ�
        doc.append_node(pCurNode);
        pCurRoot = pCurNode;
    }
    else
    {
        //�Ѿ���root�ڵ�ʱ������ǰ���½ڵ���Ϊ���ڵ���ӽڵ�
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
        //��û��root�ڵ�ʱ������ǰ���½ڵ���Ϊ���ڵ�
        doc.append_node(pCurNode);
        pCurRoot = pCurNode;
    }
    else
    {
        //�Ѿ���root�ڵ�ʱ������ǰ���½ڵ���Ϊ���ڵ���ӽڵ�
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
    //����ǰ�ڵ����Ϊname���ӽڵ���ת��Ϊ�ַ�������
    string s;
    impl->SaveState();
    impl->EnterNode();  //����Ҫ���뵱ǰ�ڵ�
    impl->ReadNodeTree(name,s); //�ڵ�ǰ�ڵ��¶�ȡ��Ϊname���ӽڵ���
    impl->ExitNode(); //�ӵ�ǰ�ڵ����һ�㷵�ص���ǰ�ڵ�
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


