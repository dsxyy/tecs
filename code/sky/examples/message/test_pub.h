#include "sky.h"

//以下几个名称都必须用小写字母加下划线来组成
#define TEST_CLIENT         "test_client"
#define TEST_SERVER         "test_server"
#define CLIENT_PROCESS      "client"
#define SERVER_PROCESS      "server"
#define TEST_GROUP          "test_grp"

#define EV_POWER_ON          1001
#define EV_INFO_REQ          1002
#define EV_INFO_ACK          1003
#define EV_BROADCAST_INFO    1004
#define EV_INFO_STRING       1005

class Information: public Serializable
{
public:
    string content;
    int sequence;

    SERIALIZE
    {
        SERIALIZE_BEGIN(Information);
        WRITE_STRING(content);
        WRITE_DIGIT(sequence);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Information);
        READ_STRING(content);
        READ_DIGIT(sequence);
        DESERIALIZE_END();
    };
};




