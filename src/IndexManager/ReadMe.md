### 2020/06/26
按照要求更改了。

### old：
已经和buffer耦合成功，需要调用index的模块，可以根据我的接口改改。

### notice：
index部分最重要的bplustree已经测试完毕（int，float，string都可以使用）。具体流程是，文件的第一个block存的是bplustree的信息，包括root的block是哪个，第一个leafnode的block是哪个。当需要使用index的时候，indexmanager会让buffer把需要的block拿过来。每次访问一个节点都需要拿相应的block来。（原先实现的是直接把整棵树读到内存里，不过这样确实不太好233）  

### indexmanager已有的接口
new实例： IndexManager(BufferManager* bm)

成功返回SUCCESS，不存在的类型返回WRONG_TYPE（下面所有函数都会有这个类型的判断，不存在的类型都会返回WRONG_TYPE）  
Result create_index(const std::string &db_name, const std::string &table_name, const std::string &attribute_name, FieldType type);

寻找特定值的record的block_id。其中data用string存（不管是int还是float），然后再type中给出data的类型。返回是否找到，如果找到了，会将所要的block id放到block_id里。block_id需要clear  
没找到返回NO_SUCH_ELEMENT，失败（没有该index）返回NO_INDEX，成功返回SUCCESS  
Result find_element(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id);

参数和上面含义一样。    
失败（没有该index）返回NO_INDEX，成功返回SUCCESS  
Result insert_index(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, int block_id);

参数和上面含义一样。返回是否delete成功。    
失败（没有该index）返回NO_INDEX，成功返回SUCCESS  
Result delete_index(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data);

丢弃一个index。
失败（没有该index）返回NO_INDEX，成功返回SUCCESS  
Result drop_index(const std::string db_name, const std::string &table_name, const std::string &attribute_name, FieldType type);

查找小于或者小于等于某个值所对应的所有record的对应block_id。其中isEqual为1表示小于等于，0为小于。使用前，vector需要clear。  
void less_than(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name,const std::string &data, std::vector<int> &block_id, int type, int isEqual);

查找大于或者大于等于某个值所对应的所有record的对应block_id。其中isEqual为1表示大于等于，0为大于。使用前，vector需要clear。  
没有该index返回NO_INDEX，成功返回SUCCESS。  
Result greater_than(const std::string &db_name, const std::string &table_name,
		const std::string &attribute_name, FieldType type, const std::string &data, std::vector<int> &block_id, int isEqual);
