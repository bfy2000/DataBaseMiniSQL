#ifndef DEBUG
#define DEBUG
#include"buffer.h"
#endif // !DEBUG


string rootdir = ".\\minisql\\";

FileInfo filelist = nullptr;
int fileNum = 0;


//寻找空块
BlockInfo findBlock()
{
	static int i = 0;//现有block数量
	FileInfo fileptr = filelist;
	BlockInfo resblock = nullptr;
	while (fileptr)
	{
		BlockInfo blockptr = fileptr->firstBlock;
		if (fileptr->freeNum == 0)//这个文件下面没有空块了
		{
			fileptr = fileptr->next;
			continue;
		}
			
		while (blockptr)//这个文件下面有空块
		{
			if (blockptr->isfree)//找到一个空块
			{
				resblock = blockptr;
				fileptr->freeNum--;//空块数量-1
				blockptr->isfree = false;
				remove_block_from_file(resblock);//把这个块从原文件中移出
				break;
			}
			blockptr = blockptr->next;
		}
		if (!resblock)
			cout << "findBlock::freeNum record may be wrong." << endl;
		else 
			return resblock;

		fileptr = fileptr->next;
	}
	//已经没有空块了
	//还能申请空块
	if (i < MAX_BLOCK)
	{
		resblock = new struct blockInfo;
		
		if (resblock)
		{
			resblock->call_times = 0;
			resblock->charNum = 0;
			resblock->dirtyBit = 0;
			resblock->file = nullptr;
			resblock->isfree = 1;
			resblock->lock = 0;
			resblock->next = nullptr;
			resblock->cBlock = new char[BLOCK_SIZE];
			if (resblock->cBlock)
				return resblock;
		}
	}

	//不能申请空块了，用LRU遍历所有块，替换掉一个
	int time = 0x7fffffff;
	fileptr = filelist;
	while (fileptr)
	{
		BlockInfo blockptr = fileptr->firstBlock;
		while (blockptr)
		{
			if (blockptr->call_times < time && !blockptr->lock)//用LRU，没被锁的可以替换
			{
				time = blockptr->call_times;
				resblock = blockptr;
			}
			blockptr = blockptr->next;
		}
		fileptr = fileptr->next;
	}
	resblock->call_times = 1;
	remove_block_from_file(resblock);
	return resblock;

}

//在filelist中找一个文件
FileInfo get_file_info(string fileName, int fileType)
{
	if (!filelist)return nullptr;
	FileInfo fileptr = filelist;

	//找文件
	while (fileptr)
	{
		if (fileptr->fileName == fileName && fileptr->type == fileType)
			return fileptr;
		fileptr = fileptr->next;
	}

	//文件不存在
	cout << "get_file_info::No such file" << endl;
	return nullptr;
}

//在buffer中查找一个块
BlockInfo get_block_in_file(FileInfo file, int blockNum)
{
	if (!file)
		return nullptr;

	BlockInfo blockptr = file->firstBlock;
	
	while (blockptr)
	{
		if (blockptr->blockNum == blockNum)
			return blockptr;

		blockptr = blockptr->next;
	}
	return nullptr;
}

//在buffer中把一个块移出对应文件
void remove_block_from_file(BlockInfo block_to_remove)
{
	FileInfo file = block_to_remove->file;
	BlockInfo last_block = nullptr;
	BlockInfo blockptr = file->firstBlock;

	while (blockptr)//在该文件中寻找对应块
	{
		if (blockptr->next == block_to_remove)
			break;
		last_block = blockptr;
		blockptr = blockptr->next;
	}
	if (blockptr)//找到了
	{
		if (last_block)
			file->firstBlock = blockptr->next;
		else
			last_block->next = blockptr->next;

	}
	else
		cout << "remove_block_from_file::Cannot find this block" << endl;
	return;
}

//缓冲区操作：把一个块挂到文件上
void add_block_to_file(BlockInfo block,FileInfo file)
{
	block->file = file;
	block->next = file->firstBlock;
	file->firstBlock = block;
	return;
}

//从磁盘读一个块
BlockInfo read_block_from_disk(FileInfo file, string db_name, int blocknum, BlockInfo tmpblock)
{
	if (!tmpblock)//没提供块，先找一个
	{
		tmpblock = findBlock();
		tmpblock->blockNum = blocknum;
		tmpblock->isfree = 0;
	}
	//打开文件
	string filepath = rootdir + db_name;
	if (file->type == 0)	//表
		filepath += "\\table\\";
	else					//索引
		filepath += "\\index\\";
	filepath += file->fileName + ".txt";

	ifstream infile;
	infile.open(filepath);

	if (!infile.is_open())
	{
		cout << "readblock::no such file!" + filepath << endl;
		return nullptr;
	}

	//读取数据
	int offset = BLOCK_SIZE * (blocknum);
	infile.seekg(offset, ios::beg);
	if(!tmpblock->cBlock)
		tmpblock->cBlock = new char[BLOCK_SIZE];
	infile.read(tmpblock->cBlock,BLOCK_SIZE);
	infile.close();
	return tmpblock;
}

//把一个块写到磁盘
void write_block_to_disk(string db_name, BlockInfo block)
{
	if (!block->dirtyBit || block->isfree || block->charNum == 0)
		return;

	FileInfo file = block->file;
	if (!file)
	{
		cout << "write_block_to_disk::No such file opened!" << endl;
		return;
	}

	//打开文件
	string filepath = rootdir + db_name;
	if (file->type == 0)	//表
		filepath += "\\table\\";
	else					//索引
		filepath += "\\index\\";
	filepath += file->fileName + ".txt";

	//读取数据
	int offset = BLOCK_SIZE * (block->blockNum);
	ofstream ofile;
	ofile.open(filepath);
	ofile.seekp(offset, ios::beg);
	string str(block->cBlock);
	ofile.write(block->cBlock, str.length());
	ofile.close();

}

//从文件中读取某个块
BlockInfo get_block_info(string db_name, string table_name, int file_type, int BlockNum)
{
	
	FileInfo fileptr = get_file_info(table_name, file_type);
	if (!fileptr)
	{
		cout << "get_block_info::No such file!" << endl;
		return nullptr;//没找到对应文件
	}
	

	BlockInfo blockptr = get_block_in_file(fileptr, BlockNum);
	
	if (blockptr && blockptr->lock)//块是否被锁住
	{
		cout << "get_block_info::The block has been locked" << endl;
		return blockptr;
	}

	if (blockptr)//找到了blocknum的块
	{
		if (!blockptr->isfree)//不是空块，直接返回
		{
			blockptr->call_times++;
			return blockptr;
		}
		else//是空块，先从磁盘中读取出来再返回
		{
			blockptr = read_block_from_disk(fileptr, db_name, BlockNum, blockptr);
			blockptr->call_times = 1;
			return blockptr;
		}
	}


	//文件在buffer中没有编号为blocknum的块
	cout << "get_block_info::can't find this block in buffer" << endl;
	blockptr = read_block_from_disk(fileptr, db_name, BlockNum, nullptr);
	blockptr->call_times = 1;
	return blockptr;
}

//关闭文件
void closefile(string db_name, FileInfo file)
{
	if (file == nullptr)
	{
		cout << "closefile::No such file!" << endl;
		return;
	}
	BlockInfo blockptr = file->firstBlock;
	BlockInfo tmpblock = blockptr;
	fileNum--;
	while (blockptr)
	{
		write_block_to_disk(db_name, blockptr);
		tmpblock = blockptr;
		blockptr = blockptr->next;
		delete[] tmpblock->cBlock;
		delete tmpblock;
	}

	//从文件表中删除
	FileInfo fileptr = filelist;
	if (fileptr == file)//filelist头结点
	{
		filelist = file->next;
		delete file;
		return;
	}

	while (fileptr)
	{
		if (fileptr->next == file)
		{
			fileptr->next = file->next;
			delete file;
			return;
		}
		fileptr = fileptr->next;
	}
}

//把一个文件加入filelist
FileInfo add_file_to_list(string table_name, int type, int recordAmount, int recordlen)
{
	if (fileNum >= MAX_ACTIVE_FILE)
	{
		cout << "Cannot add more file to list" << endl;
	}
	FileInfo newfile = new struct fileInfo;
	newfile->next = filelist;
	filelist = newfile;

	newfile->fileName = table_name;
	newfile->firstBlock = nullptr;
	newfile->freeNum = 0;
	newfile->recordAmount = recordAmount;
	newfile->recordLength = recordlen;
	newfile->type = type;

	fileNum++;
	return newfile;
}

//锁住一个块
void lock_block(BlockInfo block)
{
	if (block == nullptr)
	{
		cout << "lock_block::Block doesn't exist" << endl;
		return;
	}
	block->lock = 1;
}

//解锁一个块
void unlock_block(BlockInfo block)
{
	if (block == nullptr)
	{
		cout << "unlock_block::Block doesn't exist" << endl;
		return;
	}
	block->lock = 0;
}

void write_to_block(BlockInfo block, char to_write[])
{
	block->charNum += strlen(to_write);
	strcpy_s(block->cBlock, BLOCK_SIZE, to_write);
}

