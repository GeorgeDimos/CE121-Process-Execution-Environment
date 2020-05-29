/*
 Giorgos Dimos
 27/05.2020
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <poll.h>
#include "util.h"

#define MAXINPUT 500
#define MAXARGS 20

static int tokenize(char *input,char *args[]);
static int action(int numofArgs, char *args[]);
static void checkchildren();
static void killemall();
static void sendSigusr1ToAll();
static void switchactive();

static volatile sig_atomic_t update = 0;
static void childhandler(int sig) {
	update = 1;
}

static volatile sig_atomic_t sigusr1rec = 0;
static void sigusr1handler(int sig) {
	sigusr1rec = 1;
}

static volatile sig_atomic_t sigalrmrec = 0;
static void sigalrmhandler(int sig) {
	sigalrmrec = 1;
}

struct node_t *head = NULL;

int main (int argc, char *argv[]){

	char input[MAXINPUT];
	char *args[MAXARGS];
	int numofArgs, n, quit = 0;

	struct sigaction act_chl ={{0}};
	act_chl.sa_handler = childhandler;
	act_chl.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &act_chl, NULL)==-1){
		fprintf(stderr, "seting signal action failed.Exiting\n");
		return 1;
	}

	struct sigaction act_susr1 ={{0}};
	act_susr1.sa_handler = sigusr1handler;
	act_susr1.sa_flags = SA_RESTART;
	if(sigaction(SIGUSR1, &act_susr1, NULL)==-1){
		fprintf(stderr, "seting signal action failed.Exiting\n");
		return 1;
	}

	struct sigaction act_alrm ={{0}};
	act_alrm.sa_handler = sigalrmhandler;
	act_alrm.sa_flags = SA_RESTART;
	if(sigaction(SIGALRM, &act_alrm, NULL)==-1){
		fprintf(stderr, "seting signal action failed.Exiting\n");
		return 1;
	}

	struct itimerval t = { {0} };
	t.it_value.tv_sec = 20;
	t.it_value.tv_usec = 0;
	t.it_interval.tv_sec = 20;
	t.it_interval.tv_usec=0;
	if(setitimer(ITIMER_REAL,&t,NULL)==-1){
		fprintf(stderr, "Error seting timer\n");
		return 1;
	}

	struct pollfd fdinfo={0};
	fdinfo.fd = STDIN_FILENO;
	fdinfo.events = POLLIN;

	while(!quit){
		n=poll(&fdinfo, 1, 5000000);
		if(n<0){
			if(errno != EINTR){
				fprintf(stderr,"Read poll failed\n");
			}
		}
		else if (n==1){
			n = read(STDIN_FILENO,input,MAXINPUT);
			input[n]='\0';
			input[strcspn(input, "\n")] = 0;
			numofArgs = tokenize(input, args);
			quit = action(numofArgs, args);
		}

		if(update){
			update = 0;
			checkchildren();
		}

		if(sigusr1rec){
			sigusr1rec = 0;
			sendSigusr1ToAll();
		}

		if(sigalrmrec){
			sigalrmrec = 0;
			switchactive();
		}
	}

	killemall();

	return 0;
}

static int tokenize(char *input,char *args[]){
	
	args[0] = strtok(input," ");
	if(args[0]==NULL){
		return -1;
	}
	int i = 1;
	while((args[i] = strtok(NULL, " "))){
		i++;
	}

	return i;
}

static int action(int numofArgs, char *args[]){
	pid_t pid;
	struct sigaction act_susr1 = {{0}};

	if(numofArgs>1 && !strcmp(args[0],"exec")){
		if((pid = fork())==0){
			act_susr1.sa_handler = SIG_DFL;
			if(sigaction(SIGUSR1,&act_susr1,NULL)==-1){
				fprintf(stderr, "seting default action for SIGUSR1 to child failed.Exiting\n");
				return 1;
			}
			if(!isEmpty()){
				if(raise(SIGSTOP)==-1){
					fprintf(stderr, "Error with raise\n");
				}
			}
			execv(args[1], args+1);  //terminate with NULL pointer, well that was a freebie
			fprintf(stderr,"Error execv\n");
			return 1;
		}
		addNode(args[1], pid, numofArgs-1, args+1, isEmpty());
	}

	else if(numofArgs==2 && !strcmp(args[0],"term")){
		if(kill(atoi(args[1]), SIGTERM)==-1){
			fprintf(stderr,"Error sending SIGTERM to proccess %d\n", atoi(args[1]));
		}
	}

	else if(numofArgs==2 && !strcmp(args[0],"sig")){
		if(kill(atoi(args[1]), SIGUSR1)==-1){
			fprintf(stderr,"Error sending SIGUSR1 to proccess %d\n", atoi(args[1]));
		}
	}

	else if(numofArgs==1 && !strcmp(args[0],"list")){
		printList();
	}

	else if(numofArgs==1 && !strcmp(args[0],"quit")){
		printf("Exiting\n");
		return 1;
	}

	else{
		printf("Invalid input. Try again.\n");
	}
	return 0;
}

static void checkchildren(){
	int check, status;
	struct node_t *runner = head;
	
	if(isEmpty()){
		printf("No children active\n");
		return ;
	}

	do{
		if((check = waitpid(runner->pid, &status, WNOHANG))>0){
			if(WIFEXITED(status) || WIFSIGNALED(status)){
				printf("pid: %d deleted\n",runner->pid);
				if(runner->active==1 && runner!=runner->nxt){
					if(kill(runner->nxt->pid, SIGCONT)==-1){
						fprintf(stderr,"Error sending SIGCONT to proccess %d\n", runner->nxt->pid);
					}
					runner->active = 0;
					runner->nxt->active = 1;
				}
				deleteNode(runner);
			}
		}

		if(check == 1){
			fprintf(stderr,"waitpid exited with error %d\n", errno);
			exit(EXIT_FAILURE);
		}
		
		runner = runner->nxt;
		
	}while(runner!=head && head!=NULL);
}

static void sendSigusr1ToAll(){
	
	struct node_t *runner = head;
	if(isEmpty()){
		printf("No children left\n");
		return;
	}

	do{
		kill(runner->pid, SIGUSR1);
		runner = runner->nxt;
	}while(runner!=head);
}

static void killemall(){
	struct node_t *runner = head;
	int status;

	if(isEmpty()){
		printf("No children left\n");
		return;
	}

	do{
		kill(runner->pid, SIGTERM);
		runner = runner->nxt;
	}while(runner!=head);

	do{
		if(waitpid(runner->pid, &status, 0)==-1){
			fprintf(stderr,"waitpid exited with error %d\n", errno);
			exit(EXIT_FAILURE);
		}
		if(WIFEXITED(status) || WIFSIGNALED(status)){
			printf("pid: %d deleted\n", runner->pid);
			deleteNode(runner);
		}
		runner = runner->nxt;
	}while(head!=NULL);
}

static void switchactive(){

	if(isEmpty()){
		return;
	}
	struct node_t *p = findRunning();
	if(kill(p->pid, SIGSTOP)==-1){
		fprintf(stderr,"Error sending SIGSTOP to proccess %d\n", p->pid);
	}
	
	if(kill(p->nxt->pid, SIGCONT)==-1){
		fprintf(stderr,"Error sending SIGCONT to proccess %d\n", p->nxt->pid);
	}
	p->active = 0;
	p->nxt->active = 1;
}