#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DEBUG

#ifndef DEBUG
#define DBG if(0)
#else
#define DBG
#endif

#define SizeOK(list) if (list->size >= list->capacity - 1) ListResizeUp(list);
#define GetEnumName(enum_const) #enum_const
#define IsOK(list) if (ListVerificator(list) == -1) {printf("Error code: %d\nfunc_sig %s\n", list->err_no, __FUNCSIG__ ); \
													 GraphicDump(*list); return list->err_no;}


enum errors
{
	NEG_CAPACITY = 1,
	NEG_SIZE = 2,
	CORRUPTED_EDGE = 3,
	FREE_AMOUNT_ERROR = 4,
	NULL_DATA = 5,
};

enum parametors
{
	MAX_SIZE = 10,
	CAPACITY_MULTIPLIER = 2,
};

struct ListElement
{
	double data;
	int next;
	int prev;
};

struct List
{
	int size;
	int capacity;
	struct ListElement* list_array;

	int head;
	int tail;

	int free_head;
	int free_tail;

	int err_no;
	int corrupted_edge_prev;
	int corrupted_edge_next;
};

int ListLogicOrder(struct List* dis);
void ListBuild(struct List* dis);
void GraphicDump(struct List dis);
int ListPushHead(struct List* dis, double value);
int ListInsertAfter(struct List* dis, int pos, double value);
int ListInsertBefore(struct List* dis, int pos, double value);
int ListPushTail(struct List* dis, double value);
int ListDelete(struct List* dis, int pos);
void ListDestruct(struct List* dis);
void ListResizeUp(struct List* dis);
double ListGetLogPos(struct List* dis, int log_num);
int ListVerificator(struct List* dis);



int main()
{
	struct List list = {
		.size = 0,
	};


	ListBuild(&list);
	for (int i = 0; i < 10; i++)
	{
		ListInsertAfter(&list, i, i * i);
	}

	ListInsert(&list, 5, 67);
	ListDelete(&list, 7);
	ListInsert(&list, 3, 34);
	printf("3: %lf\n", ListGetLogPos(&list, 3));
	ListLogicOrder(&list);
	DBG printf("corrupt %d %d\n", list.corrupted_edge_next, list.corrupted_edge_prev);
	GraphicDump(list);

	ListDestruct(&list);

	return 0;
}

void ListBuild(struct List* dis)
{
	dis->size = 0;
	dis->capacity = MAX_SIZE;
	dis->list_array = (struct ListElement*) calloc(MAX_SIZE, sizeof(struct ListElement));

	for (int i = 0; i < MAX_SIZE; i++)
	{
		dis->list_array[i].data = NAN;
		dis->list_array[i].next = i + 1;
		dis->list_array[i].prev = -1;
	}
	dis->list_array[0].next = 0;
	dis->list_array[0].prev = 0;

	dis->head = 0;
	dis->tail = 0;

	dis->list_array[MAX_SIZE - 1].next = -1;

	dis->free_tail = 1;
	dis->free_head = MAX_SIZE - 1;

	dis->err_no = 0;
	dis->corrupted_edge_prev = -1;
	dis->corrupted_edge_next = -1;
}

void ListDestruct(struct List* dis)
{
	free(dis->list_array);
	dis->head = 0;
	dis->size = 0;
	dis->capacity = 0;
	dis->tail = 0;
	dis->free_head = 0;
	dis->free_tail = 0;
}

void ListInsertInitialize(struct List* dis, int pos, double value)
{
	SizeOK(dis)
	dis->free_tail = dis->list_array[pos].next;

	dis->list_array[pos].data = value;
	dis->list_array[pos].next = 0;
	dis->list_array[pos].prev = 0;

	dis->head = pos;
	dis->tail = pos;

}

double ListGetLogPos(struct List* dis, int log_num)
{
	IsOK(dis)

	int log_pos = dis->tail;
	for (int i = 0; i < log_num; i++)
	{
		log_pos = dis->list_array[log_pos].next;
	}

	return dis->list_array[log_pos].data;
}

int ListPushHead(struct List* dis, double value)
{
	SizeOK(dis)
	int empty_pos = dis->free_tail;
	dis->free_tail = dis->list_array[empty_pos].next;
	dis->list_array[empty_pos].data = value;

	dis->list_array[empty_pos].prev = dis->head;
	dis->list_array[empty_pos].next = 0;

	dis->list_array[dis->head].next = empty_pos;
	dis->head = empty_pos;

	return 0;
}

int ListInsertAfter(struct List* dis, int pos, double value)
{
	IsOK(dis)
	SizeOK(dis)
	if (dis->size == 0)
		ListInsertInitialize(dis, 1, value);
	else if (pos == dis->head)
		ListPushHead(dis, value);
	else
	{
		int empty_pos = dis->free_tail;
		dis->free_tail = dis->list_array[empty_pos].next;
		dis->list_array[empty_pos].data = value;

		int old_next = dis->list_array[pos].next;

		dis->list_array[old_next].prev = empty_pos;
		dis->list_array[empty_pos].next = old_next;
		dis->list_array[empty_pos].prev = pos;
		dis->list_array[pos].next = empty_pos;
	}

	dis->size++;

	return 0;
}

int ListPushTail(struct List* dis, double value)
{
	SizeOK(dis)
	int empty_pos = dis->free_tail;
	dis->free_tail = dis->list_array[empty_pos].next;
	dis->list_array[empty_pos].data = value;

	dis->list_array[empty_pos].next = dis->tail;
	dis->list_array[empty_pos].prev = 0;

	dis->list_array[dis->tail].prev = empty_pos;
	dis->tail = empty_pos;

	return 0;
}

int ListInsertBefore(struct List* dis, int pos, double value)
{
	IsOK(dis)
	SizeOK(dis)
	if (dis->size == 0)
		ListInsertInitialize(dis, pos, value);
	else if (pos == dis->tail + 1)
		ListPushTail(dis, value);
	else
	{
		int empty_pos = dis->free_tail;
		dis->free_tail = dis->list_array[empty_pos].next;
		dis->list_array[empty_pos].data = value;

		int old_prev = dis->list_array[pos].prev;
		dis->list_array[empty_pos].prev = old_prev;
		dis->list_array[empty_pos].next = pos;
		dis->list_array[old_prev].next = empty_pos;
		dis->list_array[pos].prev = empty_pos;
	}

	dis->size++;

	return 0;
}

int ListDelete(struct List* dis, int pos)
{
	SizeOK(dis)

	dis->list_array[dis->free_head].next = pos;
	dis->free_head = pos;

	dis->list_array[pos].data = NAN;
	dis->list_array[dis->list_array[pos].next].prev = dis->list_array[pos].prev;
	dis->list_array[dis->list_array[pos].prev].next = dis->list_array[pos].next;
	dis->list_array[pos].next = -1;
	dis->list_array[pos].prev = -1;

	dis->size--;

	return 0;
}

void ListResizeUp(struct List* dis)
{
	dis->capacity *= CAPACITY_MULTIPLIER;
	dis->list_array = (struct ListElement*) realloc(dis->list_array, sizeof(struct ListElement) * dis->capacity);
	assert(dis->list_array);
	DBG printf("resize size %d\n", dis->size);
	for (int i = dis->size + 1; i < dis->capacity; i++)
	{
		dis->list_array[i].data = NAN;
		dis->list_array[i].next = i + 1;
		dis->list_array[i].prev = -1;
	}

	dis->list_array[dis->capacity - 1].next = -1;

	dis->free_tail = dis->size + 1;
	DBG printf("free_tail resize %d\n", dis->free_tail);
	dis->free_head = dis->capacity - 1;
}

int ListVerificator(struct List* dis)
{
	assert(dis);
	int nFree = 0;
	if (dis->capacity <= 0)
	{
		dis->err_no = NEG_CAPACITY;
		return -1;
	}
	else if (dis->size < 0)
	{
		dis->err_no = NEG_SIZE;
		return -1;
	}
	else if (dis->list_array == NULL)
	{
		dis->err_no = NULL_DATA;
		return -1;
	}
	else if (dis->size >= 2)
	{
		int logic_index = dis->list_array[dis->tail].next;
		for (int i = 0; i < dis->size - 2; i++)
		{
			if (dis->list_array[logic_index].data != dis->list_array[dis->list_array[dis->list_array[logic_index].prev].next].data)
			{
				dis->err_no = CORRUPTED_EDGE;
				dis->corrupted_edge_prev = logic_index;
				printf("corrupted prev %d\n", logic_index);
				return -1;
			}
			if (dis->list_array[logic_index].data != dis->list_array[dis->list_array[dis->list_array[logic_index].next].prev].data)
			{
				dis->err_no = CORRUPTED_EDGE;
				dis->corrupted_edge_next = logic_index;
				printf("corrupted next %d\n", logic_index);
				return -1;
			}
			logic_index = dis->list_array[logic_index].next;
		}
		for (int i = 0; i < dis->capacity; i++)
			if (dis->list_array[i].prev == -1) nFree++;                                                                                                                                  
		if (nFree != dis->capacity - dis->size - 1)
		{
			dis->err_no = FREE_AMOUNT_ERROR;
			return -1;
		}
	}

	return 0;
}

int ListLogicOrder(struct List* dis)
{
	IsOK(dis);
	int logic_index = dis->tail;
	struct ListElement* logical_list_array = (struct ListElement*) calloc(dis->capacity, sizeof(struct ListElement));

	logical_list_array[0].data = NAN;
	logical_list_array[0].prev = 0;
	logical_list_array[0].next = 0;


	for (int i = 1; i < dis->size + 1; i++)
	{
		DBG printf("logic no %d\n", logic_index);
		logical_list_array[i].data = dis->list_array[logic_index].data;
		logical_list_array[i].prev = i - 1;
		logical_list_array[i].next = i + 1;

		logic_index = dis->list_array[logic_index].next;
	}

	logical_list_array[dis->size].next = 0;

	for (int i = dis->size + 1; i < dis->capacity; i++)
	{
		logical_list_array[i].data = NAN;
		logical_list_array[i].prev = -1;
		logical_list_array[i].next = i + 1;
	}
	logical_list_array[dis->capacity - 1].next = -1;


	dis->free_tail = dis->size + 1;
	dis->free_head = dis->capacity - 1;

	free(dis->list_array);
	dis->list_array = logical_list_array;

	return 0;
}

void GraphicDump(struct List dis)
{
	FILE* Graph = fopen("Graph.dot", "w");
	fprintf(Graph, "digraph G{\nrankdir=RL;\n");

	fprintf(Graph, "peak%d [style=\"filled\", shape=record, fillcolor=\"#FF00FF\" label=\"%lf | { <prev%d> %d | %d | <next%d> %d}\"];\n", 0, dis.list_array[0].data, 0, 
		dis.list_array[0].prev, 0, 0, dis.list_array[0].next);

	for (int i = 1; i < dis.capacity; i++)
	{
		if (dis.list_array[i].prev == -1) fprintf(Graph, "node[fillcolor=\"grey\"]\n");
		else fprintf(Graph, "node[fillcolor=\"darkkhaki\"]\n");
		fprintf(Graph, "peak%d [style=\"filled\", shape=record, label=\"%lf | { <prev%d> %d | %d | <next%d> %d}\"];\n", i, dis.list_array[i].data, i, 
			dis.list_array[i].prev, i, i, dis.list_array[i].next);
	}

	fprintf(Graph, "edge[color = \"darkorange\"];\n");

	for (int i = 0; i < dis.capacity; i++)
	{	
		if (dis.list_array[i].prev == -1 && dis.list_array[i].next != -1)
			fprintf(Graph, "peak%d -> peak%d;\n", i, dis.list_array[i].next);
	}

	fprintf(Graph, "edge[dir = both];\n");

	for (int i = 1; i < dis.capacity; i++)
	{
		if (dis.list_array[i].prev != -1)
		{
			if (i == dis.corrupted_edge_prev)
				fprintf(Graph, "peak%d -> peak%d[color=\"red\"];\n", i, dis.list_array[i].prev);
			else if (i == dis.corrupted_edge_next)
				fprintf(Graph, "peak%d -> peak%d[color=\"red\"];\n", i, dis.list_array[i].next);
			else if (dis.list_array->prev != -1)
				fprintf(Graph, "peak%d -> peak%d;\n", i, dis.list_array[i].prev);
		}
	}

	fprintf(Graph, "}");

	fclose(Graph);

	system("D:\\Program_Files\\ATT\\Graphviz\\bin\\dot.exe -Tpng D:\\progi\\List\\Graph.dot -o D:\\progi\\List\\GraphDump.png");
	system("start D:\\progi\\List\\GraphDump.png");
}