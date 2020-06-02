#ifndef DEBUG
#define DEBUG
#include"buffer.h"
#endif // !DEBUG
#include<cstring>
using namespace std;


int main()
{
	//建立目录
	string folderPath = ".\\minisql";
	string command;
	command = "mkdir " + folderPath;
	system(command.c_str());

	folderPath += "\\database";
	command += "\\database";
	system(command.c_str());

	string indexdir = folderPath + "\\index";
	command = "mkdir " + indexdir;
	system(command.c_str());

	string tabledir = folderPath + "\\table";
	command = "mkdir " + tabledir;
	system(command.c_str());

	string db_name("database");
	string filename1("table1");
	string filename2("table2");
	string filename3("table3");

	//在filelist中插入一个名为filename1，type=0（表），0条记录，记录长20的文件，并返回这个文件的fileinfo
	//这里只是用例，使用的一些具体参数要从catalog获得
	//filelist主要用于控制文件数量，防止同时处理太多文件，上限为5。
	FileInfo file1 = add_file_to_list(filename1, 0, 0, 20);

	//获取filelist中已有的文件可以用这个函数
	//FileInfo file1 = get_file_info(filename1, 0);

	//申请一个空块
	BlockInfo emptyblock = findBlock();

	//随便写点什么
	char ss[300] = "abcdefghijklmnopqrstuvwxyz\n";
	char rr[300] = "xyxyxyxyxyxyxyxyxyxyxyxy\n";
	//把这个空块放到file1下面
	add_block_to_file(emptyblock, file1);
	emptyblock->dirtyBit = 1;
	emptyblock->isfree = 0;
	emptyblock->blockNum = 0;

	//write是以覆盖方式写块的内容，内容长度限制为4096
	write_to_block(emptyblock, ss);
	//add是以追加的方式写块的内容，长度限制4096
	for (int i = 0;i < 100;i++)
		add_to_block(emptyblock, ss);
		

	//直接写入磁盘，不建议直接用这个函数，最好用closefile写
	write_block_to_disk(db_name, emptyblock);

	emptyblock->blockNum = 1;
	write_to_block(emptyblock, rr);
	//for (int i = 0;i < 100;i++)
		add_to_block(emptyblock, rr);
	//closefile会把文件在buffer中所有块都写进去，并delete掉这些块，然后将文件从filelist中移除
	closefile(db_name, file1);


	file1 = add_file_to_list(filename1, 0, 0, 20);
	//此时之前的emptyblock已经被delete了，因此重新分配
	emptyblock = findBlock();
	//读file1的1号块
	emptyblock = read_block_from_disk(file1, db_name, 1, emptyblock);
	cout << strlen(emptyblock->cBlock) << endl;
	closefile(db_name, file1);
	
	

	system("pause");
	return 0;
}
