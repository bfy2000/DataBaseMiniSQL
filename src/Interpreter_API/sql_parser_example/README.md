# Flex Bison SQL Example

这是一个Flex & Bison的Parser示例。

它读入用户的输入流，并将SQL语句转换为逆波兰式存储在堆栈中。

## 使用

```bash
$ cd ./src/Interpreter_API/sql_parser_example
$ make clean
$ make 
$ pmysql.exe
$ Start Testing...
```

注：使用win10的MinGW工具链进行配置，在gcc x64-built下可以正常运行，不保证在linux和macos中能够正常运行。