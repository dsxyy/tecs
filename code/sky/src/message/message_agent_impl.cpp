/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：message_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：消息代理Qpid实现文件
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
#include "message_agent_impl.h"

static int ma_impl_print_on = 0;
DEFINE_DEBUG_VAR(ma_impl_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(ma_impl_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

//Qpid: in the AMQP 0-10 mapping, a topic node defaults to the topic exchange,
//x-declare may be used to specify other exchange types.
//from <<Red_Hat_Enterprise_MRG-1.3-Programming_in_Apache_Qpid-en-US.pdf>>
string ChannelAgentQpid::topic_address_option = \
        "{create: always, delete:never, \
        node:{ type: topic, durable: false },link:{reliability:unreliable}}";

string ChannelAgentQpid::fanout_address_option = \
        "{create: always, delete:never, \
        node:{ type: topic, durable: false,\
        x-declare:{type:fanout}}, link:{reliability:unreliable}}";

const string SKY_TOPIC_EXCHANGE = "sky.topic";
const string SKY_FANOUT_EXCHANGE_PREFIX = "sky_mc_";

void MultiRelations::ShowMembers()
{
    map<string,NameList>::iterator it;

    printf("--------------member groups-----------\n");
    for (it=members.begin();it!=members.end();it++)
    {
        printf("member: %s\n", it->first.c_str());
        printf("groups = ");
        NameList::iterator git;
        for (git = it->second.begin();git != it->second.end();git++)
        {
            printf("%s ",git->c_str());
        }
        printf("\n");
    }
}

void MultiRelations::ShowGroups()
{
    map<string,NameList>::iterator it;
    printf("--------------group members-----------\n");
    for (it=groups.begin();it!=groups.end();it++)
    {
        printf("group: %s\n", it->first.c_str());
        printf("members = ");
        NameList::iterator git;
        for (git = it->second.begin();git != it->second.end();git++)
        {
            printf("%s ",git->c_str());
        }
        printf("\n");
    }
}

void MultiRelations::DeleteMember(const string & member)
{
    const NameList *grouplist = GetGroupList(member);
    if (!grouplist)
    {
        //成员无任何归属组，即成员不存在
        return;
    }

    //将所属组列表信息复制出来使用
    NameList::const_iterator it;
    NameList temp(*grouplist);
    for (it = temp.begin();it != temp.end();it++)
    {
        Debug("Delete member %s from group %s ...\n",member.c_str(),it->c_str());
        DelMemberFromGroup(member,*it);
    }
}

void MultiRelations::DeleteGroup(const string & group)
{
    const NameList *memberlist = GetMemberList(group);
    if (!memberlist)
    {
        //组中无成员，即组不存在
        return;
    }

    //将组成员信息复制出来使用
    NameList temp(*memberlist);
    NameList::const_iterator it;
    for (it = temp.begin();it != temp.end();it++)
    {
        DelMemberFromGroup(*it,group);
    }
}

bool MultiRelations::HasGroup(const string & group)
{
    map<string,NameList>::iterator it;
    it = groups.find(group);
    if (it == groups.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

const NameList* MultiRelations::GetMemberList(const string & group)
{
    map<string,NameList>::iterator it;
    it = groups.find(group);
    if (it == groups.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
};

const NameList* MultiRelations::GetGroupList(const string & member)
{
    map<string,NameList>::iterator it;
    it = members.find(member);
    if (it == members.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
};

void MultiRelations::AddMemberToGroup(const string & member,const string & group)
{
    map<string,NameList>::iterator it;

    //将成员加入组
    it = groups.find(group);
    if (it == groups.end())
    {
        //还不存在的组，临时新建一个，将成员加入
        groups[group].push_back(member);
    }
    else
    {
        NameList::iterator mit;
        for (mit=it->second.begin();mit!=it->second.end();mit++)
        {
            if (*mit == member)
            {
                //不允许插入重复元素
                goto update_grps;
            }
        }
        it->second.push_back(member);
    }

update_grps:
    //更新成员所属组
    it = members.find(member);
    if (it == members.end())
    {
        //还不存在的成员，临时新建一个，更新其所属组
        members[member].push_back(group);
    }
    else
    {
        NameList::iterator git;
        for (git=it->second.begin();git!=it->second.end();git++)
        {
            if (*git == group)
            {
                //不允许插入重复元素
                return;
            }
        }
        it->second.push_back(group);
    }
}

void MultiRelations::DelMemberFromGroup(const string & member,const string & group)
{
    map<string,NameList>::iterator it;

    //将成员从组中删除
    it = groups.find(group);
    if (it != groups.end())
    {
        NameList::iterator mit;
        for (mit=it->second.begin();mit!=it->second.end();)
        {
            if (*mit == member)
            {
                //已经保证不会存在重复成员
                //找到之后删除即可，不需全部遍历
                mit = it->second.erase(mit);
                break;
            }
            else
            {
                ++mit;
            }
        }

        //如果组中已经没有任何成员，则将其删除
        if (groups[group].empty())
        {
            Debug("Delete empty group %s ...\n",group.c_str());
            groups.erase(it);
        }
    }

    //更新成员所属组
    it = members.find(member);
    if (it != members.end())
    {
        NameList::iterator git;
        for (git=it->second.begin();git!=it->second.end();)
        {
            if (*git == group)
            {
                //已经保证不会存在重复成员
                //找到之后删除即可，不需全部遍历
                git = it->second.erase(git);
                break;
            }
            else
            {
                ++git;
            }
        }

        //如果成员已经不属于任何组，则将其删除
        if (it->second.empty())
        {
            members.erase(it);
        }
    }
}

extern "C" void *maWorkLoopTaskEntry(void *arg)
{
    if ( arg == 0 )
    {
        //todo: 记录日志
        printf("maWorkLoopTaskEntry failed to get arg!\n");
        SkyExit(-1, "maWorkLoopTaskEntry: 0 == arg.");
        return 0;
    }

    ChannelAgentQpid *agt = static_cast<ChannelAgentQpid *>(arg);
    agt->WorkLoop();
    return 0;
}

STATUS ChannelAgentQpid::Connect(void)
{
    if(_connection)
    {
        return ERROR_DUPLICATED_OP;
    }

    _connection = new QpidConnection(_broker,_option);
    if (!_connection)
    {
        printf("MessageAgentQpidImpl Connect failed!");
        return ERROR;
    }

    try
    {
        //建立与消息服务器的AMQP连接
        _connection->open();
        //创建消息代理工作线程与消息服务器之间的AMQP会话
        _channel_session = _connection->createSession();

        //创建用于点对点单播消息的Exchange,Sender和Receiver
        string exchange = SKY_TOPIC_EXCHANGE;
        string address_send = exchange + "/" + "default" + ";" + TopicAddressOption();
        QpidSender sender = _channel_session.createSender(address_send);
        _channel_sender = sender.getName();
        
        string subject_receive = _subject;
        string address_receive = exchange + "/" + subject_receive + ";" + TopicAddressOption();
        QpidReceiver receiver = _channel_session.createReceiver(address_receive);
        _channel_receiver = receiver.getName();
        
        //设置Receiver的prefetch数量，这样才能启用会话的nextReceiver操作
        receiver.setCapacity(1);
        //这里的sender和receiver只是一个引用，本函数返回之后仍然存在

        //todo: 将来可以考虑为本集群内通信单独设置一个exchange
        //但是这样的话sky要看到tecs的架构了
    }
    catch (const exception& error)
    {
        printf("Failed to connect to message broker %s!\n",_broker.c_str());
        cout << error.what() << endl;
        return ERROR;
    }
    return SUCCESS;
}

STATUS ChannelAgentQpid::CreateSender(const string & name)
{
    //未连接到服务器之前，不能创建会话
    if (!_connection)
        return ERROR_NOT_READY;

    //不允许注册无名会话
    if (name.empty())
        return ERROR_INVALID_ARGUMENT;
        
    //创建一个私有的Qpid会话
    MutexLock lock_sessions (_sessions_mutex);
    try
    {
        _sessions[name] = _connection->createSession();
    }
    catch (const exception& error)
    {
        Debug("Failed to create session for name %s!\n",name.c_str());
        cout << error.what() << endl;
        return ERROR;
    }

    //私有会话创建一个sender对象来发送消息
    string exchange = SKY_TOPIC_EXCHANGE;
    string address_send = exchange + "/" + "unknown" + ";" + topic_address_option;
    string sender_name;
    try
    {
        QpidSender sender = _sessions[name].createSender(address_send);
        sender_name = sender.getName();
        Debug("New qpid sender created! sender name = %s!\n",sender_name.c_str());
    }
    catch (const exception& error)
    {
        Debug("Failed to create sender for name %s!\n",name.c_str());
        cout << error.what() << endl;
        return ERROR;
    }

    //将sender的名称保存起来供以后用于获取该sender的引用
    //这个名称在session内部是唯一的
    _senders.insert(make_pair(name,sender_name));
    return SUCCESS;
}

STATUS ChannelAgentQpid::DeleteSender(const string & name)
{
    //关闭一个有名私有会话
    Debug("Close name %s session ...\n",name.c_str());
    map<string,QpidSession>::iterator sit;
    MutexLock lock2(_sessions_mutex);
    sit = _sessions.find(name);
    if (sit != _sessions.end())
    {
        //会话一旦关闭，建立在其上的sender和receiver会被qpid释放
        sit->second.close();
        _sessions.erase(sit);
    }
    _senders.erase(name);

    //删除组播sender
    string mc_sender_idx_part = name + ".";
    map<string,string>::iterator mit = _mc_senders.begin();
    while(mit != _mc_senders.end())
    {
        if(mit->first.find(mc_sender_idx_part,0) != string::npos)
        {
            _mc_senders.erase(mit++);
        }
        else
        {
            ++mit;
        }
    }

    return SUCCESS;
}

STATUS ChannelAgentQpid::RegisterHandler(MessageFrameHandler *phandler)
{
    if (_handler)
    {
        return ERROR_DUPLICATED_OP;
    }
    Debug("Register messageframe handler to %s broker.\n",_broker.c_str());
    _handler = phandler;

    return SUCCESS;
}

STATUS ChannelAgentQpid::CreateMcReceiver(const string & group)
{
    MutexLock lock(_mc_receivers_mutex);
    //如果所需的组播receiver已经缓存在表中，直接返回即可
    map<string,string>::iterator it;
    it = _mc_receivers.find(group);
    if(it != _mc_receivers.end())
    {
        Debug("Receiver for mc group %s already exists!\n",group.c_str());
        return SUCCESS;
    }

    //否则利用消息代理的session创建一个组播recevier
    string receiver_name;
    try
    {
        string exchange = SKY_FANOUT_EXCHANGE_PREFIX + group;
        string address = exchange + "/" + "default" + ";" + FanoutAddressOption(group);
        QpidReceiver receiver = _channel_session.createReceiver(address);
        //设置Receiver的prefetch数量，这样才能启用会话的nextReceiver操作
        receiver.setCapacity(1);
        receiver_name = receiver.getName();
    }
    catch (const exception& error)
    {
        Debug("Failed to create receiver for mc group %s!\n",group.c_str());
        cout << error.what() << endl;
        return ERROR;
    }

    //保存receiver的名称，以后用来引用本receiver的对象
    //receiver对象的名称字符串在消息代理的session内部是唯一的
    _mc_receivers.insert(make_pair(group,receiver_name));
    return SUCCESS;
}

STATUS ChannelAgentQpid::DeleteMcReceiver(const string & group)
{
    MutexLock lock(_mc_receivers_mutex);
    map<string,string>::iterator it;
    it = _mc_receivers.find(group);
    if(it == _mc_receivers.end())
    {
        Debug("No mc receiver for group %s\n",group.c_str());
        return SUCCESS;
    }

    //删除之前要先close，这样就不会再收到组播消息了
    //这里删除掉的recevier对象其实只是Qpid内部对象的一个引用
    //Qpid并未提供真正"删除"一个receiver对象的接口
    QpidReceiver receiver = _channel_session.getReceiver(it->second);
    receiver.close();
    _mc_receivers.erase(it);
    Debug("Mc receiver %s has been removed!\n",group.c_str());
    return SUCCESS;
}

STATUS ChannelAgentQpid::SendMcMessage(const string& data,const MessageOption& option)
{
    //获取消息单元私有会话
    MutexLock lock1(_sessions_mutex);
    map<string,QpidSession>::iterator sit;
    sit = _sessions.find(option.sender()._unit);
    if (sit == _sessions.end())
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    //获取消息单元组播sender，注意: 针对要发往的组播组，每个发送方向都有一个sender
    string mc_sender_name;
    string mc_sender_idx = option.sender()._unit + "." + option.receiver()._unit;
    map<string,string>::iterator msit = _mc_senders.find(mc_sender_idx);
    if(msit != _mc_senders.end())
    {
        //已经创建过组播sender,获取其名称
        mc_sender_name = msit->second;
    }
    else
    {
        //尚未创建过组播sender，需要新建一个
        //向指定的组播Exchange发送消息，对于组播subject域没有实际作用
        string exchange = SKY_FANOUT_EXCHANGE_PREFIX + option.receiver()._unit;
        string address_send = exchange + "/" + "default" + ";" + fanout_address_option;
        //使用该消息单元的私有会话临时创建一个组播sender
        try
        {
            QpidSender sender = sit->second.createSender(address_send);
            mc_sender_name = sender.getName();
            //将新创建的组播sender名称保存起来，供后续用来获取其引用
            _mc_senders.insert(make_pair(mc_sender_idx,mc_sender_name));
        }
        catch (const exception& error)
        {
            cout << error.what() << endl;
            return ERROR;
        }
    }

    STATUS ret;
    try
    {
        Debug("Channel: mc send %u from [%s.%s.%s] to [%s.%s.%s].\n",
            option.id(),
            option.sender()._application.c_str(),
            option.sender()._process.c_str(),
            option.sender()._unit.c_str(),
            option.receiver()._application.c_str(),
            option.receiver()._process.c_str(),
            option.receiver()._unit.c_str());
        QpidSender sender = sit->second.getSender(mc_sender_name);
        ret = QpidSend(sender,string("default"),data,option);
    }
    catch (const exception& error)
    {
        cout << error.what() << endl;
        return ERROR;
    }

    return ret;
}

STATUS ChannelAgentQpid::SendMessage(const string& data,const MessageOption& option)
{
    STATUS ret = ERROR;
    //组播消息发送
    if (option.receiver()._process == "group")
    {
        return SendMcMessage(data, option);
    }

    QpidSession *psession = NULL;
    string sender_name;
    
    if(option.sender()._unit == MESSAGE_AGENT)
    {
        //以消息代理自身名义发送进程间消息时，使用_agent_session会话
        psession = &_channel_session;
        sender_name = _channel_sender;
    }
    else
    {
        //消息单元需要发送进程间消息时，需要使用该消息单元自己的私有会话，
        _sessions_mutex.Lock();
        map<string,QpidSession>::iterator it;
        it = _sessions.find(option.sender()._unit);
        if (it == _sessions.end())
        {
            _sessions_mutex.Unlock();
            return ERROR_OBJECT_NOT_EXIST;
        }
        psession = &(it->second);
        sender_name = _senders[option.sender()._unit];
        _sessions_mutex.Unlock();
    }

    Debug("Channel: send %u from [%s.%s.%s] to [%s.%s.%s] by sender: %s.\n",
        option.id(),
        option.sender()._application.c_str(),
        option.sender()._process.c_str(),
        option.sender()._unit.c_str(),
        option.receiver()._application.c_str(),
        option.receiver()._process.c_str(),
        option.receiver()._unit.c_str(),
        sender_name.c_str());
        
    try
    {
        QpidSender sender = psession->getSender(sender_name);
        string subject = option.receiver()._application + "." + \
                     option.receiver()._process + "." + \
                     option.receiver()._unit;
        ret = QpidSend(sender, subject, data, option);
        if(SUCCESS != ret)
        {
            Debug("QpidSend failed! ret = %d\n",ret);
        }
    }
    catch (const exception& error)
    {
        ostringstream err;
        err << "Qpid send error:" << error.what() << endl;
        SkyExit(-1,err.str());
        return ERROR;
    }
    return ret;
}

STATUS ChannelAgentQpid::DirectSend(const string& data,const MessageOption& option)
{
    //本函数不使用任何消息单元甚至消息代理的会话，而是在AMQP连接上另建会话直接发送消息
    QpidSession session = _connection->createSession();
    string exchange = SKY_TOPIC_EXCHANGE;
    string address_send = exchange + "/" + "unknown" + ";" + topic_address_option;
    QpidSender sender = session.createSender(address_send);
    string subject = option.receiver()._application + "." + \
                     option.receiver()._process + "." + \
                     option.receiver()._unit;
    STATUS ret = QpidSend(sender, subject, data, option);
    session.close();
    return ret;
}

void ChannelAgentQpid::WorkLoop()
{
    Debug("Channel agent %s %s running!\n",_broker.c_str(),_subject.c_str());
    string contenttype;
    Variant::Map content;
    QpidMessage qpidmsg;
    int count = 0;

    while (1)
    {
        Debug("Channel agent %s %s running!\n",_broker.c_str(),_subject.c_str());
        QpidReceiver receiver;
        try
        {
            if (false == _channel_session.nextReceiver(receiver, Duration::FOREVER))
            {
                continue;
            }

            //接收
            if (false == receiver.fetch(qpidmsg,Duration::FOREVER))
            {
                continue;
            }

            //cout << "Message agent receive message subject: " << qpidmsg.getSubject() << endl;
            contenttype = qpidmsg.getContentType();
            if ("amqp/map" == contenttype)
            {
                content.clear();
                decode(qpidmsg,content); //解码
            }
            else
            {
                Debug("incorrect content type: %s\n",contenttype.c_str());
                continue;
            }
        }
        catch (const exception& error)
        {
            ostringstream err;
            err << "Qpid receive error:" << error.what() << endl;
            SkyExit(-1,err.str());
            continue;
        }
        _messages_from_broker++;

        MessageOption option;
        string body;
        //todo:查找目的unit不需要将body也unpack出来，这里可以优化一下
        QpidUnPack(body,option,content);
        MessageFrame message(body,option);
        Debug("%s %s MessageAgent: Message %u from [%s.%s.%s] to [%s.%s.%s].\n",
                    _broker.c_str(),_subject.c_str(),
                    option.id(),
                    option.sender()._application.c_str(),
                    option.sender()._process.c_str(),
                    option.sender()._unit.c_str(),
                    option.receiver()._application.c_str(),
                    option.receiver()._process.c_str(),
                    option.receiver()._unit.c_str());

        if (!_handler)
        {
            Debug(" The %s has no messageframe handler!\n",_broker.c_str());
            continue;
        }

        if (ERR_MCRECEIVER_NOT_EXIST == _handler->Receive(message,_broker))
        {
            DeleteMcReceiver(message.option.receiver()._unit);
        }
        
        //当接收到的消息数量较多时，一次性确认
        //todo:需要研究qpid能否取消这个确认过程
        count++;
        if (count == 100)
        {
            count = 0;
            _channel_session.acknowledge(false);
        }
    }
}

STATUS ChannelAgentQpid::Start(int priority)
{
    if (_workthread_id != INVALID_THREAD_ID)
        return ERROR_DUPLICATED_OP;

    _workthread_id = StartThread("ChannelAgent",maWorkLoopTaskEntry,(void*)this);
    if(INVALID_THREAD_ID == _workthread_id)
        return ERROR;
    else
        return SUCCESS;
}

STATUS ChannelAgentQpid::Stop(void)
{
    if(INVALID_THREAD_ID != _workthread_id)
            KillThread(_workthread_id);
    _workthread_id = INVALID_THREAD_ID;

    return SUCCESS;
}

int ChannelAgentQpid::SendDebug()
{
    //std::string broker = "localhost:5672";
    std::string connectionOptions = "";
    //string address_send = "zwj.topic/test; {create: always, delete:always, node:{type: topic, durable: false}}";
    string exchange = SKY_TOPIC_EXCHANGE;
    //string subject_send = "test";
    string subject_send = "mycloud.mutest2.test";
    string address_send = exchange + "/" + subject_send + ";" + topic_address_option;

    cout << "broker "<< _broker << " address_send=" <<  address_send << endl;
    Connection debug_connection(_broker, connectionOptions);

    try
    {
        debug_connection.open();
        Session debug_session = debug_connection.createSession();
        Sender debug_sender = debug_session.createSender(address_send);

        DemoMessage tecsmsg("hello,world!");
        MessageOption option;
        option.setId(123);
        option.setType(456);
        option.setVersion(789);
        Variant::Map content;
        content.clear();
        QpidPack(tecsmsg.serialize(),option,content); //打包
        QpidMessage qpidmsg;
        encode(content,qpidmsg); //编码

        //Message qpidmsg("hello");
        for (int i=0;i<10;i++)
        {
            debug_sender.send(qpidmsg);
        }
        debug_connection.close();
        return 0;
    }
    catch (const std::exception& error)
    {
        cerr << error.what() << endl;
        debug_connection.close();
        return 1;
    }

    return 0;
}

int ChannelAgentQpid::ReceiveDebug()
{
    //std::string broker = "localhost:5672";
    std::string connectionOptions = "";
    //string address_recv = "zwj.topic/test; {create: always, delete:always, mode:consume,node:{type: topic, durable: false}}";
    string exchange = SKY_TOPIC_EXCHANGE;
    //string subject_receive = _application + "." +  _process + ".test";
    //string subject_receive = "test";
    string address_option ="{create: always, delete:never," \
                        "node:{ type: topic, durable: false }}";
    string subject_receive = "mycloud.mutest2.test";
    string address_recv = exchange + "/" + subject_receive + ";" + address_option;
    cout << "broker="<< _broker <<" address_recv=" <<  address_recv << endl;

    Connection debug_connection(_broker, connectionOptions);
    try
    {
        debug_connection.open();
        Session debug_session = debug_connection.createSession();
        Receiver debug_receiver = debug_session.createReceiver(address_recv);
        while (1)
        {
            Message message = debug_receiver.fetch(Duration::SECOND * 1000);
            Variant::Map content;
            content.clear();
            decode(message,content); //解码
            //content = message.getContent();
            cout << "message=" <<  content["body"] << endl;
            debug_session.acknowledge(false);
        }

        debug_connection.close();
        return 0;
    }
    catch (const exception& error)
    {
        cerr << error.what() << endl;
        debug_connection.close();
        return 1;
    }

    return 0;
}

STATUS ChannelAgentQpid::Summary()
{
    cout << "broker: " << _broker << endl;
    cout << "messages in: " << _messages_from_broker << endl;
    cout << "messages out: " << _messages_to_broker << endl;
    cout <<"--------------------------------------------"<< endl;
    return SUCCESS;
}

STATUS ChannelAgentQpid::ShowMcTable()
{
    printf("broker = %s\n",_broker.c_str());

    MutexLock lock2(_mc_receivers_mutex);
    map<string,string>::iterator it;
    for(it=_mc_receivers.begin();it!=_mc_receivers.end();it++)
    {
        printf("%s %s\n",it->first.c_str(),it->second.c_str());
    }
    return SUCCESS;
}

STATUS ChannelAgentQpid::QpidPack(const string& data, const MessageOption& option,Variant::Map &content)
{
    //cout << "MessageAgentQpidImpl::pack" << endl;
    content["type"] = option.type();
    content["id"] = option.id();
    content["header_version"] = option.version();
    content["body_version"] = option.version();
    content["priority"] = option.priority();
    content["sender_application"] = option.sender()._application;
    content["sender_process"] = option.sender()._process;
    content["sender_unit"] = option.sender()._unit;
    content["receiver_application"] = option.receiver()._application;
    content["receiver_process"] = option.receiver()._process;
    content["receiver_unit"] = option.receiver()._unit;

    content["body"] = data;
    return SUCCESS;
}

STATUS ChannelAgentQpid::QpidUnPack(string& data,MessageOption& option,Variant::Map &content)
{
    option.setType(content["type"]);
    option.setId(content["id"]);
    option.setVersion(content["header_version"]);
    option.setPriority(content["priority"]);

    MID receiver;
    receiver._application = content["receiver_application"].getString();
    receiver._process = content["receiver_process"].getString();
    receiver._unit = content["receiver_unit"].getString();
    option.setReceiver(receiver);

    MID sender;
    sender._application = content["sender_application"].getString();
    sender._process = content["sender_process"].getString();
    sender._unit = content["sender_unit"].getString();
    option.setSender(sender);

    data = content["body"].getString();
    return SUCCESS;
}

STATUS ChannelAgentQpid::QpidSend(QpidSender& sender,
    const string subject,
    const string& data,
    const MessageOption& option)
{
    try
    {
        Variant::Map content;
        QpidPack(data,option,content);
        QpidMessage qpidmsg;
        encode(content,qpidmsg); //编码
        qpidmsg.setSubject(subject); //设置主题
        qpidmsg.setContentType("amqp/map"); //设置类型
        sender.send(qpidmsg); //发送
    }
    catch (const exception& error)
    {
        Debug("QpidSend exception!\n");
        cout << error.what() << endl;
        return ERROR;
    }

    _messages_to_broker++;
    return SUCCESS;
}

//link:{name:sky.topic_tecs,reliability:unreliable}}"; unreliable exactly-once*/
string&  ChannelAgentQpid::TopicAddressOption()
{
    if (_is_ack_required)
    {
        string name = "name:" + SKY_TOPIC_EXCHANGE + "_" + _subject;
        string reliable = "reliability:exactly-once";
        string link = "link:{" + name + "," + reliable + "}";
        string create = "create: always, delete:never";
        string node = "node:{ type: topic, durable: false }";
        topic_address_option = "{" + create + "," + node + "," + link + "}";                     
    }
    return topic_address_option;
}

string& ChannelAgentQpid::FanoutAddressOption(const string& group)
{
    if (_is_ack_required)
    {
        string name = "name:" + SKY_FANOUT_EXCHANGE_PREFIX + group;
        string reliable = "reliability:exactly-once";
        string link = "link:{" + name + "," + reliable + "}";
        string create = "create: always, delete:never";
        string node = "node:{ type: topic, durable: false, x-declare:{type:fanout}}";
        fanout_address_option = "{" + create + "," + node + "," + link + "}";     
    }
    return fanout_address_option;
}

void MessageAgentQpidImpl::TracePrint(const string& data,const MessageOption& option)
{
    uint32 id = 0;

    if(!NeedTraceMessage(option.sender()._unit.c_str(),id))
    {
        return;
    }
    if(id == 0xFFFFFFFF || id == option.id())
    {
	Datetime btime(time(0));
        printf("\n%s\n%s\nsend a message, id = %u, type = %u,\n" 
                "from application = %s ,process = %s, unit = %s,\n"
                "to application = %s ,process = %s, unit = %s,\n"
                "content: \n%s\n",
                "-----------------------message trace-----------------------",
                btime.tostr().c_str(),
                option.id(),
                option.type(),
                option.sender()._application.c_str(),
       		option.sender()._process.c_str(),
                option.sender()._unit.c_str(),
                option.receiver()._application.c_str(),
                option.receiver()._process.c_str(),
                option.receiver()._unit.c_str(),
                data.c_str());
    }
    return;
}

STATUS MessageAgentQpidImpl::SetCommId(const string& application,const string& process)
{
    if (application.empty() || process.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (!_application.empty() && !_process.empty())
    {
        return ERROR_DUPLICATED_OP;
    }

    _application = application;
    _process = process;

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::Connect(const string& broker,const string& option,bool ackRequired)
{
    if (_application.empty() || _process.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }
    
    MutexLock lock_conn(_connect_agents_mutex);
    
    if(_connect_agents[broker])
    {
        return ERROR_DUPLICATED_OP;
    }
    _connect_agents[broker] = new ChannelAgentQpid(broker,option,ackRequired,
                                    _application + "." +  _process + ".*");

    if (!_connect_agents[broker])
    {
        printf("MessageAgentQpidImpl Connect %s failed!",broker.c_str());
        return ERROR;
    }
    _connect_agents[broker]->RegisterHandler(this);
        
    return _connect_agents[broker]->Connect();
}

STATUS MessageAgentQpidImpl::RegisterUnit(MessageUnit *pmu)
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    //未连接到服务器之前，不能注册消息单元
    if (_connect_agents.empty())
        return ERROR_NOT_READY;

    string newunit = pmu->name();

    //不允许注册无名消息单元
    if (newunit.empty())
        return ERROR_INVALID_ARGUMENT;

    //不允许注册消息代理使用的私有保留名称
    if(newunit == MESSAGE_AGENT)
        return ERROR_BAD_ADDRESS;
    
    MutexLock lock (_units_mutex);
    //消息单元名称在进程内不能重复
    if (_units.count(newunit) > 0)
    {
        Debug("Duplicated mu %s!\n",newunit.c_str());
        return ERROR_DUPLICATED_OP;
    }
    
    uint32 id;
    if (NeedTraceMessage(newunit,id))
    {
        pmu->TraceRcvMessage(id);
    }
    //注册消息单元
    _units[newunit] = pmu;

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        ret = cit->second->CreateSender(pmu->name());
        if (SUCCESS != ret)
        {
            printf("Register %s to %s connect failed!",
                newunit.c_str(),cit->first.c_str());
            //如果一个注册失败，则直接返回
            return ret;
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::DeregisterUnit(MessageUnit *pmu)
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    MutexLock lock(_units_mutex);
    if (_units.empty())
    {
        return SUCCESS;
    }

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        Debug("Deregister mu %s ...from %s \n",pmu->name().c_str(),cit->first.c_str());
        ret = cit->second->DeleteSender(pmu->name());
        if (SUCCESS != ret)
        {
            printf("Deregister %s from %s connect failed!",
                pmu->name().c_str(),cit->first.c_str());
            //如果一个注销失败，则不再处理
            return ret;
        }
    }

    //从消息单元注册表中删除
    _units.erase(pmu->name());

    //从组播成员表中删除
    MutexLock lock2(_mc_relation_mutex);
    //DeleteMember函数将当前消息单元从所有归属的组中删除
    //注意该函数如果发现删除消息单元后某个组已经为空，也会进一步将组也删除掉
    _mc_relations.DeleteMember(pmu->name());
    return SUCCESS;
}

MessageUnit *MessageAgentQpidImpl::FindUnit(const string& unit)
{
    map<string,MessageUnit*>::iterator it;
    MutexLock lock (_units_mutex);
    it = _units.find(unit);
    if (it != _units.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }
}

MessageUnit *MessageAgentQpidImpl::CurrentUnit()
{
    THREAD_ID current_thread_id = pthread_self();
    map<string,MessageUnit*>::iterator it;
    MutexLock lock (_units_mutex);
    for ( it=_units.begin() ; it != _units.end(); it++ )
    {
        if (it->second->get_task_id() == current_thread_id)
        {
            return it->second;
        }
    }
    return NULL;
}

STATUS MessageAgentQpidImpl::JoinMcGroup(const string & group,const string & unit)
{
    STATUS ret;
    //首先检查名为unit的消息单元是否存在
    _units_mutex.Lock();
    if (_units.count(unit) == 0)
    {
        _units_mutex.Unlock();
        return ERROR_OBJECT_NOT_EXIST;
    }
    _units_mutex.Unlock();

    //必须先把消息单元加到组播关系表中，否则WorkLoop找不到这个关系，会再次把Receiver释放掉
    _mc_relation_mutex.Lock();
    _mc_relations.AddMemberToGroup(unit, group);
    _mc_relation_mutex.Unlock();

    
    MutexLock lock_conn(_connect_agents_mutex);
    //未连接到服务器之前，不能注册
    if (_connect_agents.empty())
    {
        _mc_relations.DelMemberFromGroup(unit, group);
        return ERROR_NOT_READY;
    }

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        ret = cit->second->CreateMcReceiver(group);
        if (SUCCESS != ret)
        {
            Debug("CreateMcReceiver for group %s to %s failed!\n",
                group.c_str(),cit->first.c_str());
            MutexLock lock(_mc_relation_mutex);
            _mc_relations.DelMemberFromGroup(unit, group);
            return ret;
        }
    }
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::ExitMcGroup(const string & group,const string & unit)
{
    MutexLock lock (_units_mutex);
    if (_units.count(unit) == 0)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    MutexLock lock2(_mc_relation_mutex);
    if (_mc_relations.HasGroup(group) == false)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    //这里不能直接去调用DeleteMcReceiver关闭group receiver，因为receiver还正在被
    //WorkThread的session监听使用中，直接close会导致死锁，这是Qpid的内部实现限制，
    //所以只能暂时缓存Receiver对象，让WorkThread再次收到组播消息之后自己去close

    //删除组播关系
    _mc_relations.DelMemberFromGroup(unit, group);
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::InternalSend(const string& data,const MessageOption& option)
{
    STATUS ret;
    MessageUnit *pmu = NULL;
    _units_mutex.Lock();
    
    map<string,MessageUnit*>::iterator it;
    it = _units.find(option.receiver()._unit);
    if (it != _units.end())
    {
        pmu = it->second;
    }
    else
    {
        _units_mutex.Unlock();
        Debug("Can not find unit [%s] when send message!\n",
            option.receiver()._unit.c_str());
        return ERROR_ADDR_NOT_EXIST;
    }
    MessageFrame frame(data,option);
    ret = pmu->Receive(frame);
    _units_mutex.Unlock();

    return ret;
}

STATUS MessageAgentQpidImpl::RouteSend(const string& route,const string& data,const MessageOption& option)
{
    /* 此处不再加锁，由上层调用者进行加锁 */
    if (_connect_agents.empty())
    {
        return ERROR_NOT_READY;
    }
    return _connect_agents[route]->SendMessage(data,option);
}

STATUS MessageAgentQpidImpl::MutliSend(const string& data,const MessageOption& option)
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);
    
    if (_connect_agents.empty())
    {
        return ERROR_NOT_READY;
    }

    map<string,ChannelAgentQpid*>::iterator cit;
    //没有找路由或组播，则进行双发
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        ret = cit->second->SendMessage(data,option);
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {   
            Debug("Send on connect %s failed!\n",cit->first.c_str());
            return ret;
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::SendMessage(const string& data,const MessageOption& option)
{
    TracePrint(data,option);
        
    //进程内部消息派发
    //如果接收方unit为空，即便是本进程内部消息，也需要到消息服务器绕一圈再回来
    if (option.isLocal() && option.receiver()._unit != MESSAGE_AGENT)
    {
        return InternalSend(data,option);
    }

    //组播消息进行多发
    if (option.receiver()._process == "group")
    {
        return MutliSend(data,option);
    }

    //进行路由查找单发
    if (_connect_agents.size() > 1)
    {
        MutexLock lock_conn(_connect_agents_mutex);
        MutexLock lock_route(_routes_mutex);
        map<string,string>::iterator it;
        it = _routes.find(option.receiver()._application.c_str());
        if(it != _routes.end())
        {
            Debug("The %s.%s.%s'message send on %s route.\n",
                 option.receiver()._application.c_str(),
                 option.receiver()._process.c_str(),
                 option.receiver()._unit.c_str(),
                 it->second.c_str());
            return RouteSend(it->second,data,option);
        }
    }

    //没有路由则多发
    return MutliSend(data,option);
}

STATUS MessageAgentQpidImpl::GetRunningInfo(ProcRuntimeInfo& info)
{
    info.exist = true;
    info.name = _process;

    //采集本进程的运行时信息
    info.pid = getpid();
    T_TimeValue uptime;
    GetTimeFromPowerOn(&uptime);
    info.running_seconds = uptime.second;

    //采集异常信息
    info.exceptions = GetExceptionCount();
    GetAsserts(info.asserts);
    
    //采集关键消息单元运行时信息
    MutexLock lock_units (_units_mutex);
    MutexLock lock_key_units (_key_units_mutex);
    map<string,int64>::iterator kuit;
    map<string,MessageUnit*>::iterator uit;
    for(kuit = _key_units.begin(); kuit != _key_units.end(); kuit++)
    {
        MuRuntimeInfo i(kuit->first);
        i.exist = false;
        //计算一下本消息单元的年龄，即距离注册为关键消息单元已经过去多长时间了
        //age的单位是毫秒
        i.age = (uptime.second * 1000 + uptime.milisecond) - kuit->second;
        
        //在消息单元表中查找其运行时信息
        uit = _units.find(i.name);
        if (uit != _units.end())
        {
            i.exist = true;
            i.state= uit->second->get_state();
            i.pid = uit->second->get_task_tid();
            i.tid = uit->second->get_task_id();
            i.push_count = uit->second->get_push_count();
            i.pop_count = uit->second->get_pop_count();
            i.queued_count = uit->second->get_queued_count();
            i.timer_count = uit->second->GetTimerCount();
            const MessageFrame *p = uit->second->CurrentMessageFrame();
            if(p)
            {
                i.last_message = p->option.id();
            }
        }
        Debug("Sys message: %d\n" 
            "state:%d\n"
            "pid:%d\n"
            "tid:0x%lx\n"
            "push_count:%llu(0x%lx)\n"
            "pop_count:%llu(0x%lx)\n"
            "queued_count:%d(0x%x)\n"
            "timer_count:%d\n"
            "last_message:%d!\n",
            i.exist,i.state,i.pid,i.tid,
            i.push_count,i.push_count,
            i.pop_count,i.pop_count,
            i.queued_count,i.queued_count,
            i.timer_count,i.last_message);
        info.muinfo.push_back(i);
    }

    //采集其它运行时信息，如定时器，线程登记表等
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::DealSysMessage(const MessageFrame& message)
{
    STATUS ret;
    Debug("MessageAgent get a sys message: %s!\n",message.data.c_str());
    //目前的系统管理消息都是使用默认的消息type
    if(message.option.type() != MESSAGE_TYPE_DEFAULT)
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    
    switch(message.option.id()) 
    {
        case EV_RUNTIME_INFO_REQ:
        {
            AppRuntimeInfoReq req;
            req.deserialize(message.data);
            //在req.message_route中查找本application
            vector<MID>::iterator it;
            for(it = req.message_route.begin(); it != req.message_route.end(); it ++)
            {
                if(it->_application == _application)
                    break;
            }
            
            int route_length = req.message_route.size();
            int my_order = distance(req.message_route.begin(),it);
            MID receiver;    
            if(it == req.message_route.end())
            {
                //如果本application没有出现在路由表中，则拒绝处理该请求
                Debug("MessageAgent will ignore req from: %s!\n",message.option.sender().serialize().c_str());
                return ERROR_BAD_ADDRESS;
            }
            else if(my_order + 1 == route_length)
            {
                //如果自己已经是查询的最终目标节点，将采集好的应答消息直接回复给消息发送方
                receiver = message.option.sender();
                Debug("MessageAgent will collect running info for: %s!\n",receiver.serialize().c_str());
                ProcRuntimeInfo info;
                info.message_route = req.message_route;
                if(SUCCESS != GetRunningInfo(info))
                    return ERROR;
                
                MessageOption option(receiver,EV_RUNTIME_INFO_ACK,0,0);
                option.setSender(MID(_application,_process,MESSAGE_AGENT));
                return SendMessage(info.serialize(),option);
            }
            else
            {
                //不是最后一个节点，需要转发给下一节点
                receiver = req.message_route[my_order + 1];
                if(!receiver._process.empty())
                {
                    //下一节点如果填写了进程名称，转发给该进程
                    Debug("MessageAgent will forward running info req to next hop: %s!\n",
                            receiver._application.c_str());
                    MessageOption option(receiver,message.option.id(),message.option.priority(),message.option.version());
                    option.setSender(MID(_application,_process,MESSAGE_AGENT));
                    return SendMessage(message.data,option);
                }
                else
                {
                    //下一节点没有填写进程名称，则转发给它的所有进程
                    Debug("MessageAgent will forward running info req to final target: %s!\n",
                            receiver._application.c_str());
                    vector<string>::iterator itp;
                    for(itp = req.processes.begin(); itp != req.processes.end(); itp ++)
                    {
                        receiver._process = *itp;
                        receiver._unit = MESSAGE_AGENT;
                        MessageOption option(receiver,message.option.id(),message.option.priority(),message.option.version());
                        option.setSender(MID(_application,_process,MESSAGE_AGENT));
                        ret = SendMessage(message.data,option);
                        if(ret != SUCCESS)
                        {
                            Debug("MessageAgent failed to forward running info req to final target: %s!\n",
                                    receiver._application.c_str());
                            return ret;
                        }
                    }
                }
            }
            break;
        }

        case EV_RUNTIME_INFO_ACK:
        {
            ProcRuntimeInfo ack;
            ack.deserialize(message.data);
            //在ack.message_route中查找本application
            vector<MID>::iterator it;
            for(it = ack.message_route.begin(); it != ack.message_route.end(); it ++)
            {
                if(it->_application == _application)
                    break;
            }

            //int route_length = ack.message_route.size();
            int my_order = distance(ack.message_route.begin(),it);

            if(it == ack.message_route.end())
            {
                //如果本进程没有出现在路由表中，则拒绝处理该应答
                return ERROR_BAD_ADDRESS;
            }
            else if(my_order > 0)
            {
                //如果出现了，应该不是第一个，就将消息转发给自己的上一个节点
                MID receiver =ack.message_route[my_order - 1];
                Debug("MessageAgent will forward running info ack to previous hop: %s!\n",
                    receiver.serialize().c_str());
                MessageOption option(receiver,message.option.id(),message.option.priority(),message.option.version());
                option.setSender(MID(_application,_process,MESSAGE_AGENT));
                return SendMessage(message.data,option);
            }
            break;
        }
        
        default:
        {
            Debug("MessageAgent receives unknown message id: %d!\n",message.option.id());
            break;
        }
    }
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::Receive(const MessageFrame& message,const string& broker)
{
    if ("group" == message.option.receiver()._process)
    {
        //过滤自己发给自己的冗余组播消息
        if (FilterRedundantMc(message,broker))
        {
            return SUCCESS;
        }
        
        //如果收到的是组播消息，需要找到组中所有成员，逐个派发消息
        _mc_relation_mutex.Lock();
        const NameList *memberlist = _mc_relations.GetMemberList(message.option.receiver()._unit);
        if (!memberlist)
        {
             _mc_relation_mutex.Unlock();
            //组中无成员，即组已经不存在，这种情况一般不会出现的
            Debug("No member found in mc group %s\n",message.option.receiver()._unit.c_str());
            //不再关闭该receiver，如果存在错误问题，则应由注册与注销模块处理，不在此处处理
            //DeleteMcReceiver(message.option.receiver()._unit);
            return ERR_MCRECEIVER_NOT_EXIST;
        }

        //将组成员信息复制出来使用
        NameList unit_list(*memberlist);
        _mc_relation_mutex.Unlock();

        //向组中每个成员派发消息
        NameList::iterator it;
        for (it = unit_list.begin();it != unit_list.end();it++)
        {
            MutexLock lock (_units_mutex);
            map<string,MessageUnit*>::iterator uit;
            uit = _units.find(*it);
            if (uit != _units.end())
            {
                Debug("Mc message %u dispached to unit %s!\n",
                    message.option.id(),uit->second->name().c_str());
                uit->second->Receive(message); //派发组播消息
            }
            else
            {
                //找不到消息单元的注册信息，可能是消息单元已经反注册了
                Debug("Can not find unit info of %s in group %s\n!",
                    it->c_str(),
                    message.option.receiver()._unit.c_str());
            }
        }
        InsertRoute(message.option.sender()._application,broker);
    }
    else if(message.option.receiver()._unit == MESSAGE_AGENT)
    {
        //如果收到的是单播消息，且目的消息单元为*，则由agent自己处理
        DealSysMessage(message);
    }
    else
    {
        //如果收到的是单播消息，且目的消息单元不为空，直接向接收者派发消息
        _units_mutex.Lock();
        map<string,MessageUnit*>::iterator uit;
        uit = _units.find(message.option.receiver()._unit);
        if (uit != _units.end())
        {
            Debug("Direct message %u dispached to unit %s!\n",message.option.id(),uit->second->name().c_str());
            uit->second->Receive(message); //派发组播消息
        }
        _units_mutex.Unlock();
        InsertRoute(message.option.sender()._application,broker);
    }
    
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::DirectSend(const string& data,MessageOption& option)
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    //未连接到服务器之前，不能发送
    if (_connect_agents.empty())
    {
        return ERROR;
    }

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        ret = cit->second->DirectSend(data,option);
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {
            Debug("Direct send on connect %s failed!\n",cit->first.c_str());
            return ret;
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::InsertRoute(const string& route,const string& broker)
{
    //由于是单连接，则不需要生成路由，单发即可
    if (_connect_agents.size() <= 1)
    {
        return SUCCESS;
    }

    MutexLock lock_route(_routes_mutex);

    if (route.empty())
    {
        printf("Empty %s route\n",route.c_str());
        return SUCCESS;
    }

    map<string,string>::iterator it;
    it = _routes.find(route);
    //不存在，则加入
    if(it == _routes.end())
    {
        Debug("Insert %s route:%s\n",route.c_str(),broker.c_str());
        _routes[route] = broker;
        return SUCCESS;
    }//存在，但是broker没有变化，则不更新
    else if (_routes[route] == broker)
    {
        return SUCCESS;
    }
    //存在，但是broker有变化，则也更新
    Debug("Insert %s route:%s\n",route.c_str(),broker.c_str());
    _routes[route] = broker;
    return SUCCESS;
}

bool MessageAgentQpidImpl::FilterRedundantMc(const MessageFrame& message,const string& broker)
{
    /*
      在组播的情况下:
      1.多连接情况下，则会多发，每个通道上都会组播。
      2.自己发给自己的组播消息，则在每个连接上都会接收到，消息存在冗余，需要进行过滤
      3.则在第一条连接上接收自己发给自己的组播
      4.组播消息在其他情况下，则不存在冗余，除非，另一个通道上内部创建了同样的一个组播。
    */
    if (message.option.sender()._application == _application)
    {
        map<string,ChannelAgentQpid*>::iterator cit;
        cit = _connect_agents.begin();        
        if (broker == cit->first)
        {
            return false;
        }
        Debug("Mc message %u is filtered from %s!\n",message.option.id(),broker.c_str());
        //printf("zyb...Mc message %u is filtered from %s app %s!\n",message.option.id(),broker.c_str(),message.option.receiver()._application.c_str());
        return true;
    }
    
    return false;
}

STATUS MessageAgentQpidImpl::AddKeyUnit(const string& unit)
{
    T_TimeValue uptime;
    GetTimeFromPowerOn(&uptime);
    int64 age = uptime.second * 1000 + uptime.milisecond;
        
    MutexLock lock(_key_units_mutex);
    map<string,int64>::iterator it = _key_units.find(unit);
    if(it == _key_units.end())
    {
        _key_units.insert(make_pair(unit,age));
    }
    else
    {
        it->second = age;
    }
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::DelKeyUnit(const string& unit)
{
    MutexLock lock(_key_units_mutex);
    _key_units.erase(unit);
    return SUCCESS;
}

bool MessageAgentQpidImpl::IsKeyUnit(const string& unit)
{
    MutexLock lock(_key_units_mutex);
    if(_key_units.find(unit) != _key_units.end())
    {
        return true;
    }
    return false;
}

STATUS MessageAgentQpidImpl::ShowKeyUnit()
{
    printf("-------------- key units -----------\n");
    MutexLock lock_key_units (_key_units_mutex);
    if (_key_units.empty())
    {
        printf("No key units!\n");
        return SUCCESS;
    }

    map<string,int64>::iterator it;
    for (it = _key_units.begin();it!= _key_units.end(); it++)
    {
        printf("key unit: %s added at %lld mili seconds from power on.\n",it->first.c_str(),it->second);
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::InsertTraceUnit(const string &unit,int message_id)
{
    if (unit.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (_trace_units.count(unit) > 0)
    {
        printf("Duplicated name %s!\n",unit.c_str());
    }
    
    _trace_units[unit] = message_id;

    MutexLock lock (_units_mutex);
    map<string,MessageUnit*>::iterator it;
    for (it = _units.begin();it!= _units.end(); it++)
    {
        if(it->first.find(unit) == 0)
        {
            Debug("trace %s(match %s) unit's message\n",
                unit.c_str(),it->first.c_str());
            it->second->TraceRcvMessage(message_id);
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::DelTraceUnit(const string& unit)
{
    MutexLock lock (_units_mutex);

    if (unit.empty())
    {
        _trace_units.clear();
  
        map<string,MessageUnit*>::iterator it;
        for (it = _units.begin();it!= _units.end(); it++)
        {
            printf("untrace %s(match %s) unit's message\n",
                unit.c_str(),it->first.c_str());
            it->second->TraceRcvMessage(0);
        }
        return SUCCESS;
    }    

    printf("untrace %s unit's message\n",unit.c_str());
    _trace_units.erase(unit);

    map<string,MessageUnit*>::iterator it;
    for (it = _units.begin();it!= _units.end(); it++)
    {
        if(it->first.find(unit) == 0)
        {
            printf("untrace %s(match %s) unit's message\n",
                unit.c_str(),it->first.c_str());
            it->second->TraceRcvMessage(0);
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::ShowTraceUnit()
{
    printf("-------------- trace units -----------\n");
    if (_trace_units.empty())
    {
        printf("No trace units!\n");
        return SUCCESS;
    }

    map<string,uint32>::iterator it;
    for (it = _trace_units.begin();it!= _trace_units.end(); it++)
    {
        printf("trace unit = %s, trace message = %u.\n",it->first.c_str(),it->second);
    }

    return SUCCESS;
}

bool MessageAgentQpidImpl::NeedTraceMessage(const string& unit,uint32& id)
{
    if (_trace_units.empty())
    {
        return false;
    }

    if (unit.empty())
    {
        return false;
    }    
    
    map<string,uint32>::iterator it;
    for (it = _trace_units.begin();it!= _trace_units.end(); it++)
    {
        if(unit.find(it->first) == 0)
        {
            Debug("need trace %s(match %s) unit's %u message\n",
                unit.c_str(),it->first.c_str(),it->second);
            id = it->second;
            return true;
        }
    }
    
    return false;
}

STATUS MessageAgentQpidImpl::Summary()
{
    cout <<"---------Messsage Agent Information---------"<< endl;
    //cout << "address_option: " << address_option << endl;
    cout << "AMQP impl: Apache Qpid" << endl;
    cout << "application: " << _application  << endl;
    cout << "process: " << _process << endl;

    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        ret = cit->second->Summary();
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {
            return ret;
        }
    }
    cout <<"--------------------------------------------"<< endl;
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::Start(int priority)
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);
    
    if (_connect_agents.empty())
    {
        return ERROR;
    }
    
    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        ret = cit->second->Start(priority);
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {
            printf("Start on connect %s failed!\n",cit->first.c_str());
            return ret;
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::Stop()
{

    MutexLock lock_conn(_connect_agents_mutex);
    //停止连接消息服务器的处理任务
    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        Debug("Stop connect %s....\n",cit->first.c_str());
        cit->second->Stop();
    }

    _units_mutex.Lock();

    map<string,MessageUnit*>::iterator it;
    //停止注册消息单元对消息的处理任务
    for (it = _units.begin(); it != _units.end(); it++)
    {
        Debug("Stop unit %s....\n",it->first.c_str());
        
        //如果是自己则不进行exit,否则下面的流程不能完成。需要依赖于线程自己的处理!!
        //或sky模块上层的处理。此处代码实现不是很统一，一个事情在两个暂时没有好的方法，
        //
        if (it->second->get_task_id() == pthread_self())
        {
            continue;
        }
        it->second->Stop();
    }
    _units_mutex.Unlock();

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::DestroyAllUnits()
{
    _units_mutex.Lock();
    if (_units.empty())
    {
        _units_mutex.Unlock();
        return SUCCESS;
    }

    //将消息单元列表拷贝出一份,因为它自己的析构函数也要访问unit_table
    list<MessageUnit*> unit_list;
    map<string,MessageUnit*>::iterator it;
    for (it = _units.begin(); it != _units.end(); it++)
    {
        unit_list.push_back(it->second);
    }
    _units_mutex.Unlock();

    //遍历拷贝出来的临时列表，将所有消息单元delete
    list<MessageUnit*>::iterator uit;
    for (uit = unit_list.begin(); uit != unit_list.end(); uit++)
    {
        delete *uit;
    }
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::ShowUnitTable()
{
    printf("--------------------------------------Message Units--------------------------------------\n");
    printf("%-6s %-24s %-8s %-18s %-8s %-12s %-8s %-8s %-8s\n",
           "Index","Name","Tid","Thread","State","MsgState","Queued","Push","Pop");
    int i = 0;
    string name;
    map<string,MessageUnit*>::const_iterator it;
    MutexLock lock (_units_mutex);
    for ( it=_units.begin() ; it != _units.end(); it++)
    {
        string state;
        if(it->second->get_state() == S_Startup)
        {
            state = "Startup";
        }
        else if(it->second->get_state() == S_Work)
        {
            state = "Work";
        }
        else if(it->second->get_state() == S_Exit)
        {
            state = "Exit";
        }
        else
        {
            state = to_string(it->second->get_state(),dec);
        }

        string msg_state="Unkown";
        if(it->second->get_handle_msg_state() == S_Wait_Msg)
        {
            msg_state = "Wait";
        }
        else if(it->second->get_handle_msg_state() == S_Rcv_Msg)
        {
            msg_state = "Receive";
        }
        else if(it->second->get_handle_msg_state() == S_Snd_Msg)
        {
            msg_state = "send";
        }

        if(IsKeyUnit(it->first))
        {
            name = "*" + it->first;
        }
        else
        {
            name = it->first;
        }
        printf("%-6d %-24s %-8d 0x%-16lx %-8s %-12s %-8lu %-8llu %-8llu\n",
                ++i,
                name.c_str(),
                it->second->get_task_tid(),
                it->second->get_task_id(),
                state.c_str(),
                msg_state.c_str(),
                it->second->get_queued_count(),
                it->second->get_push_count(),
                it->second->get_pop_count());
    }
    for ( it=_units.begin() ; it != _units.end(); it++,i++)
    {
           it->second->PrintTraceMsg();
    }
    return SUCCESS;
}

STATUS MessageAgentQpidImpl::ShowMcTable()
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        printf("-------------- mc receivers-----------\n");
        ret = cit->second->ShowMcTable();
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {
            printf("Show mac on connect %s failed!\n",cit->first.c_str());
            return ret;
        }
    }

    return SUCCESS;
}

STATUS MessageAgentQpidImpl::ShowRouteTable()
{
    int i = 0;
    MutexLock lock_conn(_connect_agents_mutex);
    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        i++;
        printf("No.%d broker:%s\n",i,cit->first.c_str());
    }
    
    MutexLock lock_route(_routes_mutex);    

    map<string,string> brokers;
    map<string,string>::iterator it_r;
    map<string,string>::iterator it_b;
    i = 0;
    printf("-------------- routes-----------\n");
    if (_routes.empty())
    {
        printf("No route!\n");
    }
    /*for (it_r = _routes.begin();it_r!= _routes.end(); it_r++)
    {
        printf("%-6d %-24s %-24s\n",
            ++i,
            it_r->first.c_str(),
            it_r->second.c_str());
    }*/
    
    for (it_r = _routes.begin();it_r!= _routes.end(); it_r++)
    {
        it_b = brokers.find(it_r->second);
        if (it_b == brokers.end())
        {
            brokers[it_r->second] = "";
        }
    }

    for (it_b = brokers.begin();it_b!= brokers.end(); it_b++)
    {
        for (it_r = _routes.begin();it_r!= _routes.end(); it_r++)
        {
            if (it_b->first == it_r->second)
            {
                printf("%-6d %-24s %-s\n",
                    ++i,
                    it_r->second.c_str(),
                    it_r->first.c_str());
            }        
        }
    }
        
    return SUCCESS;
}

int MessageAgentQpidImpl::SendDebug()
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        printf("--------------send -----------\n");
        ret = cit->second->SendDebug();
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {
            return 1;
        }
    }

    return 0;
}

int MessageAgentQpidImpl::ReceiveDebug()
{
    STATUS ret;
    MutexLock lock_conn(_connect_agents_mutex);

    map<string,ChannelAgentQpid*>::iterator cit;
    for (cit = _connect_agents.begin();cit!= _connect_agents.end(); cit++)
    {
        printf("--------------%s receive-----------\n",cit->first.c_str());
        ret = cit->second->ReceiveDebug();
        //如果一个处理失败，则直接返回
        if (SUCCESS != ret)
        {
            return 1;
        }
    }

    return 0;
}

#if 0
static MultiRelations *test_relations = NULL;
void DbgAddMemberToGroup(const char *member,const char *group)
{
    if (!test_relations)
        test_relations = new MultiRelations();
    test_relations->AddMemberToGroup(member,group);
}
DEFINE_DEBUG_FUNC(DbgAddMemberToGroup);

void DbgDelMemberFromGroup(const char *member,const char *group)
{
    if (!test_relations)
        test_relations = new MultiRelations();
    test_relations->DelMemberFromGroup(member,group);
}
DEFINE_DEBUG_FUNC(DbgDelMemberFromGroup);

void DbgDelMember(const char *member)
{
    if (!test_relations)
        test_relations = new MultiRelations();
    test_relations->DeleteMember(member);
}
DEFINE_DEBUG_FUNC(DbgDelMember);

void DbgDelGroup(const char *group)
{
    if (!test_relations)
        test_relations = new MultiRelations();
    test_relations->DeleteGroup(group);
}
DEFINE_DEBUG_FUNC(DbgDelGroup);

void DbgShowRelations()
{
    if (!test_relations)
        test_relations = new MultiRelations();
    test_relations->ShowMembers();
    test_relations->ShowGroups();
}
DEFINE_DEBUG_FUNC(DbgShowRelations);
#endif

