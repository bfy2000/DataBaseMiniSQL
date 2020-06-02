#### *Catalog Manager 需求*

`Catalog Manager`负责管理数据库的所有模式信息，包括：

- 数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。

- 表中每个字段的定义信息，包括字段类型、是否唯一等。

- 数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。

`Catalog Manager`还必需提供访问及操作上述信息的接口，供`Interpreter`和`API`模块使用。

#### *Catalog Manager 设计思想*

`CatalogManager`负责管理数据库的所有模式信息，主要是表的所有信息以及表中字段上所有的索引信息，其总体设计思想如下:

* **哈希存储**： 采用c++ STL中`map`这一数据结构存储所有的表和索引信息。作为哈希表，插入的`Table`和`Index`不允许重复，可以根据表名和索引名得到对应的表和索引信息。由于`map`的特性，表和索引均按照名字的字典序排序。
* **抽象类型**： 将`Table`、`Index`、`Attribute`等模式信息全部各自封装为一个完整的类，可以访问其public部分的模式信息和public的函数。`Table`作为整个模块的核心，它的类中封装有所有相关的`Index`和`Attribute`。
* **信息传递**：`CatalogManager`模块没有依赖于其他模块而存在，与之相对的，`CatalogManager`提供了大量的接口供`BufferManager`以外的所有模块使用。`CatalogManager`实现虽然难度不大，却是整个项目的核心之一所在。
* **文件读写**：生成`table_catalog`和`index_catalog`两个文件，分别保存`Table`和`Index` 的所有模式信息。为减小模块之间的耦合，Catalog模块采用直接访问磁盘文件的形式，不通过Buffer Manager，Catalog中的数据也不要求分块存储。

#### *Catalog Manager 主要功能*

* **初始化**：从`table_catalog`和`index_catalog`两个文件中读取所有模式信息并插入到两个哈希表中。
* **存储**：程序关闭时将哈希表中所有模式信息写回到两个文件中。
* **打印**：`show_table()`和`show_index()`两个函数负责打印出所有表的信息和索引信息。打印时的分隔符可随打印的字符串的长度而改变。
* **读取信息**：大量的`get_xx（）`接口帮助其他模块在仅知道表名/索引名等有限的信息时查询到其他有关联的信息。
* **插入**：插入表时直接插入到哈希表，插入索引时需要在哈希表和索引对应的那张表中写入索引信息。
* **删除**：删除表时直接从哈希表中删除。删除索引时需要在哈希表和索引对应的那张表中都删除索引信息。

#### *Catalog Manager 接口*

* 建立表

  ```c++
  bool create_table(Table newTable);
  ```

* 删除表

  ```c++
  bool drop_table(string tableName);
  ```

* 建立索引

  ```c++
  bool create_index(Index newIndex);
  ```

* 删除索引

  ```c++
  bool drop_index(string indexName);
  ```

* 初始化Catalog，读取table_catalog和index_catalog中所有模式信息

  ```c++
  void initial_catalog();
  ```

* 将内存中所有信息的写入文件table_catalog和index_catalog中

  ```c++
  void store_catalog();
  ```

* 打印内存中信息

  ```c++
  //打印内存中table和index的信息
  void show_catalog();
  
  //打印内存中table的信息
  void show_table();
  
  //打印内存中index的信息
  void show_index();
  ```

* 获得Table类对象的信息

  ```c++
  //获取整个table信息
  Table get_table(string tableName); 
  
  //获取该表primary key
  String get_primary_key(string tableName);
  
  //获取该表每个record的长度
  int get_row_length(string tableName);
  
  //获取该表record数
  int get_row_num(string tableName);
  
  //获取该表attribute个数
  int get_attribute_num(string tableName);
  ```

* 属性的判断函数，是则返回true

  ```c++
  //判断该字段是否为主键
  bool is_primary_key(string tableName, string attributeName);
  
  //判断该属字段名是否唯一
  bool is_unique(String tableName, string attributeName);
  
  //判断该字段是否建立索引
  boolean is_index_key(string tableName, string attributeName);
  ```

* 获得某个index或attribute的信息

  ```c++
  //获取某个字段的索引名
  string get_index_name(string tableName, string attributeName);
  
  //获取第i个字段名
  String get_attribute_name(String tableName, int i);
  
  //检查attributeVector中是否有特定attribute
  bool is_attribute_exist(vector<Attribute> attributeVector, string attributeName);
  
  //获取某个字段的编号，如果不存在则返回-1，并输出不存在提示
  int get_attribute_index(string tableName, string attributeName);
  
  //获取某个字段的类型，如果不存在则输出提示，并返回空类型（type为defult,size=0)
  FieldType get_attribute_type(string tableName, string attributeName);
  
  //获取第i个字段的类型，如果不存在则输出提示，并返回空类型（type为defult,size=0)
  string get_type(string tableName, int i);
  
  //获取某个字段的长度
  int get_length(string tableName, string attributeName);
  
  //获取第i个字段的长度
  int get_length(string tableName, int i);
  ```

* record数（表的行数）修改

  ```c++
  //对某个表增加一行
  void add_row_num(string tableName);
  
  //删除某个表删除num行
  void delete_row_num(String tableName, int num);
  ```

* 对索引进行操作

  ```c++
  //通过索引名获取该表的索引信息
  Index get_index(string indexName);
  
  //对某个index进行更新
  bool update_index_table(string indexName, Index tmpIndex);
  ```

#### *Catalog Manager 实现*

##### 1、类

* Index类

  ```c++
  //变量
  string indexName;
  string tableName;
  string attributeName;
  int rootNum;
  int blockNum = 0;
  
  //构造函数
  Index();
  Index(string indexName, string tableName, string attributeName);
  Index(string indexName, string tableName, string attributeName, int blockNum, int rootNum);
  ```

* attribute类

  ```c++
  //变量
  string attributeName; 
  bool isUnique;
  FieldType type; 
  
  //构造函数
  Attribute();
  Attribute(string attributeName, NumType type, int length, bool isUnique);
  Attribute(string attributeName, NumType type, bool isUnique);
  ```

* FieldType类

  ```c++
  //变量
  NumType type; //其中 enum NumType { CHAR, INT, FLOAT,DEFULT };
  int length; //length of type
  static const int CHARSIZE = 1;  	//1 byte for a char
  static const int INTSIZE = 4;   	//4 bytes for an integer
  static const int FLOATSIZE = 4; 	//4 bytes for a float number
  static const int DEFULTSIZE = 0; 	//0 bytes for null
  
  //构造函数
  FieldType(); //defult类型
  FieldType(NumType type);
  FieldType(NumType type, int length);
  
  //其它函数
  NumType get_type(); //获得该字段类型
  int get_length(); //获得该字段类型长度
  void set_type(NumType type); //设置该字段类型
  void set_length(int length); //设置该字段类型长度
  FieldType operator=(const FieldType& x); //重载赋值号
  ```

* Table类

  ```c++
  //变量
  string tableName;
  string primaryKey;
  vector<Attribute> attributeVector;
  vector<Index>	indexVector;
  int indexNum;
  int attributeNum;
  int rowNum;
  int rowLength;
  
  //构造函数
  Table();
  Table(string tableName, string primaryKey, vector<Attribute> attributeVector);
  Table(string tableName, string primaryKey, vector<Attribute> attributeVector, vector<Index> indexVector, int rowNum);
  ```

* CatalogManager类

  ```c++
  //变量
  map<string, Table> tables;
  map<string, Index> indexes;
  string tableFilename = "table_catalog";
  string indexFilename = "index_catalog";
  //函数部分在上述接口部分中已详细阐述其功能
  ```

* Address类

  B+树将索引类型映射到`CatalogManager`所提供的`Address`地址类，再由`RecordManager`调用`BufferManager`从硬盘中读取数据，而不是直接映射为数据块。本模块（CatalogManager）中没有用到，其它模块可以使用，具体见代码。

##### **2、函数具体实现**

