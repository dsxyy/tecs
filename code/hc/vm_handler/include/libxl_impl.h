/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_driver.h
* �ļ���ʶ��
* ����ժҪ��VmDriver��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2011��7��25��
*
*******************************************************************************/

#ifndef LIBXL_IMPL_H
#define LIBXL_IMPL_H


#ifdef __cplusplus
extern "C" {
#endif 
#include <libxl.h>
#include <libxl_utils.h>
#include <libxl_event.h>
#ifdef __cplusplus
}
#endif 
#include <_libxl_types.h>
#include "libxlu_internal.h"
#include "libxlutil.h"
#include  <string>

using namespace std;

int libxl_init(void);

// �����ӿ�
uint32_t main_create(const char *filename);
// ͨ��dom_name ��ȡdom_id
int main_domid(const char *domname);
// ��ȡ���е�vmID �б�
int list_vm(int* ids);
// ��ȡָ��domid��������Ϣ
int domain_info_show(int domid, libxl_dominfo &info_buf);
// ͨ��domid��ȡdomin_name
string main_domname(int domid);
// CPU bind �ӿ�֧��domid,all, all  or domid 1,5(��domid ��1
// �������CPUbind��5������CPU��)
int vcpupin(int domid, const char *vcpu, char *cpu);
// ����Ȩ��
int sched_credit_weight_set(int domid, int weight);
//���õ�������
int sched_credit_cap_set(int domid, int cap);
// ���������
int reboot_domain(int domid);

int pause_domain(int domid);

int unpause_domain(int domid);
int main_shutdown(int domid);
int main_destroy(int domid);
int main_migrate(int domid, const char *host);








#endif 
