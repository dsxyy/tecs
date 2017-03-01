#ifndef SKY_ERROR_CODE_H
#define SKY_ERROR_CODE_H

typedef int STATUS;
#define SUCCESS                             0          //成功
#define ERROR                               1          //未知错误
#define ERROR_INVALID_ARGUMENT              (ERROR+1)  //参数不合法

#define ERROR_NOT_PERMITTED                 (ERROR+2)  //操作无权限
#define ERROR_PERMISSION_DENIED             (ERROR+3)  //操作被拒绝

#define ERROR_FILE_NOT_EXIST                (ERROR+4)  //文件或目录不存在
#define ERROR_ADDR_NOT_EXIST                (ERROR+5)  //通信地址不存在
#define ERROR_DEVICE_NOT_EXIST              (ERROR+6)  //设备不存在
#define ERROR_OBJECT_NOT_EXIST              (ERROR+7)  //操作对象不存在

#define ERROR_IO_FAILED                     (ERROR+8)  //IO操作失败
#define ERROR_BAD_FD                        (ERROR+9)  //错误的文件描述符
#define ERROR_BAD_ADDRESS                   (ERROR+10) //错误的通信地址
#define ERROR_DEVICE_BUSY                   (ERROR+11) //设备正忙
#define ERROR_RESOURCE_UNAVAILABLE          (ERROR+12) //资源不可用
#define ERROR_NO_MEMORY                     (ERROR+13) //内存分配失败
#define ERROR_NO_SPACE                      (ERROR+14) //没有存储空间

#define ERROR_NO_TIMER                      (ERROR+15) //分配定时器失败
#define ERROR_TIMER_MESSAGE_INVALID         (ERROR+16) //非法的定时器超时消息
#define ERROR_TIME_OUT                      (ERROR+17) //操作超时
#define ERROR_INVALID_DATETIME              (ERROR+18) //非法的时间

#define ERROR_DUPLICATED_OP                 (ERROR+19) //重复的操作
#define ERROR_NOT_READY                     (ERROR+20) //还没有初始化或准备好接受操作

#define ERROR_DB_CREATE_FAIL                (ERROR+21) //数据库创建失败
#define ERROR_DB_SELECT_FAIL                (ERROR+22) //数据库查询操作失败
#define ERROR_DB_INSERT_FAIL                (ERROR+23) //数据库插入操作失败
#define ERROR_DB_UPDATE_FAIL                (ERROR+24) //数据库更新操作失败
#define ERROR_DB_DELETE_FAIL                (ERROR+25) //数据库删除操作失败
#define ERROR_DB_UNKNOWN_FAIL               (ERROR+26) //数据库未知操作失败

#define ERROR_MESSAGE_FAIL                  (ERROR+27) //消息发送失败
#define ERROR_NOT_SUPPORT                   (ERROR+28) //不支持的功能
#define ERROR_ACK_OVERDUE                   (ERROR+29) //应答已经过期
#define ERROR_SERIALIZATION                 (ERROR+30)//序列化失败
#define ERROR_DESERIALIZATION               (ERROR+31) //反序列化失败
#define ERROR_OP_RUNNING                    (ERROR+32) //操作正在进行中

#define ERR_OBJECT_ALLOCATE_FAILED      (ERROR+33)  // "Error, failed to allocate object instance"
#define ERR_OBJECT_DROP_FAILED          (ERROR+34)  // "Error, failed to drop object instance"
#define ERR_OBJECT_GET_FAILED           (ERROR+35)  // "Error, failed to get object instance"
#define ERR_OBJECT_UPDATE_FAILED        (ERROR+36)  // "Error, failed to update object instance"
#define ERR_OBJECT_NOT_EXIST            (ERROR+37)  // "Error, object instance not exist"

#define ERR_AUTHORIZE_FAILED            (ERROR+38)  // "Error, failed to authorize usr's operation"
#define ERR_MCRECEIVER_NOT_EXIST        (ERROR+39)  // "Error, failed to receive group message"
#define ERROR_CREATE_SOCKET             (ERROR+40)  //创建socket失败
#define ERROR_CREATE_THREAD             (ERROR+41)  //创建线程失败

#define ERROR_IMAGE_FILENAME_INVALID    (ERROR+42)  //image 文件名非法
#define ERROR_OPTION_NOT_EXIST          (ERROR+43) // "Error, option not exist"
#define ERR_EXIST                       (ERROR+44)  //资源已经存在
#define ERROR_THREAD_WILL_EXIT          (ERROR+45)  //线程需要停止

#define ERROR_UNKOWN                        (ERROR+46)  //无升级的包
#define ERROR_PROCESS_WILL_EXIT             (ERROR+47)  //进程需要停止
#define ERROR_POPEN_FAIL                    (ERROR+48)  //打开popen失败
#define ERROR_NO_UPDATE_PACKAGE             (ERROR+49)  //无升级的包
#define ERROR_NO_INSTALLED_PACKAGE        (ERROR+50)    //本地没有安装包
#define ERROR_POPEN_SELECT_FAIL           (ERROR+51)    //POEN获取信息错误
#define ERROR_EXIT_OPERATION              (ERROR+52)    //退出操作
#define ERROR_SA_OPERATE_ERR              (ERROR+53)  //sa操作失败
#define ERROR_IMG_BACKUP_ACTIVED          (ERROR+54)  //镜像备份是激活态
#define ERROR_LOAD_NEED_REFRESH           (ERROR+55)  //sa操作失败
#define ERROR_PORTNO_GET                  (ERROR+56)  //端口号获取失败
#define ERROR_SA_UNREGISTER               (ERROR+57)  //sa未完成向sc注册


/* 操作系统错误码重定义 */
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

//工作流系统专用错误码，严禁修改数值!!! 严禁在非工作流场合使用!!!
#define ERROR_WORKFLOW_ERR_BEGIN         990
#define ERROR_ACTION_READY               (ERROR_WORKFLOW_ERR_BEGIN + 1)
#define ERROR_ACTION_RUNNING             (ERROR_WORKFLOW_ERR_BEGIN + 2)
#define ERROR_ACTION_TIMEOUT             (ERROR_WORKFLOW_ERR_BEGIN + 3)
#define ERROR_ACTION_CANCELLED           (ERROR_WORKFLOW_ERR_BEGIN + 4)
#define SKY_ERROR_END                       (1000)
#endif

