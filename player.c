/*name:音乐播放器
 *use :包含音乐的文件夹作为参数输入
 */
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
//窗口显示音乐的数量
#define    SIZE   5 

enum{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	UNKNOWN,
	ESC,
	ENTER,
};

void show_menu(int start, int index, int count, char *array[]);
int get_usrinput(int *start, int *index, int count);

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		perror("ivalid operation");
		return -1;
	}
	DIR *dir = NULL;
	//打开音乐目录
	dir = opendir(argv[1]);
	int a = 1024;
	char buffer[a];
	struct dirent *ent;
	int count = 0;
	//读取文件夹中的文件，直到读取完
	while(ent = readdir(dir))
		count++;
	//除去"."和".."这两个文件
	count -= 2;
	//从新让流指针指向文件夹的起始地址
	rewinddir(dir);
	char *array[count];
	int i = 0;
	while(i != count)
	{	
		ent = readdir(dir);
		if(!strcmp(ent->d_name,".") || !strcmp(ent->d_name,".."))
			continue;
		//保存文件名指针到数组中
		array[i] = ent->d_name;
		i++;
	}
	int index = 0;
	int start = 0;
	int tmp;
	int pid;
	//隐藏光标
	printf("\033[?25l");
	//更改工作目录
	chdir(argv[1]);
	while(1)
	{
		show_menu(start, index, count, array);
		tmp = get_usrinput(&start, &index, count);
		if(tmp == ESC)
		{
			//显示光标
			printf("\033[?25h");
			//退出前关闭掉所有进程
			if(pid != 0)
				kill(pid, 9);
			break;
		}
		if(tmp == ENTER)
		{
			//切换歌曲是关闭原来进程
			if(pid != 0)
				kill(pid, 9);
			//创建新进程
			pid = fork();
			if(pid == 0)
			{
				//execl会占用掉该进程所有的直言
				execl("/usr/local/bin/mplayer" ,"",array[start+index], "-slave", "-quiet", NULL) ;
				return 0;
			}
			usleep(100);
			show_menu(start, index, count, array);
		}
	}	
	return 0;
}

void show_menu(int start, int index, int count, char *array[])
{
	int i;
	int tmp;
	system("clear");
	for(i = start; i < start+SIZE; i++)
	{
		tmp = (i+count)	% count;
		if(i == index+start)
			printf("\033[%d;%dH\033[31m%s\033[0m", 5+i-start, 20, array[tmp]);
			//printf("\033[%d;%dH\033[30m\033[47m%s\033[0m", 5+i-start, 20, array[tmp]);
		else
			//printf("\033[%d;%dH\033[37m\033[40m%s\033[0m", 5+i-start, 20, array[tmp]);
			printf("\033[%d;%dH\033[34m%s\033[0m", 5+i-start, 20, array[tmp]);
	}
	fflush(stdout);
}

static int _get_usrinput()
{
	int type = UNKNOWN;	
	char ch;
	system("stty -echo -icanon");
	ch = getchar();
	if(ch == 'q')
		type = ESC;
	if(ch == '\n')
		type = ENTER;
	if((ch == '\033') && (getchar() == '['))
	{
		ch = getchar();
		switch (ch)
		{
			case 'A':type = UP;break;
			case 'B':type = DOWN;break;
			case 'C':type = RIGHT;break;
			case 'D':type = LEFT;break;
		}
	}
	system("stty echo icanon");
	return type;
}

int get_usrinput(int *start, int *index, int count)
{
	int type ; 
	type = _get_usrinput();
	switch (type)
	{
		//index 0-SIZE
		case UP:
				if(*index > 0)
					//防止index小于0
					*index = (*index + SIZE -1) % SIZE;
				else
					//防止start小于0,小于0时，跑到末尾
					*start = (*start + count -1) % count;
				break;
		case DOWN:
				if(*index < SIZE -1)
					//防止index大于index最大值
					*index = (*index + 1) %SIZE;
				else
					//start大于最大值，从头开始
					*start = (*start + 1)%count;
				break;	
		case ESC: return ESC;
		case ENTER: return ENTER;
	}
	return UNKNOWN;
}
