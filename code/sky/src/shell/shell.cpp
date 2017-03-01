/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�shell.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��shellʵ���ļ�
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
#include "shell.h"
#include "symbol.h"
#include "parser.h"
#include "token.h"
#include "message_agent.h"
#include "timer.h"
#define READLINE
#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

static THREAD_ID shell_thread_id;
static char shell_prompt[64] = {'\0'};
static const char* default_prompt = "sky-> ";

class Command
{
public:
    #define MAX_CMD_ARGS  (8)
    #define MAX_CMD_LEN   (256)
    #define MAX_ARG_LEN   MAX_CMD_LEN
    //���Ժ������֧��8������
    typedef uintptr (*DBGFUNC)(uintptr arg0,uintptr arg1,uintptr arg2,
                                uintptr arg3,uintptr arg4,uintptr arg5,
                                uintptr arg6,uintptr arg7);
    typedef void (*DBGCMD)();

    enum SyntaxType
    {
        SINGLE_TOKEN,
        ASSIGN_VAR,
        CALL_FUNC,
        UNKNOWN_CMD
    };

    enum CommandState
    {
        CMD_WAIT_NAME,
        CMD_WAIT_TYPE,
        CMD_WAIT_ARGS,
        CMD_OK,
        CMD_ERR
    };

    Command()
    {
        Clear();
    };

    void Clear()
    {
        memset(name,0,sizeof(name));
        memset(args,0,sizeof(args));
        memset(string_args_pool,0,sizeof(string_args_pool));
        number_of_args = 0;
        string_pool_index = 0;
        state = CMD_WAIT_NAME;
        type = UNKNOWN_CMD;
    }
    
    STATUS AddArg(const char* argstr,int argflag);
    STATUS Parse(const char* input);
    STATUS Execute();

private:
    char name[MAX_CMD_LEN];
    SyntaxType type;
    CommandState state;   
    uintptr args[MAX_CMD_ARGS];
    int number_of_args;    
    char string_args_pool[MAX_ARG_LEN];
    int string_pool_index;
};

STATUS Command::Execute()
{
    const Symbol *sym = NULL;
    STATUS ret = SUCCESS;
    if(state != CMD_OK)
    {
        return ERROR;
    }
    
    switch(type)
    {
        case UNKNOWN_CMD:
            ret = ERROR;
            break;
            
        case SINGLE_TOKEN:
            {
                //����ֻ����������ʶ��ʱ�������ǲ鿴����ֵ�����߼�����
                sym = GetSymbol(name);
                if(!sym)
                {
                    printf("symbol %s not found!\n",name);
                    ret = ERROR;
                    break;
                }
                
                if(SYMBOL_VAR == sym->_type)
                {
                    sym->Display();
                }
                else if(SYMBOL_CMD == sym->_type)
                {
                    DBGCMD pfunc = (DBGCMD)(sym->_addr);
                    (*pfunc)();
                }
                else
                {
                    printf("symbol %s is not a variable!\n",name);
                    ret = ERROR;
                }
                break;
            }

        case  ASSIGN_VAR:
            {
                //��������ֵ
                sym = GetSymbol(name);
                if(!sym)
                {
                    printf("symbol %s not found!\n",name);
                    ret = ERROR;
                    break;
                }

                if(SYMBOL_VAR != sym->_type)
                {
                    printf("symbol %s is not a variable!\n",name);
                    ret = ERROR;
                    break;
                }
                sym->Assign(args[0]);
                break;
            }
        
        case CALL_FUNC:
            {
                //���к�������
                sym = GetSymbol(name);
                if(!sym)
                {
                    printf("symbol %s not found!\n",name);
                    ret = ERROR;
                    break;
                }

                if(SYMBOL_FUNC != sym->_type && SYMBOL_CMD != sym->_type)
                {
                    printf("symbol %s is not executable!\n",name);
                    ret = ERROR;
                    break;
                }
                
                DBGFUNC pfunc = (DBGFUNC)(sym->_addr);
                (*pfunc)(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]);
                break;
            } 
        
        default:
            ret = ERROR;
            break;
    }
    return ret;
}

STATUS Command::AddArg(const char* argstr,int argflag)
{
    int remained = 0;
    int copylen = 0;
    const char *copystart = NULL;
    if(!argstr || 0 == strlen(argstr))
    {
        return ERROR;
    }

    //���������Ѿ��ﵽ����
    if(number_of_args >= MAX_CMD_ARGS)
    {
        return ERROR;
    }
    
    //arg���ô������������ͣ���Ϊ��֧�����븺��
    intptr arg = 0;
    switch(argflag)
    {
        case INTEGER_HEX:
            from_string<intptr>(arg,argstr,hex);
            break;
            
        case INTEGER_DEC:
            from_string<intptr>(arg,argstr,dec);
            break;

        case STRING:
            remained = sizeof(string_args_pool) - string_pool_index;
            copystart = argstr;
            //���ַ���ͷβ������ȥ��
            if(copystart[0] == '"')
            {
                copystart += 1;
            }
            copylen = strlen(copystart);
            if(copystart[copylen-1] == '"')              
            {
                copylen -= 1;
            }
            //ʣ��ռ��޷������µ��ַ�������
            if(copylen >= remained)
            {
                return ERROR;
            }
            
            strncpy(string_args_pool + string_pool_index,copystart,copylen);
            arg = (uintptr)(string_args_pool + string_pool_index);
            string_pool_index += strlen(argstr) + 2;
            break;
            
        default:
            break;
    }
    args[number_of_args++] = (uintptr)arg;
    return SUCCESS;
}

STATUS Command::Parse(const char* input)
{
    yyscan_t scanner;
    YY_BUFFER_STATE buf;
    skylex_init(&scanner);
    buf = sky_scan_string(input, scanner);

    int flag = EMPTY;
    state = CMD_WAIT_NAME;
    while(1)
    {
        //ѭ�����ôʷ����������������token
        flag = skylex(scanner);
        switch(state)
        {
            case CMD_ERR:
            case CMD_OK:
                break;
                
            case CMD_WAIT_NAME:
            {
                //�ȴ���������ʱ��ֻ����IDENTIFIER
                if(flag == IDENTIFIER)
                {
                    strncpy(name,skyget_text(scanner),MAX_CMD_LEN - 1);
                    state = CMD_WAIT_TYPE;
                }
                else
                {
                    state = CMD_ERR;
                }
                break;
            }
        
            case CMD_WAIT_TYPE:
            {
                //�ȴ���������ʱ���ȵ�����'='��ʾ������ֵ��'('��ʾ�������ã�EMPTY��ʾ�����鿴
                if(flag == '=')
                {
                    type = ASSIGN_VAR;
                    state = CMD_WAIT_ARGS;
                }
                else if(flag == '(')
                {
                    type = CALL_FUNC;
                    state = CMD_WAIT_ARGS;
                }
                else if(flag == EMPTY)
                {
                    type = SINGLE_TOKEN;
                    state = CMD_OK;
                }
                else
                {
                    state = CMD_ERR;
                }
                break;
            }
        
            case CMD_WAIT_ARGS:
            {
                //�ȴ�����ʱ������Ǻ������ã��յ�')'��ʾ���ý���
                if(flag == ')' && type == CALL_FUNC)
                {
                    state = CMD_OK;
                }
                else if(IsConstant(flag))
                {
                    //���������ֻ���ܳ������ݲ�֧��ʹ�ñ�����Ϊ����
                    if(type == ASSIGN_VAR)
                    {
                        AddArg(skyget_text(scanner),flag);
                        state = CMD_OK;
                    }
                    else if(type == CALL_FUNC)
                    {
                        AddArg(skyget_text(scanner),flag);
                        if(number_of_args >= MAX_CMD_ARGS)
                        {
                            state = CMD_OK;
                        }
                        else
                        {
                            state = CMD_WAIT_ARGS;
                        }
                    }
                    else
                    {
                        state = CMD_ERR;
                    }
                }
                else
                {
                    state = CMD_ERR;
                }
                break;
            }
        }
        
        if(state == CMD_ERR || state == CMD_OK)
        {
            break;
        }
    }

    //�ͷ�lex��Դ
    sky_delete_buffer(buf, scanner);
    skylex_destroy(scanner);
    return SUCCESS;
}

#ifdef READLINE
static char *stripwhite (char *str)
{
    register char *s, *t;
    for (s = str; whitespace (*s); s++)
        ;

    if (*s == 0)
        return (s);

    t = s + strlen (s) - 1;
    while (t > s && whitespace (*t))
        t--;
    *++t = '\0';

    return s;
}
#endif

static struct termios initial_settings;
static void SaveTermAttr()
{
    tcgetattr(STDIN_FILENO,&initial_settings);
}

static void RestoreTermAttr()
{
    tcsetattr(STDIN_FILENO,TCSANOW,&initial_settings);
}

extern "C" void* ShellTaskEntry(void *arg)
{
    Command cmd;
    cmd.Clear();
    SaveTermAttr();
    atexit(RestoreTermAttr);
    
    while(1)
    {
#ifdef READLINE
        char *line, *s;
        line = readline (shell_prompt);

        if (!line)
            break;

        /* Remove leading and trailing whitespace from the line.
           Then, if there is anything left, add it to the history list
           and execute it. */
        s = stripwhite (line);
        if(strlen(s) > MAX_CMD_LEN)
        {
            printf("command too long, the max length is %d!\n",MAX_CMD_LEN);
            free (line);
            continue;
        }
        
        if(SUCCESS != cmd.Parse(s))
        {
            printf("command parse failed!\n");
            free (line);
            continue;
        }
        
        if (*s)
        {
            add_history (s);
        }
        free (line);
#else
        string input;
        cout << shell_prompt;
        getline (cin,input);
        if(input.empty())
        {
            continue;
        }

        if(input.size() > MAX_CMD_LEN)
        {
            printf("command too long, the max length is %d!\n",MAX_CMD_LEN);
            continue;
        }
        
        if(SUCCESS != cmd.Parse(input.c_str()))
        {
            printf("command parse failed!\n");
            continue;
        }
#endif
        cmd.Execute();
        cmd.Clear();
    }

    RestoreTermAttr();
    return NULL;
}

STATUS Shell(const char* prompt)
{
    memset(shell_prompt,0,sizeof(shell_prompt));
    if(!prompt || strlen(prompt) == 0 || strlen(prompt) > sizeof(shell_prompt))
    {
        //ʹ��Ĭ�ϵ�������ʾ��
        strncpy(shell_prompt,default_prompt,sizeof(shell_prompt));
    }
    else
    {
        strncpy(shell_prompt,prompt,sizeof(shell_prompt));
    }

    shell_thread_id = StartThread("Shell",ShellTaskEntry,NULL);
    if(INVALID_THREAD_ID == shell_thread_id)
        return ERROR;
    else
        return SUCCESS;
}
  
static void ShellExit()
{
    char x,c;
    char buf[128] = {'\0'};
    snprintf(buf, sizeof(buf), "%s", "Are you sure to make process exit? y/n ");
    write(STDOUT_FILENO,buf,strlen(buf));
    scanf("%c",&x);
    
    //������뻺����
    while ( (c = getchar()) != '\n' && c != EOF );
    
    if (x != 'y' && x != 'Y')
    {
        return;
    }

    //DestroyTimerManager();
    //DestroyMessageAgent();
    kill(getpid(),SIGKILL);
}
DEFINE_DEBUG_CMD(exit,ShellExit);

