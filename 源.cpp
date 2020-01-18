#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>      // 引用图形库头文件
#include <conio.h>

#define MaxLenOfName 101

typedef struct node
{
	char* name;	//节点名
	int n_children;	//子节点个数
	int level;	//记录该节点在多叉树中的层数
	int width;	//记录横坐标位置
	struct node* parent;	//指向父节点
	struct node** children;	// 指向其自身的子节点，children一个数组，该数组中的元素时node_t*指针
}NODE;	   //定义多叉书结点结构体  

typedef struct queue_t
{
	NODE** array; // array是个数组，其内部元素为NODE*型指针
	int    head;  // 队列的头
	int    tail;  // 队列的尾
	int    num;   // 队列中元素的个数
	int    size;  // 队列的大小
} QUEUE;		// 实现一个队列，用于后续操作

// 内存分配函数
void* util_malloc(int size)
{
	void* ptr = malloc(size);

	if (ptr == NULL) // 如果分配失败，则终止程序
	{
		printf("Memory allocation error!\n");
		exit(EXIT_FAILURE);
	}

	// 分配成功，则返回
	return ptr;
}

// 字符串赋值函数
// 对strdup函数的封装，strdup函数直接进行字符串赋值，不用对被赋值指针分配空间
// 比strcpy用起来方便，但其不是标准库里面的函数
// 用strdup函数赋值的指针，在最后也是需要free掉的
char* util_strdup(char* src)
{
	char* dst = _strdup(src);

	if (dst == NULL) // 如果赋值失败，则终止程序
	{
		printf("Memroy allocation error!\n");
		exit(EXIT_FAILURE);
	}

	// 赋值成功，返回
	return dst;
}

// 对fopen函数封装
FILE* util_fopen(char* name, char* access)
{
	FILE* fp = fopen(name, access);

	if (fp == NULL) // 如果打开文件失败，终止程序
	{
		printf("Error opening file %s!\n", name);
		exit(EXIT_FAILURE);
	}

	// 打开成功，返回
	return  fp;
}

// 实现队列的操作
QUEUE* QUEUEinit(int size)
{
	QUEUE* qp;

	qp = (QUEUE*)util_malloc(sizeof(QUEUE));
	qp->size = size;
	qp->head = qp->tail = qp->num = 0;
	qp->array = (NODE**)util_malloc(size * sizeof(NODE*));

	return qp;
}

// 入队列
int QUEUEenqueue(QUEUE* qp, NODE* data)
{
	if (qp == NULL || qp->num >= qp->size) // qp未初始化或已满
	{
		return 0; // 入队失败
	}

	qp->array[qp->tail] = data; // 入队，tail一直指向最后一个元素的下一个位置
	qp->tail = (qp->tail + 1) % (qp->size); // 循环队列
	++qp->num;
	return 1;
}

// 出队列
int QUEUEdequeue(QUEUE* qp, NODE** data_ptr)
{
	if (qp == NULL || qp->num <= 0) // qp未初始化或队列内无元素
	{
		return 0;
	}

	*data_ptr = qp->array[qp->head]; // 出队
	qp->head = (qp->head + 1) % (qp->size); // 循环队列
	--qp->num;

	return 1;
}

// 检测队列是否为空
int QUEUEempty(QUEUE* qp)
{
	if (qp == NULL || qp->num <= 0)
	{
		return 0;
	}

	return 1;
}

// 销毁队列
void QUEUEdestroy(QUEUE* qp)
{
	free(qp->array);
	free(qp);
}
// 以上是队列的有关操作实现

// 生成多叉树节点
NODE* create_node()
{
	NODE* q;

	q = (NODE*)util_malloc(sizeof(NODE));
	q->n_children = 0;
	q->level = 0;
	q->width = 320;
	q->children = NULL;
	q->parent = NULL;

	return q;
}

// 按节点名字查找
NODE* search_node_r(char name[MaxLenOfName], NODE* head)
{
	NODE* temp = NULL;
	int i = 0;

	if (head != NULL)
	{
		if (strcmp(name, head->name) == 0) // 如果名字匹配
		{
			temp = head;
		}
		else // 如果不匹配，则查找其子节点
		{
			for (i = 0; i < head->n_children && temp == NULL/*如果temp不为空，则结束查找*/; ++i)
			{
				temp = search_node_r(name, head->children[i]); // 递归查找子节点
			}
		}
	}

	return temp; // 将查找到的节点指针返回，也有可能没有找到，此时temp为NULL
}

// 从文件中读取多叉树数据，并建立多叉树
void read_file(NODE** head, char* filename)
{
	NODE* temp = NULL;
	int i = 0, n = 0;
	char name[MaxLenOfName], child[MaxLenOfName];
	FILE* fp;

	fp = util_fopen(filename, "r"); // 打开文件

	while (fscanf(fp, "%s %d", name, &n) != EOF) // 先读取节点名字和当前节点的子节点个数
	{
		if (*head == NULL) // 若为空
		{
			temp = *head = create_node();   // 生成一个新节点
			temp->name = util_strdup(name); // 赋名
		}
		else
		{
			temp = search_node_r(name, *head); // 根据name找到节点
											   // 这里默认数据文件是正确的，一定可以找到与name匹配的节点
											   // 如果不匹配，那么应该忽略本行数据
		}
		// 找到节点后，对子节点进行处理
		temp->n_children = n;
		temp->children = (NODE**)malloc(n * sizeof(NODE*));
		if (temp->children == NULL) // 分配内存失败
		{
			fprintf(stderr, "Dynamic allocation error!\n");
			exit(EXIT_FAILURE);
		}

		// 如果分配成功，则读取后面的子节点，并保存
		for (i = 0; i < n; ++i)
		{
			fscanf(fp, "%s", child); // 读取子节点
			temp->children[i] = create_node(); // 生成子节点
			temp->children[i]->name = util_strdup(child); // 读子节点赋名
			temp->children[i]->parent = temp;
			temp->children[i]->level = temp->level + 1; // 对子节点深度进行赋值：父节点深度加1
			if (temp->n_children % 2 == 0)
			{
				temp->children[i]->width = temp->width + (i - (temp->n_children / 2)) * 80 + 40;
			}
			else
			{
				temp->children[i]->width = temp->width + (i - (temp->n_children / 2)) * 80;
			}
		}
	}

	// 读取完毕，关闭文件
	fclose(fp);
}

//层序遍历_非递归实现
int  sequence(NODE* head)	
{
	NODE* p = NULL;
	QUEUE* q = NULL; // 定义一个队列
	QUEUE* myQ = NULL;
	int i = 0;

	q = QUEUEinit(100); // 将队列初始化大小为100
	myQ = QUEUEinit(100);

	QUEUEenqueue(q, head);	// 将根节点入队列

	while (QUEUEempty(q) != 0) // 如果队列q不为空
	{
		QUEUEdequeue(q, &p); // 出队列
		for (i = 0; i < p->n_children; ++i)
		{
			QUEUEenqueue(q, p->children[i]);      // 将子节点入队列
		}
		QUEUEenqueue(myQ, p);
	}

	initgraph(640, 480);
	setbkcolor(LIGHTGRAY);
	cleardevice();

	int num = 0;
	while (QUEUEempty(myQ))
	{
		QUEUEdequeue(myQ, &p);
		setlinestyle(PS_SOLID, 5);
		circle(p->width, p->level * 80 + 40, 30);
		LOGFONT f;
		gettextstyle(&f);                     // 获取当前字体设置
		f.lfHeight = 48;                      // 设置字体高度为 48
		_tcscpy(f.lfFaceName, _T("黑体"));    // 设置字体为“黑体”(高版本 VC 推荐使用 _tcscpy_s 函数)
		f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
		settextstyle(&f);                     // 设置字体样式
		outtextxy(p->width-10, p->level * 80 + 18, *(p->name));
		if (0 != num)
		{
			line(p->parent->width, p->parent->level * 80 + 68, p->width, p->level * 80 + 12);
		}
		num++;
		Sleep(1000);
	}
	_getch();              // 按任意键继续
	closegraph();          // 关闭绘图窗口

	QUEUEdestroy(q); // 消毁队列
	QUEUEdestroy(myQ);
	
	return num;
}

//先序遍历_递归实现
void preorder (NODE* head, int *num)	
{
	if (head)
	{
		//输出部分
		setlinestyle(PS_SOLID, 5);
		circle(head->width, head->level * 80 + 40, 30);
		LOGFONT f;
		gettextstyle(&f);                     // 获取当前字体设置
		f.lfHeight = 48;                      // 设置字体高度为 48
		_tcscpy(f.lfFaceName, _T("黑体"));    // 设置字体为“黑体”
		f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
		settextstyle(&f);                     // 设置字体样式
		outtextxy(head->width -10, head->level * 80 + 18, *(head->name));
		if (1 != *num)
		{
			line(head->parent->width, head->parent->level * 80 + 68, head->width, head->level * 80 + 12);
		}
		Sleep(1000);
		for (int i = 0; i < head->n_children; i++)
		{
			(*num)++;
			preorder (head->children[i], num);
		}
	}
}

//后序遍历
void epilogue(NODE* head, int* num)
{
	if (head)
	{
		for (int i = 0; i < head->n_children; i++)
		{
			(*num)++;
			epilogue(head->children[i], num);
			//输出部分
			setlinestyle(PS_SOLID, 5);
			circle(head->children[i]->width, head->children[i]->level * 80 + 40, 30);
			LOGFONT f;
			gettextstyle(&f);                     // 获取当前字体设置
			f.lfHeight = 48;                      // 设置字体高度为 48
			_tcscpy(f.lfFaceName, _T("黑体"));    // 设置字体为“黑体”
			f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
			settextstyle(&f);                     // 设置字体样式
			outtextxy(head->children[i]->width - 10, head->children[i]->level * 80 + 18, *(head->children[i]->name));
			if (1 != *num)
			{
				line(head->children[i]->parent->width, head->children[i]->parent->level * 80 + 68, head->children[i]->width, head->children[i]->level * 80 + 12);
			}
			Sleep(1000);
		}
		setlinestyle(PS_SOLID, 5);
		circle(head->width, head->level * 80 + 40, 30);
		LOGFONT f;
		gettextstyle(&f);                     // 获取当前字体设置
		f.lfHeight = 48;                      // 设置字体高度为 48
		_tcscpy(f.lfFaceName, _T("黑体"));    // 设置字体为“黑体”
		f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
		settextstyle(&f);                     // 设置字体样式
		outtextxy(head->width - 10, head->level * 80 + 18, *(head->name));
	}
}

// 消毁树
void free_tree_r(NODE* head)
{
	int i = 0;
	if (head == NULL)
	{
		return;
	}

	for (i = 0; i < head->n_children; ++i)
	{
		free_tree_r(head->children[i]);
	}

	free(head->name);
	// free(head->children); // 消毁子节点指针数组
	free(head);
}

int main(int argc, char* argv[])
{
	NODE* head = NULL;
	int flag = 0;

	read_file(&head, "data.txt");
	while (1)
	{
		printf("1.层序遍历，2.先序遍历, 3.后序遍历, 4.退出\n");
		scanf("%d", &flag);
		if (1 == flag)
		{
			int num = sequence(head);
			printf("执行完毕！\n");
			printf("共有%d个节点\n\n", num);
		}
		else if (2 == flag)
		{
			initgraph(640, 480);
			setbkcolor(LIGHTGRAY);
			cleardevice();
			int num = 1;
			preorder(head, &num);
			_getch();              // 按任意键继续
			closegraph();          // 关闭绘图窗口
			printf("执行完毕！\n");
			printf("共有%d个节点\n\n", num);
		}
		else if (3 == flag)
		{
			initgraph(640, 480);
			setbkcolor(LIGHTGRAY);
			cleardevice();
			int num = 1;
			epilogue(head, &num);
			_getch();              // 按任意键继续
			closegraph();          // 关闭绘图窗口
			printf("执行完毕！\n");
			printf("共有%d个节点\n\n", num);
		}
		else if (4 == flag)
		{
			printf("程序退出");
			break;
		}
		else
		{
			printf("输入有误，请重新输入！\n");
		}
	}
	
	free_tree_r(head);
	return EXIT_SUCCESS;
	
}