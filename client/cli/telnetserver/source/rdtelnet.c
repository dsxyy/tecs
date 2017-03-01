/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� RDTELNET.CPP
* �ļ���ʶ�� 
* ����ժҪ�� 
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ�
* ������ڣ� 
**********************************************************************/

/***********************************************************
 *                      ͷ�ļ�                             *
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
 *                     ȫ�ֱ���                            *
***********************************************************/
WORD16 g_wWhileLoopCnt = 0;
extern JID g_tSSHServerJid;
static CHAR buffer_send_to_ssh[MAX_RESULT_TO_SSH_LEN];
/***********************************************************
 *                     ���� ����                            *
***********************************************************/
static CHAR more_page[] = "--------MORE PAGE--------";  /*-��ҳʱ��ʾ����ʾ��Ϣ-*/
static CHAR sty_tmpbuf[OAM_STY_LINE_MAX*3] = {0};     /*-�ṩ���������ʱ����-*/
/***********************************************************
 *                     ��������                            *
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

/*������Ϣ�����ͽ���*/
static void send_to_remote_parse(sty *sty, CHAR *line, remote_parse_cookie *cookie);
/*����������صĺ���*/
static int cease_command_to_remote_parse(sty *vty);
static void send_str_to_remote_parse(sty *vty, CHAR *line, BYTE determiner);

static BOOLEAN char_is_chinese(BYTE ucIn);

static void oam_send_to_ssh(sty *cty, CHAR *text);
/***********************************************************
 *                     �궨��                             *
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


/*��sty�����ж�ȡ��ǰ��һ���ַ�*/
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

/*~��ʱ��ʹ��*/
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
/*��ӡһ���ַ����ն�*/
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
/*��ӡһ���ַ������ն�*/
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
/*����һ���ַ���sty�����뻺��*/
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
        /*���ݵ�ǰ�༭λ���ж��������׷�ӻ��ǲ���༭λ��*/
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
        /*������ʾ��ǰ��*/
        if (cty->line_len > cty->line_max)
        {
            oam_sty_movehead(cty);
            /*���������м�༭ʱ��,������볬���п�,�����ַ�������*/
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
            /*??��θо��Ƕ����,��β���м���������ͬ����
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
                /*�ӱ༭λ�ÿ�ʼ,������ʾ������ַ�*/
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
                /*��ʾ���,����ƻر༭λ��*/
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

/*�ѹ��ӱ༭λ���Ƶ���ʾλ�ÿ�ʼ��*/
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
* �������ƣ� static void oam_sty_puts_cmdline(sty *cty)
* ���������� ����ն˻����е���������Ϣ
* ���������
* ���������
* �� �� ֵ��
* ����˵������ֱ�������ȣ��ú���֧�ֳ����п������
* �޸�����      �汾��  �޸���      �޸�����
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
    /*�����ʾλ�ò��Ǵ�ͷ��ʼ�����ز����������� ���*/
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

    /*ѭ������ʾλ�ö�ȡ�ַ�*/
    for ( ; i < length; i ++, k ++)
    {
        /*�����ʾ���ȳ����п�ɾ��������һ���ַ�*/
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
            /* ��ֹ���뱩¶ */
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

    /*����ַ����ﵽ�п�û�н������������.. ����*/
    if (i == length)
    {
        for ( ; i < cty->line_max; i ++, k ++)
        {/*���ѭ������Ӧ�þ���OAM_STY_HIDE_LEN*/
            sty_tmpbuf[k] = MARK_DOT;
        }
    }
    
    sty_tmpbuf[k] = MARK_END;
    oam_sty_puts(cty, sty_tmpbuf);
}

/*�ѹ�����β�Ƶ��༭����ͨ������ʾ�����к����*/
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

/*��������*/
static void oam_sty_goleft(sty *cty)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->edit_pos <= 0)
    {
        return;
    }

    /*������Ƶ�����ʾ��ʼλ�ã���ʾλ��ǰ��һ�Σ�����ֻ�ǹ������*/
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

/*����������*/
static void oam_sty_goright(sty *cty)
{
    TELNETSVR_VOID_ASSERT(cty != NULL);

    if (cty->edit_pos >= cty->line_len )
    {
        return;
    }

    /*������Ƶ����п�λ�ã���ʾλ�ú�����һ�Σ�����ֻ�ǹ������*/
#if OAM_STY_SCROLL_LINE
    if (((cty->disp_pos + cty->line_max - OAM_STY_SEPARATE) <= cty->edit_pos + 1) &&
            ((cty->edit_pos + 1 + OAM_STY_SEPARATE) < (cty->line_len)))
    {
        oam_sty_movehead(cty);
        cty->disp_pos += OAM_STY_JUMP_LEN;
        if ((cty->disp_pos + cty->line_max) > cty->line_len)
        {/*����ʱ�����һ����ʾ�п���ַ�*/
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
            /* ��ֹ���뱩¶ */
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

/*�༭λ���Ƶ���ǰ������������ǰ��*/
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
        /*??�������������ν��
        oam_sty_movehead(cty); */
        cty->disp_pos = 0;
        oam_sty_puts_cmdline(cty);
        cty->edit_pos = 0;
        oam_sty_goedit(cty);
    }
    else
#endif
    {/*??������������Ƿ���Թ�������ο��Բ�Ҫ������if �ľͿ���*/
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

/*-����Ƶ���β"ctrl-e"-*/
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

/*��ʶ��ǰ�ն���������*/
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
* �������ƣ�  void oam_sty_put_prompt_cmdline(sty *cty)
* �������������ϵͳ��ʾ��Ϣ+ ������
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
void oam_sty_put_prompt_cmdline(sty *cty)
{
    WORD16 wLen = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    /*611001779292 ���¼��㵱ǰ���������볤�ȣ���ֹǰ����ʾ
      �����޸Ĺ����ֵ */
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;        
    wLen = cty->line_len;
    /*���ϵͳ��ʾ��*/
    if(!sdp_puts(cty, cty->prompt_line))
    {/*�����ǰ�Ѿ���һҳ*/
        return;
    }
    cty->edit_pos = wLen;
    cty->line_len = wLen;

    /*611001897277 �����������ʱ�򣬻�����޷����� �� �鿴���������*/
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;  
    /*���������*/
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
    /*611001779292 ���¼��㵱ǰ���������볤�ȣ�sdp_puts�������ʾģʽ����ʾʱ�����ģʽ�п���
      �޸����ֵ */
    cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;        
}

/*����ַ������������л��ߴﵽ�п�ʱ������һ������
  ���û����ҳ����ʾ�����򱣴浽���������*/
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
        /*611001058985 ����ǿմ����鿴�Ƿ������һ��*/
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

    /*-�ر�����ʾֱ�ӷ���-*/
    if(cty->flags & OAM_STY_FLAG_NODISPLAY)
    {
        return ((cty->disp_line > OAM_STY_PAGE_LINE) ? 0 : 1);
    }

    str_len = 0;
    WHILE (*p != 0)
    {
        /*611001924617 ��������ѭ������*/
        /*LOOP_CHECK();*/
        tmptext[str_len] = *p;
        if(*p == MARK_LEFT)
        {
            cty->fill_len = (cty->fill_len  > 0) ? (cty->fill_len--) : cty->fill_len ;
        }
        else
        {
            if (char_is_chinese(*p))
            {/*�����ַ��������������ֽ�*/
                if (cty->fill_len == OAM_STY_OUTPUT_LINE_MAX)
                {/*��������ַ���һ��ʹﵽ�п������Ѿ���ȡ��һ�뵽���ж�*/
                    /*�Ѿ���ȡ�ķŵ����ж�*/
                    p--;
                    tmptext[str_len] = ' ';
                }
                else
                {/*�����û�дﵽ�п��ͽ��Ŷ�ȡ��һ���ֽڣ���֤�����ַ�û�б��ֿ�*/
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

        /*-���س���������ʱ��һ��-*/
        if ((*p == '\n') || (cty->fill_len >= OAM_STY_OUTPUT_LINE_MAX))
        {
            tmptext[str_len] = MARK_END;
            str_len = 0;
            cty->disp_line ++;
            cty->edit_pos = 0;
            cty->line_len = 0;
            cty->line_max = OAM_STY_OUTPUT_LINE_MAX - 1;

            oam_sty_putline(cty, tmptext);  /*-�ݽ���ʾ�򱣴�����-*/
            cty->fill_len = 0;
            
        }
        p ++;
    }
    /*-��һ����������ʾ��-*/
    /*-��MP��ʽ��,�ںܶೡ�ϴ��������ǰ�����Ϣ,���л�����ʾ���ַ�����fill_len�ó�-*/
    if((cty->fill_len == str_len) && (str_len))
    {
        cty->disp_line ++;
        cty->line_max = OAM_STY_OUTPUT_LINE_MAX - str_len - 1;        /*-��¼��һ�л�����ʾ���ַ���-*/
    }
    tmptext[str_len] = MARK_END;
    oam_sty_putline(cty, tmptext);

    /*-��ʾ����ʱ����0-*/
    if(cty->disp_line > OAM_STY_PAGE_LINE)
    {
        return 0;
    }

    return 1;
}

/*��Ҫ��ʾ������ÿ��һ���ڵ�ŵ���ʾ�������������ʾ*/
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
         (NULL == cty->p_disp_head))         /*-����һҳֱ�ӵݽ���ʾ-*/
    {
        WORD16 wLen = 0;
        WORD16 wPromtLen = 0;
        /*�����������*/
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

        /*?? Ӧ�ò������disp_head��Ϊ�յ�����ɣ�������Ϊ�˱������*/
        if (cty->p_disp_head)
        {
            OAM_RETUB(cty->p_disp_head->disp_info);
            OAM_RETUB(cty->p_disp_head);
            cty->p_disp_head = NULL;
        }

    }
    else /*-��������-*/
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
        /*-������һ����Ϣ-*/
        if (NULL == cty->p_disp_head)
        {/*��ʾ��������û�м�¼*/
            /*��ʱTEXT��"\n",����Ҫ�������������ʾ����*/
	     #if 0
            if(!strcmp(text, "\n"))
            {
                oam_sty_putc(cty,'\n');
            }
            #endif
            oam_sty_puts(cty, more_page);           /*-������ʾ-*/
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
                plastnode = plastnode->p_next;   /*-�ҵ����һ���ڵ�-*/
            }
            strlength = strlen(plastnode->disp_info);

            /*-��һ������Ϊ����ʱӦ�ð��������ݴ����һ��-*/
            /*2005.1.28 �޸ģ�֧��ÿ�г���80�ַ��Ļ��ԣ��޸��������
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

                /*�����ʣ�µ�,��ʣ�µķ���newnode*/
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

/*�����뻺���ж�ȡ���룬���з�������*/
static void oam_sty_read_loop(sty *vty)
{
    DWORD c = 0, i = 0;
    static BYTE ucPreCRLF = 0; /*��������������\n\r �ظ���������*/

    TELNETSVR_VOID_ASSERT(vty != NULL);

    if (vty->flags & OAM_STY_FLAG_STOPINPUT)
    {
        return;
    }

    /*��sty�Ļ����������ȡ�ַ�*/
    WHILE ((c = oam_sty_getch(vty)) != (DWORD)-10000)
    {
        LOOP_CHECK();

        if (OAM_CTL_IAC == c)
        {/*�ͻ��˷�����Э�����ݣ��ݲ�����*/
            oam_sty_getch(vty);
            oam_sty_getch(vty);

            continue;
        }

/*disp_line Ӧ����oam_sty_page���д���������ע�͵�*/
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
        /*-��ҳʱ��������-*/
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
        
        /*        else if ((vty->flags & OAM_STY_FLAG_RAWMODE)|| (vty->flags & OAM_STY_FLAG_EXECMODE))*//*��Ϊ�ͻ��˴���*/

        /*?? OAM_STY_FLAG_RAWMODE ���񲻻����*/
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
                {/*����*/
                    oam_sty_putc(vty, c);
                }
            }
            vty->line_buf[0] = (CHAR) c;
            return;
        }
        /*else*/
#endif
        {
            /*�����ʱ���������ַ���ֻ��ȡ�������������ƶ�ȡ*/
            static CHAR cTmp = 0;

            /*-���뷽���-*/
            if (c == 27)
            {
                cTmp = 1;
                continue;
            }

            if(1 == cTmp)
            {/*����Ѿ���ȡ��һ���ַ����ٶ�ȡһ����������������*/
                cTmp = 2;
                continue;
            }
            else if(2 == cTmp)
            {/*��ȡ�����������ж�*/
                cTmp = 0;
                switch(c)
                {
                    /* -ת��һ�·���ͳһ����-*/
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

            /*-�������崦������-*/
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
                    {/*�������������ϴβ�ͬ�ģ�������*/
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
                    {/*������Ϊ����Ҫ����ʷ�����ָ���������һ����ʼ��*/
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
                    vty->line_max = OAM_STY_LINE_MAX - 1;/*��һ����ʾmore_page*/
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
                    /*�鿴������ʺ��Ƿ�����ͨ�ַ�*/      
                    for (swCnt = strlen(vty->line_buf); swCnt > 0; swCnt--)
                    {
                        if ('\\' == vty->line_buf[swCnt - 1])
                        {
                            bIsNormalChar = bIsNormalChar ? FALSE : TRUE;
                        }
                        else
                        {/*��ǰ���������κ��ַ���break*/
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

                        line_len -= 2;/*��ȡ��������һ���ַ���buf�е��±�*/
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

            case 'P' & 077:          /*-ctrl-p- ���ϼ�ͷ*/
                vty->history_ptr--;
                if((short)vty->history_ptr < 0)
                {
                    vty->history_ptr = 0;
                    continue;
                }
                if(vty->history[vty->history_ptr] == NULL)
                {
                    vty->history_ptr++;/*ָ�븴ԭ*/
                    continue;
                }
                if(strlen(vty->history[vty->history_ptr]) == 0)
                {
                    vty->history_ptr++;/*ָ�븴ԭ*/
                    continue;
                }
                oam_sty_yank(vty);
                continue;
            case 'N' & 077:          /*-ctrl-n-���¼�ͷ*/
                vty->history_ptr++;
                if(vty->history_ptr > OAM_STY_HISTORY_SIZE - 1)
                {
                    vty->history_ptr = OAM_STY_HISTORY_SIZE - 1;
                    continue;
                }
                if(vty->history[vty->history_ptr] == NULL)
                {
                    vty->history_ptr--;/*ָ�븴ԭ*/
                    continue;
                }
                if(strlen(vty->history[vty->history_ptr]) == 0)
                {
                    vty->history_ptr--;/*ָ�븴ԭ*/
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

/*�����Ǽ��㵽�༭λ�õ��ַ���*/
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
        {/*�����MARK_LEFT�����ȼ�һ*/
            len --;
        }
        p ++;
    }

    return len;
}

/*-��ҳ��ʾ����,disp_lineָʾ��ǰ�ն�����ʾ������-*/
static void oam_sty_page(sty *cty, CHAR c)
{
    WORD16 i = 0;
    DISPLAY_NODE *p_node = NULL, *release_node = NULL;
    CHAR *pinfo = NULL;
    WORD16 wLen = 0;

    TELNETSVR_VOID_ASSERT(cty != NULL);

    /*�����֧���������ֹ����ֱ���˳�*/
    if ((c != ' ') && (c != '\r') && (c != '\n') && (cty->bCeaseAttr == 2))
    {
        oam_sty_puts(cty, "\n\r%Note: The current command doesn't support [ctrl+c] or [anykey] operation!\n\r");
        return;
    }

    p_node = cty->p_disp_head;

    wLen = strlen(more_page);
    for (i = 0; i < wLen; i ++)
    {
        oam_sty_puts(cty, CUT_CHAR);       /*-����������ʾ��Ϣ-*/
    }

    switch(c)
    {
        /*-���ո���ʾ��һҳ-*/
    case ' ':
    case '\r':
    case '\n':
        if (' ' == c)
        {
            cty->disp_line = 1;        /*-����ҳ��ʾ������-*/
        }
        
        for (i = 1; i < OAM_STY_PAGE_LINE; i ++) /*-��һ��Ԥ������ʾ��Ϣ-*/
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
                    {/*�������*/
                        snprintf(cty->szStrToShow, OAM_STY_OUTPUT_LINE_MAX * 2 - 1, "%s%s", cty->szStrToShow, p_node->disp_info);
                        oam_sty_puts(cty, cty->szStrToShow);
                        cty->ucToShowLabel = 0;
                        memset(cty->szStrToShow, 0, sizeof(cty->szStrToShow));
                    }
                    else
                    {
                        oam_sty_puts(cty, p_node->disp_info);      /*-��ʾ-*/
                    }

                    /*��������Ի��н�β����ӻ���*/
                    if(*(p_node->disp_info + wDispLen - 1) != '\n')
                    {
                        oam_sty_puts(cty, "\n");
                    }
                        
                    oam_sty_puts(cty, "\r");
                }
                OAM_RETUB(p_node->disp_info);   /*-�ͷ���ʾ�ռ�-*/
            }
            release_node = p_node;
            p_node = p_node->p_next;
            cty->p_disp_head = p_node;
            OAM_RETUB(release_node);  /*-�ͷŽڵ�ռ�-*/

            if ((p_node) && (p_node->p_next == NULL))
            {/*�������һ��*/
                pinfo = p_node->disp_info;
                /*-���������,�ǰ�����Ϣ,�����м���,����MP�ٷ���Ϣ��ʾ�����°��-*/
                if ((oam_sty_strlen(pinfo) < OAM_STY_OUTPUT_LINE_MAX) && (pinfo[strlen(pinfo) - 1] != '\n'))
                {
                    /*CRDCM00313101 ��snprintf�滻strcpy��strcat����*/
                    snprintf(cty->szStrToShow,
                                 sizeof(cty->szStrToShow) -1,
                                 "%s",
                                 pinfo);

                    cty->ucToShowLabel = 1;
                    cty->disp_line --;

                    release_node = p_node;
                    p_node = p_node->p_next;
                    OAM_RETUB(release_node);  /*-�ͷŽڵ�ռ�-*/
                    break;
                }
            }

            /*������ǿո���ǻس����������break*/
            if(c != ' ')
            {
                break;
            }
        }
        
        cty->p_disp_head = p_node;
        if (p_node == NULL)        /*-��ʾ���-*/
        {
            remote_parse_cookie *rpc = NULL;
            rpc = cty->link_cookie;
            if (!rpc)
            {
                return;
            }
            
            /*����ǻس�����ʾ��ģ�����λdisp_line
              �Ա�����յ������ݻ��Ǽ�����ʾ����*/
            if (' ' == c)
            {
                cty->disp_line = 1;        /*-����ҳ��ʾ������-*/
            }
#if INSTALL_MP_TELNET
            if ((cty->flags & OAM_STY_FLAG_REMOTE_PARSE) &&  (MSG_MORE_PKT == rpc->last_pkt_flag))
            {
                if (req_more_data_to_remote_parse(cty))
                {/*���ò��ɹ�(�����һ�������߳���)*/
                    /*-��ʾ��ʾ����������-*/
                    oam_sty_put_prompt_cmdline(cty);
                    cty->line_len = strlen(cty->line_buf);
                }

            }
            else
#endif
            {
                cty->disp_line = 1;        /*-����ҳ��ʾ������-*/
                /*-��ʾ��ʾ����������-*/
                oam_sty_put_prompt_cmdline(cty);
            }
        }
        else
        {
            oam_sty_puts(cty, more_page);       /*-δ��ʾ����ʾ��ʾ��Ϣ-*/
        }
        return;
#if 0
        /*-���س���ֻ��ʾ��һ��-*/
    case '\r':
    case '\n':
        if ((p_node != NULL) && ((p_node->disp_info) != NULL) )
        {
            if(strcmp(p_node->disp_info,cty->prompt_line) != 0)
            {
                int iCnt = 0;
                CHAR *p = p_node->disp_info;
                /*�����Ƿ��������Լ����л��Ǵﵽ�п�ݽ���ʾ
                ������س����е���ʾ*/
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
                else/* if(p != p_node->disp_info) ??����ж�����ν�ɣ�ֻ��\r��\nҲ������ͬ����*//*����ֻ�� '\r''\n'*/
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
            OAM_RETUB(p_node->disp_info);       /*-�ͷ���ʾ�ռ�-*/
            release_node = p_node;
            p_node = p_node->p_next;
            OAM_RETUB(release_node);            /*-�ͷŽڵ�ռ�-*/
            cty->p_disp_head = p_node;
            if ((p_node) && (p_node->p_next == NULL))
            {
                pinfo = p_node->disp_info;
                /*-���������,�ǰ�����Ϣ,�����м���,����MP�ٷ���Ϣ��ʾ�����°��-*/
                if ((oam_sty_strlen(pinfo) < OAM_STY_OUTPUT_LINE_MAX) && (pinfo[strlen(pinfo) - 1] != '\n'))
                {
                    strncpy(cty->szStrToShow, pinfo);
                    cty->ucToShowLabel = 1;
                    cty->disp_line = OAM_STY_PAGE_LINE - 1;
                }
            }

            if (p_node == NULL)        /*-��ʾ���-*/
            {
#if INSTALL_MP_TELNET
                if (cty->flags & OAM_STY_FLAG_REMOTE_PARSE)
                {
                    if (req_more_data_to_remote_parse(cty))
                    {
                        cty->disp_line = 1;
                        /*-��ʾ��ʾ����������-*/
                        oam_sty_put_prompt_cmdline(cty);
                    }
                }
                else
#endif
                {
                    cty->disp_line = 1;
                    /*-��ʾ��ʾ����������-*/
                    oam_sty_put_prompt_cmdline(cty);
                }
            }
            else
            {
                oam_sty_puts(cty, more_page);   /*-��ʾ����-*/
            }
        }
        return;
#endif
        /*-������������ʾ������Ϣ�ͷſռ�-*/
    default:
        if ( c == 27 )   /*-�Է�����Ĵ���,������뻺����(��Ϊ����������Ƕ���ַ�?)-*/
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
                OAM_RETUB(p_node->disp_info);   /*-�ͷ���ʾ�ռ�-*/
            }
            release_node = p_node;
            p_node = p_node->p_next;
            OAM_RETUB(release_node);            /*-�ͷŽڵ�ռ�-*/
        }
        cty->disp_line = 1;
        cty->p_disp_head = NULL;
        /*-ȡ����ʾ-*/
#if INSTALL_MP_TELNET
        if (cty->flags & OAM_STY_FLAG_REMOTE_PARSE)/*-MP��TELNET-PARSE-*/
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
* �������ƣ� void oam_sty_close(sty *cty)
* ���������� �ر�sty������ն��������
* ���������
* ���������
* �� �� ֵ��
* ����˵���� ��
* �޸�����   �汾��  �޸���      �޸�����
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

    /*-�ͷ���ʾ�ռ�-*/
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

    /*�ͷ���ʷ��¼�ռ�*/
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
* �������ƣ�void oam_sty_put_prompt_line(sty *cty, CHAR *p, int nlen)
* ���������������ַ�����sty ��ϵͳ��ʾ����
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����  �汾��  �޸���      �޸�����
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
* �������ƣ�    void oam_sty_put_command_line(sty *cty, CHAR *p, int nlen)
* ���������������ַ�����sty��line_buf��
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
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
* �������ƣ�      void oam_sty_input_password(sty *cty, int flag)
* ������������ʶ��ǰ�ն�׼����������
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
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

/*��ͬһ����ʾ����Ϣ*/
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

/*�����ǰ����������У�ֻ�������ʾ��line_buf������û��*/
static void oam_sty_eraseline(sty *cty)
{
    DWORD i = 0, k = 0;

    if (!(cty->flags & OAM_STY_FLAG_ECHOING))
    {
        return;
    }

    /*������ո����ԭ�����룬Ȼ��ѹ���Ƶ�����*/
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

/*������ʷ����ʱ�򣬰���ʷ��¼д�뵱ǰ�����л���*/
static void oam_sty_yank(register sty *cty)
{
    DWORD i = 0;
    if (cty->history[cty->history_ptr] == 0)
    {
        return;
    }

    /*�༭λ���Ƶ�����*/
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

    /*��ȡ��ʷ����*/
    WHILE ((cty->line_buf[cty->line_len] = cty->history[cty->history_ptr][cty->line_len]) != '\0')
    {
        LOOP_CHECK();
        cty->line_len++;
    }
    cty->edit_pos = cty->line_len;

    /*������������ʾ��ʼλ��*/
#if OAM_STY_SCROLL_LINE
    if (cty->line_len > cty->line_max)
    {
        cty->disp_pos = cty->line_len - cty->line_max;
    }
    else
    {
        cty->disp_pos = 0;
    }

    /*�����������*/
    oam_sty_puts_cmdline(cty);
#else
    if (cty->flags & OAM_STY_FLAG_ECHOING)
    {
        oam_sty_puts(cty, (CHAR *) cty->line_buf);
    }
#endif
}

/*-ɾ�����һ���ַ�-*/
static void oam_sty_cutleft(sty *cty)
{
    if (cty->edit_pos <= 0 )
    {
        return;
    }
#if OAM_STY_SCROLL_LINE
    if ((cty->line_len == cty->edit_pos) && (cty->disp_pos == 0))
    {/*�������β��ֱ��ɾ��*/
        if (cty->line_len)

        {
            cty->line_len --;
        }
        oam_sty_puts(cty, CUT_CHAR);
        cty->edit_pos --;
    }
    else
    {/*������β��ָ��ɾ��λ�����������ַ�ǰ��һλ��Ȼ��������ʾ������*/
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

/*-ɾ��������-*/
static void oam_sty_cuthead(sty *cty)
{
    DWORD i = 0;

    if (cty->edit_pos <= 0)
    {
        return;
    }
    /*�༭λ�ú������ַ�ǰ��n λ*/
    for (i = 0; i < (DWORD)(cty->line_len - cty->edit_pos); i ++)
    {
        cty->line_buf[i] = cty->line_buf[cty->edit_pos + i];
    }

    /*������ʾ������*/
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

/*-ɾ������β-*/
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
        /*�༭λ�ÿ�ʼ�������ַ�ȫ�����*/
        for (i = cty->edit_pos; (i < cty->line_len) && (i < cty->disp_pos + cty->line_max); i ++)
        {
            oam_sty_putc(cty, ' ');
        }
        /*����ƻر༭λ��*/
        for (i = cty->edit_pos; (i < cty->line_len) && (i < cty->disp_pos + cty->line_max); i ++)
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
    }
    /*�����µ������г��ȣ����¼�����ʾ��ʼλ�ò���ʾ������*/
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

/*-ɾ��һ���У���ͬ��cutline�����������line_buf-*/
static void oam_sty_cutline(sty *cty)
{
    if (!cty->line_len)
    {
        return;
    }

#if OAM_STY_SCROLL_LINE
    /*������ո����ԭ�����룬Ȼ��ѹ���Ƶ�����*/
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
    /*���line_buf*/
    cty->line_len = 0;
    cty->edit_pos = 0;

    return;
}

/*-ɾ���ұ�һ���ַ�-*/
static void oam_sty_cutright(sty *cty)
{
    DWORD i = 0;

    if ( cty->edit_pos >= cty->line_len )
    {
        return;
    }

    cty->line_len --;

    /*�༭λ�ÿ�ʼ����β���ַ���ǰ��һλ��Ȼ��������ʾ������*/
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
        cty->line_buf[i] = cty->line_buf[i + 1]; /*-�ı����ݲ���ʾ-*/
        if (cty->flags & OAM_STY_FLAG_ECHOING)
        {
            /* ��ֹ���뱩¶ */
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
        oam_sty_putc(cty, ' '); /*-ɾ�����һ���ַ�-*/
        for (i = cty->line_len; i >= cty->edit_pos; i --) /*-�ѹ���Ƶ�ԭ����λ��-*/
        {
            oam_sty_putc(cty, MARK_LEFT);
        }
    }
#endif

    return;
}
/************************************************************************
* �������ƣ�static void send_to_remote_parse(sty *sty, CHAR *line, remote_parse_cookie *cookie)
* ��������������ͽ��̷���������Ϣ
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
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

    /*�������лỰ�л�ȡ��Ϣͷ��Ϣ*/
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

    /*��ȡ�����ַ���*/
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

    /*?? ����ͺ����ִ��ģʽ*/
    sty->flags |= OAM_STY_FLAG_EXECMODE;
    /* �����Ƿ���OAM_STY_FLAG_EXEINPUT�����Ƿ��������룬��̫����*/
    oam_sty_enable_input(sty, (sty->flags & OAM_STY_FLAG_EXEINPUT));

    /*�������terminate������Ҫ���ö�ʱ��*/
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
* �������ƣ�     int req_more_data_to_remote_parse(sty *cty)
* ����������    ���ģʽ������ͳ���������һ������
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
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

/*֪ͨ���ͳ���ر��ն�*/
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

/*���з���0�����з������һ���ַ�������*/
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
        /*611001924617 ��������ѭ������*/
        /*LOOP_CHECK();*/
        aucLineBuf[str_len] = *p;
        
        if(*p == MARK_LEFT && cty->fill_len>0)
        {
            cty->fill_len = (cty->fill_len  > 0) ? (cty->fill_len--) : cty->fill_len ;
        }
        else
        {
            if (char_is_chinese(*p))
            {/*�����ַ��������������ֽ�*/
                if (cty->fill_len == OAM_STY_LINE_MAX)
                {/*��������ַ���һ��ʹﵽ�п������Ѿ���ȡ��һ�뵽���ж�*/
                    /*�Ѿ���ȡ�ķŵ����ж�*/
                    p--;
                    aucLineBuf[str_len] = '\0';
                }
                else
                {/*�����û�дﵽ�п��ͽ��Ŷ�ȡ��һ���ֽڣ���֤�����ַ�û�б��ֿ�*/
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
        
        /*-�����з�������ʱ��һ��-*/
        if ((*p == '\n') || 
             (*p == '\r') || 
             ((cty->fill_len >= OAM_STY_LINE_MAX) && (*(p+1) != '\n') && (*(p+1) != '\r')))
        {
            aucLineBuf[str_len] = MARK_END;
            oam_sty_puts(cty, aucLineBuf);  /*-�ݽ���ʾ-*/

            /*\n��\r��\r\n��\n\r ����Ϊ��һ���س�*/
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
                    /*������к���պ��ǻس����У�������ӣ�������ӻ���*/
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

    /*������е����*/
    if((aucTextIn[wStrlen - 1] == '\n') || (aucTextIn[wStrlen - 1] == '\r'))
    {
        wRet= 0 ;
    }
    else
    {
        /*-�ݽ���ʾ-*/
        oam_sty_puts(cty, aucLineBuf);  
        wRet = str_len;
    }

    return wRet;
}


/*�����½�������Ϣ������*/
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

/*��ȡ�ն�����*/
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
    
    /*judge the flag ���Ƿ���մ����ն�����*/
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

    /*�����ǰ���֧����ֹ*/
    if(((cTmp == 3) && (nRecvLen == 1)) && 
        (vty->bCeaseAttr == 2) &&
        ((vty->flags & OAM_STY_FLAG_EXECMODE) || (vty->p_disp_head)))/*Ctrl+C*/
    {
        oam_sty_puts(vty, "\n\r%Note: The current command doesn't support [ctrl+c] or [anykey] operation!\n\r");
        return;
    }

    /*����������ն˵Ļ���*/
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

/*֪ͨ���ͳ���ȡ������ִ�е�����*/
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

/*����һ���ַ��������ͳ���*/
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

    /*611001634156 ��ETCA���������顿show atmpvc\atmoamlock��ʾ���û�ж���*/
    vty->fill_len= 0;
	
    send_to_remote_parse(vty, line, vty->link_cookie);
}

/*����������������г������ϸ��Ϣ*/
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
    
    /* ���͸�SSH Server */
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


