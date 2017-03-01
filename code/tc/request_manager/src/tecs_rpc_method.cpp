/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tecs_rpc_method.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��TecsRpcMethod���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
 �޸����ڣ�2011/11/10
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ� ���Ӳ�����־
*******************************************************************************/
#include "tecs_rpc_method.h"
#include "sky.h"
#include "sessionmanager.h"

using namespace xmlrpc_c;

namespace zte_tecs
{
bool DbgTraceRpcCall = false;
class TecsMethodTrace
{
public:
    TecsMethodTrace()
    {
        _bTraceOut = false;
        _bWriteToOplog = false;
    };
    void EnableWriteToOpLog(bool bWriteToOplog){ _bWriteToOplog = bWriteToOplog; };
    void EnableTraceOut(bool bTraceOut) { _bTraceOut = bTraceOut;};
    ~TecsMethodTrace()
    {
        if (_bWriteToOplog)
        {
            RecOperatorLog(&_log);
        }
        if (_bTraceOut)
        {
            time_t begin_time = _log._begin_time.tointeger();
            time_t end_time   = _log._end_time.tointeger();
            cout << endl
                 << " ## rpc call: " << _log._rpc_name << endl
                 << "   user name: " << _log._user_name << endl
                 << "     user id: " << _log._uid << endl
                 << " from ipaddr: " << _log._client_ip << endl
                 << "  user agent: " << _log._user_agent << endl
                 << "  begin time: " << asctime(localtime(&begin_time))
                 << "    end time: " << asctime(localtime(&end_time))
                 << "  parameters: " << _log._parameters << endl
                 << "     returns: " << _log._returns << endl
                 << "write to log: " << _bWriteToOplog << endl;
        }
    };

    OpLogInfo _log;
private:
    bool _bTraceOut;
    bool _bWriteToOplog;
};

/******************************************************************************/
void TecsRpcMethod::execute(const xmlrpc_c::paramList& paramList,
                            const xmlrpc_c::callInfo * const  callInfoP,
                            xmlrpc_c::value* const retval)
{
    string              session, secret;
    string              username, pass;
    ostringstream       oss;
    int64               rc = -1;
    int                 ErrorCode = 0;
    bool                bNotQueryMethod  = true;

    /*   -- RPC specific vars --  */
    vector<xmlrpc_c::value> arrayData;

    // ��ȡ�û��Ự����
    session = xmlrpc_c::value_string(paramList.getString(0));

    if (_method_name == "tecs.session.login")
    {
        rc = 0;
    }
    else
    {
        secret = SessionManager::GetInstance()->GetSecretBySession(session);

        if(secret.empty())
        {
            ErrorCode = TECS_ERR_SESSION_INVALID;
            oss.str("Session is invalid! Maybe timeout or login at other machine!");          
            goto error_common;        
        }

        // �ж��û���Դ��ָ���Ƿ���Ч
        if (NULL != _upool)
        {
            User::SplitSecret(secret, username, pass);
            
            rc = TecsRpcMethod::_upool->Authenticate(secret);
        }
        else
        {
            goto error_handle;
        }
    }

    // �ж��û��Ự��Ȩ����Ƿ�ɹ�
    if (-1 == rc)
    {
        goto error_authenticate;
    }
    else
    {
        TecsMethodTrace       rpc_trace;   // ��̬����
        string                method_name; // RPC��������
        set_userid(rc);                  // �����û�ID
        set_username(username);          // �����û�����

        // ��ȡע�᷽������
        method_name = get_method_name();

        // �ж��Ƿ��ǲ�ѯ����?
        if (METHOD_READONLY == get_method_type())
        {
            bNotQueryMethod = false;
        }

        if (true == bNotQueryMethod || true == DbgTraceRpcCall)
        {
            // ��¼RPC����
            rpc_trace._log._rpc_name = method_name;

            // ��¼�Ƿ��Query����
            rpc_trace.EnableWriteToOpLog(bNotQueryMethod);

            // ��¼�Ƿ��ӡ���
            rpc_trace.EnableTraceOut(DbgTraceRpcCall);

            // ��¼�û�ID
            rpc_trace._log._uid = get_userid();

            // ��¼�û�����
            rpc_trace._log._user_name = get_username();

            // ��ȡ����������
            const xmlrpc_c::callInfo_serverAbyss * const callInfoPtr(
                dynamic_cast<const xmlrpc_c::callInfo_serverAbyss*>(callInfoP));

            // ��ȡ�ỰChannel��Ϣָ��
            void * chanInfoPtr;
            SessionGetChannelInfo(callInfoPtr->abyssSessionP, &chanInfoPtr);

            // ��ȡ�ỰChannel��Ϣ
            struct abyss_unix_chaninfo * const chanInfoP(
                static_cast<struct abyss_unix_chaninfo *>(chanInfoPtr));

            // ��ȡ�ͻ���ip��ַ
            struct sockaddr const clientAddr(chanInfoP->peerAddr);
            struct sockaddr_in sin;

            // ǿ��ת��Ϊsocketaddr_in
            memcpy(&sin, &clientAddr, sizeof(sin));

            // ��¼�ͻ���ip��ַ
            rpc_trace._log._client_ip = inet_ntoa(sin.sin_addr);

            const TRequestInfo * RequestInfoP;
            // ��ȡTRequestInfo����ָ��
            SessionGetRequestInfo(callInfoPtr->abyssSessionP, &RequestInfoP);

            // ��¼user_agent
            rpc_trace._log._user_agent = RequestHeaderValue(callInfoPtr->abyssSessionP, "user-agent");

            // ��¼���
            rpc_trace._log._parameters = ParamListToXml(paramList);
            rpc_trace._log._visible = 1;

            // ��ȡ���ÿ�ʼʱ��
            Datetime begin_time(time(0));
            rpc_trace._log._begin_time = begin_time;
        }

        // ���þ��巽��
        MethodEntry(paramList,retval);

        if (true == bNotQueryMethod || true == DbgTraceRpcCall)
        {
            // ��ȡ���ý���ʱ��
            Datetime end_time(time(0));
            rpc_trace._log._end_time = end_time;
            // ��¼���ز���
            rpc_trace._log._returns =  ParamValue(*retval);
        }
        return;
    }

error_handle:
    oss.str(HandleError(get_method_name()));
    ErrorCode = TECS_ERR_AUTHENTICATE_INVALID;
    goto error_common;

error_authenticate:
    oss.str(AuthenticateError(get_method_name()));
    ErrorCode = TECS_ERR_AUTHENTICATE_FAILED;
    goto error_common;

error_common:
    arrayData.push_back(xmlrpc_c::value_int(ErrorCode));        // ����ʧ��
    arrayData.push_back(xmlrpc_c::value_string(oss.str())); // ����ʧ��ԭ��

    //��¼����־
    OutPut(SYS_ERROR, "%s\n", oss.str().c_str());

    xmlrpc_c::value_array arrayresult_error(arrayData);

    *retval = arrayresult_error;

    return;
}

/******************************************************************************/
string TecsRpcMethod::HandleError(const string& method)
{
    ostringstream oss;

    oss << "[" << method << "]" << " Authentication handler is invalid,"
        << " aborting call.";

    return oss.str();

}

/******************************************************************************/
string TecsRpcMethod::SessionError(const string& method)
{
    ostringstream oss;

    oss << "[" << method << "]" << " Session format is invalid,"
        << " aborting call.";

    return oss.str();

}
/******************************************************************************/
string TecsRpcMethod::AuthenticateError(const string& method)
{
    ostringstream oss;

    oss << "[" << method << "]" << " User can not be authenticated,"
        << " aborting call.";

    return oss.str();

}

/******************************************************************************/
string TecsRpcMethod::AuthorizeError(const string& method,
                                     const string& object,
                                     const string& action,
                                     int64 user_id,
                                     int64 object_id)
{
    ostringstream oss;
    oss << "[" << method << "]" << " User [" << user_id << "] can not be "
        << " authorized to perform " << action << " on " << object;


    if (-1 != object_id)
    {
        oss << " [" << object_id << "].";
    }
    else
    {
        oss << " Pool";
    }

    return oss.str();
}

/******************************************************************************/
string TecsRpcMethod::GetObjectError(const string& method,
                                     const string& object,
                                     int64 object_id)
{

    ostringstream oss;

    oss << "[" << method << "]" << " Error getting " << object;

    switch(object_id)
    {
    case -2:
        oss << ".";
        break;
    case -1:
        oss << " Pool.";
        break;
    default:
        oss << " [" << object_id << "].";
        break;
    }

    return oss.str();
}

/******************************************************************************/
string TecsRpcMethod::ActionError(const string& method,
                                  const string& object,
                                  const string& action,
                                  int64 object_id,
                                  int64 error_code)
{
    ostringstream oss;

    oss << "[" << method << "]" << " Error trying to " << action << " "
        << object;

    switch(object_id)
    {
    case -2:
        oss << ".";
        break;

    case -1:
        oss << "Pool.";
        break;

    default:
        oss << " [" << object_id << "].";
        break;

    }

    if (0 != error_code)
    {
        oss << " Returned error code [" << error_code << "].";
    }

    return oss.str();
}

/******************************************************************************/
string TecsRpcMethod::ParamListToXml(const xmlrpc_c::paramList& paramList)
{
    ostringstream oss;

    for(unsigned int i = 1; i < paramList.size(); i++)
    {
        oss << ParamValue(paramList[i]);
    }

    return oss.str();
}

/******************************************************************************/
string TecsRpcMethod::ParamValue(const xmlrpc_c::value xmlrpc_value)
{
    ostringstream oss;

    if (false == xmlrpc_value.isInstantiated())
    {
        oss << "Uninstantiated value, please check the *retval of rpc method!";
        return oss.str();
    }

    switch (xmlrpc_value.type())
    {
    case value::TYPE_INT:
        {
            xmlrpc_c:: value_int t_value(xmlrpc_value);
            oss << "<int>" << t_value.cvalue() << "</int>";
        }
        break;

    case xmlrpc_c::value::TYPE_BOOLEAN:
        {
            xmlrpc_c:: value_boolean t_value(xmlrpc_value);
            oss << "<boolean>" << t_value.cvalue() << "</boolean>";
        }
        break;

    case xmlrpc_c::value::TYPE_STRING:
        {
            xmlrpc_c:: value_string t_value(xmlrpc_value);
            oss << "<string>" << t_value.cvalue() << "</string>";
        }
        break;

    case xmlrpc_c::value::TYPE_DATETIME:
        {
            xmlrpc_c:: value_datetime t_value(xmlrpc_value);
            oss << "<datetime>" << t_value.cvalue() << "</datetime>";
        }
        break;

    case xmlrpc_c::value::TYPE_BYTESTRING:
        {
            xmlrpc_c::value_bytestring v_bstring(xmlrpc_value);
            vector <unsigned char> t_vector = v_bstring.vectorUcharValue();
            vector <unsigned char>::iterator iter;

            oss << "<bytestring>";

            for( iter = t_vector.begin(); iter != t_vector.end(); iter++ )
            {
                oss << (*iter) << " ";
            }

            oss << "</bytestring>";
        }
        break;

    case xmlrpc_c::value::TYPE_I8:
        {
            xmlrpc_c:: value_i8 t_value(xmlrpc_value);
            oss << "<i8>" <<  t_value.cvalue() <<"</i8>";
        }
        break;

    case xmlrpc_c::value::TYPE_NIL:
        {
            xmlrpc_c:: value_nil t_value(xmlrpc_value);
            oss << "<nil>" << t_value.cvalue() <<"</nil>";
        }
        break;

    case xmlrpc_c::value::TYPE_DOUBLE:
        {
            xmlrpc_c:: value_double t_value(xmlrpc_value);
            oss << "<double>" << t_value.cvalue() <<"</double>";
        }
        break;

    case xmlrpc_c::value::TYPE_ARRAY:
        {
            xmlrpc_c::value_array v_array(xmlrpc_value);
            vector<xmlrpc_c::value> t_array = v_array.vectorValueValue();
            vector<xmlrpc_c::value>::iterator iter;

            oss << "<array>";

            for( iter = t_array.begin(); iter != t_array.end(); iter++ )
            {
                oss << ParamValue(*iter);
            }

            oss << "</array>";
        }

        break;

    case xmlrpc_c::value::TYPE_STRUCT:
        {
            string param_name;

            map <string,xmlrpc_c::value> t_struct =
                xmlrpc_c::value_struct(xmlrpc_value) ;

            map <string,xmlrpc_c::value>::iterator iter;

            oss << "<struct>";

            for(iter = t_struct.begin(); iter != t_struct.end(); ++iter)
            {
                param_name = iter->first;
                oss << "<" << param_name << ">";
                oss << ParamValue(iter->second);
                oss << "</" << param_name << ">";
            }

            oss << "</struct>";
        }

        break;

    case xmlrpc_c::value::TYPE_C_PTR:

        break;

    default:
        break;
    }

    return oss.str();
}

DEFINE_DEBUG_VAR(DbgTraceRpcCall);
/******************************************************************************/
}//namespace zte_tecs
/******************************************************************************/

