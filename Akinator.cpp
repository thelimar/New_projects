#define _CRT_SECURE_NO_WARNINGS

#define TX_USE_SPEAK

#define PrintfTabs(nTabs) 	for (int i = 0; i < nTabs; i++) {BufferPutc(file_buffer, '\t');}

#define streql(str1, str2) !strcmp(str1, str2)

#define PlayAgain(answer) while (True)                         \
	{                                                          \
		scanf("%3s", answer);                                  \
		if (streql(answer, "yes"))                             \
			return 1;                                          \
		else if (streql(answer, "no"))                         \
			return 0;                                          \
		else                                                   \
		{                                                      \
			printf("Please answer yes or no\n");               \
			TLK txSpeak("Пожалуйста отвечайте да или нет");    \
		}                                                      \
	}

#include "D:\progi\Core i DED\Core i DED\TXLib.h"
#include "tree.h"
#include "oneg.h"
#include "stek.h"

//#define TALK

#ifndef TALK
#define TLK if(0)
#else
#define TLK
#endif


enum Bool
{
	True  = 1,
	False = 0,
};

enum Parametors
{
	MAX_BUFFER_DATA = 5000,
};

enum game_modes
{
	IGRA         = 1,
	DEF          = 2,
	DIF          = 3,
	DUMP         = 4,
	SAVE_EXIT    = 5,
	EXIT_NO_SAVE = 6,
};

struct Buffer
{
	char* data;
	int len;
};

struct Tree* AkinatorTreeIn(FILE* source, char* base_buffer);
struct Node* ReadNode(struct Tree* akinator_tree, struct Node* parent, char** base_buffer);
void BufferBuild(struct Buffer* dis);
void BufferDestruct(struct Buffer* dis);
void AkinatorDestruct(struct Tree* dis_tree, struct Buffer* dis_buffer, char* dis_base);


int AkinatorGame(struct Tree* akinator_tree, struct Buffer* add_data);
int AkinatorDefenition(struct Tree* akinator_tree);
struct Node* TreeSearch(char* search_str, struct Node* dis);
int AkinatorDifference(struct Tree* akinator_tree);
void WriteNode(struct Node* dis, struct Buffer* file_buffer, int* nTabs);
void SaveDatabase(struct Tree* akinator_tree, FILE* output, char* base_buffer);
void BufferPuts(struct Buffer* dis, const char* str);
void BufferPutc(struct Buffer* dis, char c);
int AkinatorGraphicDump(struct Tree* akinator_tree);


int main()
{
	struct Tree* akinator_tree = 0;
	struct Buffer add_data{};
	BufferBuild(&add_data);
	char* base_buffer = 0;

	FILE* source = fopen("base.txt", "rb");
	if (source)
	{
		int nChars = 0;
		int nLines = 0;
		base_buffer = PoemEnter(&nChars, &nLines, source);
		akinator_tree = AkinatorTreeIn(source, base_buffer);
	}


	else
	{
		char* root_data = (char*) calloc(MAX_DATA_LEN, sizeof(char));
		sscanf("dont_know_who", "%s", root_data);
		akinator_tree = TreeBuild(root_data);
	}

	int permission_out = 1;

	while (permission_out)
	{
		printf("Hello fellow player! Choose mode...\n1 - Ugadaika\n2 - Defenition\n3 - Compare\n4 - Tree view\n5 - save & exit\n6 - exit\n");
		TLK txSpeak("Привет тебе игрок. Выбирай режим. 1 Угадайка. 2 Определение. 3 Сравнение. 4 Просмотр дерева. 5 Сохранить и выйти. 6 Выйти...");

		int mode = 0;

		DBG TreeTextDump(akinator_tree->root);

		if (!scanf("%d", &mode)) { printf("Try better!\n");  TLK txSpeak("Ввыйди и введи нормально"); }
		switch (mode)
		{
			case IGRA: permission_out = AkinatorGame(akinator_tree, &add_data);                                                             break;
			case DEF:  permission_out = AkinatorDefenition(akinator_tree);                                                                  break;
			case DIF:  permission_out = AkinatorDifference(akinator_tree);                                                                  break;
			case DUMP: permission_out = AkinatorGraphicDump(akinator_tree);                                                                 break;
			case SAVE_EXIT: source = fopen("base.txt", "wb");  SaveDatabase(akinator_tree, source, base_buffer); permission_out = 0;        break;
			case EXIT_NO_SAVE: permission_out = 0;                                                                                          break;
			default: printf("Try better!\n"); break;  TLK txSpeak("Ввыйди и введи нормально");                                              break;
		}

	}

	AkinatorDestruct(akinator_tree, &add_data, base_buffer);

	return 0;
}

struct Node* ReadNode(struct Tree* akinator_tree, struct Node* parent, char** base_buffer)
{
	while (**base_buffer)
	{
		if (**base_buffer == '[')
		{
			DBG printf("size %d\n", akinator_tree->size);
			struct Node* dis_node = (struct Node*) calloc(1, sizeof(struct Node));
			dis_node->parent = parent;
			(*base_buffer)++;
			DBG printf("base_buffer %s\n", *base_buffer);

			while (True)
			{
				if (**base_buffer == '?')
				{
					dis_node->data = *base_buffer + 1;
					*base_buffer = strchr(*base_buffer + 1, '?');
					**base_buffer = '\0';
					(*base_buffer)++;
					DBG printf("node data %s\n", dis_node->data);
					dis_node->left = ReadNode(akinator_tree, dis_node, base_buffer);
					DBG printf("base buffer middle %s\n", *base_buffer);
					dis_node->right = ReadNode(akinator_tree, dis_node, base_buffer);

					return dis_node;
				}

				else if (**base_buffer == '`')
				{
					dis_node->data = *base_buffer + 1;
					*base_buffer = strchr(*base_buffer + 1, '`');
					DBG printf("node data %s\n", dis_node->data);
					**base_buffer = '\0';
					(*base_buffer)++;
					DBG printf("base_buffer %s\n", *base_buffer);

					return dis_node;
				}

				else
					(*base_buffer)++;
			}
		}

		else if (**base_buffer == ']')
		{
			(*base_buffer)++;
			akinator_tree->size++;
			DBG printf("base_buffer close %s\n", *base_buffer);
		}

		else
		{
			(*base_buffer)++;
			DBG printf("base_buffer else %s\n", *base_buffer);
		}	
	}

	return 0;
}

struct Tree* AkinatorTreeIn(FILE* source, char* base_buffer)
{

	char* root_data = 0;
	struct Tree* akinator_tree = TreeBuild(root_data);

	char* str_begin = strchr(base_buffer, (int)'[');

	DBG printf("data\n%s\n", str_begin);

	akinator_tree->root = ReadNode(akinator_tree, 0, &str_begin);

	return akinator_tree;
}

int AkinatorGame(struct Tree* akinator_tree, struct Buffer* add_data)
{
	TLK txSpeak("Начнем игру");
	char answer[] = "yes";
	struct Node* current_node = akinator_tree->root;
	while (current_node->left)
	{
		printf("Is this %s?\n", current_node->data);
		TLK txSpeak("Is this %s?", current_node->data);
		scanf("%3s", answer);
		printf("\r\n");
		if (streql(answer, "yes"))
			current_node = current_node->left;
		else if (streql(answer, "no"))
			current_node = current_node->right;
		else
		{
			printf("Please answer yes or no\n");
			TLK txSpeak("Пожалуйста отвечайте да или нет");
		}
	}


	int permission_out = 1;
	while (permission_out)
	{
		printf("Is this %s?\n", current_node->data);
		TLK txSpeak("Is this %s?", current_node->data);
		scanf("%3s", answer);
		if (streql(answer, "yes"))
		{
			printf("HAHA! Knew it! In your face!\n");
			TLK txSpeak("Ничего удивительного, я ведь знаю всё!");
			permission_out = 0;
		}
		else if (streql(answer, "no"))
		{
			printf("Really? I thought I got you. What is it?\n");
			TLK txSpeak("Как не угадал? А что это тогда?");

			gets_s(add_data->data + add_data->len, MAX_DATA_LEN);
			struct Node* new_node= (struct Node*) calloc(1, sizeof(struct Node));
			AddNodeLeft(akinator_tree, current_node, add_data->data + add_data->len);
			AddNodeRight(akinator_tree, current_node, current_node->data);

			printf("And what is the difference between %s and %s?\n", add_data->data + add_data->len, current_node->data);
			TLK txSpeak("А в чём тогда разница?");

			add_data->len += MAX_DATA_LEN + 1;
			gets_s(add_data->data + add_data->len, MAX_DATA_LEN);
			current_node->data = add_data->data + add_data->len;
			add_data->len += MAX_DATA_LEN + 1;

			printf("Alright, I remembered now!\n");
			TLK txSpeak("Ну всё, я это запомнил!");
			permission_out = 0;

		}
		else
		{
			printf("Please answer yes or no\n");
			TLK txSpeak("Пожалуйста отвечайте да или нет");
		}
	}

	printf("Whould you like to play again?\n");
	TLK txSpeak("Сыграем снова?");

	PlayAgain(answer)
}

void BufferBuild(struct Buffer* dis)
{
	dis->data = (char*) calloc(MAX_BUFFER_DATA, sizeof(char));
	dis->len = 0;
}

void BufferDestruct(struct Buffer* dis)
{
	free(dis->data);
	dis->len = 0;
}

void AkinatorDestruct(struct Tree* dis_tree, struct Buffer* dis_buffer, char* dis_base)
{
	TreeDestruct(dis_tree);
	BufferDestruct(dis_buffer);
	if (dis_base)
		free(dis_base);
}

struct Node* TreeSearch(char* search_str, struct Node* dis)
{
	if (streql(dis->data, search_str))
		return dis;
	if (dis->left)
	{
		struct Node* found = TreeSearch(search_str, dis->left);
		if (found)
			return found;
	}
	if (dis->right)
	{
		struct Node* found = TreeSearch(search_str, dis->right);
		if (found)
			return found;
	}

	return 0;
}

int AkinatorDefenition(struct Tree* akinator_tree)
{
	printf("What to search\n");
	TLK txSpeak("Что искать?");

	char search_data[MAX_DATA_LEN];
	scanf("%s", search_data);

	struct Node* search_node = TreeSearch(search_data, akinator_tree->root);

	if (search_node)
	{
		struct Stack def_stek{};
		StackBuild(&def_stek);

		while (search_node)
		{
			StackPush(&def_stek, (int) search_node);
			search_node = search_node->parent;
		}

		printf("%s is ", ((struct Node*)def_stek.data[0])->data);
		TLK txSpeak("%s это ", ((struct Node*)def_stek.data[0])->data);

		for (int i = def_stek.count - 1; i > 1; i--)
		{
			if (((struct Node*)def_stek.data[i-1])->parent->right == ((struct Node*)def_stek.data[i-1]))
			{
				printf("NOT ");
				TLK txSpeak("не");
			}
			printf("%s, ", ((struct Node*) def_stek.data[i])->data);
			TLK txSpeak("%s, ", ((struct Node*)def_stek.data[i])->data);
		}


		printf("and most importanly ");
		TLK txSpeak("а главное");

		if (((struct Node*)def_stek.data[0])->parent->right == ((struct Node*)def_stek.data[0]))
		{
			printf("NOT ");
			TLK txSpeak("не");
		}

		printf("%s!\n", ((struct Node*)def_stek.data[1])->data);
		TLK txSpeak("%s!", ((struct Node*)def_stek.data[1])->data);

		StackDelete(&def_stek);
	}
	else
	{
		printf("Don't know him\n");
		TLK txSpeak("Таких не знаем");
	}

	printf("Whould you like to play again?\n");
	TLK txSpeak("Сыграем снова?");

	char answer[] = "yes";
	PlayAgain(answer)
}

int AkinatorDifference(struct Tree* akinator_tree)
{
	printf("Tell me who is first differencable?\n");
	TLK txSpeak("Кого сравниваем?");

	char first_dif[MAX_DATA_LEN];
	scanf("%s", first_dif);

	printf("Tell me who is second differencable?\n");
	TLK txSpeak("С кем сравниваем?");

	char second_dif[MAX_DATA_LEN];
	scanf("%s", second_dif);

	struct Node* first_dif_node = TreeSearch(first_dif, akinator_tree->root);
	struct Node* second_dif_node = TreeSearch(second_dif, akinator_tree->root);

	if (!first_dif_node || !second_dif_node)
	{
		printf("Don't know him\n");
		TLK txSpeak("Таких не знаем");
	}
	else
	{
		struct Stack first_def_stek {};
		StackBuild(&first_def_stek);

		while (first_dif_node)
		{
			StackPush(&first_def_stek, (int)first_dif_node);
			first_dif_node = first_dif_node->parent;
		}

		struct Stack second_def_stek {};
		StackBuild(&second_def_stek);

		while (second_dif_node)
		{
			StackPush(&second_def_stek, (int)second_dif_node);
			second_dif_node = second_dif_node->parent;
		}

		printf("%s and %s both ", ((struct Node*)first_def_stek.data[0])->data, ((struct Node*)second_def_stek.data[0])->data);
		TLK txSpeak("Оба %s и %s", ((struct Node*)first_def_stek.data[0])->data, ((struct Node*)second_def_stek.data[0])->data);

		int first_current_index = first_def_stek.count - 1;
		int second_current_index = second_def_stek.count - 1;

		for (; first_current_index > -1 && second_current_index > -1 && 
			first_def_stek.data[first_current_index - 1] == second_def_stek.data[second_current_index - 1]; 
			first_current_index--, second_current_index--)
		{
			if (((struct Node*)first_def_stek.data[first_current_index - 1])->parent->right == ((struct Node*)first_def_stek.data[first_current_index - 1]))
			{
				printf("NOT ");
				TLK txSpeak("не");
			}

			printf("%s, ", ((struct Node*)first_def_stek.data[first_current_index])->data);
			TLK txSpeak("%s, ", ((struct Node*)first_def_stek.data[first_current_index])->data);

			first_current_index--;
			second_current_index--;
		}
		printf("\nBut the difference is that %s is ", ((struct Node*)first_def_stek.data[0])->data);
		TLK txSpeak("а различаются они тем, что %s ", ((struct Node*)first_def_stek.data[0])->data);

		for (int i = first_current_index; i > 0; i--)
		{

			if (((struct Node*)first_def_stek.data[i-1])->parent->right == ((struct Node*)first_def_stek.data[i-1]))
			{
				printf("NOT ");
				TLK txSpeak("не");
			}

			printf("%s, ", ((struct Node*)first_def_stek.data[i])->data);
			TLK txSpeak("%s, ", ((struct Node*)first_def_stek.data[i])->data);
		}

		printf("\nAnd %s is ", ((struct Node*)second_def_stek.data[0])->data);
		TLK txSpeak("А %s ", ((struct Node*)second_def_stek.data[0])->data);

		for (int i = second_current_index; i > 0; i--)
		{

			if (((struct Node*)second_def_stek.data[i-1])->parent->right == ((struct Node*)second_def_stek.data[i-1]))
			{
				printf("NOT ");
				TLK txSpeak("не");
			}

			printf("%s, ", ((struct Node*)second_def_stek.data[i])->data);
			TLK txSpeak("%s, ", ((struct Node*)second_def_stek.data[i])->data);
		}

	}

	printf("\nWhould you like to play again?\n");
	TLK txSpeak("Сыграем снова?");

	char answer[] = "yes";
	PlayAgain(answer)
}

void BufferPutc(struct Buffer* dis, char c)
{
	dis->data[dis->len] = c;
	(dis->len)++;
}

void BufferPuts(struct Buffer* dis, const char* str)
{
	for (int i = 0; str[i]; i++)
	{
		dis->data[dis->len] = str[i];
		(dis->len)++;
	}
}

void WriteNode(struct Node* dis, struct Buffer* file_buffer, int* nTabs)
{
	PrintfTabs(*nTabs)

	if (dis->left)
	{
		BufferPutc(file_buffer, '?');
		BufferPuts(file_buffer, dis->data);
		BufferPuts(file_buffer, "?\r\n");

		PrintfTabs(*nTabs)
		BufferPuts(file_buffer, "[\r\n");

		(*nTabs)++;

		WriteNode(dis->left, file_buffer, nTabs);

		PrintfTabs(*nTabs)
		BufferPuts(file_buffer, "[\r\n");

		(*nTabs)++;
		WriteNode(dis->right, file_buffer, nTabs);
	}

	else
	{
		BufferPutc(file_buffer, '`');
		BufferPuts(file_buffer, dis->data);
		BufferPuts(file_buffer, "`\r\n");
	}

	(*nTabs)--;

	PrintfTabs(*nTabs)
	BufferPuts(file_buffer, "]\r\n");

	return;

}

void SaveDatabase(struct Tree* akinator_tree, FILE* output, char* base_buffer)
{
	struct Buffer file_buffer{};
	BufferBuild(&file_buffer);

	for (int i = 0; i < (int)strrchr(base_buffer, '}') - (int)base_buffer + 1; i++)
	{
		file_buffer.data[i] = base_buffer[i];
		file_buffer.len++;
	}

	BufferPuts(&file_buffer, "\r\n");

	int nTabs = 1;
	BufferPuts(&file_buffer, "[\r\n");

	WriteNode(akinator_tree->root, &file_buffer, &nTabs);

	fwrite(file_buffer.data, sizeof(char), file_buffer.len, output);

	fclose(output);

}

int AkinatorGraphicDump(struct Tree* akinator_tree)
{
	TreeGraphicDump(akinator_tree);

	char answer[] = "yes";
	PlayAgain(answer)
}