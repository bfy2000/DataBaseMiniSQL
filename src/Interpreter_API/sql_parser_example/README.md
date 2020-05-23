# Flex Bison SQL Example

这是一个Flex & Bison的Parser示例。

它读入用户的输入流，并将SQL语句转换为逆波兰式存储在堆栈中。

## 使用
首先要安装[win_flex](http://gnuwin32.sourceforge.net/packages/flex.htm)和[win_flex](http://gnuwin32.sourceforge.net/packages/bison.htm)两个工具，分别进入下载界面，找到第一个Setup安装包，然后将安装目录中的win_flex_bison添加到系统PATH中。

```bash
$ cd ./src/Interpreter_API/sql_parser_example
$ make clean
$ make 
$ lpmysql.exe
$ Start Testing...
```

注：使用win10的MinGW工具链进行配置，在gcc x64-built下可以正常运行，不保证在linux和macos中能够正常运行。