这里的模块与tecs的xmlrpc method基本上是一一对应，
例如tecs.abc.methodN在这里就有一个abc.py，其中定义了python类class abc，
它有如下方法：method1，method2，……，methodN
但是并不限于这些方法！！！为了测试方便，也可以在这些已有的method基础上，
封装更易用更简便的python函数给测试用例使用，例如vmcfg.py文件中，vmcfg类
就额外提供了get_vm_id和get_vm_state。


python代码运行时会产生后缀名为pyc的临时文件，这是为了提高性能的缓冲功能，
check in时不要把这些临时文件提交到svn！！！

