#ifndef SKY_ERROR_CODE_H
#define SKY_ERROR_CODE_H

typedef int STATUS;
#define SUCCESS                             0          //�ɹ�
#define ERROR                               1          //δ֪����
#define ERROR_INVALID_ARGUMENT              (ERROR+1)  //�������Ϸ�

#define ERROR_NOT_PERMITTED                 (ERROR+2)  //������Ȩ��
#define ERROR_PERMISSION_DENIED             (ERROR+3)  //�������ܾ�

#define ERROR_FILE_NOT_EXIST                (ERROR+4)  //�ļ���Ŀ¼������
#define ERROR_ADDR_NOT_EXIST                (ERROR+5)  //ͨ�ŵ�ַ������
#define ERROR_DEVICE_NOT_EXIST              (ERROR+6)  //�豸������
#define ERROR_OBJECT_NOT_EXIST              (ERROR+7)  //�������󲻴���

#define ERROR_IO_FAILED                     (ERROR+8)  //IO����ʧ��
#define ERROR_BAD_FD                        (ERROR+9)  //������ļ�������
#define ERROR_BAD_ADDRESS                   (ERROR+10) //�����ͨ�ŵ�ַ
#define ERROR_DEVICE_BUSY                   (ERROR+11) //�豸��æ
#define ERROR_RESOURCE_UNAVAILABLE          (ERROR+12) //��Դ������
#define ERROR_NO_MEMORY                     (ERROR+13) //�ڴ����ʧ��
#define ERROR_NO_SPACE                      (ERROR+14) //û�д洢�ռ�

#define ERROR_NO_TIMER                      (ERROR+15) //���䶨ʱ��ʧ��
#define ERROR_TIMER_MESSAGE_INVALID         (ERROR+16) //�Ƿ��Ķ�ʱ����ʱ��Ϣ
#define ERROR_TIME_OUT                      (ERROR+17) //������ʱ
#define ERROR_INVALID_DATETIME              (ERROR+18) //�Ƿ���ʱ��

#define ERROR_DUPLICATED_OP                 (ERROR+19) //�ظ��Ĳ���
#define ERROR_NOT_READY                     (ERROR+20) //��û�г�ʼ����׼���ý��ܲ���

#define ERROR_DB_CREATE_FAIL                (ERROR+21) //���ݿⴴ��ʧ��
#define ERROR_DB_SELECT_FAIL                (ERROR+22) //���ݿ��ѯ����ʧ��
#define ERROR_DB_INSERT_FAIL                (ERROR+23) //���ݿ�������ʧ��
#define ERROR_DB_UPDATE_FAIL                (ERROR+24) //���ݿ���²���ʧ��
#define ERROR_DB_DELETE_FAIL                (ERROR+25) //���ݿ�ɾ������ʧ��
#define ERROR_DB_UNKNOWN_FAIL               (ERROR+26) //���ݿ�δ֪����ʧ��

#define ERROR_MESSAGE_FAIL                  (ERROR+27) //��Ϣ����ʧ��
#define ERROR_NOT_SUPPORT                   (ERROR+28) //��֧�ֵĹ���
#define ERROR_ACK_OVERDUE                   (ERROR+29) //Ӧ���Ѿ�����
#define ERROR_SERIALIZATION                 (ERROR+30)//���л�ʧ��
#define ERROR_DESERIALIZATION               (ERROR+31) //�����л�ʧ��
#define ERROR_OP_RUNNING                    (ERROR+32) //�������ڽ�����

#define ERR_OBJECT_ALLOCATE_FAILED      (ERROR+33)  // "Error, failed to allocate object instance"
#define ERR_OBJECT_DROP_FAILED          (ERROR+34)  // "Error, failed to drop object instance"
#define ERR_OBJECT_GET_FAILED           (ERROR+35)  // "Error, failed to get object instance"
#define ERR_OBJECT_UPDATE_FAILED        (ERROR+36)  // "Error, failed to update object instance"
#define ERR_OBJECT_NOT_EXIST            (ERROR+37)  // "Error, object instance not exist"

#define ERR_AUTHORIZE_FAILED            (ERROR+38)  // "Error, failed to authorize usr's operation"
#define ERR_MCRECEIVER_NOT_EXIST        (ERROR+39)  // "Error, failed to receive group message"
#define ERROR_CREATE_SOCKET             (ERROR+40)  //����socketʧ��
#define ERROR_CREATE_THREAD             (ERROR+41)  //�����߳�ʧ��

#define ERROR_IMAGE_FILENAME_INVALID    (ERROR+42)  //image �ļ����Ƿ�
#define ERROR_OPTION_NOT_EXIST          (ERROR+43) // "Error, option not exist"
#define ERR_EXIST                       (ERROR+44)  //��Դ�Ѿ�����
#define ERROR_THREAD_WILL_EXIT          (ERROR+45)  //�߳���Ҫֹͣ

#define ERROR_UNKOWN                        (ERROR+46)  //�������İ�
#define ERROR_PROCESS_WILL_EXIT             (ERROR+47)  //������Ҫֹͣ
#define ERROR_POPEN_FAIL                    (ERROR+48)  //��popenʧ��
#define ERROR_NO_UPDATE_PACKAGE             (ERROR+49)  //�������İ�
#define ERROR_NO_INSTALLED_PACKAGE        (ERROR+50)    //����û�а�װ��
#define ERROR_POPEN_SELECT_FAIL           (ERROR+51)    //POEN��ȡ��Ϣ����
#define ERROR_EXIT_OPERATION              (ERROR+52)    //�˳�����
#define ERROR_SA_OPERATE_ERR              (ERROR+53)  //sa����ʧ��
#define ERROR_IMG_BACKUP_ACTIVED          (ERROR+54)  //���񱸷��Ǽ���̬
#define ERROR_LOAD_NEED_REFRESH           (ERROR+55)  //sa����ʧ��
#define ERROR_PORTNO_GET                  (ERROR+56)  //�˿ںŻ�ȡʧ��
#define ERROR_SA_UNREGISTER               (ERROR+57)  //saδ�����scע��


/* ����ϵͳ�������ض��� */
#define ERROR_SYS_ERR_BEGIN         900
#define ERROR_SYS_ERRNO(num)       (ERROR_SYS_ERR_BEGIN+num)    /**<  os errno */
#define ERROR_SYS_ERR_EPERM        (ERROR_SYS_ERR_BEGIN+1)  /**<  Operation not permitted(0x80E) */
#define ERROR_SYS_ERR_ENOENT       (ERROR_SYS_ERR_BEGIN+2)  /**<  No such file or directory */
#define ERROR_SYS_ERR_ESRCH        (ERROR_SYS_ERR_BEGIN+3)  /**<  No such process */
#define ERROR_SYS_ERR_EINTR        (ERROR_SYS_ERR_BEGIN+4)  /**<  Interrupted system call */
#define ERROR_SYS_ERR_EIO          (ERROR_SYS_ERR_BEGIN+5)  /**<  I/O error */
#define ERROR_SYS_ERR_ENXIO        (ERROR_SYS_ERR_BEGIN+6)  /**<  No such device or address */
#define ERROR_SYS_ERR_E2BIG        (ERROR_SYS_ERR_BEGIN+7)  /**<  Argument list too long */
#define ERROR_SYS_ERR_ENOEXEC      (ERROR_SYS_ERR_BEGIN+8)  /**<  Exec format error */
#define ERROR_SYS_ERR_EBADF        (ERROR_SYS_ERR_BEGIN+9)  /**<  Bad file number */
#define ERROR_SYS_ERR_ECHILD       (ERROR_SYS_ERR_BEGIN+10) /**<  No child processes */
#define ERROR_SYS_ERR_EAGAIN       (ERROR_SYS_ERR_BEGIN+11) /**<  Try again */
#define ERROR_SYS_ERR_ENOMEM       (ERROR_SYS_ERR_BEGIN+12) /**<  Out of memory */
#define ERROR_SYS_ERR_EACCES       (ERROR_SYS_ERR_BEGIN+13) /**<  Permission denied */
#define ERROR_SYS_ERR_EFAULT       (ERROR_SYS_ERR_BEGIN+14) /**<  Bad address */
#define ERROR_SYS_ERR_ENOTBLK      (ERROR_SYS_ERR_BEGIN+15) /**<  Block device required */
#define ERROR_SYS_ERR_EBUSY        (ERROR_SYS_ERR_BEGIN+16) /**<  Device or resource busy */
#define ERROR_SYS_ERR_EEXIST       (ERROR_SYS_ERR_BEGIN+17) /**<  File exists */
#define ERROR_SYS_ERR_EXDEV        (ERROR_SYS_ERR_BEGIN+18) /**<  Cross-device link */
#define ERROR_SYS_ERR_ENODEV       (ERROR_SYS_ERR_BEGIN+19) /**<  No such device */
#define ERROR_SYS_ERR_ENOTDIR      (ERROR_SYS_ERR_BEGIN+20) /**<  Not a directory */
#define ERROR_SYS_ERR_EISDIR       (ERROR_SYS_ERR_BEGIN+21) /**<  Is a directory */
#define ERROR_SYS_ERR_EINVAL       (ERROR_SYS_ERR_BEGIN+22) /**<  Invalid argument */
#define ERROR_SYS_ERR_ENFILE       (ERROR_SYS_ERR_BEGIN+23) /**<  File table overflow */
#define ERROR_SYS_ERR_EMFILE       (ERROR_SYS_ERR_BEGIN+24) /**<  Too many open files */
#define ERROR_SYS_ERR_ENOTTY       (ERROR_SYS_ERR_BEGIN+25) /**<  Not a typewriter */
#define ERROR_SYS_ERR_ETXTBSY      (ERROR_SYS_ERR_BEGIN+26) /**<  Text file busy */
#define ERROR_SYS_ERR_EFBIG        (ERROR_SYS_ERR_BEGIN+27) /**<  File too large */
#define ERROR_SYS_ERR_ENOSPC       (ERROR_SYS_ERR_BEGIN+28) /**<  No space left on device */
#define ERROR_SYS_ERR_ESPIPE       (ERROR_SYS_ERR_BEGIN+29) /**<  Illegal seek */
#define ERROR_SYS_ERR_EROFS        (ERROR_SYS_ERR_BEGIN+30) /**<  Read-only file system */
#define ERROR_SYS_ERR_EMLINK       (ERROR_SYS_ERR_BEGIN+31) /**<  Too many links */
#define ERROR_SYS_ERR_EPIPE        (ERROR_SYS_ERR_BEGIN+32) /**<  Broken pipe */
#define ERROR_SYS_ERR_EDOM         (ERROR_SYS_ERR_BEGIN+33) /**<  Math argument out of domain of func */
#define ERROR_SYS_ERR_ERANGE       (ERROR_SYS_ERR_BEGIN+34) /**<  Math result not representable */

//������ϵͳר�ô����룬�Ͻ��޸���ֵ!!! �Ͻ��ڷǹ���������ʹ��!!!
#define ERROR_WORKFLOW_ERR_BEGIN         990
#define ERROR_ACTION_READY               (ERROR_WORKFLOW_ERR_BEGIN + 1)
#define ERROR_ACTION_RUNNING             (ERROR_WORKFLOW_ERR_BEGIN + 2)
#define ERROR_ACTION_TIMEOUT             (ERROR_WORKFLOW_ERR_BEGIN + 3)
#define ERROR_ACTION_CANCELLED           (ERROR_WORKFLOW_ERR_BEGIN + 4)
#define SKY_ERROR_END                       (1000)
#endif

