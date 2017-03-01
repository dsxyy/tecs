#include "sky.h"
#include "event.h"
#include "mid.h"
#include "image_db.h"
#include "api_pub.h"

namespace zte_tecs
{
STATUS GetImageMetadata(int64 iid, int64 uid, ImageMetadata &data)
{
    ImageMetadataReq req(iid,uid);
    //ʹ����ʱ��Ϣ��Ԫ��image manager��ѯָ��image��metadata
    MessageUnit temp_mu(TempUnitName("GetImageMetadata"));
    STATUS ret = temp_mu.Register();
    if(SUCCESS != ret)
    {
        return ret;
    }
    
    MessageOption option(MID(PROC_IMAGE_MANAGER,MU_IMAGE_MANAGER), EV_IMAGE_METADATA_REQ,0,0);
    temp_mu.Send(req, option);

    MessageFrame message;
    ret = temp_mu.Wait(&message,3000);
    if(SUCCESS != ret)
    {
        return ret;
    }
    ImageMetadataAck ack;
    if(true != ack.deserialize(message.data))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    data = ack._image;
    return ack._result;
}

STATUS DeployImage(const vector<ImageDeployMsg> &vec_msg)
{
    STATUS ret = SUCCESS;
    
    if (0 != vec_msg.size())
    {
        /*  ����Ϣ�� IMAGE_MANAGE ��ѡ��  */
        MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                             EV_IMAGE_DEPLOY_REQ,0,0);

        /*  �ȴ�MU_IMAGE_MANAGER��Ӧ �ṹ */
        MessageFrame message;
        ImageOperateAckMsg  ack_message;
        
        /* 2. ������ʱ��Ϣ��Ԫ */
        MessageUnit temp_mu(TempUnitName("tecs_image_deploy"));
        if(SUCCESS != temp_mu.Register())
        {
            return ERROR;
        }
        vector<ImageDeployMsg>::const_iterator it;
        for (it = vec_msg.begin(); it != vec_msg.end(); it++)
        {
            // ���Ƴ��ȺϷ���У��
            if (it->_cluster_name.size() != 0)
            {
                if (StringCheck::CheckComplexName(it->_cluster_name,1,STR_LEN_64) != true)
                {
                    return ERROR_INVALID_ARGUMENT;
                }     
            }
            temp_mu.Send(*it, option);
            if(SUCCESS == temp_mu.Wait(&message,3000))
            {
                ack_message.deserialize(message.data);                
                ack_message.Print();
                if (ERROR_OP_RUNNING == ack_message._ret_code)
                {//ֻҪ��һ�����ڲ��𣬷���ֵ�����������������image�������������
                    ret = ERROR_OP_RUNNING;
                }
                else if (SUCCESS != ack_message._ret_code)
                {
                    return ack_message._ret_code;
                }              
            }
            else
            {
                return ERROR_TIME_OUT;
            }
        }
    }
    
    return ret;
}

}
                        
