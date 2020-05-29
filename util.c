#include "util.h"

//εισαγωγή νέου στοιχείου(πάντα στην «ουρά» της δομής)
void addNode(char *name, pid_t pid, int argc, char *argv[], int active){

	struct node_t *newNode = malloc(sizeof(struct node_t));
	if(newNode==NULL){
		fprintf(stderr,"Malloc failed\n");
		exit(EXIT_FAILURE);
	}
	newNode->name = malloc(strlen(name)*sizeof(char));
	if(newNode->name==NULL){
		fprintf(stderr,"Malloc failed\n");
		exit(EXIT_FAILURE);
	}
	strcpy(newNode->name, name);
	newNode->active = active;
	newNode->pid = pid;
	newNode->argc = argc;
	newNode->argv = malloc(argc*sizeof(char **));
	for(int i=0; i<argc; i++){
		newNode->argv[i] = malloc(strlen(argv[i])*sizeof(char));
		if(newNode->argv[i]==NULL){
			fprintf(stderr,"Malloc failed\n");
			exit(EXIT_FAILURE);
		}
		strcpy(newNode->argv[i], argv[i]);
	}

	if(isEmpty()){
		head = newNode;
		head->nxt = head;
		head->prv = head;
	}
	else{
		struct node_t *last= head->prv;
		last->nxt = newNode;
		head->prv = newNode;
		newNode->prv = last;
		newNode->nxt = head;
	}
}

//διαγραφή στοιχείου
void deleteNode(struct node_t *p){
	if(p==NULL){
		fprintf(stderr, "Trying to delete invalid node.\n");
		return;
	}

	if(p == p->nxt){
		head=NULL;
	}

	if(p==head){
		head = p->nxt;
	}

	free(p->name);
	for(int i=0; i<p->argc; i++){
		free(p->argv[i]);
	}
	free(p->argv);
	
	p->nxt->prv = p->prv;
	p->prv->nxt = p ->nxt;

	free(p);
}

//εύρεση της επόμενης διεργασίας
void nextActive(){
	struct node_t *running = findRunning();
	running->active = 0;
	running->nxt->active = 1;
}

//εκτύπωση των περιεχομένων της λίστας

void printList(){
	
	if(isEmpty()){
		printf("No process running\n");
		return;
	}

	struct node_t  *runner = head;

	do{
		printf("pid: %d, name: (", runner->pid);
		for (int i=0; i<runner->argc; i++){
			printf("%s", runner->argv[i]);
			if(i<runner->argc-1){
				printf(", ");
			}
		}
		printf(") ");
		if(runner->active){
			printf("(R)");
		}
		printf("\n");
		runner = runner->nxt;
	}while(runner!=head);
}

struct node_t *findpid(pid_t pid){
	struct node_t *runner;

	if(isEmpty()){
		printf("No process is running\n");
		return NULL;
	}

	runner = head;
	
	do{
		if(runner->pid == pid){
			return runner;
		}
		runner = runner->nxt;
	}while(runner!=head);

	printf("Process with pid %d not a child process\n", pid);
	return NULL;
}

//εύρεση της διεργασίας που τρέχει
struct node_t *findRunning(){
	struct node_t *runner;

	if(isEmpty()){
		printf("No process is running\n");
		return NULL;
	}

	runner = head;
	do{
		if(runner->active){
			return runner;
		}
		runner = runner->nxt;
	}while(runner!=head);

	return NULL;
}

//ανίχνευση άδειας δομής
int isEmpty(){
	if(head==NULL){
		return 1;
	}
	return 0;
}