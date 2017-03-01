/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称： RDTELNET.CPP
* 文件标识： 
* 内容摘要： 
* 其它说明： 无
* 当前版本： V1.0
* 作    者：
* 完成日期： 
**********************************************************************/

/***********************************************************
 *                      头文件                             *
***********************************************************/
#include <stdio.h>
#include <ctype.h>
#include "pub_addition.h"

/*#include "sockets.h"*/
#include "rdtelnet.h"
#include "telserver.h"
#include "clivty.h"
#include "oam_fd.h"
/***********************************************************
 *                     全局变量                            *
***********************************************************/
WORD16 g_wWhileLoopCnt = 0;
extern JID g_tSSHServerJid;
static CHAR buffer_send_to_ssh[MAX_RESULT_TO_SSH_LEN];
/***********************************************************
 *                     本地 变量                            *
***********************************************************/
static CHAR more_page[] = "--------MORE PAGE--------";  /*-满页时显示的提示信息-*/
static CHAR sty_tmpbuf[OAM_STY_LINE_MAX*3] = {0};     /*-提供给输出的临时缓冲-*/
/***********************************************************
 *                     函数申明                            *
***********************************************************/
void oam_sty_puts(sty *cty, CHAR *text);
static void oam_sty_read_loop(sty *vty);
static void oam_sty_page(sty *cty, CHAR c);
static int oam_sty_getch(sty *cty);
static void oam_sty_insert(sty *vty, CHAR c);
static void oam_sty_movehead(sty *vty);
static void oam_sty_puts_cmdline(sty *vty);
static void oam_sty_goedit(sty *vty);
static void oam_sty_gohead(sty *cty);
static void oam_sty_goleft(sty *cty);
static void oam_sty_goright(sty *cty);
static void oam_sty_gotail(sty *cty);
static WORD16 oam_sty_strlen(CHAR *text);
static void oam_sty_putline(sty *cty, CHAR *text);
static void oam_sty_yank(register sty *cty);
static void oam_sty_cutleft(sty *cty);
static void oam_sty_cutright(sty *cty);
static void oam_sty_cuthead(sty *cty);
static void oam_sty_cuttail(sty *cty);
static void oam_sty_cutline(sty *cty);
static void oam_sty_eraseline(sty *cty);
static void oam_sty_putc(sty *cty, int c);

/*发送消息给解释进程*/
static void send_to_remote_parse(sty *sty, CHAR *line, remote_parse_cookie *cookie);
/*具体命令相关的函数*/
static int cease_command_to_remote_parse(sty *vty);
static void send_str_to_remote_parse(sty *vty, CHAR *line, BYTE determiner);

static BOOLEAN char_is_chinese(BYTE ucIn);

static void oam_send_to_ssh(sty *cty, CHAR *text);
/***********************************************************
 *                     宏定义                             *
***********************************************************/
#define oam_sty_ring_full(_sty_) \
  ((_sty_)->ring_end - (_sty_)->ring_start + 1 == 0)
#define oam_sty_ring_empty(_sty_) \
  ((_sty_)->ring_start == (_sty_)->ring_end)
#define oam_sty_historical_interest(_sty_) \
  ((_sty_)->line_len != 0 && \
   (!(_sty_)->history[OAM_STY_HISTORY_SIZE - 1] || \
    strcmp((_sty_)->line_buf, (_sty_)->history[OAM_STY_HISTORY_SIZE - 1]) != 0))

static BOOLEAN char_is_chinese(BYTE ucIn)
{
	return ((ucIn & 0x80) == 0x80);
}


/*从sty缓存中读取最前面一个字符*/
static int oam_sty_getch(sty *cty)
{
    TELNETSVR_0_ASSERT(cty != NULL);

    if (!oam_sty_ring_empty(cty))
    {
        DWORD c = cty->ring_buf[cty->ring_start++];
        if (cty->ring_start >= sizeof(cty->ring_buf))
        {
            cty->ring_start = 0;
        }
        return c;
    }
    return -10000;
}

/*~暂时不使用*/
#if 0
static int oam_sty_getprech(sty *cty)
{
    WORD16 wStart = 0, wPre = 0;
    TELNETSVR_0_ASSERT(cty != NULL);

    wStart = (cty->ring_start > 0) ? cty->ring_start - 1 : sizeof(cty->ring_buf) - 1;
    wPre = (wStart > 0) ? wStart - 1 :  sizeof(cty->ring_buf) - 1;

    return cty->ring_buf[wPre];
}
#endif
/*打印一个字符到终端*/
void oam_sty_putc(sty *cty, int c)
{
    CHAR text[2] = {0};
    text[0] = (CHAR) c;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    if((FD_TYPE_SERIAL == cty->tSocket->ucFDType) || (FD_TYPE_OSSTELNET == cty->tSocket->ucFDType))
    {
        WORD16 fd = (WORD16)cty->tSocket->ucFd;
        WORD16 flag = 0;
        if(FD_TYPE_SERIAL == cty->tSocket->ucFDType)
        {
            flag = CONNECTION_FROM_SERIAL;
        }
        else
        {
            flag = CONNECTION_FROM_TELNET;
        }
        OAM_FD_Send(fd, flag, text, 1);
    }
    else if (FD_TYPE_OSSSSH == cty->tSocket->ucFDType)
    {
        oam_send_to_ssh(cty, text);
    }
    else
    {
        /*~*/
#if 0
        brs_send(cty->tSocket->ucFd, text, 1, 0);
#endif
    }
}
/*打印一个字符串到终端*/
void oam_sty_puts(sty *cty, CHAR *text)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if((FD_TYPE_SERIAL == cty->tSocket->ucFDType) || (FD_TYPE_OSSTELNET == cty->tSocket->ucFDType))
    {
        WORD16 fd = (WORD16)cty->tSocket->ucFd;
        WORD16 flag = 0;
        if(FD_TYPE_SERIAL == cty->tSocket->ucFDType)
        {
            flag = CONNECTION_FROM_SERIAL;
        }
        else
        {
            flag = CONNECTION_FROM_TELNET;
        }
        OAM_FD_Send(fd, flag, text, strlen(text));
    }
    else if (FD_TYPE_OSSSSH == cty->tSocket->ucFDType)
    {
        oam_send_to_ssh(cty, text);
    }
    else
    {
        /*~*/
#if 0
        brs_send(cty->tSocket->ucFd, text, strlen(text), 0);
#endif
    }
}
/*插入一个字符到sty的输入缓存*/
static void oam_sty_insert(sty *cty, CHAR c)
{
    DWORD i = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->line_len >= (sizeof(cty->line_buf) - 1))
    {
    	CHAR aucTmp[200] = {0};
        snprintf(aucTmp, sizeof(aucTmp), "\n\rThe max length of a command is %u bytes, can not input more characters!\n\r", (WORD16)sizeof(cty->line_buf) - 1);
        oam_sty_puts(cty, aucTmp);
        oam_sty_put_prompt_cmdline(cty);
        return;	
    }
    
    if (isascii(c) && isprint((BYTE)c) && (cty->line_len < (sizeof(cty->line_buf) - 1)))
    {
        /*根据当前编辑位置判断是在最后追加还是插入编辑位置*/
        if (cty->line_len == cty->edit_pos)
        {
            cty->line_buf[cty->line_len ++] = (CHAR) c;
        }
        else
        {
            for (i = cty->line_len; i > cty->edit_pos; i --)
            {
                cty->line_buf[i] = cty->line_buf[i - 1];
            }
            cty->line_buf[cty->edit_pos] = c;
            cty->line_len ++;
        }
#if OAM_STY_SCROLL_LINE
        /*重新显示当前行*/
        if (cty->line_len > cty->line_max)
        {
            oam_sty_movehead(cty);
            /*在最后或者中间编辑时候,如果输入超过行宽,隐藏字符数增加*/
            if (((cty->line_len == cty->edit_pos + 1) &&
                    (cty->edit_pos - cty->disp_pos == cty->line_max)) ||
                    ((cty->line_len != cty->edit_pos + 1) &&
                     (cty->edit_pos - cty->disp_pos + OAM_STY_SEPARATE == cty->line_max)))
            {
                cty->disp_pos += OAM_STY_JUMP_LEN;
            }
            oam_sty_puts_cmdline(cty);
            cty->edit_pos ++;
            oam_sty_goedit(cty);
        }
        else
#endif
        {
            cty->edit_pos ++;
            /*??这段感觉是多余的,行尾和中间插入可以相同处理
            if (cty->line_len == cty->edit_pos)
            {
                if (cty->flags & OAM_STY_FLAG_ECHOING)
                {
                    if (cty->flags & OAM_STY_FLAG_PASSWORD)
                    {
                        oam_sty_putc(cty, MARK_PSWD);
                    }
                    else
                    {
                        oam_sty_putc(cty, c);
                    }
                }
            }
            else*/
            {
                /*从编辑位置开始,重新显示后面的字符*/
                for (i = cty->edit_pos - 1; i < cty->line_len; i ++)
                {
                    if (cty->flags & OAM_STY_FLAG_ECHOING)
                    {
                        if (cty->flags & OAM_STY_FLAG_PASSWORD)
                        {
                            oam_sty_putc(cty, MARK_PSWD);
                        }
                        else
                        {
                            oam_sty_putc(cty, cty->line_buf[i]);
                        }
                    }
                }
                /*显示完后,光标移回编辑位置*/
                for ( ; i > cty->edit_pos; i --)
                {
                    if (cty->flags & OAM_STY_FLAG_ECHOING)
                    {
                        oam_sty_putc(cty, MARK_LEFT);
                    }
                }
            }
        }
    }
}

/*把光标从编辑位置移到显示位置开始处*/
static void oam_sty_movehead(sty *cty)
{
    DWORD i = 0, k = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (!(cty->flags & OAM_STY_FLAG_ECHOING))
    {
        return;
    }

    for (i = cty->disp_pos, k = 0; i < cty->edit_pos; i ++, k ++)
    {
        sty_tmpbuf[k] = MARK_LEFT;
    }
    sty_tmpbuf[k] = MARK_END;
    oam_sty_puts(cty, sty_tmpbuf);
}

/************************************************************************
* 函数名称： static void oam_sty_puts_cmdline(sty *cty)
* 功能描述： 输出终端缓存中的命令行信息
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：和直接输出相比，该函数支持超过行宽的隐藏
* 修改日期      版本号  修改人      修改内容
************************************************************************/
static void oam_sty_puts_cmdline(sty *cty)
{
    DWORD i = 0, j = 0, k = 0, length = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (!(cty->flags & OAM_STY_FLAG_ECHOING))
    {
        return;
    }

    i = 0;
    k = 0;
    memset(sty_tmpbuf, 0, sizeof(sty_tmpbuf));
    /*如果显示位置不是从头开始，隐藏部分用两个点 替代*/
    if (cty->disp_pos)
    {
        for ( ; i < OAM_STY_HIDE_LEN; i ++, k ++)
        {
            sty_tmpbuf[k] = MARK_DOT;
        }
    }
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;

    length = cty->line_max;
    if (cty->disp_pos + cty->line_max < cty->line_len)
    {
        length -= OAM_STY_HIDE_LEN;
    }

    /*循环从显示位置读取字符*/
    for ( ; i < length; i ++, k ++)
    {
        /*如果显示长度超过行宽，删除超出的一个字符*/
        if (i + cty->disp_pos >= cty->line_len)
        {
            for (j = i; j < length; j ++, k ++)
            {
                sty_tmpbuf[k] = MARK_BLANK;
            }
            for ( ; j > i; j --, k ++)
            {
                sty_tmpbuf[k] = MARK_LEFT;
            }
            break;
        }
        else
        {
            /* 防止密码暴露 */
            if (cty->flags & OAM_STY_FLAG_PASSWORD)
            {
                sty_tmpbuf[k] = MARK_PSWD;
            }
            else
            {
                sty_tmpbuf[k] = cty->line_buf[i + cty->disp_pos];
            }
        }
    }

    /*如果字符数达到行宽还没有结束，后面的用.. 隐藏*/
    if (i == length)
    {
        for ( ; i < cty->line_max; i ++, k ++)
        {/*这个循环次数应该就是OAM_STY_HIDE_LEN*/
            sty_tmpbuf[k] = MARK_DOT;
        }
    }
    
    sty_tmpbuf[k] = MARK_END;
    oam_sty_puts(cty, sty_tmpbuf);
}

/*把光标从行尾移到编辑处，通常在显示命令行后调用*/
static void oam_sty_goedit(sty *cty)
{
    DWORD i, k;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (!(cty->flags & OAM_STY_FLAG_ECHOING))
    {
        return;
    }

    for (i = cty->edit_pos, k = 0; i < cty->disp_pos + cty->line_max; i ++, k ++)
    {
        if (i == cty->line_len)
        {
            break;
        }
        sty_tmpbuf[k] = MARK_LEFT;
    }
    sty_tmpbuf[k] = MARK_END;
    oam_sty_puts(cty, sty_tmpbuf);
}

/*处理左移*/
static void oam_sty_goleft(sty *cty)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->edit_pos <= 0)
    {
        return;
    }

    /*如果左移到了显示开始位置，显示位置前移一次，否则只是光标左移*/
#if OAM_STY_SCROLL_LINE
    if ((cty->disp_pos) && (cty->edit_pos == cty->disp_pos + OAM_STY_SEPARATE))
    {
        oam_sty_movehead(cty);
        if (cty->disp_pos >= OAM_STY_JUMP_LEN)
        {
            cty->disp_pos -= OAM_STY_JUMP_LEN;
        }
        else
        {
            cty->disp_pos = 0;
        }
        oam_sty_puts_cmdline(cty);
        cty->edit_pos --;
        oam_sty_goedit(cty);
    }
    else
#endif
    {
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
        cty->edit_pos --;
    }
    return;
}

/*处理光标右移*/
static void oam_sty_goright(sty *cty)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->edit_pos >= cty->line_len )
    {
        return;
    }

    /*如果右移到了行宽位置，显示位置后移移一次，否则只是光标右移*/
#if OAM_STY_SCROLL_LINE
    if (((cty->disp_pos + cty->line_max - OAM_STY_SEPARATE) <= cty->edit_pos + 1) &&
            ((cty->edit_pos + 1 + OAM_STY_SEPARATE) < (cty->line_len)))
    {
        oam_sty_movehead(cty);
        cty->disp_pos += OAM_STY_JUMP_LEN;
        if ((cty->disp_pos + cty->line_max) > cty->line_len)
        {/*右移时候最后一段显示行宽个字符*/
            cty->disp_pos = cty->line_len - cty->line_max;
        }
        oam_sty_puts_cmdline(cty);
        cty->edit_pos ++;
        oam_sty_goedit(cty);
    }
    else
#endif
    {
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            /* 防止密码暴露 */
            if (cty->flags & OAM_STY_FLAG_PASSWORD)
            {
                oam_sty_putc(cty, MARK_PSWD);
            }
            else
            {
                oam_sty_putc(cty, cty->line_buf[cty->edit_pos]);
            }
        }
        cty->edit_pos ++;
    }
    return;
}

/*编辑位置移到当前输入命令行最前面*/
static void oam_sty_gohead(sty *cty)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->edit_pos == 0)
    {
        return;
    }

#if OAM_STY_SCROLL_LINE
    if (cty->disp_pos)
    {
        /*??这个调不调无所谓吧
        oam_sty_movehead(cty); */
        cty->disp_pos = 0;
        oam_sty_puts_cmdline(cty);
        cty->edit_pos = 0;
        oam_sty_goedit(cty);
    }
    else
#endif
    {/*??如果不考虑行是否可以滚动，这段可以不要，上面if 的就可以*/
        DWORD i;
        for (i = cty->line_len - cty->edit_pos; i < cty->line_len; i ++)
        {
            if (cty->flags & OAM_STY_FLAG_ECHOING)
            {
                oam_sty_putc(cty, MARK_LEFT);
            }
        }
        cty->edit_pos = 0;
    }
    return;
}

/*-光标移到行尾"ctrl-e"-*/
static void oam_sty_gotail(sty *cty)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->edit_pos == cty->line_len )
    {
        return;
    }

#if OAM_STY_SCROLL_LINE
    if ((cty->disp_pos + cty->line_max) < cty->line_len)
    {
        /*??oam_sty_movehead(cty);*/
        cty->disp_pos = cty->line_len - cty->line_max;
        oam_sty_puts_cmdline(cty);
        cty->edit_pos = cty->line_len;
    }
    else
#endif
    {
        DWORD i = 0;
        for (i = cty->edit_pos; i < cty->line_len; i ++)
        {
            if (cty->flags & OAM_STY_FLAG_ECHOING)
            {
                oam_sty_putc(cty, cty->line_buf[i]);
            }
        }
        cty->edit_pos = cty->line_len;
    }
    return;
}

/*标识当前终端允许输入*/
void oam_sty_enable_input(sty *vty, BOOL bFlag)
{
    TELNETSVR_VOID_ASSERT(vty != NULL);

    if (bFlag)
    {
        vty->flags &= ~OAM_STY_FLAG_STOPINPUT;
    }
    else
    {
        vty->flags |= OAM_STY_FLAG_STOPINPUT;
    }
}

/************************************************************************
* 函数名称：  void oam_sty_put_prompt_cmdline(sty *cty)
* 功能描述：输出系统提示信息+ 命令行
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
************************************************************************/
void oam_sty_put_prompt_cmdline(sty *cty)
{
    WORD16 wLen = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    /*611001779292 重新计算当前行最大可输入长度，防止前面显示
      操作修改过这个值 */
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;        
    wLen = cty->line_len;
    /*输出系统提示符*/
    if(!sdp_puts(cty, cty->prompt_line))
    {/*如果当前已经满一页*/
        return;
    }
    cty->edit_pos = wLen;
    cty->line_len = wLen;

    /*611001897277 命令输入过长时候，会出现无法输入 ？ 查看帮助的情况*/
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;  
    /*输出命令行*/
    if (cty->line_len)
#if OAM_STY_SCROLL_LINE
    {
        if (cty->line_len > cty->line_max)
        {
            cty->disp_pos = cty->line_len - cty->line_max;
        }
        oam_sty_puts_cmdline(cty);
    }
#else
        sdp_puts(cty, cty->line_buf);
#endif
    /*611001779292 重新计算当前行最大可输入长度，sdp_puts会根据显示模式（显示时候分屏模式行宽变大）
      修改这个值 */
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;        
}

/*输出字符集，遇到换行或者达到行宽时候生成一行数据
  如果没有满页就显示，否则保存到输出行链表*/
WORD16 sdp_puts(sty *cty, CHAR *text)
{
    int str_len = 0;
    CHAR *p = NULL;
    CHAR tmptext[OAM_STY_OUTPUT_LINE_MAX * 2] = {0};

    TELNETSVR_0_ASSERT(cty != NULL);

    if (!cty)
    {
        return 0;
    }
    p=text;
    if(!text)
    {
        /*611001058985 如果是空串，查看是否是最后一包*/
        remote_parse_cookie *rpc = cty->link_cookie;
        if (rpc)
        {
            if (MSG_MORE_PKT == rpc->last_pkt_flag)
            {
                req_more_data_to_remote_parse(cty);
            }
        }
        return 0;
    }

    /*-关闭了显示直接返回-*/
    if(cty->flags & OAM_STY_FLAG_NODISPLAY)
    {
        return ((cty->disp_line > OAM_STY_PAGE_LINE) ? 0 : 1);
    }

    str_len = 0;
    WHILE (*p != 0)
    {
        /*611001924617 输出不检查循环次数*/
        /*LOOP_CHECK();*/
        tmptext[str_len] = *p;
        if(*p == MARK_LEFT)
        {
            cty->fill_len = (cty->fill_len  > 0) ? (cty->fill_len--) : cty->fill_len ;
        }
        else
        {
            if (char_is_chinese(*p))
            {/*中文字符，连续读两个字节*/
                if (cty->fill_len == OAM_STY_OUTPUT_LINE_MAX)
                {/*如果中文字符读一半就达到行宽，回退已经读取的一半到下行读*/
                    /*已经读取的放到下行读*/
                    p--;
                    tmptext[str_len] = ' ';
                }
                else
                {/*如果还没有达到行宽，就接着读取下一个字节，保证中文字符没有被分开*/
                    p++;
                    str_len++;
                    tmptext[str_len] = *p;
                    if(*p == MARK_LEFT)
                        cty->fill_len = (cty->fill_len  > 0) ? (cty->fill_len--) : cty->fill_len ;
                    else
                        cty->fill_len ++;
                }
            }
            cty->fill_len ++;
        }
        str_len ++;

        /*-遇回车符或满行时加一行-*/
        if ((*p == '\n') || (cty->fill_len >= OAM_STY_OUTPUT_LINE_MAX))
        {
            tmptext[str_len] = MARK_END;
            str_len = 0;
            cty->disp_line ++;
            cty->edit_pos = 0;
            cty->line_len = 0;
            cty->line_max = OAM_STY_OUTPUT_LINE_MAX - 1;

            oam_sty_putline(cty, tmptext);  /*-递交显示或保存起来-*/
            cty->fill_len = 0;
            
        }
        p ++;
    }
    /*-这一行往往是提示符-*/
    /*-在MP方式下,在很多场合传过来的是半行信息,该行还能显示的字符数由fill_len得出-*/
    if((cty->fill_len == str_len) && (str_len))
    {
        cty->disp_line ++;
        cty->line_max = OAM_STY_OUTPUT_LINE_MAX - str_len - 1;        /*-记录这一行还能显示的字符数-*/
    }
    tmptext[str_len] = MARK_END;
    oam_sty_putline(cty, tmptext);

    /*-显示区满时返回0-*/
    if(cty->disp_line > OAM_STY_PAGE_LINE)
    {
        return 0;
    }

    return 1;
}

/*把要显示的数据每行一个节点放到显示缓存链表或者显示*/
static void oam_sty_putline(sty *cty, CHAR *text)
{
    DISPLAY_NODE *pnewnode = NULL, *plastnode = NULL;
    WORD16 strlength = 0;
    CHAR *combinstr = NULL;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (!cty || !text || strlen(text) == 0)
    {
        return;
    }

    if ((cty->disp_line < OAM_STY_PAGE_LINE) &&
         (NULL == cty->p_disp_head))         /*-不足一页直接递交显示-*/
    {
        WORD16 wLen = 0;
        WORD16 wPromtLen = 0;
        /*处理半行数据*/
        if(1 == cty->ucToShowLabel)
        {
            snprintf(cty->szStrToShow, OAM_STY_OUTPUT_LINE_MAX * 2 - 1, "%s%s", cty->szStrToShow, text);
            oam_sty_puts(cty, cty->szStrToShow);
            cty->ucToShowLabel = 0;
            memset(cty->szStrToShow, 0, sizeof(cty->szStrToShow));
        }
        else
        {
            oam_sty_puts(cty, text);
        }

        wPromtLen = strlen(cty->prompt_line);
        wLen = ((cty->fill_len - wPromtLen) < 0) ? 0 :(cty->fill_len - wPromtLen);

        cty->line_len = wLen;
        cty->edit_pos = wLen;

        if(text[strlen(text) - 1] == '\n')
        {
            if (!cty->sshauthmsg)
                oam_sty_puts(cty, "\r");
        }
        else 
        {
            if(cty->fill_len >= OAM_STY_OUTPUT_LINE_MAX)
            {
                oam_sty_puts(cty, "\n\r");
            }
        }

        /*?? 应该不会出现disp_head不为空的情况吧，可能是为了保险起见*/
        if (cty->p_disp_head)
        {
            OAM_RETUB(cty->p_disp_head->disp_info);
            OAM_RETUB(cty->p_disp_head);
            cty->p_disp_head = NULL;
        }

    }
    else /*-保存起来-*/
    {
        pnewnode = (DISPLAY_NODE *)XOS_GetUB(sizeof(DISPLAY_NODE));
        if (!pnewnode)
        {
            return;
        }
        memset(pnewnode, 0, sizeof(DISPLAY_NODE));
        pnewnode->disp_info = (CHAR *)XOS_GetUB(strlen(text) + 1);
        if (!pnewnode->disp_info)
        {
            OAM_RETUB(pnewnode);
            return;
        }
        memset(pnewnode->disp_info, 0, strlen(text) + 1);
        strncpy(pnewnode->disp_info, text, strlen(text) + 1);
        pnewnode->p_next = NULL;
        /*-插入下一行信息-*/
        if (NULL == cty->p_disp_head)
        {/*显示缓存里面没有记录*/
            /*有时TEXT是"\n",就需要将其输出后再提示按键*/
	     #if 0
            if(!strcmp(text, "\n"))
            {
                oam_sty_putc(cty,'\n');
            }
            #endif
            oam_sty_puts(cty, more_page);           /*-按键提示-*/
            if(cty->ucToShowLabel)
            {
                snprintf(cty->szStrToShow, OAM_STY_OUTPUT_LINE_MAX * 2 - 1, "%s%s", cty->szStrToShow, pnewnode->disp_info);
                OAM_RETUB(pnewnode->disp_info);
                pnewnode->disp_info = (CHAR *)XOS_GetUB(strlen(cty->szStrToShow) + 1);
                if(!pnewnode->disp_info)
                {
                    OAM_RETUB(pnewnode);
                    return;
                }
                memcpy(pnewnode->disp_info, cty->szStrToShow, strlen(cty->szStrToShow) + 1);
                cty->ucToShowLabel = 0;
                memset(cty->szStrToShow, 0, sizeof(cty->szStrToShow));
            }
            cty->p_disp_head= pnewnode;
        }
        else
        {
            plastnode = cty->p_disp_head;
            WHILE (plastnode->p_next != NULL)
            {
                LOOP_CHECK();
                plastnode = plastnode->p_next;   /*-找到最后一个节点-*/
            }
            strlength = strlen(plastnode->disp_info);

            /*-上一行内容为半行时应该把两块内容存放在一起-*/
            /*2005.1.28 修改：支持每行超过80字符的回显，修改条件语句
            if ((oam_sty_strlen(plastnode->disp_info) + oam_sty_strlen(text) <= OAM_STY_LINE_MAX) &&*/
            if((plastnode->disp_info[strlength - 1] != '\n') && (strlength < OAM_STY_OUTPUT_LINE_MAX))
            {
                WORD16 wAppendLen = 0;
                if (strlen(text) < (WORD16)(OAM_STY_OUTPUT_LINE_MAX - strlength))
                {
                    wAppendLen = strlen(text);
                }
                else
                {
                    wAppendLen = OAM_STY_OUTPUT_LINE_MAX - strlength;
                }
                
                combinstr = (CHAR *)XOS_GetUB(wAppendLen + strlength + 1);
                if (!combinstr)
                {
                    OAM_RETUB(pnewnode->disp_info);
                    OAM_RETUB(pnewnode); 
                    return;                           
                }
                
                strncpy(combinstr, plastnode->disp_info, wAppendLen + strlength + 1);
                strncat(combinstr, pnewnode->disp_info, wAppendLen + strlength + 1); 
                OAM_RETUB(plastnode->disp_info);
                plastnode->disp_info = combinstr;

                /*如果有剩下的,把剩下的放在newnode*/
                if (strlen(text) > wAppendLen)
                {
                    memset(pnewnode->disp_info, 0, strlen(text) + 1);
                    strncpy(pnewnode->disp_info, text + wAppendLen, strlen(text) + 1);
                    plastnode->p_next = pnewnode;
                }
                else
                {
                    OAM_RETUB(pnewnode->disp_info);
                    OAM_RETUB(pnewnode);
                }
            }
            else
            {
                plastnode->p_next = pnewnode;
            }
        }
    }
}

/*从输入缓存中读取输入，进行分析处理*/
static void oam_sty_read_loop(sty *vty)
{
    DWORD c = 0, i = 0;
    static BYTE ucPreCRLF = 0; /*用来处理连续的\n\r 重复输入问题*/

    TELNETSVR_VOID_ASSERT(vty != NULL);

    if (vty->flags & OAM_STY_FLAG_STOPINPUT)
    {
        return;
    }

    /*从sty的缓存中逐个获取字符*/
    WHILE ((c = oam_sty_getch(vty)) != (DWORD)-10000)
    {
        LOOP_CHECK();

        if (OAM_CTL_IAC == c)
        {/*客户端发来的协商数据，暂不处理*/
            oam_sty_getch(vty);
            oam_sty_getch(vty);

            continue;
        }

/*disp_line 应该在oam_sty_page进行处理，这里先注释掉*/
 /*       vty->disp_line = 1;*/
        if ((vty->flags & OAM_STY_FLAG_EXECMODE) || (vty->flags & OAM_STY_FLAG_RAWMODE))
        {
            vty->disp_line = 1;
            if (c == 3)/*Ctrl+C*/
            {
                /*receiver = vty->app_receiver;*/
                /*vty->app_receiver = 0;*/
                vty->flags &= ~OAM_STY_FLAG_EXECMODE;
                /*receiver(vty, "break by user", vty->app_cookie);*/
                /*2003.8.22 add by kongyj*/
                cease_command_to_remote_parse(vty);
            }
        }
        /*-满页时按键处理-*/
        else if (vty->p_disp_head)
        {
            oam_sty_page(vty, (CHAR)c);
            /*edit by wangmh*/
            vty->ring_start = vty->ring_end;
            return;
        }
        else
        {
            vty->disp_line = 1;
        }
        
        /*        else if ((vty->flags & OAM_STY_FLAG_RAWMODE)|| (vty->flags & OAM_STY_FLAG_EXECMODE))*//*作为客户端处理*/

        /*?? OAM_STY_FLAG_RAWMODE 好像不会出现*/
#if 0
        if ((vty->flags & OAM_STY_FLAG_RAWMODE))/*|| (vty->flags & OAM_STY_FLAG_EXECMODE))*/
        {
            vty->line_buf[1] = 0;
            if (c == 27)
            {
                vty->line_buf[1] = (CHAR) oam_sty_getch(vty);
                vty->line_buf[2] = (CHAR) oam_sty_getch(vty);
                vty->line_buf[3] = 0;
            }
            else
            {
                if (vty->flags & OAM_STY_FLAG_ECHOING)
                {/*回显*/
                    oam_sty_putc(vty, c);
                }
            }
            vty->line_buf[0] = (CHAR) c;
            return;
        }
        /*else*/
#endif
        {
            /*方向键时候有三个字符。只读取第三个用来控制读取*/
            static CHAR cTmp = 0;

            /*-输入方向键-*/
            if (c == 27)
            {
                cTmp = 1;
                continue;
            }

            if(1 == cTmp)
            {/*如果已经读取了一个字符，再读取一个，跳过不做处理*/
                cTmp = 2;
                continue;
            }
            else if(2 == cTmp)
            {/*读取第三个用来判断*/
                cTmp = 0;
                switch(c)
                {
                    /* -转换一下方便统一处理-*/
                case 65:
                    c = 'P' & 077;
                    break;       /*- up  -*/
                case 66:
                    c = 'N' & 077;
                    break;       /*-down -*/
                case 67:
                    c = 'F' & 077;
                    break;       /*-right-*/
                case 68:
                    c = 'B' & 077;
                    break;       /*-left -*/
                default:
                    break;
                }
            }

            /*-按键具体处理流程-*/
            switch (c)
            {
            case '\r':
            case '\n':
                {
                    if (0 == ucPreCRLF)
                    {
                        ucPreCRLF = c;
                    }
                    else if (c != ucPreCRLF)
                    {/*如果连续输入和上次不同的，不处理*/
                        ucPreCRLF = 0;
                        break;
                    }
                    else
                    {
                        ucPreCRLF = 0;
                    }

                    if (vty->flags & OAM_STY_FLAG_ECHOING)
                    {
                        oam_sty_puts(vty, "\r\n");
                    }
                    vty->line_buf[vty->line_len] = '\0';
                    if (oam_sty_historical_interest(vty) && (!(vty->flags & OAM_STY_FLAG_PASSWORD)))
                    {/*可能因为访问要求，历史命令从指针数组最后一个开始放*/
                        if (vty->history[0])
                        {
                            OAM_RETUB(vty->history[0]);
                        }
                        for (i = 0; i < OAM_STY_HISTORY_SIZE - 1; i ++)
                        {
                            vty->history[i] = vty->history[i + 1];
                        }
                        vty->history[OAM_STY_HISTORY_SIZE - 1] = (CHAR *)XOS_GetUB(strlen(vty->line_buf) + 1);
                        if (vty->history[OAM_STY_HISTORY_SIZE - 1])
                        {
                            strncpy(vty->history[OAM_STY_HISTORY_SIZE - 1], vty->line_buf , strlen(vty->line_buf) + 1);
                        }
                    }
                    vty->history_ptr = OAM_STY_HISTORY_SIZE;

                    /*save vty extra info*/
                    {
                        sty * extra_vty = NULL;
                        extra_vty = FindExtraSty(vty->tSocket);
                        if(!extra_vty)
                        {
                            return;
                        }
                        extra_vty->line_len = vty->line_len;
                        extra_vty->edit_pos = vty->edit_pos;
                        extra_vty->disp_pos = vty->disp_pos;
                        extra_vty->fill_len = vty->fill_len;
                        extra_vty->line_max = vty->line_max;
                    }

                    vty->line_len = 0;
                    vty->edit_pos = 0;
                    vty->disp_pos = 0;
                    vty->fill_len  = 0;
                    vty->line_max = OAM_STY_LINE_MAX - 1;/*空一行显示more_page*/
    #if INSTALL_MP_TELNET
                    if (vty->flags & OAM_STY_FLAG_REMOTE_PARSE)
                    {
                        send_str_to_remote_parse(vty, vty->line_buf, DM_NORMAL);
                    }
                    else
    #endif
                    {
                        ;
                    }
                    break;
                }
            case '?':
                {
                    BOOLEAN bIsNormalChar = FALSE;
                    SWORD16 swCnt = 0;
              
                    vty->line_buf[vty->line_len] = '\0';
                    /*查看输入的问号是否是普通字符*/      
                    for (swCnt = strlen(vty->line_buf); swCnt > 0; swCnt--)
                    {
                        if ('\\' == vty->line_buf[swCnt - 1])
                        {
                            bIsNormalChar = bIsNormalChar ? FALSE : TRUE;
                        }
                        else
                        {/*往前遇到其他任何字符都break*/
                            break;
                        }
                    }
                    
                    if (bIsNormalChar)
                    {
                        oam_sty_insert(vty, (CHAR)c);
                        continue;
                    }
                     
                    if (oam_sty_historical_interest(vty) && (!(vty->flags & OAM_STY_FLAG_PASSWORD)))
                    {
                        if (vty->history[0])
                        {
                            OAM_RETUB(vty->history[0]);
                        }
                        for (i = 0; i < OAM_STY_HISTORY_SIZE - 1; i ++)
                        {
                            vty->history[i] = vty->history[i + 1];
                        }
                        vty->history[OAM_STY_HISTORY_SIZE - 1] = (CHAR *)XOS_GetUB(strlen(vty->line_buf) + 1);
                        if (vty->history[OAM_STY_HISTORY_SIZE - 1])
                        {
                            strncpy(vty->history[OAM_STY_HISTORY_SIZE - 1], vty->line_buf, strlen(vty->line_buf) + 1);
                        }
                    }
                    vty->history_ptr = OAM_STY_HISTORY_SIZE;
                    if (vty->flags & OAM_STY_FLAG_PASSWORD)
                    {
                        oam_sty_insert(vty, (CHAR)c);
                        continue;
                    }
                    if (vty->line_len < sizeof(vty->line_buf) - 1)
                    {
                        vty->line_buf[vty->line_len++] = '?';
                        vty->edit_pos ++;
                        if (vty->flags & OAM_STY_FLAG_ECHOING)
                        {
                            oam_sty_putc(vty, c);
                        }
                    }
                    if (vty->flags & OAM_STY_FLAG_ECHOING)
                    {
                        oam_sty_puts(vty, "\r\n");
                    }
                    vty->line_buf[vty->line_len] = '\0';
                    /*save vty extra info*/
                    {
                        sty * extra_vty = NULL;
                        extra_vty = FindExtraSty(vty->tSocket);
                        extra_vty->line_len = vty->line_len;
                        extra_vty->edit_pos = vty->edit_pos;
                        extra_vty->disp_pos = vty->disp_pos;
                        extra_vty->fill_len = vty->fill_len;
                        extra_vty->line_max = vty->line_max;
                    }

                    vty->line_len = 0;
                    vty->edit_pos = 0;
                    vty->fill_len = 0;
                    vty->line_max = OAM_STY_LINE_MAX - 1;

#if INSTALL_MP_TELNET
                    if (vty->flags & OAM_STY_FLAG_REMOTE_PARSE)
                    {
                        SWORD32 line_len = 0;
                        line_len = strlen(vty->line_buf);

                        line_len -= 2;/*获取输入的最后一个字符在buf中的下标*/
                        if ((line_len >= 0) && (vty->line_buf[line_len] == ' '))
                        {
                            vty->line_buf[line_len + 1] = '\0';
                            send_str_to_remote_parse(vty, vty->line_buf, DM_BLANKQUESTION);
                        }
                        else
                        {
                            vty->line_buf[line_len + 1] = '\0';
                            send_str_to_remote_parse(vty, vty->line_buf, DM_QUESTION);
                        }
                    }
                    else
#endif
                    {
                        /*receiver(vty, vty->line_buf, vty->app_cookie);*/
                    }
                    return;
                }

            case 9:              /*-tab-*/
                if (vty->flags & OAM_STY_FLAG_PASSWORD)
                {
                    oam_sty_insert(vty, (CHAR)c);
                    continue;
                }
                if (vty->line_len < sizeof(vty->line_buf) - 1)
                {
                    vty->line_buf[vty->line_len++] = 9; /*tab*/
                    vty->edit_pos ++;
                }
                if (vty->flags & OAM_STY_FLAG_ECHOING)
                {
                    oam_sty_puts(vty, "\r\n");
                }
                vty->line_buf[vty->line_len] = '\0';
                /*save vty extra info*/
                {
                    sty * extra_vty = NULL;
                    extra_vty = FindExtraSty(vty->tSocket);
                    extra_vty->line_len = vty->line_len;
                    extra_vty->edit_pos = vty->edit_pos;
                    extra_vty->disp_pos = vty->disp_pos;
                    extra_vty->fill_len = vty->fill_len;
                    extra_vty->line_max = vty->line_max;
                }

                vty->line_len = 0;
                vty->edit_pos = 0;
                vty->fill_len = 0;
                vty->line_max = OAM_STY_LINE_MAX - 1;

#if INSTALL_MP_TELNET
                if (vty->flags & OAM_STY_FLAG_REMOTE_PARSE)
                {
                    DWORD line_len = 0;
                    line_len = strlen(vty->line_buf);

                    vty->line_buf[line_len - 1] = '\0';
                    send_str_to_remote_parse(vty, vty->line_buf, DM_TAB);
                }
                else
#endif
                {
                    ;
                }
                return;

            case 'P' & 077:          /*-ctrl-p- 或上箭头*/
                vty->history_ptr--;
                if((short)vty->history_ptr < 0)
                {
                    vty->history_ptr = 0;
                    continue;
                }
                if(vty->history[vty->history_ptr] == NULL)
                {
                    vty->history_ptr++;/*指针复原*/
                    continue;
                }
                if(strlen(vty->history[vty->history_ptr]) == 0)
                {
                    vty->history_ptr++;/*指针复原*/
                    continue;
                }
                oam_sty_yank(vty);
                continue;
            case 'N' & 077:          /*-ctrl-n-或下箭头*/
                vty->history_ptr++;
                if(vty->history_ptr > OAM_STY_HISTORY_SIZE - 1)
                {
                    vty->history_ptr = OAM_STY_HISTORY_SIZE - 1;
                    continue;
                }
                if(vty->history[vty->history_ptr] == NULL)
                {
                    vty->history_ptr--;/*指针复原*/
                    continue;
                }
                if(strlen(vty->history[vty->history_ptr]) == 0)
                {
                    vty->history_ptr--;/*指针复原*/
                    continue;
                }

                oam_sty_yank(vty);
                continue;
            case 'H' & 077:          /*-ctrl-h-*/
                oam_sty_cutleft(vty);
                continue;
            case '\177':             /*-delete-*/
            case 'D' & 077:          /*-ctrl-d-*/
                oam_sty_cutright(vty);
                continue;
            case 'B' & 077:          /*-ctrl-b-*/
                oam_sty_goleft(vty);
                continue;
            case 'F' & 077:          /*-ctrl-f-*/
                oam_sty_goright(vty);
                continue;
            case 'A' & 077:          /*-ctrl-a-*/
                oam_sty_gohead(vty);
                continue;
            case 'E' & 077:          /*-ctrl-a-*/
                oam_sty_gotail(vty);
                continue;
            case 'W' & 077:          /*-ctrl-w-*/
                oam_sty_cuthead(vty);
                continue;
            case 'K' & 077:          /*-ctrl-k-*/
                oam_sty_cuttail(vty);
                continue;
            case 'U' & 077:          /*-ctrl-u-*/
                oam_sty_cutline(vty);
                continue;
            case 'Y' & 077:          /*-ctrl-y-*/
                vty->history_ptr = OAM_STY_HISTORY_SIZE - 1;
                oam_sty_yank(vty);
                continue;
            default:
                oam_sty_insert(vty, (CHAR)c);
                continue;
            }/*switch(c)*/
        }/*if (vty->flags & OAM_STY_FLAG_RAWMODE)*/
    }/*while ((c = oam_sty_getch(sty)) != -10000)*/
}

/*好像是计算到编辑位置的字符数*/
static WORD16 oam_sty_strlen(CHAR *text)
{
    CHAR *p = text;
    WORD16 len = 0;

    WHILE (*p)
    {
        LOOP_CHECK();
        if (*p != MARK_LEFT)
        {
            len ++;
        }
        else
        {/*如果是MARK_LEFT，长度减一*/
            len --;
        }
        p ++;
    }

    return len;
}

/*-分页显示控制,disp_line指示当前终端已显示的行数-*/
static void oam_sty_page(sty *cty, CHAR c)
{
    WORD16 i = 0;
    DISPLAY_NODE *p_node = NULL, *release_node = NULL;
    CHAR *pinfo = NULL;
    WORD16 wLen = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    /*如果不支持任意键中止，则直接退出*/
    if ((c != ' ') && (c != '\r') && (c != '\n') && (cty->bCeaseAttr == 2))
    {
        oam_sty_puts(cty, "\n\r%Note: The current command doesn't support [ctrl+c] or [anykey] operation!\n\r");
        return;
    }

    p_node = cty->p_disp_head;

    wLen = strlen(more_page);
    for (i = 0; i < wLen; i ++)
    {
        oam_sty_puts(cty, CUT_CHAR);       /*-擦除按键提示信息-*/
    }

    switch(c)
    {
        /*-按空格显示下一页-*/
    case ' ':
    case '\r':
    case '\n':
        if (' ' == c)
        {
            cty->disp_line = 1;        /*-计新页显示的行数-*/
        }
        
        for (i = 1; i < OAM_STY_PAGE_LINE; i ++) /*-有一行预留给提示信息-*/
        {
            WORD16 wDispLen = 0;
            
            if (p_node == NULL || p_node->disp_info == NULL)
            {
                break;
            }

            wDispLen = strlen(p_node->disp_info);
            
            cty->disp_line ++;
            if ((p_node->disp_info != NULL) && (wDispLen >= 1))
            {
                if(strcmp(p_node->disp_info,cty->prompt_line) != 0)
                {
                    if(cty->ucToShowLabel == 1)
                    {/*处理半行*/
                        snprintf(cty->szStrToShow, OAM_STY_OUTPUT_LINE_MAX * 2 - 1, "%s%s", cty->szStrToShow, p_node->disp_info);
                        oam_sty_puts(cty, cty->szStrToShow);
                        cty->ucToShowLabel = 0;
                        memset(cty->szStrToShow, 0, sizeof(cty->szStrToShow));
                    }
                    else
                    {
                        oam_sty_puts(cty, p_node->disp_info);      /*-显示-*/
                    }

                    /*如果不是以换行结尾，添加换行*/
                    if(*(p_node->disp_info + wDispLen - 1) != '\n')
                    {
                        oam_sty_puts(cty, "\n");
                    }
                        
                    oam_sty_puts(cty, "\r");
                }
                OAM_RETUB(p_node->disp_info);   /*-释放显示空间-*/
            }
            release_node = p_node;
            p_node = p_node->p_next;
            cty->p_disp_head = p_node;
            OAM_RETUB(release_node);  /*-释放节点空间-*/

            if ((p_node) && (p_node->p_next == NULL))
            {/*处理最后一行*/
                pinfo = p_node->disp_info;
                /*-这种情况下,是半行信息,更正行计数,允许MP再发消息显示本行下半截-*/
                if ((oam_sty_strlen(pinfo) < OAM_STY_OUTPUT_LINE_MAX) && (pinfo[strlen(pinfo) - 1] != '\n'))
                {
                    /*CRDCM00313101 用snprintf替换strcpy、strcat函数*/
                    snprintf(cty->szStrToShow,
                                 sizeof(cty->szStrToShow) -1,
                                 "%s",
                                 pinfo);

                    cty->ucToShowLabel = 1;
                    cty->disp_line --;

                    release_node = p_node;
                    p_node = p_node->p_next;
                    OAM_RETUB(release_node);  /*-释放节点空间-*/
                    break;
                }
            }

            /*如果不是空格而是回车，到这里就break*/
            if(c != ' ')
            {
                break;
            }
        }
        
        cty->p_disp_head = p_node;
        if (p_node == NULL)        /*-显示完毕-*/
        {
            remote_parse_cookie *rpc = NULL;
            rpc = cty->link_cookie;
            if (!rpc)
            {
                return;
            }
            
            /*如果是回车符显示完的，不复位disp_line
              以便后面收到的数据还是加入显示链表*/
            if (' ' == c)
            {
                cty->disp_line = 1;        /*-计新页显示的行数-*/
            }
#if INSTALL_MP_TELNET
            if ((cty->flags & OAM_STY_FLAG_REMOTE_PARSE) &&  (MSG_MORE_PKT == rpc->last_pkt_flag))
            {
                if (req_more_data_to_remote_parse(cty))
                {/*调用不成功(是最后一包，或者出错)*/
                    /*-显示提示符和命令行-*/
                    oam_sty_put_prompt_cmdline(cty);
                    cty->line_len = strlen(cty->line_buf);
                }

            }
            else
#endif
            {
                cty->disp_line = 1;        /*-计新页显示的行数-*/
                /*-显示提示符和命令行-*/
                oam_sty_put_prompt_cmdline(cty);
            }
        }
        else
        {
            oam_sty_puts(cty, more_page);       /*-未显示完显示提示信息-*/
        }
        return;
#if 0
        /*-按回车键只显示下一行-*/
    case '\r':
    case '\n':
        if ((p_node != NULL) && ((p_node->disp_info) != NULL) )
        {
            if(strcmp(p_node->disp_info,cty->prompt_line) != 0)
            {
                int iCnt = 0;
                CHAR *p = p_node->disp_info;
                /*根据是返回数据自己换行还是达到行宽递交显示
                来处理回车换行的显示*/
                WHILE (*p!= '\r' && *p!= '\n' && iCnt <= OAM_STY_OUTPUT_LINE_MAX)
                {
                    LOOP_CHECK();
                    p++;
                    iCnt++;
                }

                if(i > OAM_STY_OUTPUT_LINE_MAX)
                {
                    oam_sty_puts(cty, "\n\r");
                }
                else/* if(p != p_node->disp_info) ??这个判断无所谓吧，只是\r或\n也可以相同处理*//*不是只有 '\r''\n'*/
                {
                    oam_sty_puts(cty, p_node->disp_info);
                    if(*p == '\n')
                    {
                        oam_sty_puts(cty, "\r");
                    }
                    else
                    {
                        oam_sty_puts(cty, "\n");
                    }
                }
            }
            OAM_RETUB(p_node->disp_info);       /*-释放显示空间-*/
            release_node = p_node;
            p_node = p_node->p_next;
            OAM_RETUB(release_node);            /*-释放节点空间-*/
            cty->p_disp_head = p_node;
            if ((p_node) && (p_node->p_next == NULL))
            {
                pinfo = p_node->disp_info;
                /*-这种情况下,是半行信息,更正行计数,允许MP再发消息显示本行下半截-*/
                if ((oam_sty_strlen(pinfo) < OAM_STY_OUTPUT_LINE_MAX) && (pinfo[strlen(pinfo) - 1] != '\n'))
                {
                    strncpy(cty->szStrToShow, pinfo);
                    cty->ucToShowLabel = 1;
                    cty->disp_line = OAM_STY_PAGE_LINE - 1;
                }
            }

            if (p_node == NULL)        /*-显示完毕-*/
            {
#if INSTALL_MP_TELNET
                if (cty->flags & OAM_STY_FLAG_REMOTE_PARSE)
                {
                    if (req_more_data_to_remote_parse(cty))
                    {
                        cty->disp_line = 1;
                        /*-显示提示符和命令行-*/
                        oam_sty_put_prompt_cmdline(cty);
                    }
                }
                else
#endif
                {
                    cty->disp_line = 1;
                    /*-显示提示符和命令行-*/
                    oam_sty_put_prompt_cmdline(cty);
                }
            }
            else
            {
                oam_sty_puts(cty, more_page);   /*-提示按键-*/
            }
        }
        return;
#endif
        /*-其它键不再显示下面信息释放空间-*/
    default:
        if ( c == 27 )   /*-对方向键的处理,清空输入缓冲区(因为方向键输入是多个字符?)-*/
        {
            c = oam_sty_getch(cty);
            c = oam_sty_getch(cty);
        }
        WHILE (p_node != NULL)
        {
            LOOP_CHECK();
            if (p_node->disp_info)
            {
                oam_sty_puts(cty, p_node->disp_info);
                oam_sty_puts(cty, "\r");
                OAM_RETUB(p_node->disp_info);   /*-释放显示空间-*/
            }
            release_node = p_node;
            p_node = p_node->p_next;
            OAM_RETUB(release_node);            /*-释放节点空间-*/
        }
        cty->disp_line = 1;
        cty->p_disp_head = NULL;
        /*-取消显示-*/
#if INSTALL_MP_TELNET
        if (cty->flags & OAM_STY_FLAG_REMOTE_PARSE)/*-MP—TELNET-PARSE-*/
        {
            if (cease_command_to_remote_parse(cty))
            {
                oam_sty_put_prompt_cmdline(cty);
            }
        }
        else/*-LOCAL-TELNET-PARSE-*/
#endif
        {
            oam_sty_put_prompt_cmdline(cty);
        }

        return;
    }
}

/************************************************************************
* 函数名称： void oam_sty_close(sty *cty)
* 功能描述： 关闭sty，清除终端相关数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明： 无
* 修改日期   版本号  修改人      修改内容
************************************************************************/
void oam_sty_close(sty *cty)
{
    DWORD i = 0;

    if (!cty)
    {
        return;
    }

    if (cty->flags & OAM_STY_FLAG_EXECMODE)
    {
        cty->flags &= ~OAM_STY_FLAG_EXECMODE;
#if INSTALL_MP_TELNET
        cease_command_to_remote_parse(cty);
#endif
    }

    /*-释放显示空间-*/
    if (cty->p_disp_head)
    {
        DISPLAY_NODE *p_node = NULL, *release_node = NULL;
        p_node = cty->p_disp_head;
        WHILE (p_node != NULL)
        {
            LOOP_CHECK();
            if (p_node->disp_info)
            {
                OAM_RETUB(p_node->disp_info);
            }
            release_node = p_node;
            p_node = p_node->p_next;
            OAM_RETUB(release_node);
        }
        cty->p_disp_head = NULL;
    }

    /*释放历史记录空间*/
    for (i = 0; i < OAM_STY_HISTORY_SIZE; i++)
    {
        if (cty->history[i])
        {
            OAM_RETUB(cty->history[i]);
        }
    }

#if INSTALL_MP_TELNET
    if (cty->flags & OAM_STY_FLAG_REMOTE_PARSE)
    {
        ;
    }
#endif
}

/************************************************************************
* 函数名称：void oam_sty_put_prompt_line(sty *cty, CHAR *p, int nlen)
* 功能描述：拷贝字符串到sty 的系统提示符中
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期  版本号  修改人      修改内容
************************************************************************/
void oam_sty_put_prompt_line(sty *cty, CHAR *p, int nlen)
{
    if (nlen >= (OAM_STY_PROMPT_LEN - 1))
    {
        return;
    }

    if (nlen)
    {
        memset(cty->prompt_line, 0, sizeof(cty->prompt_line));
        memcpy(cty->prompt_line, p, nlen);
    }
	
    XOS_SysLog(OAM_CFG_LOG_DEBUG, "611002030455 telnet recv MSGID_CHANGEPROMPT: [%s], nlen=%d\n", cty->prompt_line, nlen);
    cty->prompt_line[nlen] = '\000';
}
/************************************************************************
* 函数名称：    void oam_sty_put_command_line(sty *cty, CHAR *p, int nlen)
* 功能描述：拷贝字符串到sty的line_buf中
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
************************************************************************/
void oam_sty_put_command_line(sty *cty, CHAR *p, WORD16 nlen)
{
    if(nlen >= sizeof(cty->line_buf) - 1)
    {
        return;
    }

    if(nlen)
    {
        memset(cty->line_buf, 0, sizeof(cty->line_buf));
        memcpy(cty->line_buf, p, nlen);
    }

    cty->line_buf[nlen] = '\000';
    cty->edit_pos = nlen;
    cty->line_len = nlen;
}

/************************************************************************
* 函数名称：      void oam_sty_input_password(sty *cty, int flag)
* 功能描述：标识当前终端准备输入密码
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
************************************************************************/
void oam_sty_input_password(sty *cty, int flag)
{
    if (flag)
    {
        cty->flags |= OAM_STY_FLAG_PASSWORD;
    }
    else
    {
        cty->flags &= ~OAM_STY_FLAG_PASSWORD;
    }
}

/*在同一行显示新信息*/
void oam_sty_roll_progress(sty *cty, CHAR *text)
{
    DWORD i = 0;
    static DWORD last_roll_len = 0;

    for (i = 0; i < last_roll_len; i++)
    {
        oam_sty_puts(cty, CUT_CHAR);
    }

    last_roll_len = strlen(text);
    oam_sty_puts(cty, text);
}

/*清除当前输出的命令行，只是清除显示，line_buf中数据没变*/
static void oam_sty_eraseline(sty *cty)
{
    DWORD i = 0, k = 0;

    if (!(cty->flags & OAM_STY_FLAG_ECHOING))
    {
        return;
    }

    /*先输出空格清除原有输入，然后把光标移到行首*/
    for (i = 0, k = 0; i < cty->line_max; i ++, k ++)
    {
        sty_tmpbuf[k] = MARK_BLANK;
    }
    for ( ; i > 0; i --, k ++)
    {
        sty_tmpbuf[k] = MARK_LEFT;
    }
    sty_tmpbuf[k] = MARK_END;
    oam_sty_puts(cty, sty_tmpbuf);
}

/*调用历史输入时候，把历史记录写入当前命令行缓存*/
static void oam_sty_yank(register sty *cty)
{
    DWORD i = 0;
    if (cty->history[cty->history_ptr] == 0)
    {
        return;
    }

    /*编辑位置移到行首*/
#if OAM_STY_SCROLL_LINE
    if (cty->line_len > cty->line_max)
    {
        oam_sty_movehead(cty);
        cty->line_len = 0;
    }
    else
#endif
    {
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            if (cty->edit_pos != cty->line_len )
            {
                for (i = cty->edit_pos; i < cty->line_len; i ++)
                {
                    oam_sty_putc(cty, cty->line_buf[i]);
                }
            }
            for ( ; cty->line_len > 0; cty->line_len --)
            {
                oam_sty_puts(cty, CUT_CHAR);
            }
        }
    }

    /*读取历史命令*/
    WHILE ((cty->line_buf[cty->line_len] = cty->history[cty->history_ptr][cty->line_len]) != '\0')
    {
        LOOP_CHECK();
        cty->line_len++;
    }
    cty->edit_pos = cty->line_len;

    /*计算命令行显示开始位置*/
#if OAM_STY_SCROLL_LINE
    if (cty->line_len > cty->line_max)
    {
        cty->disp_pos = cty->line_len - cty->line_max;
    }
    else
    {
        cty->disp_pos = 0;
    }

    /*输出新命令行*/
    oam_sty_puts_cmdline(cty);
#else
    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        oam_sty_puts(cty, (CHAR *) cty->line_buf);
    }
#endif
}

/*-删除左边一个字符-*/
static void oam_sty_cutleft(sty *cty)
{
    if (cty->edit_pos <= 0 )
    {
        return;
    }
#if OAM_STY_SCROLL_LINE
    if ((cty->line_len == cty->edit_pos) && (cty->disp_pos == 0))
    {/*如果是行尾，直接删除*/
        if (cty->line_len)

        {
            cty->line_len --;
        }
        oam_sty_puts(cty, CUT_CHAR);
        cty->edit_pos --;
    }
    else
    {/*不是行尾，指定删除位置右面所有字符前移一位，然后重新显示命令行*/
        DWORD i = 0;
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
        cty->edit_pos --;
        cty->line_len --;
        for (i = cty->edit_pos; i < cty->line_len; i ++)
        {
            cty->line_buf[i] = cty->line_buf[i + 1];
        }
        oam_sty_movehead(cty);
        if ((cty->disp_pos) && (cty->edit_pos  + 1 == cty->disp_pos + OAM_STY_SEPARATE))
        {
            if (cty->disp_pos > OAM_STY_JUMP_LEN)
            {
                cty->disp_pos -= OAM_STY_JUMP_LEN;
            }
            else
            {
                cty->disp_pos = 0;
            }
        }
        oam_sty_puts_cmdline(cty);
        oam_sty_goedit(cty);
    }
#else
    if (cty->line_len != cty->edit_pos )
    {
        cty->edit_pos --;
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            oam_sty_putc(sty, MARK_LEFT);
        }
        oam_sty_cutright(sty);
        return;
    }
    if (cty->line_len > 0 )
    {
        cty->line_len --;
        cty->edit_pos --;
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            oam_sty_puts(sty, CUT_CHAR);
        }
    }
#endif
}

/*-删除到行首-*/
static void oam_sty_cuthead(sty *cty)
{
    DWORD i = 0;

    if (cty->edit_pos <= 0)
    {
        return;
    }
    /*编辑位置后所有字符前移n 位*/
    for (i = 0; i < (DWORD)(cty->line_len - cty->edit_pos); i ++)
    {
        cty->line_buf[i] = cty->line_buf[cty->edit_pos + i];
    }

    /*重新显示命令行*/
#if OAM_STY_SCROLL_LINE

    oam_sty_movehead(cty);
    oam_sty_eraseline(cty);
    cty->disp_pos = 0;
    cty->line_len -= cty->edit_pos;
    cty->edit_pos = 0;
    oam_sty_puts_cmdline(cty);
    oam_sty_goedit(cty);

#else

    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        for (i = 0; i < cty->edit_pos; i ++)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
        for (i = 0; i < (cty->line_len) - (cty->edit_pos); i ++)
        {
            oam_sty_putc(sty, cty->line_buf[i]);
        }
        for (i = 0; i < cty->edit_pos; i ++)
        {
            oam_sty_putc(cty, ' ');
        }
        for (i = 0; i < cty->line_len; i ++)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
    }
    cty->line_len -= cty->edit_pos;
    cty->edit_pos = 0;
#endif

    return;
}

/*-删除到行尾-*/
static void oam_sty_cuttail(sty *cty)
{
    DWORD i = 0;

    if (cty->edit_pos == cty->line_len)
    {
        return;
    }

#if OAM_STY_SCROLL_LINE

    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        /*编辑位置开始到最后的字符全部清除*/
        for (i = cty->edit_pos; (i < cty->line_len) && (i < cty->disp_pos + cty->line_max); i ++)
        {
            oam_sty_putc(cty, ' ');
        }
        /*光标移回编辑位置*/
        for (i = cty->edit_pos; (i < cty->line_len) && (i < cty->disp_pos + cty->line_max); i ++)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
    }
    /*按照新的命令行长度，重新计算显示开始位置并显示命令行*/
    cty->line_len = cty->edit_pos;
    if (cty->disp_pos)
    {
        oam_sty_movehead(cty);
        if (cty->line_len > cty->line_max)
        {
            cty->disp_pos = cty->line_len - cty->line_max;
        }
        else
        {
            cty->disp_pos = 0;
        }
        oam_sty_puts_cmdline(cty);
    }

#else

    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        for (i = cty->edit_pos; i < cty->line_len; i ++)
        {
            oam_sty_putc(cty, ' ');
        }
        for (i = cty->edit_pos; i < cty->line_len; i ++)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
    }
    cty->line_len = cty->edit_pos;
#endif

    return;
}

/*-删除一整行，不同于cutline函数，会清除line_buf-*/
static void oam_sty_cutline(sty *cty)
{
    if (!cty->line_len)
    {
        return;
    }

#if OAM_STY_SCROLL_LINE
    /*先输出空格清除原有输入，然后把光标移到行首*/
    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        DWORD i = 0, k = 0;

        for (i = cty->disp_pos, k = 0; i < cty->edit_pos; i ++, k ++)
        {
            sty_tmpbuf[k] = MARK_LEFT;
        }
        for (i = 0; i < cty->line_max; i ++, k ++)
        {
            sty_tmpbuf[k] = MARK_BLANK;
        }
        for ( ; i > 0; i --, k ++)
        {
            sty_tmpbuf[k] = MARK_LEFT;
        }
        sty_tmpbuf[k] = MARK_END;

        oam_sty_puts(cty, sty_tmpbuf);
    }
    cty->disp_pos = 0;

#else
    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        if (cty->edit_pos != cty->line_len )
        {
            for (; cty->edit_pos < cty->line_len; cty->edit_pos ++)
            {
                oam_sty_putc(cty, cty->line_buf[cty->edit_pos]);
            }
        }
        WHILE (cty->line_len-- > 0)
        {
            LOOP_CHECK();
            oam_sty_puts(cty, CUT_CHAR);
        }
    }

#endif
    /*清除line_buf*/
    cty->line_len = 0;
    cty->edit_pos = 0;

    return;
}

/*-删除右边一个字符-*/
static void oam_sty_cutright(sty *cty)
{
    DWORD i = 0;

    if ( cty->edit_pos >= cty->line_len )
    {
        return;
    }

    cty->line_len --;

    /*编辑位置开始到行尾的字符都前移一位，然后重新显示命令行*/
#if OAM_STY_SCROLL_LINE
    for (i = cty->edit_pos; i < cty->line_len; i ++)
    {
        cty->line_buf[i] = cty->line_buf[i + 1];
    }

    oam_sty_movehead(cty);
    oam_sty_puts_cmdline(cty);
    oam_sty_goedit(cty);
#else
    for (i = cty->edit_pos; i < cty->line_len; i ++)
    {
        cty->line_buf[i] = cty->line_buf[i + 1]; /*-改变内容并显示-*/
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            /* 防止密码暴露 */
            if (cty->flags & OAM_STY_FLAG_PASSWORD)
            {
                oam_sty_putc(cty, MARK_PSWD);
            }
            else
            {
                oam_sty_putc(cty, cty->line_buf[i]);
            }
        }
    }
    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        oam_sty_putc(cty, ' '); /*-删除最后一个字符-*/
        for (i = cty->line_len; i >= cty->edit_pos; i --) /*-把光标移到原来的位置-*/
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
    }
#endif

    return;
}
/************************************************************************
* 函数名称：static void send_to_remote_parse(sty *sty, CHAR *line, remote_parse_cookie *cookie)
* 功能描述：向解释进程发送请求消息
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
************************************************************************/
static void send_to_remote_parse(sty *sty, CHAR *line, remote_parse_cookie *cookie)
{
    static BYTE buffer[MAX_SEND_STRING_LEN + MSG_HDR_SIZE_OUT] = {0};
    TYPE_TELNET_MSGHEAD  *data_ptr = NULL;
    WORD16 data_len = 0;
    WORD16 str_len = 0;
    remote_parse_cookie *rpc = NULL;

    data_ptr = (TYPE_TELNET_MSGHEAD *)(buffer);
    rpc = (remote_parse_cookie *)cookie;

    if(!rpc)
    {
        return;
    }

    /*从命令行会话中获取消息头信息*/
    data_ptr->bMsgID = rpc->msg_id;
    data_ptr->bVtyNum = rpc->vty_num;
    data_ptr->bLastPacket = rpc->last_pkt_flag;
    data_ptr->bDeterminer = rpc->determiner;
    data_ptr->bSSHAuth = sty->sshauthmsg;
    if(0 == rpc->seq_no)
    {
        rpc->seq_no++;
    }

    data_ptr->wSeqNo = rpc->seq_no;
    data_ptr->wDataLen = rpc->data_len;

    /*获取命令字符串*/
    if(line)
    {
        if(data_ptr->bMsgID != MSGID_ASKTELACK)
        {
            if(strlen(line) > 0)
            {
                str_len = MAX_SEND_STRING_LEN >strlen(line) ? strlen(line) : MAX_SEND_STRING_LEN;
                memcpy(&data_ptr->pData, line, str_len);
                data_len = sizeof(TYPE_TELNET_MSGHEAD) + str_len - sizeof(BYTE);
            }
            else
            {
                data_len = sizeof(TYPE_TELNET_MSGHEAD);
            }
        }
        else
        {
            str_len = MAX_SEND_STRING_LEN > data_ptr->wDataLen ? data_ptr->wDataLen : MAX_SEND_STRING_LEN;
            memcpy(&data_ptr->pData, line, str_len);
            data_len = sizeof(TYPE_TELNET_MSGHEAD) + str_len - sizeof(BYTE);
        }
    }
    else
    {
        data_len = sizeof(TYPE_TELNET_MSGHEAD);
    }

    XOS_SysLog(OAM_CFG_LOG_DEBUG, "telnetserver: send to remote parse, inter jno is %d\n", sty->interpJid.dwJno);

    /*send message to interpret instance*/
    XOS_SendAsynMsg(OAM_MSG_TELNET_TO_INTERPRET, (BYTE *)buffer, (WORD16)data_len,
                    XOS_MSG_VER0, XOS_MSG_MEDIUM, &(sty->interpJid));

    /*?? 命令发送后进入执行模式*/
    sty->flags |= OAM_STY_FLAG_EXECMODE;
    /* 根据是否有OAM_STY_FLAG_EXEINPUT决定是否允许输入，不太明白*/
    oam_sty_enable_input(sty, (sty->flags & OAM_STY_FLAG_EXEINPUT));

    /*如果不是terminate命令需要重置定时器*/
    if(data_ptr->bMsgID != MSGID_TERMINATE)
    {
        SWORD32 sdwTimeLen = MAX_WAIT_ACK_LEN;
        if (MSGID_NEWCONNECT == data_ptr->bMsgID)
        {
            sdwTimeLen = MIN_WAIT_ACK_LEN;
        }
        else
        {
            sdwTimeLen = MAX_WAIT_ACK_LEN;
        }
        
        if(INVALID_TIMER_ID != rpc->dwTimerNo)
        {
            XOS_KillTimerByTimerId(rpc->dwTimerNo);
            rpc->dwTimerNo = INVALID_TIMER_ID;
        }
        rpc->dwTimerNo = XOS_SetRelativeTimer(PROCTYPE_OAM_TELNETSERVER, TIMER_WAIT_INTERPRET_ACK, sdwTimeLen, (WORD32)sty->sty_id);
    }
}

/************************************************************************
* 函数名称：     int req_more_data_to_remote_parse(sty *cty)
* 功能描述：    大包模式下向解释程序请求下一包数据
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
************************************************************************/
int req_more_data_to_remote_parse(sty *cty)
{
    remote_parse_cookie *rpc = NULL;
    if (!cty)
    {
        return 1;
    }
    rpc = (remote_parse_cookie *) cty->link_cookie;
    if (!rpc)
    {
        return 1;
    }
#if 0
    if (rpc->last_pkt_flag == MSG_LAST_PKT)
    {
        return 1;
    }
#endif
    /* Fill the msg header, ignore some fields which we do not care */
    rpc->msg_id = MSGID_REQUESTDATA;
    rpc->last_pkt_flag = MSG_LAST_PKT;
    rpc->data_len = 0;

    send_to_remote_parse(cty, 0, cty->link_cookie);

    return 0;
}

/*通知解释程序关闭终端*/
void terminate_conn_to_remote_parse(sty *cty)
{
    remote_parse_cookie *rpc = NULL;

    if (!cty)
    {
        return;
    }

    rpc = cty->link_cookie;
    if (!rpc)
    {
        return;
    }

    /* Fill the msg header, ignore some fields which we do not care */
    rpc->msg_id = MSGID_TERMINATE;
    rpc->last_pkt_flag = MSG_LAST_PKT;
    rpc->data_len = 0;

    send_to_remote_parse(cty, 0, cty->link_cookie);
}

/*整行返回0，半行返回最后一行字符串长度*/
WORD16 oam_sty_puts_with_line_process(sty *cty, CHAR *text)
{
    WORD16 wStrlen = 0;
    CHAR aucLineBuf[MAX_RESULT_DATA_LEN] = {0};
    CHAR aucTextIn[MAX_RESULT_DATA_LEN + 1] = {0};
    WORD16 str_len = 0;
    CHAR *p;
    WORD16 wRet = 0;

    TELNETSVR_0_ASSERT(cty != NULL && text != NULL);

    wStrlen =  MAX_RESULT_DATA_LEN  >  strlen(text) ? strlen(text) : MAX_RESULT_DATA_LEN;
    memcpy(aucTextIn, text, wStrlen);
    p = aucTextIn;

    WHILE (*p != 0)
    {
        /*611001924617 输出不检查循环次数*/
        /*LOOP_CHECK();*/
        aucLineBuf[str_len] = *p;
        
        if(*p == MARK_LEFT && cty->fill_len>0)
        {
            cty->fill_len = (cty->fill_len  > 0) ? (cty->fill_len--) : cty->fill_len ;
        }
        else
        {
            if (char_is_chinese(*p))
            {/*中文字符，连续读两个字节*/
                if (cty->fill_len == OAM_STY_LINE_MAX)
                {/*如果中文字符读一半就达到行宽，回退已经读取的一半到下行读*/
                    /*已经读取的放到下行读*/
                    p--;
                    aucLineBuf[str_len] = '\0';
                }
                else
                {/*如果还没有达到行宽，就接着读取下一个字节，保证中文字符没有被分开*/
                    p++;
                    str_len++;
                    aucLineBuf[str_len] = *p;
                    if(*p == MARK_LEFT)
                        cty->fill_len = (cty->fill_len  > 0) ? (cty->fill_len--) : cty->fill_len ;
                    else
                        cty->fill_len ++;
                }
            }
            cty->fill_len ++;
        }
        str_len ++;
        
        /*-遇换行符或满行时加一行-*/
        if ((*p == '\n') || 
             (*p == '\r') || 
             ((cty->fill_len >= OAM_STY_LINE_MAX) && (*(p+1) != '\n') && (*(p+1) != '\r')))
        {
            aucLineBuf[str_len] = MARK_END;
            oam_sty_puts(cty, aucLineBuf);  /*-递交显示-*/

            /*\n、\r、\r\n、\n\r 都认为是一个回车*/
            if(*p == '\n')
            {
                if (!cty->sshauthmsg)
                    oam_sty_putc(cty, (int)'\r');
                if (*(p + 1) == '\r')
                {
                    p++;
                }
            }
            else if(*p == '\r')
            {
                if (!cty->sshauthmsg)
                oam_sty_putc(cty, (int)'\n');
                if (*(p + 1) == '\n')
                {
                    p++;
                }
            }
            else if (cty->fill_len >= OAM_STY_LINE_MAX)
            {               
                if (!cty->sshauthmsg)
                {
                    /*如果满行后面刚好是回车换行，不再添加，否则添加换行*/
                    if ((*(p + 1) != '\n') && (*(p + 1) != '\r'))
                    {
                        oam_sty_puts(cty, "\n\r");
                    }
                 }
            }
            str_len = 0;
            cty->fill_len = 0; 
            memset(aucLineBuf, 0, sizeof(aucLineBuf));
        }

        p ++;
    }

    /*处理半行的情况*/
    if((aucTextIn[wStrlen - 1] == '\n') || (aucTextIn[wStrlen - 1] == '\r'))
    {
        wRet= 0 ;
    }
    else
    {
        /*-递交显示-*/
        oam_sty_puts(cty, aucLineBuf);  
        wRet = str_len;
    }

    return wRet;
}


/*发送新建连接消息给解释*/
void make_conn_to_remote_parse(sty *vty)
{
    remote_parse_cookie *rpc = NULL;
    CHAR str_temp[LEN_IP_ADDR] = {0};

    if (!vty)
    {
        return;
    }

    rpc = (remote_parse_cookie *)vty->link_cookie;
    if (!rpc)
    {
        return;
    }

    /* Fill the msg header, ignore some fields which we do not care */
    rpc->msg_id = MSGID_NEWCONNECT;
    rpc->last_pkt_flag = MSG_LAST_PKT;

    memset(str_temp, 0, sizeof(str_temp));
    memcpy(str_temp, vty->tSocket->szIpAddr, strlen(vty->tSocket->szIpAddr));
    rpc->data_len = strlen(str_temp);

    send_to_remote_parse(vty, str_temp, vty->link_cookie);
}

/*读取终端输入*/
void ReadTelnet(TYPE_FD *tSocket, CHAR *buffer, WORD16 nRecvLen)
{
    DWORD nLoop = 0;
    BYTE cTmp = 0;
    sty *vty = NULL;

   // XOS_SysLog(OAM_CFG_LOG_DEBUG, "telnetserver: ReadTelnet called\n");

    if (!(vty = FindSty(tSocket)))
    {
        return;
    }

    vty->sshauthmsg = FALSE;
    
    /*judge the flag 看是否接收处理终端输入*/
    if((vty->flags & OAM_STY_FLAG_EXECMODE) && !(vty->flags & OAM_STY_FLAG_EXEINPUT))
    {
        vty->flags |= OAM_STY_FLAG_STOPINPUT;
        cTmp = buffer[0];
        if((cTmp == 3) && (nRecvLen == 1))/*Ctrl+C*/
        {
            vty->flags &= ~OAM_STY_FLAG_STOPINPUT;
        }
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "telnetserver: can not input now, vty->flags is %d\n", vty->flags);
    }
    else
    {
        vty->flags &= ~OAM_STY_FLAG_STOPINPUT;
    }

    if (vty->flags & OAM_STY_FLAG_STOPINPUT)
    {
        return;
    }

    cTmp = buffer[0];

    /*如果当前命令不支持中止*/
    if(((cTmp == 3) && (nRecvLen == 1)) && 
        (vty->bCeaseAttr == 2) &&
        ((vty->flags & OAM_STY_FLAG_EXECMODE) || (vty->p_disp_head)))/*Ctrl+C*/
    {
        oam_sty_puts(vty, "\n\r%Note: The current command doesn't support [ctrl+c] or [anykey] operation!\n\r");
        return;
    }

    /*把输入读到终端的缓存*/
    for (nLoop = 0; nLoop <  nRecvLen && !oam_sty_ring_full(vty); nLoop++)
    {
        cTmp = buffer[nLoop];
        vty->ring_buf[vty->ring_end++] = cTmp;
        if (vty->ring_end >= sizeof(vty->ring_buf))
        {
            vty->ring_end = 0;
        }
    }
    oam_sty_read_loop(vty);
}

/*通知解释程序取消正在执行的命令*/
int cease_command_to_remote_parse(sty *cty)
{
    remote_parse_cookie *rpc = NULL;

    if (!cty)
    {
        return 1;
    }

    rpc = (remote_parse_cookie *) cty->link_cookie;
    if (!rpc)
    {
        return 1;
    }
#if 0
    if (rpc->last_pkt_flag == MSG_LAST_PKT)
    {
        return 1;
    }
#endif
    /* Fill the msg header, ignore some fields which we do not care */
    rpc->msg_id = MSGID_CEASEDATA;
    rpc->last_pkt_flag = MSG_LAST_PKT;
    rpc->data_len = 0;

    send_to_remote_parse(cty, 0, cty->link_cookie);

    return 0;
}

/*发送一个字符串给解释程序*/
void send_str_to_remote_parse(sty *vty, CHAR *line, BYTE determiner)
{
    remote_parse_cookie *rpc = NULL;
    if (!vty)
    {
        return;
    }

    rpc = (remote_parse_cookie *)vty->link_cookie;
    if (!rpc)
    {
        return;
    }

    rpc->msg_id = MSGID_SENDSTRING;
    rpc->last_pkt_flag = MSG_LAST_PKT;
    rpc->determiner = determiner;
    if (line)
    {
        rpc->data_len = strlen(line);
    }
    else
    {
        rpc->data_len = 0;
    }

    /*611001634156 【ETCA】【对齐检查】show atmpvc\atmoamlock显示结果没有对齐*/
    vty->fill_len= 0;
	
    send_to_remote_parse(vty, line, vty->link_cookie);
}

/*向解释器请求命令行出错的详细信息*/
void send_data_to_remote_parse(sty *cty, CHAR *data, BYTE datalen)
{
    remote_parse_cookie *rpc = NULL;
    BYTE determiner = 0;
    if (!cty)
    {
        return;
    }

    rpc = cty->link_cookie;
    if (!rpc)
    {
        return;
    }
    determiner = DM_NORMAL;

    rpc->msg_id = MSGID_ASKTELACK;
    rpc->last_pkt_flag = MSG_LAST_PKT;
    rpc->determiner = determiner;
    rpc->data_len = datalen;
    send_to_remote_parse(cty, data, cty->link_cookie);
}


void ProcessSSHAuthEvent(TYPE_FD *tSocket, CHAR *buf, int iLen)
{
    sty *vty = NULL;
    remote_parse_cookie *rpc = NULL;

    if (!(vty = FindSty(tSocket)))
    {
        return;
    }

    rpc = (remote_parse_cookie *)vty->link_cookie;
    if (!rpc)
    {
        return;
    }

    rpc->msg_id = MSGID_SENDSTRING;
    rpc->last_pkt_flag = MSG_LAST_PKT;    
    rpc->determiner = DM_NORMAL;
    if (buf)
    {
        rpc->data_len = strlen(buf);
    }
    else
    {
        rpc->data_len = 0;
    }
    
    vty->sshauthmsg = TRUE;
    send_to_remote_parse(vty, buf, vty->link_cookie);

}


void oam_send_to_ssh(sty *cty, CHAR *text)
{
    WORD data_len = 0;
    
    /* 发送给SSH Server */
    if (cty->sshauthmsg)
    {
        OAM_SSH_TEL_MSG *pAuthMsg = NULL;
        BYTE buffer[MAX_BUF_LEN] = {0};

        pAuthMsg = (OAM_SSH_TEL_MSG *)(buffer);
        pAuthMsg->fd = cty->tSocket->ucFd;
        if (cty->sshauthrslt)
            pAuthMsg->result = 1;
        else
            pAuthMsg->result = 0;
        pAuthMsg->len = strlen(text);
        data_len = sizeof(OAM_SSH_TEL_MSG);
        memcpy(pAuthMsg->buf, text, strlen(text));
        XOS_SendAsynMsg(OAM_MSG_SSH_AUTH_ACK, (BYTE *)buffer, (WORD16)data_len,
                            XOS_MSG_VER0, XOS_MSG_MEDIUM, &(cty->sshsvrJid));
    }
    else
    {
        OAM_FD_MSG_EX *pCfgMsg = NULL;
        BYTE *pPacket = (BYTE *)buffer_send_to_ssh;

        memset(buffer_send_to_ssh, 0, MAX_RESULT_TO_SSH_LEN);
        pCfgMsg = (OAM_FD_MSG_EX *)(pPacket);
        pCfgMsg->fd = cty->tSocket->ucFd;
        pCfgMsg->flag = CONNECTION_FROM_SSH;
        pCfgMsg->len = strlen(text)+1;
        data_len = sizeof(OAM_FD_MSG_EX) + strlen(text);
        memcpy(pPacket + sizeof(WORD16) + sizeof(WORD16) + sizeof(WORD16), 
               text,
               strlen(text));
        
        XOS_SendAsynMsg(OAM_MSG_TELNET_TO_SSH, (BYTE *)buffer_send_to_ssh, (WORD16)data_len,
                        XOS_MSG_VER0, XOS_MSG_MEDIUM, &(cty->sshsvrJid));
    }
}

void send_close_ssh(sty *vty)
{
    OAM_FD_MSG *pAuthMsg = NULL;
    BYTE buffer[MAX_BUF_LEN] = {0};
    WORD data_len = 0;

    pAuthMsg = (OAM_FD_MSG *)(buffer);
    pAuthMsg->fd = vty->tSocket->ucFd;
    pAuthMsg->flag = CONNECTION_FROM_SSH;
    pAuthMsg->len = 0;
    data_len = sizeof(OAM_FD_MSG);
    XOS_SendAsynMsg(MSG_FD_CLOSE, (BYTE *)buffer, (WORD16)data_len,
                    XOS_MSG_VER0, XOS_MSG_MEDIUM, &(vty->sshsvrJid));
    
}

void oam_ssh_conn_ack(sty *cty, BYTE* pMsgPara)
{
    OAM_SSH_TEL_MSG *pConnAck = NULL;
    BYTE *pPacket = (BYTE *)buffer_send_to_ssh;
    WORD16 wMsgLen = 0;
    WORD16 wLen = 0;
    TYPE_INTERPRET_MSGHEAD *pMsgHead = (TYPE_INTERPRET_MSGHEAD *)pMsgPara;
    CHAR szConnStr[512];

    memset(buffer_send_to_ssh, 0, MAX_RESULT_TO_SSH_LEN);
    pConnAck = (OAM_SSH_TEL_MSG *)(pPacket);
    pConnAck->fd = cty->tSocket->ucFd;
    if (pMsgHead->bSSHConnRslt)
        pConnAck->result = 1;
    else
        pConnAck->result = 0;

    pMsgPara += MSG_HDR_SIZE_IN;

    wLen = *(WORD16 *)(pMsgPara + 1);

    memset(szConnStr, 0, sizeof(szConnStr));
    memcpy(szConnStr, (CHAR *)(pMsgPara + 3), wLen);
    szConnStr[wLen] = '\0';
    pConnAck->len = strlen(szConnStr)+1;

    wMsgLen = sizeof(OAM_SSH_TEL_MSG) + strlen(szConnStr);
    memcpy(pPacket + sizeof(WORD16) + sizeof(WORD16) + sizeof(WORD16), 
               szConnStr,
               strlen(szConnStr));
    
    XOS_SendAsynMsg(OAM_MSG_SSH_CONN_ACK, (BYTE *)buffer_send_to_ssh, (WORD16)wMsgLen,
                    XOS_MSG_VER0, XOS_MSG_MEDIUM, &(cty->sshsvrJid));

    if (!pMsgHead->bSSHConnRslt)
    {
        oam_close_socket_for_ssh(cty->tSocket);
    }
}

void oam_fd_send_for_ssh(WORD16 fd, CHAR *text)
{
    OAM_FD_MSG_EX *pCfgMsg = NULL;
    BYTE *pPacket = (BYTE *)buffer_send_to_ssh;
    WORD data_len = 0;

    memset(buffer_send_to_ssh, 0, MAX_RESULT_TO_SSH_LEN);
    pCfgMsg = (OAM_FD_MSG_EX *)(pPacket);
    pCfgMsg->fd = fd;
    pCfgMsg->flag = CONNECTION_FROM_SSH;
    pCfgMsg->len = strlen(text)+1;
    data_len = sizeof(OAM_FD_MSG_EX) + strlen(text);
    memcpy(pPacket + sizeof(WORD16) + sizeof(WORD16) + sizeof(WORD16), 
           text,
           strlen(text));
    
    XOS_SendAsynMsg(OAM_MSG_TELNET_TO_SSH, (BYTE *)buffer_send_to_ssh, (WORD16)data_len,
                    XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_tSSHServerJid);
}

void oam_fd_quit_for_ssh(WORD16 fd)
{
    OAM_FD_MSG *pAuthMsg = NULL;
    BYTE buffer[MAX_BUF_LEN] = {0};
    WORD data_len = 0;

    pAuthMsg = (OAM_FD_MSG *)(buffer);
    pAuthMsg->fd = fd;
    pAuthMsg->flag = CONNECTION_FROM_SSH;
    pAuthMsg->len = 0;
    data_len = sizeof(OAM_FD_MSG);
    XOS_SendAsynMsg(MSG_FD_CLOSE, (BYTE *)buffer, (WORD16)data_len,
                    XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_tSSHServerJid);
    
}


void Oam_CfgClearSendBuf(sty *ptSty)
{
    T_StySendBuf *pBuf = NULL;

    if (!ptSty)
    {
        return;
    }

    pBuf = ptSty->ptSendBufHead;
    if (!pBuf)
    {
        return;
    }

    while (pBuf != NULL)
    {
        T_StySendBuf *pTmpBuf = pBuf;

        pBuf = pBuf->pNext;
       
        OAM_RETUB(pTmpBuf->buf);
        OAM_RETUB(pTmpBuf);
    }
    ptSty->ptSendBufHead = NULL;
    ptSty->ptSendBufTail   = NULL;
}


