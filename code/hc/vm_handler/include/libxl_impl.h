/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_driver.h
* 文件标识：
* 内容摘要：VmDriver类的定义文件
* 当前版本：1.0
* 作    者：lixch
* 完成日期：2011年7月25日
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

// 创建接口
uint32_t main_create(const char *filename);
// 通过dom_name 获取dom_id
int main_domid(const char *domname);
// 获取运行的vmID 列表
int list_vm(int* ids);
// 获取指定domid的运行信息
int domain_info_show(int domid, libxl_dominfo &info_buf);
// 通过domid获取domin_name
string main_domname(int domid);
// CPU bind 接口支持domid,all, all  or domid 1,5(把domid 的1
// 号虚拟机CPUbind到5号物理CPU上)
int vcpupin(int domid, const char *vcpu, char *cpu);
// 设置权重
int sched_credit_weight_set(int domid, int weight);
//设置调度上限
int sched_credit_cap_set(int domid, int cap);
// 重启虚拟机
int reboot_domain(int domid);

int pause_domain(int domid);

int unpause_domain(int domid);
int main_shutdown(int domid);
int main_destroy(int domid);
int main_migrate(int domid, const char *host);








#endif 
