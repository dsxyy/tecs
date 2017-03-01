/* -------------------------------------------------------------------------- */
/* Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#include "attribute_doc.h"

#include "attribute.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace rapidxml;

namespace zte_tecs
{

    /**
     *  Builds a SingleAttribute from the given node
     *    @param node The xml element to build the attribute from.
     *
     *    @return the attribute, or 0 if the node doesn't contain a single att.
     */
	
static Attribute * BuildSingleAttr(const xml_node<>  *node)
{
    Attribute   *attr = 0;
    xml_node<>  *child = node->first_node();

    if((NULL != child) 
	     &&((node_data == child->type())||(node_cdata == child->type())))
    {
        attr = new SingleAttribute(
                            reinterpret_cast<const char *>(node->name()),
                            reinterpret_cast<const char *>(child->value()) );
    } 

    return attr;
}

    /**
     *  Builds a VectorAttribute from the given node
     *    @param node The xml element to build the attribute from.
     *
     *    @return the attribute, or 0 if the node doesn't contain a vector att.
     */
static Attribute * BuildVectorAttr(const xml_node<>  *node)
{
    VectorAttribute    *attr        = NULL;
    xml_node<>         *child       = NULL;
    xml_node<>         *grandchild  = NULL;

    child = node->first_node();
    if((NULL != child) 
		&&(node_element == child->type()))
    {
        attr = new VectorAttribute(
                        reinterpret_cast<const char *>(node->name()));

        for(child = child; NULL != child; child = child->next_sibling())
        {
            grandchild = child->first_node();

            if((NULL != grandchild) 
				&&((node_data == grandchild->type())||(node_cdata == grandchild->type())))
            {
                attr->Replace(
                        reinterpret_cast<const char *>(child->name()),
                        reinterpret_cast<const char *>(grandchild->value()) );
            }
        }
    } 

    return attr;
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
const char		AttributeDoc::_separator = '=' ;
AttributeDoc::~AttributeDoc()
{
    multimap<string,Attribute *>::iterator  it;

    for ( it = _attributes.begin(); it != _attributes.end(); it++)
    {
        delete it->second;
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


string AttributeDoc::serialize() const 
{
    multimap<string,Attribute *>::const_iterator  it;
    ostringstream                   		oss;

    oss << "<" << _xml_root << ">";

    for ( it = _attributes.begin(); it!=_attributes.end(); it++)
    {
        oss << it->second->Serialize();
    }

    oss << "</" << _xml_root << ">";

    return oss.str();
	 
}

/* -------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------ */

bool AttributeDoc::deserialize(const string &xml_str) 
{
    xml_document<> xml_doc;
    char        *pxmlstr   = NULL;
    xml_node<>  *root_node = NULL; 
	xml_node<>  *cur_node  = NULL;
	Attribute   *attr      = NULL;
	
    try
    {
    
	// 把xml的字符串解析成 rapidxml doc 
	pxmlstr = xml_doc.allocate_string(xml_str.c_str());
	if (NULL == pxmlstr)
	{
        std::cout <<"Create xml_doc failed"<<std::endl ;
		return false;
	}
	
	xml_doc.parse<parse_declaration_node /* | parse_no_data_nodes */>(pxmlstr);

	// 获取根结点
    root_node = xml_doc.first_node();
	if (NULL == root_node)
	{
        std::cout <<"Can not get root node"<<std::endl ;
		return false;
	}

	
    // 清空 AttributeDoc 里的数据成员
    if (!_attributes.empty())
    {
        multimap<string,Attribute *>::iterator  it;

        for (it = _attributes.begin(); it != _attributes.end(); it++)
        {
            delete it->second;
        }

        _attributes.clear();
    }

    // 获取root下的child结点 并构建attributes.
    for (cur_node = root_node->first_node();
         NULL != cur_node;
         cur_node = cur_node->next_sibling())
    {
        if (node_element == cur_node->type())
        {
            // Try to build a single attr.
            attr = BuildSingleAttr(cur_node);
            if(NULL == attr)
            {
                // Try with a vector attr.
                attr = BuildVectorAttr(cur_node);
            }

            if(NULL != attr)
            {
                Set(attr);
            }
        }
    }

    /* 清除 xml_docment对象 */
    xml_doc.clear();

    }
	catch (const exception& error) //chenww
	{
       cout << error.what() << endl;
       return false;
	}

    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Set(Attribute * attr)
{
    if ( _replace_mode == true )
    {
        multimap<string, Attribute *>::iterator         i;
        pair<
			 multimap<string, Attribute *>::iterator,
             multimap<string, Attribute *>::iterator
            >                                         index;

        index = _attributes.equal_range(attr->get_attribute_name());

        for ( i = index.first; i != index.second; i++)
        {
            delete i->second;
        }

        _attributes.erase(attr->get_attribute_name());
    }

    _attributes.insert(make_pair(attr->get_attribute_name(),attr));
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int AttributeDoc::Remove(const string& name, vector<Attribute *>& values)
{
    multimap<string, Attribute *>::iterator         i;
    pair<
		 multimap<string, Attribute *>::iterator,
         multimap<string, Attribute *>::iterator
        >                                           index;
    int                                             j;

    index = _attributes.equal_range(name);

    for ( i = index.first,j=0 ; i != index.second ; i++,j++ )
    {
        values.push_back(i->second);
    }

    _attributes.erase(index.first,index.second);

    return j;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int AttributeDoc::Erase(const string& name)
{
    multimap<string, Attribute *>::iterator         i;

    pair<
        multimap<string, Attribute *>::iterator,
        multimap<string, Attribute *>::iterator
        >                                           index;
    int                                             j;

    index = _attributes.equal_range(name);

    for ( i = index.first,j=0 ; i != index.second ; i++,j++ )
    {
        Attribute * attr = i->second;
        delete attr;
    }

    _attributes.erase(index.first,index.second);

    return j;

}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int AttributeDoc::get_attributes(multimap<string, Attribute *> &values) const
{
   multimap<string, Attribute *>::const_iterator  iter_multi;
   Attribute  *p_attr;
   Attribute  *p_new_attr;
   int        j;

   for (iter_multi = _attributes.begin(), j = 0; 
        iter_multi !=_attributes.end(); 
		iter_multi++, j++)
   {
      p_attr = iter_multi->second;
	  if (Attribute::SIMPLE == p_attr->get_type())
	  {
	    p_new_attr = new SingleAttribute(p_attr->get_attribute_name(),
			                             ((SingleAttribute *)p_attr)->get_attribute_value());
	  }
	  else
	  {          
         p_new_attr = new VectorAttribute(p_attr->get_attribute_name(), 
		 	                              ((VectorAttribute *)p_attr)->get_attribute_value());
	  }
     values.insert(make_pair(iter_multi->first, p_new_attr));
   }
    return j; 
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int AttributeDoc::Get(
    const string& name,
    vector<const Attribute*>& values) const
{
    multimap<string, Attribute *>::const_iterator       i;
    pair<multimap<string, Attribute *>::const_iterator,
    multimap<string, Attribute *>::const_iterator>      index;
    int                                                 j;

    index = _attributes.equal_range(name);

    for ( i = index.first,j=0 ; i != index.second ; i++,j++ )
    {
        values.push_back(i->second);
    }

    return j;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int AttributeDoc::Get(
    const string& name,
    vector<Attribute*>& values)
{
    multimap<string, Attribute *>::iterator       i;
    pair<multimap<string, Attribute *>::iterator,
    multimap<string, Attribute *>::iterator>      index;
    int                                           j;

    index = _attributes.equal_range(name);

    for ( i = index.first,j=0 ; i != index.second ; i++,j++ )
    {
        values.push_back(i->second);
    }

    return j;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Get(
        string& name,
        string& value) const
{
    vector<const Attribute *>   attrs;
    const SingleAttribute *     sattr;
    int                         rc;

    transform (name.begin(),name.end(),name.begin(),(int(*)(int))toupper);

    rc = Get(name,attrs);

    if  (rc == 0)
    {
        value = "";
        return;
    }

    sattr = dynamic_cast<const SingleAttribute *>(attrs[0]);

    if ( sattr != 0 )
    {
        value = sattr->get_attribute_value();
    }
    else
    {
        value="";
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Get(
        const char  *name,
        string &value) const
{
    string str;
	str = name;
	return Get(str, value);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Get(
        string& name,
        int&    value) const
{
    string sval;

    Get(name, sval);

    if ( sval == "" )
    {
        value = 0;
        return;
    }

    istringstream iss(sval);

    iss >> value;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Get(
        const char *name,
        int   &value) const
{
   string str;
   str = name;
   return Get(str, value);
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Get(
        string &name,
        long long   &value) const
{
    string sval;

    Get(name, sval);

    if ( sval == "" )
    {
        value = 0;
        return;
    }

    istringstream iss(sval);

    iss >> value;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Get(
        const char *name,
        long long   &value) const
{
   string str =name;
   return Get(str, value );
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AttributeDoc::Marshall(string &str, const char delim)
{
    multimap<string,Attribute *>::iterator  it;
    string *                                attr;

    for(it=_attributes.begin(),str="";it!=_attributes.end();it++)
    {
        attr = it->second->Marshall();

        if ( attr == 0 )
        {
            continue;
        }

        str += it->first + "=" + *attr + delim;

        delete attr;
    }
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

string& AttributeDoc::to_str(string& str) const
{
	ostringstream os;
    multimap<string,Attribute *>::const_iterator  it;
    string *                                s;

    for ( it = _attributes.begin(); it!=_attributes.end(); it++)
    {
        s = it->second->Marshall(",");

        os << it->first << _separator << *s << endl;

        delete s;
    }

	str = os.str();
    return str;
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

ostream& operator << (ostream  &oss, const AttributeDoc &ad)
{
	string str;

	oss << ad.to_str(str);

    return oss;
}

}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

