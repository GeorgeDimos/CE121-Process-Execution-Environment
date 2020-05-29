#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct node_t{
	char *name;
	pid_t pid;
	int argc;
	char **argv;
	int active;
	struct node_t *nxt;
	struct node_t *prv;
};

void addNode(char *name, pid_t pid, int argc, char *argv[], int active);
void deleteNode(struct node_t *p);
struct node_t *findpid(pid_t pid);
struct node_t *findRunning();
void printList();
int isEmpty();

extern struct node_t *head;