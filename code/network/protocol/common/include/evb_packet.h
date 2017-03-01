#if !defined(EVB_PACKET_INCLUDE_H__)
#define EVB_PACKET_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

    struct tagEvbPacket
    {
        u8      *pBuff;                 /* 分配报文buffer的起始位置; */
        u8      *pData;                 /* 指向真实数据的位置，pkt_data和 pkt_buffer之间是报文的控制块; */
        u32     uiBufLen;               /* 分配报文buffer的总长度; */
        u32     uiDataLen;              /* 真实报文的数据长度; */
        void    (*pfFreePkt)(struct tagEvbPacket *);       

        struct tagEvbIssPort *pEvbPort; /* 报文传递过程中使用的EvbIssPort结构，可以根据它找到报文驱动; */
    };

#ifdef __cplusplus
}
#endif

#endif
