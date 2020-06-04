### notice：
index部分最重要的bplustree已经测试完毕（int，float，string都可以使用）。具体流程是，文件的第一个block存的是bplustree的信息，包括root的block是哪个，第一个leafnode的block是哪个。当需要使用index的时候，indexmanager会让buffer把需要的block拿过来。每次访问一个节点都需要拿相应的block来。（原先实现的是直接把整棵树读到内存里，不过这样确实不太好233）  

IndexManager类部分的接口要添加的话，是比较简单的，我只需要调用bplustree的接口就行了，所以如果有添加接口的需要，可以直接找我。

其次，IndexManager类的具体调试应该放到整合的时候，那个时候建了分支我在具体测试一下。

### indexmanager已有的接口
new实例： IndexManager(BufferManager* bm)

type: 0 -> int, -1 -> float, >0 -> string(type的值为string长度)  
void create_index(const std::string &db_name, const std::string &table_name, const std::string &index_name, int type);

寻找特定值的record的block_id。其中data用string存（不管是int还是float），然后再type中给出data的类型。返回是否找到，如果找到了，会将所要的block id放到block_id里  
bool find_element(const std::string &db_name, const std::string &table_name,
		const std::string &index_name, const std::string &data, std::vector<int> &block_id, int type);

参数和上面含义一样。返回是否insert成功。    
bool insert_index(const std::string &db_name, const std::string &table_name,
		const std::string &index_name, const std::string &data, int block_id, int type);

参数和上面含义一样。返回是否delete成功。    
bool delete_index(const std::string &db_name, const std::string &table_name,
		const std::string &index_name, const std::string &data, int type);

丢弃一个index。  
void drop_index(const std::string &table_name, const std::string &index_name, int type);