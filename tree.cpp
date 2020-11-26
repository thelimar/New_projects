#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#define DEBUG

#ifndef DEBUG
#define DBG if(0)
#else
#define DBG
#endif

#define IsOK(Tree) int error = TreeVerificator(Tree); if (error != 0) {TreeGraphicDump(Tree); return error;}

typedef char* TreeData;

enum parametors
{
	MAX_DATA_LEN = 20,
};

enum errors
{
	SIZE_ERROR = 1,
	RELATION_ERROR = 2,
	ROOT_NULL = 3,
};

struct Node
{
	TreeData data;
	struct Node* left;
	struct Node* right;
	struct Node* parent;
};

struct Tree 
{
	struct Node* root;
	int size;
};

struct Tree* TreeBuild(TreeData root_data);
int AddNodeLeft(struct Tree* dis, struct Node* parent, TreeData node_data);
int AddNodeRight(struct Tree* dis, struct Node* parent, TreeData node_data);
void TreeGraphicDump(struct Tree* dis);
void DrawNodes(FILE* output, struct Node* node);
void NodeDestruct(struct Node* dis);
void TreeDestruct(struct Tree* dis);
int TreeVerificator(struct Tree* dis);
void TreeParentCheck(struct Node* dis, int* parent_ok);
void TreeTextDump(struct Node* dis);

/*

int main()
{
	char* root = (char*) calloc(MAX_DATA_LEN, sizeof(char));
	sscanf("root", "%s", root);
	struct Tree* tree1 = TreeBuild(root);
	DBG printf("root data %s\n", tree1->root->data);

	char label1[15];
	sscanf("first", "%s", label1);
	DBG printf("root %p label1 %p\n", root, label1);
	AddNodeLeft(tree1, tree1->root, label1);
	char label2[15];
	sscanf("second", "%s", label2);

	AddNodeRight(tree1, tree1->root, label2);
	char label3[15];
	sscanf("third", "%s", label3);

	AddNodeLeft(tree1, tree1->root, label3);

	DBG printf("size %d\n", tree1->size);
	DBG printf("root data %s\n", tree1->root->data);

	TreeGraphicDump(tree1);

	TreeDestruct(tree1);

	return 0;
}

*/

void TreeParentCheck(struct Node* dis, int* parent_ok)
{
	if (dis->left)
	{
		if (dis->data != dis->left->parent->data)
		{
			*parent_ok = 0;
			return;
		}
		TreeParentCheck(dis->left, parent_ok);
	}
	if (dis->right)
	{
		if (dis->data != dis->right->parent->data)
		{
			*parent_ok = 0;
			return;
		}
		TreeParentCheck(dis->right, parent_ok);
	}
	return;
}

int TreeVerificator(struct Tree* dis)
{
	int error = 0;
	if (dis->size < 1)
		error = SIZE_ERROR;
	else if (!dis->root)
		error = ROOT_NULL;
	else if (dis->size >= 2)
	{
		int parent_ok = 1;
		TreeParentCheck(dis->root, &parent_ok);
		if (!parent_ok)
			error = RELATION_ERROR;
	}
	if (error != 0)
		printf("ERROR #%d\n", error);
	return error;
}


struct Tree* TreeBuild(TreeData root_data)
{
	TreeData Root_data = root_data;

	struct Tree* tree = (struct Tree*) calloc(1, sizeof(Tree));
	tree->size = 1;
	tree->root = (struct Node*) calloc(1, sizeof(struct Node));
	tree->root->data = Root_data;
	DBG printf("ok data %p\n", tree->root->data);
	
	tree->root->left =  0;
	tree->root->right = 0;
	tree->root->parent = 0;
	

	return tree;
}

void NodeDestruct(struct Node* dis)
{
	if (dis->left)
	{
		NodeDestruct(dis->left);
	}
	if (dis->right)
	{
		NodeDestruct(dis->right);
	}
	
	free(dis);
	return;
}

void TreeDestruct(struct Tree* dis)
{
	NodeDestruct(dis->root);
	dis->size = 0;
	free(dis);
}

int AddNodeLeft(struct Tree* dis, struct Node* parent, TreeData node_data)
{
	IsOK(dis)

	if (!parent->left)
	{
		TreeData Node_data = node_data;
		struct Node* node = (struct Node*)calloc(1, sizeof(struct Node));
		node->data = Node_data;
		node->left = 0;
		node->parent = parent;
		node->right = 0;
		(dis->size)++;
		parent->left = node;
		return 0;
	}
	else
		AddNodeLeft(dis, parent->left, node_data);
}

int AddNodeRight(struct Tree* dis, struct Node* parent, TreeData node_data)
{
	IsOK(dis)

	if (!parent->right)
	{
		TreeData Node_data = node_data;
		struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));
		node->data = Node_data;
		node->left = 0;
		node->parent = parent;
		node->right = 0;
		(dis->size)++;
		parent->right = node;
		return 0;
	}
	else
		AddNodeRight(dis, parent->right, node_data);
}


void TreeTextDump(struct Node* dis)
{
	if (dis->data != nullptr)
		printf("%s\n", dis->data);
	else
		printf("CORRUPTED_DATA\n");

	if (dis->left)
	{
		printf("(\n");
		TreeTextDump(dis->left);
	}
	if (dis->right)
	{
		printf("(\n");
		TreeTextDump(dis->right);
	}
	printf(")\n");
	return;
}

void DrawNodes(FILE* output, struct Node* node)
{
	if (!node->left)
	{
		if (!node->right)
		{
			fprintf(output, "peak%s [label=\"%s | 0 | 0\"];\n", node->data, node->data);
			return;
		}
		else
		{
			fprintf(output, "peak%s [label=\"%s | 0 | <right%s> %p\"];\n", node->data, node->data,
				node->data, node->right);
			fprintf(output, "peak%s:<right%s> -> peak%s[label=\"no\"]\n", node->data, node->data, node->right->data);
			DrawNodes(output, node->right);
		}
	}
	else if (!node->right)
	{
		fprintf(output, "peak%s [label=\"%s | <left%s> %p | 0\"];\n", node->data, node->data,
			node->data, node->left);
		fprintf(output, "peak%s:<left%s> -> peak%s[label=\"yes\"]\n", node->data, node->data, node->left->data);
		DrawNodes(output, node->left);
	}
	else
	{
		fprintf(output, "peak%s [label=\"%s | <left%s> %p | <right%s> %p\"];\n", node->data, node->data, node->data, node->left,
			node->data, node->right);
		fprintf(output, "peak%s:<right%s> -> peak%s[label=\"no\"]\n", node->data, node->data, node->right->data);
		fprintf(output, "peak%s:<left%s> -> peak%s[label=\"yes\"]\n", node->data, node->data, node->left->data);

		DrawNodes(output, node->right);
		DrawNodes(output, node->left);
	}
}

void TreeGraphicDump(struct Tree* dis)
{
	FILE* Graph = fopen("Graph.dot", "w");
	fprintf(Graph, "digraph G{\nrankdir=TB;\n");

	fprintf(Graph, "edge[color = \"darkorange\"];\n");
	fprintf(Graph, "node [style = \"filled\", shape=record, fillcolor=\"darkkhaki\"];\n");

	DrawNodes(Graph, dis->root);

	fprintf(Graph, "}");

	fclose(Graph);

	system("D:\\Program_Files\\ATT\\Graphviz\\bin\\dot.exe -Tpng D:\\progi\\Akinator\\Graph.dot -o D:\\progi\\Akinator\\GraphDump.png");
	system("start D:\\progi\\Akinator\\GraphDump.png");
}
