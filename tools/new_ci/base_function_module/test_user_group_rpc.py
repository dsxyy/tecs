#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
#
# 测试内容：
# 当前版本：1.0
# 作    者：李孝成
# 完成日期：2012/11/24
#
#*******************************************************************************/
import sys, os
import unittest 

from interface.environment_prepare_package import prepare_environment 
from interface.base_interface.rpc import user
from interface.base_interface.rpc import usergroup
from interface.base_interface.rpc.contrib import tecs_common
from interface.config_para import config_para

class UnitTestUserAdd(unittest.TestCase):
    '''
    增加用户(tecs.user.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
              
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        pass;
    

# 下面是正常测试代码       
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        self.normal_test_level_3_3()
        self.normal_test_level_3_4()
        
    def normal_test_level_3_4(self):
        '''
        是否实现：是
        测试意图：测试管理员帐号，创建另一个管理员帐号的流程
        测试步骤：在用户组test_usergroup下面，增加一个管理员帐号，用户名为A，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        
        #还原测试环境
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_3(self):
        '''
        是否实现：是
        测试意图：测试密码和确认密码不通的流程
        测试步骤：1、在用户组cigroup下面，增加一个普通租户帐号，用户名为A，密码和确认密码不同，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(u.allocate("A", "test", "test1", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #还原测试环境
        
    def normal_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试创建多个租户帐号流程
        测试步骤：1、在用户组cigroup下面，增加一个普通租户帐号，用户名为A，应该成功
                  2、在用户组cigroup下面，增加一个普通租户帐号，用户名为B，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #还原测试环境
        self.assertTrue(u.delete("A"))
        self.assertTrue(u.delete("B"))
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试创建一个租户帐号流程
        测试步骤：在用户组test_usergroup下面，增加一个普通租户帐号，用户名为A，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #还原测试环境
        self.assertTrue(u.delete("A"))

# 下面是异常测试代码 
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()

    def Exception_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试创建重名用户的流程
        测试步骤：1、在用户组cigroup下面，增加一个普通租户帐号，用户名为A，应该成功
                  2、在用户组cigroup下面，增加一个普通租户帐号，用户名为A，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertFalse(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #还原测试环境
        self.assertTrue(u.delete("A"))


# 下面是边界测试代码 
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：命名格式测试
        测试步骤：1、创建用户名为12345678_test@zte.com.cn，普通租户帐号，应当成功
                  2、创建用户名为~12345678_test@zte.com.cn，普通租户帐号，应当失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        name = "~12345678_test@zte.com.cn"
        self.assertFalse(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        name = "12345678_test@zte.com.cn"
        self.assertTrue(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #还原测试环境
        self.assertTrue(u.delete(name))
        
    def limit_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：命名长度测试
        测试步骤：1、创建用户名称长度等于65个字符的，普通租户帐号，应当失败
                  2、创建用户名称长度等于64个字符的，普通租户帐号，应当成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        name = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz012"
        self.assertFalse(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        name = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz01"
        self.assertTrue(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #还原测试环境
        self.assertTrue(u.delete(name))

# 下面是权限控制流程测试代码         
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()

    def auth_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试普通租户权限下面的操作
        测试步骤：1、使用租户A创建一个租户B，应该失败
        '''
        root_u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        name = "A"
        self.assertTrue(root_u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        normal_session = user.login(self.environment.server, name, "test")
        normal_u = user.user(self.config_para.server_addr,  normal_session)
        self.assertFalse(normal_u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))

        #还原测试环境
        self.assertTrue(root_u.delete(name))
        
        
    def auth_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试管理员权限下面的操作
        测试步骤：1、使用管理员创建一个用户
        '''
        self.normal_test_level_3_1()

    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()
 
class UnitTestUserDel(unittest.TestCase):
    '''
    删除用户(tecs.user.delete)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()

    def normal_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试管理员删除一个不存在的帐号是否成功
        测试步骤：1、删除一个不存在的帐号A，应该失败
        '''
        root_u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(root_u.delete("A"))
        
        # 恢复环境
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试管理员是否能删除一个普通租户的帐号
        测试步骤：1、使用管理员创建一个用户,然后删除该帐户，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        # 恢复环境
        self.assertTrue(u.delete("A"))

# 异常测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()
        self.Exception_test_level_3_3()

        
    def Exception_test_level_3_3(self):
        '''
        是否实现：否
        测试意图：测试是否可以删除一个已经部署过虚拟机的用户
        测试步骤：1、创建一个普通租户A，并在A帐号下面部署一个虚拟机
                  2、用管理员帐号登录，并删除A帐号，应该失败
        '''
        pass;
        
    def Exception_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试是否可以删除默认admin帐号
        测试步骤：1、创建另一个管理员帐号A
                  2、用A帐号登录，然后删除admin帐号，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        
        self.assertFalse(A_user.delete("admin"))

        # 恢复环境
        self.assertTrue(u.delete("A"))

        
    def Exception_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试是否可以删除自己的帐号
        测试步骤：1、创建另一个管理员帐号A
                  2、用A帐号登录，然后删除自己，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        
        self.assertFalse(A_user.delete("A"))

        # 恢复环境
        self.assertTrue(u.delete("A"))
        

# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()

    def limit_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：没有边界值需要测试
        测试步骤：无
        '''
        pass;

        # 恢复环境

        
# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
    
    def auth_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试租户权限下，是否可以删除另一个租户的帐号
        测试步骤：1、创建一个普通租户帐号A
                  2、再创建另一个普通租户帐号B
                  2、用A帐号登录，然后删除B帐号，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        
        self.assertFalse(A_user.delete("B"))


        # 恢复环境
        self.assertTrue(u.delete("A"))
        self.assertTrue(u.delete("B"))
    
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestUserSet(unittest.TestCase):
    '''
    修改用户(tecs.user.set)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        self.normal_test_level_3_3()
        self.normal_test_level_3_4()
        self.normal_test_level_3_5()
        self.normal_test_level_3_6()
        self.normal_test_level_3_7()
        self.normal_test_level_3_8()
        
        
    def normal_test_level_3_8(self):
        '''
        是否实现：是
        测试意图：管理员禁用默认管理员admin
        测试步骤：1、管理员用户admin 创建一个管理员帐号A
                  2、登录帐号A，然后禁用admin,应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        self.assertFalse(A_user.set_enable_disable("admin", user.USER_DISABLE))

        # 恢复环境        
        self.assertTrue(u.delete("A"))      

    def normal_test_level_3_7(self):
        '''
        是否实现：是
        测试意图：管理员禁用另一个管理员
        测试步骤：1、管理员用户admin 创建一个管理员帐号admin_A
                  2、禁用帐号A,应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("admin_A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        self.assertTrue(u.set_enable_disable("admin_A", user.USER_DISABLE))
        
        # 恢复环境 
        self.assertTrue(u.delete("admin_A"))              

    def normal_test_level_3_6(self):
        '''
        是否实现：是
        测试意图：管理员启用一个租户
        测试步骤：1、管理员用户admin 创建一个租户A
                  2、禁用租户A,应该成功
                  3、然后再启用租户A，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.set_enable_disable("A", user.USER_DISABLE))
        self.assertTrue(u.set_enable_disable("A", user.USER_ENABLE))
        
        # 恢复环境        
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_5(self):
        '''
        是否实现：是
        测试意图：管理员禁用一个租户
        测试步骤：1、管理员用户admin 创建一个租户A
                  2、禁用租户A,应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.set_enable_disable("A", user.USER_DISABLE))

        # 恢复环境    
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_4(self):
        '''
        是否实现：是
        测试意图：管理员修改普通租户的角色为管理员
        测试步骤：1、管理员用户admin 创建一个普通租户A
                  2、修改租户A的角色为管理员，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.set_role("A", user.CLOUDADMIN))
        
        # 恢复环境
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_3(self):
        '''
        是否实现：是
        测试意图：管理员修改自己的角色
        测试步骤：1、管理员用户admin 修改自己的角色为普通用户，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(u.set_role("admin", user.ACCOUNTUSER))
        
        # 恢复环境

        
    def normal_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：管理员修改自己密码
        测试步骤：1、管理员用户admin 修改自己的密码，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.set_pwd("admin", "aaaaa", "aaaaa"))
        
        # 恢复环境
        self.environment.refresh_root_session("aaaaa")
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.set_pwd("admin", "admin", "admin"))
        self.environment.refresh_root_session("admin")
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：管理员修改另外一个管理的角色为普通租户
        测试步骤：1、管理员用户admin 创建另一个管理员admin_A
                  2、然后管理员admin修改另一个管理员admin_A的权限为普通租户，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("admin_A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        self.assertTrue(u.set_role("admin_A", user.ACCOUNTUSER))
        
        # 恢复环境
        self.assertTrue(u.delete("admin_A"))

# 异常测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()
        
        
    def Exception_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：修改用户的用户组到一个不存在的用户组情况
        测试步骤：1、使用默认管理员admin创建一个租户帐号A，密码为test
                  2、修改租户A的用户组为“test_A”,应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertFalse(u.set_user_group("A", "test_A"))

        # 恢复环境
        self.assertTrue(u.delete("A"))
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：修改一个不存在的用户
        测试步骤：1、使用默认管理员admin修改一个不存在的租户帐号 ababbaba,应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(u.set_user_group("ababbaba", self.config_para.test_usergroup))

        # 恢复环境
        

# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()
        self.limit_test_level_3_3()
        self.limit_test_level_3_4()

    def limit_test_level_3_4(self):
        '''
        是否实现：否
        测试意图：测试修改禁用，启用，超过范围值的情况(disable有效性，如果非法认为不修改，不测试)
        测试步骤：1、创建一个租户A
                  2、修改租户A的禁用属性值为USER_DISABLE+1，应该失败
        '''

        # 恢复环境
        
    def limit_test_level_3_3(self):
        '''
        是否实现：否
        测试意图：测试修改的角色不在范围内的情况(角色有效性，如果非法认为不修改，不测试)
        测试步骤：1、创建一个租户A
                  2、修改租户A的角色为ACCOUNTUSER+1，应该失败
        '''
        # 恢复环境
        
    def limit_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试修改密码和确认密码在边界上，不一致情况
        测试步骤：1、创建一个租户A
                  2、修改密码为 32个1，确认密码是33个1，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        pwd     = "11111111111111111111111111111111"
        cof_pwd = "111111111111111111111111111111111"
        self.assertFalse(u.set_pwd("A", pwd, cof_pwd))

        # 恢复环境
        self.assertTrue(u.delete("A"))
        
    def limit_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试密码长度边界
        测试步骤：1、创建一个租户A
                  2、修改密码为 32个1，应该成功
                  3、修改密码为33个1，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        limit_ok_pwd     = "11111111111111111111111111111111"
        self.assertTrue(u.set_pwd("A", limit_ok_pwd, limit_ok_pwd))
        limit_error_pwd  = "111111111111111111111111111111111"
        self.assertFalse(u.set_pwd("A", limit_error_pwd, limit_error_pwd))

        # 恢复环境
        self.assertTrue(u.delete("A"))

        
# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        self.auth_test_level_3_3()
        self.auth_test_level_3_4()
        self.auth_test_level_3_5()

    def auth_test_level_3_5(self):
        '''
        是否实现：否
        测试意图：测试租户权限下，置空自己的电话，邮件，地址，描述信息（user模块没有支持置空功能）
        测试步骤：1、创建一个普通租户帐号A
                  2、使用租户A登录，先设置自己的电话，邮件，地址，描述信息
                  3、然后再使用设置接口，清空自己的电话，邮件，地址，描述信息
        '''
        
        # 恢复环境

        
    def auth_test_level_3_4(self):
        '''
        是否实现：否
        测试意图：测试租户权限下，修改自己的群组信息(因为该用例有故障，用户可以设置自己的用户组)
        测试步骤：1、创建一个普通租户帐号A
                  2、登录A帐号，使用设置接口，修改自己的群组信息，应该失败
        '''
        return
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        test_group = usergroup.usergroup(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(test_group.allocate("test_group", usergroup.GROUP_TYPE1))
        self.assertFalse(A_user.set_user_group("A", "test_group"))
        
        # 恢复环境
        self.assertTrue(u.delete("A"))
        self.assertTrue(test_group.delete("test_group"))

        
    def auth_test_level_3_3(self):
        '''
        是否实现：是
        测试意图：测试租户权限下，修改自己的角色信息
        测试步骤：1、创建一个普通租户帐号A
                  2、登录A帐号，使用设置接口，修改自己的角色信息，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        self.assertFalse(A_user.set_role("A", user.CLOUDADMIN))
        
        # 恢复环境
        self.assertTrue(u.delete("A"))
        
    def auth_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试租户权限下，修改自己的电话，邮件，地址，描述信息
        测试步骤：1、创建一个普通租户帐号A
                  2、登录A帐号，使用设置接口，修改自己的电话，邮件，地址，描述信息，应该成功
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        self.assertTrue(A_user.set_phone("A", "13770511182"))
        self.assertTrue(A_user.set_email("A", "125aaaaa@sohu.com"))
        self.assertTrue(A_user.set_location("A", "zhong_xing_tong_xun"))
        self.assertTrue(A_user.set_description("A", "test description"))

        # 恢复环境.
        self.assertTrue(u.delete("A"))
        
    def auth_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试租户权限下，修改别人的信息
        测试步骤：1、创建一个普通租户帐号A
                  2、再创建另一个普通租户帐号B
                  3、用A帐号登录，然后修改B帐号的描述信息，应该失败
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        self.assertFalse(A_user.set_description("B", "test description"))

        # 恢复环境
        self.assertTrue(u.delete("A"))
        self.assertTrue(u.delete("B"))
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


class UnitTestUserQuery(unittest.TestCase):
    '''
    查询用户(tecs.user.query)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        self.normal_test_level_3_3()
        self.normal_test_level_3_4()

    def normal_test_level_3_4(self):
        '''
        是否实现：是
        测试意图：管理员查询创建租户的默认信息
        测试步骤：1、管理员创建一个租户A
                  2、查询租户的群组信息，应该为
                  3、查询租户的使能信息，应该为使能
                  4、查询租户角色信息，应该为
                  4、查询自己的phone,email,addr,description,都应该为NULL
        '''
        
        # 恢复环境

        
    def normal_test_level_3_3(self):
        '''
        是否实现：是
        测试意图：管理员查询自己的信息
        测试步骤：1、查询租户角色信息，应该为
                  2、查询租户的群组信息，应该为
                  3、查询租户的使能信息，应该为使能
                  4、设置自己的phone 为 13770591182
                  4、查询自己的phone应该为13770591182
        '''
        
        # 恢复环境

        
    def normal_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：管理员查询多个租户信息
        测试步骤：1、管理员创建一个租户A
                  2、管理员创建一个租户B
                  3、查询租户的数量，应该为2
        '''
        
        # 恢复环境
        
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：管理员查询指定租户的信息
        测试步骤：1、管理员创建一个租户A
                  2、管理员创建一个租户B
                  3、设置租户A的phone 为 13606581182
                  3、查询租户A角色信息，应该为
                  4、查询租户A的群组信息，应该为
                  5、查询租户A的使能信息，应该为使能
                  6、查询租户A的phone,应该为 13606581182
        '''
        
        # 恢复环境
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''

    def Exception_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试查询一个不存在的租户信息
        测试步骤：1、管理员查询一个不存在的租户A信息，应该失败
        '''
        
        # 恢复环境
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''

    def limit_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试查询用户名为边界的情况
        测试步骤：1、创建一个租户名字长度为64的用户
                  2、查询租户名字为64的长度，应该成功
        '''
        
        # 恢复环境

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        
    def auth_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：测试租户身份情况下，查询另外一个租户的信息
        测试步骤：1、
        '''
        
        # 恢复环境

        
    def auth_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：测试租户身份情况下，查询自身信息
        测试步骤：1、
        '''
        
        # 恢复环境
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()
        
class UnitTestUserFullQuery(unittest.TestCase):
    '''
    查询用户全部信息(tecs.user.full.query)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


# 下面是模块测试执行总入口
def all_test_entry(environment):
    UnitTestUserAdd(environment).test_entery()
    UnitTestUserDel(environment).test_entery()
    UnitTestUserSet(environment).test_entery()
    UnitTestUserQuery(environment).test_entery()
    UnitTestUserFullQuery(environment).test_entery()
    
    
# 模块文档输出总入口
def all_doc_entry(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "用户RPC"
    cidoc.write_doc(UnitTestUserAdd, module_name)
    cidoc.write_doc(UnitTestUserDel, module_name)
    cidoc.write_doc(UnitTestUserSet, module_name)
    cidoc.write_doc(UnitTestUserQuery, module_name)
    cidoc.write_doc(UnitTestUserFullQuery, module_name)


# 单个函数执行体的位置
if __name__ == "__main__":
        file_name = sys.argv[0]
        # 获取配置参数的句柄
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # 获取环境准备公共句柄，并初始化
        environment = prepare_environment(config_para_handler)
        environment.wait_sys_ok()
        environment.prepare_public_environment()
        all_test_entry(environment)