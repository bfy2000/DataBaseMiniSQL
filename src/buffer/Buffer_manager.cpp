#ifndef DEBUG
#define DEBUG
#include"buffer.h"
#endif // !DEBUG
#include<cstring>
using namespace std;


int main()
{
	string db_name("database");
	string filename1("table1");
	string filename2("table2");
	string filename3("table3");

	//在filelist中插入一个名为filename1，type=0（表），0条记录，记录长20的文件，并返回这个文件的fileinfo
	//这里只是用例，使用的一些具体参数要从catalog获得
	//filelist主要用于控制文件数量，防止同时处理太多文件，上限为5。
	FileInfo file1 = add_file_to_list(filename1, 0, 0, 20);
	//下面两个类似上面
	FileInfo file2 = add_file_to_list(filename2, 0, 0, 30);
	FileInfo file3 = add_file_to_list(filename2, 0, 0, 40);

	//获取filelist中已有的文件可以用这个函数
	//FileInfo file1 = get_file_info(filename1, 0);

	//申请一个空块
	BlockInfo emptyblock = findBlock();

	//随便写点什么
	char ss[30] = "abcdefghijklmnopqrstuvwxyz";

	//把这个空块放到file1下面
	add_block_to_file(emptyblock, file1);
	emptyblock->dirtyBit = 1;
	emptyblock->isfree = 0;
	emptyblock->blockNum = 0;
	
	write_to_block(emptyblock, ss);

	closefile(db_name, file1);

	return 0;
}