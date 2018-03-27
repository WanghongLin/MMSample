
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct Node {
	int value;
	struct Node* next;
} Node;

size_t size_of_list(Node* head)
{
	size_t s = 0;
	Node* l = head;
	while (l) {
		s++;
		l = l->next;
	}
	return s;
}

Node* new_node(int value)
{
	Node* n = (Node*)malloc(sizeof(Node));
	n->value = value;
	n->next = NULL;
	return n;
}

Node* new_random_list(int size)
{
	Node* head = NULL;
	Node* tail = NULL;

	srand(time(NULL));
	for (int i = 0; i < size; i++) {

		Node* n = new_node(rand() % size);
		if (head) {
			tail->next = n;
			tail = n;
		} else {
			head = n;
			tail = n;
		}
	}

	return head;
}

static void print_list(Node* list)
{
	Node* l = list;
	while (l) {
		printf("%d ", l->value);
		if (l->next) {
			printf(" -> ");
		}
		l = l->next;
	}
	putchar('\n');
}

static Node* remove_nth_from_end_of_list(Node* list, int n)
{
	const size_t list_size = size_of_list(list);
	const int remove_at = list_size - n;
	int i = 0;
	Node* l = list;
	Node* prev = list;

	if (remove_at == 0) {
		return list->next;
	}

	if (remove_at >= 0 && remove_at < list_size) {
		while (i < remove_at) {
			i++;
			prev = l;
			l = l->next;
		}
	}

	if (l) {
		prev->next = l->next;
	}

	return list;
}

void free_node(Node* node)
{
	free(node);
}

void free_list(Node* list)
{
    Node* l = list;
	while (l) {
		Node* next = l->next;
		free(l);
		l = next;
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <size-of-node> <nth-to-remove-from-end>\n", argv[0]);
		exit(0);
	}

	int node_size = atoi(argv[1]);
	int nth_to_remove_from_end = atoi(argv[2]);

	Node* list = new_random_list(node_size);
	print_list(list);

	printf("remove %dth from end of list\n", nth_to_remove_from_end);
	print_list(remove_nth_from_end_of_list(list, nth_to_remove_from_end));

	free_list(list);

	return 0;
}
