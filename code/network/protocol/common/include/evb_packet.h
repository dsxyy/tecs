#if !defined(EVB_PACKET_INCLUDE_H__)
#define EVB_PACKET_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

    struct tagEvbPacket
    {
        u8      *pBuff;                 /* ���䱨��buffer����ʼλ��; */
        u8      *pData;                 /* ָ����ʵ���ݵ�λ�ã�pkt_data�� pkt_buffer֮���Ǳ��ĵĿ��ƿ�; */
        u32     uiBufLen;               /* ���䱨��buffer���ܳ���; */
        u32     uiDataLen;              /* ��ʵ���ĵ����ݳ���; */
        void    (*pfFreePkt)(struct tagEvbPacket *);       

        struct tagEvbIssPort *pEvbPort; /* ���Ĵ��ݹ�����ʹ�õ�EvbIssPort�ṹ�����Ը������ҵ���������; */
    };

#ifdef __cplusplus
}
#endif

#endif
