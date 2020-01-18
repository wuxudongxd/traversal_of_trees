#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>      // ����ͼ�ο�ͷ�ļ�
#include <conio.h>

#define MaxLenOfName 101

typedef struct node
{
	char* name;	//�ڵ���
	int n_children;	//�ӽڵ����
	int level;	//��¼�ýڵ��ڶ�����еĲ���
	int width;	//��¼������λ��
	struct node* parent;	//ָ�򸸽ڵ�
	struct node** children;	// ָ����������ӽڵ㣬childrenһ�����飬�������е�Ԫ��ʱnode_t*ָ��
}NODE;	   //����������ṹ��  

typedef struct queue_t
{
	NODE** array; // array�Ǹ����飬���ڲ�Ԫ��ΪNODE*��ָ��
	int    head;  // ���е�ͷ
	int    tail;  // ���е�β
	int    num;   // ������Ԫ�صĸ���
	int    size;  // ���еĴ�С
} QUEUE;		// ʵ��һ�����У����ں�������

// �ڴ���亯��
void* util_malloc(int size)
{
	void* ptr = malloc(size);

	if (ptr == NULL) // �������ʧ�ܣ�����ֹ����
	{
		printf("Memory allocation error!\n");
		exit(EXIT_FAILURE);
	}

	// ����ɹ����򷵻�
	return ptr;
}

// �ַ�����ֵ����
// ��strdup�����ķ�װ��strdup����ֱ�ӽ����ַ�����ֵ�����öԱ���ֵָ�����ռ�
// ��strcpy���������㣬���䲻�Ǳ�׼������ĺ���
// ��strdup������ֵ��ָ�룬�����Ҳ����Ҫfree����
char* util_strdup(char* src)
{
	char* dst = _strdup(src);

	if (dst == NULL) // �����ֵʧ�ܣ�����ֹ����
	{
		printf("Memroy allocation error!\n");
		exit(EXIT_FAILURE);
	}

	// ��ֵ�ɹ�������
	return dst;
}

// ��fopen������װ
FILE* util_fopen(char* name, char* access)
{
	FILE* fp = fopen(name, access);

	if (fp == NULL) // ������ļ�ʧ�ܣ���ֹ����
	{
		printf("Error opening file %s!\n", name);
		exit(EXIT_FAILURE);
	}

	// �򿪳ɹ�������
	return  fp;
}

// ʵ�ֶ��еĲ���
QUEUE* QUEUEinit(int size)
{
	QUEUE* qp;

	qp = (QUEUE*)util_malloc(sizeof(QUEUE));
	qp->size = size;
	qp->head = qp->tail = qp->num = 0;
	qp->array = (NODE**)util_malloc(size * sizeof(NODE*));

	return qp;
}

// �����
int QUEUEenqueue(QUEUE* qp, NODE* data)
{
	if (qp == NULL || qp->num >= qp->size) // qpδ��ʼ��������
	{
		return 0; // ���ʧ��
	}

	qp->array[qp->tail] = data; // ��ӣ�tailһֱָ�����һ��Ԫ�ص���һ��λ��
	qp->tail = (qp->tail + 1) % (qp->size); // ѭ������
	++qp->num;
	return 1;
}

// ������
int QUEUEdequeue(QUEUE* qp, NODE** data_ptr)
{
	if (qp == NULL || qp->num <= 0) // qpδ��ʼ�����������Ԫ��
	{
		return 0;
	}

	*data_ptr = qp->array[qp->head]; // ����
	qp->head = (qp->head + 1) % (qp->size); // ѭ������
	--qp->num;

	return 1;
}

// �������Ƿ�Ϊ��
int QUEUEempty(QUEUE* qp)
{
	if (qp == NULL || qp->num <= 0)
	{
		return 0;
	}

	return 1;
}

// ���ٶ���
void QUEUEdestroy(QUEUE* qp)
{
	free(qp->array);
	free(qp);
}
// �����Ƕ��е��йز���ʵ��

// ���ɶ�����ڵ�
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

// ���ڵ����ֲ���
NODE* search_node_r(char name[MaxLenOfName], NODE* head)
{
	NODE* temp = NULL;
	int i = 0;

	if (head != NULL)
	{
		if (strcmp(name, head->name) == 0) // �������ƥ��
		{
			temp = head;
		}
		else // �����ƥ�䣬��������ӽڵ�
		{
			for (i = 0; i < head->n_children && temp == NULL/*���temp��Ϊ�գ����������*/; ++i)
			{
				temp = search_node_r(name, head->children[i]); // �ݹ�����ӽڵ�
			}
		}
	}

	return temp; // �����ҵ��Ľڵ�ָ�뷵�أ�Ҳ�п���û���ҵ�����ʱtempΪNULL
}

// ���ļ��ж�ȡ��������ݣ������������
void read_file(NODE** head, char* filename)
{
	NODE* temp = NULL;
	int i = 0, n = 0;
	char name[MaxLenOfName], child[MaxLenOfName];
	FILE* fp;

	fp = util_fopen(filename, "r"); // ���ļ�

	while (fscanf(fp, "%s %d", name, &n) != EOF) // �ȶ�ȡ�ڵ����ֺ͵�ǰ�ڵ���ӽڵ����
	{
		if (*head == NULL) // ��Ϊ��
		{
			temp = *head = create_node();   // ����һ���½ڵ�
			temp->name = util_strdup(name); // ����
		}
		else
		{
			temp = search_node_r(name, *head); // ����name�ҵ��ڵ�
											   // ����Ĭ�������ļ�����ȷ�ģ�һ�������ҵ���nameƥ��Ľڵ�
											   // �����ƥ�䣬��ôӦ�ú��Ա�������
		}
		// �ҵ��ڵ�󣬶��ӽڵ���д���
		temp->n_children = n;
		temp->children = (NODE**)malloc(n * sizeof(NODE*));
		if (temp->children == NULL) // �����ڴ�ʧ��
		{
			fprintf(stderr, "Dynamic allocation error!\n");
			exit(EXIT_FAILURE);
		}

		// �������ɹ������ȡ������ӽڵ㣬������
		for (i = 0; i < n; ++i)
		{
			fscanf(fp, "%s", child); // ��ȡ�ӽڵ�
			temp->children[i] = create_node(); // �����ӽڵ�
			temp->children[i]->name = util_strdup(child); // ���ӽڵ㸳��
			temp->children[i]->parent = temp;
			temp->children[i]->level = temp->level + 1; // ���ӽڵ���Ƚ��и�ֵ�����ڵ���ȼ�1
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

	// ��ȡ��ϣ��ر��ļ�
	fclose(fp);
}

//�������_�ǵݹ�ʵ��
int  sequence(NODE* head)	
{
	NODE* p = NULL;
	QUEUE* q = NULL; // ����һ������
	QUEUE* myQ = NULL;
	int i = 0;

	q = QUEUEinit(100); // �����г�ʼ����СΪ100
	myQ = QUEUEinit(100);

	QUEUEenqueue(q, head);	// �����ڵ������

	while (QUEUEempty(q) != 0) // �������q��Ϊ��
	{
		QUEUEdequeue(q, &p); // ������
		for (i = 0; i < p->n_children; ++i)
		{
			QUEUEenqueue(q, p->children[i]);      // ���ӽڵ������
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
		gettextstyle(&f);                     // ��ȡ��ǰ��������
		f.lfHeight = 48;                      // ��������߶�Ϊ 48
		_tcscpy(f.lfFaceName, _T("����"));    // ��������Ϊ�����塱(�߰汾 VC �Ƽ�ʹ�� _tcscpy_s ����)
		f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
		settextstyle(&f);                     // ����������ʽ
		outtextxy(p->width-10, p->level * 80 + 18, *(p->name));
		if (0 != num)
		{
			line(p->parent->width, p->parent->level * 80 + 68, p->width, p->level * 80 + 12);
		}
		num++;
		Sleep(1000);
	}
	_getch();              // �����������
	closegraph();          // �رջ�ͼ����

	QUEUEdestroy(q); // ���ٶ���
	QUEUEdestroy(myQ);
	
	return num;
}

//�������_�ݹ�ʵ��
void preorder (NODE* head, int *num)	
{
	if (head)
	{
		//�������
		setlinestyle(PS_SOLID, 5);
		circle(head->width, head->level * 80 + 40, 30);
		LOGFONT f;
		gettextstyle(&f);                     // ��ȡ��ǰ��������
		f.lfHeight = 48;                      // ��������߶�Ϊ 48
		_tcscpy(f.lfFaceName, _T("����"));    // ��������Ϊ�����塱
		f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
		settextstyle(&f);                     // ����������ʽ
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

//�������
void epilogue(NODE* head, int* num)
{
	if (head)
	{
		for (int i = 0; i < head->n_children; i++)
		{
			(*num)++;
			epilogue(head->children[i], num);
			//�������
			setlinestyle(PS_SOLID, 5);
			circle(head->children[i]->width, head->children[i]->level * 80 + 40, 30);
			LOGFONT f;
			gettextstyle(&f);                     // ��ȡ��ǰ��������
			f.lfHeight = 48;                      // ��������߶�Ϊ 48
			_tcscpy(f.lfFaceName, _T("����"));    // ��������Ϊ�����塱
			f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
			settextstyle(&f);                     // ����������ʽ
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
		gettextstyle(&f);                     // ��ȡ��ǰ��������
		f.lfHeight = 48;                      // ��������߶�Ϊ 48
		_tcscpy(f.lfFaceName, _T("����"));    // ��������Ϊ�����塱
		f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
		settextstyle(&f);                     // ����������ʽ
		outtextxy(head->width - 10, head->level * 80 + 18, *(head->name));
	}
}

// ������
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
	// free(head->children); // �����ӽڵ�ָ������
	free(head);
}

int main(int argc, char* argv[])
{
	NODE* head = NULL;
	int flag = 0;

	read_file(&head, "data.txt");
	while (1)
	{
		printf("1.���������2.�������, 3.�������, 4.�˳�\n");
		scanf("%d", &flag);
		if (1 == flag)
		{
			int num = sequence(head);
			printf("ִ����ϣ�\n");
			printf("����%d���ڵ�\n\n", num);
		}
		else if (2 == flag)
		{
			initgraph(640, 480);
			setbkcolor(LIGHTGRAY);
			cleardevice();
			int num = 1;
			preorder(head, &num);
			_getch();              // �����������
			closegraph();          // �رջ�ͼ����
			printf("ִ����ϣ�\n");
			printf("����%d���ڵ�\n\n", num);
		}
		else if (3 == flag)
		{
			initgraph(640, 480);
			setbkcolor(LIGHTGRAY);
			cleardevice();
			int num = 1;
			epilogue(head, &num);
			_getch();              // �����������
			closegraph();          // �رջ�ͼ����
			printf("ִ����ϣ�\n");
			printf("����%d���ڵ�\n\n", num);
		}
		else if (4 == flag)
		{
			printf("�����˳�");
			break;
		}
		else
		{
			printf("�����������������룡\n");
		}
	}
	
	free_tree_r(head);
	return EXIT_SUCCESS;
	
}