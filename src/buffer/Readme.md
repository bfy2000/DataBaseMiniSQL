6.3 Update

1.现在调用get_block_info时可以保证找到的block被挂在对应file的下面。
  其他情况下不建议自行调用add_block_to_file将block挂在file下面，因为有可能破坏block链表的结构。
  
2.当把文件加入filelist时请自行根据catalog传入recordAmount和recordLength
  recordAmount可以传0，在buffermanager中暂时用不到这个信息，但是对表文件，recordLength需要如实传参。
  
3.blockinfo的isfree是一个标记，可能不常用，当想要将某个块标为空块时可以将isfree改为1，以备后续调用。
  同时请记得让fileinfo的freeNum+1。
  
4.可以调用lock_block及unlock_block对一个块进行加锁或解锁，也可以直接修改blockinfo的lock值

5.增加了writeToIndex和readData两个接口，它们是根据recordManager部分的需求写的。
  由于我不太清楚indexManager文件的具体结构，它们被indexManager调用时可能会出现问题。
  
6.推荐的调用方式是通过get_block_info从获取文件的某个块，它会先检查buffer中是否有对应块，
  没有的话会从磁盘中读取。写入方面建议通过closefile函数，它会把某个文件信息移出filelist，并
  将所有在这个文件的块链表中的，非空且dirtybit为1的块，写入磁盘。
  
7.当需要把某个块的内容写到磁盘时，请将该块的dirtybit设为1。
