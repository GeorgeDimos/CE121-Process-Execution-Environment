/*
 Giorgos Dimos
 26/05/2020
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t i;

static void handler(int sig) {
	i = 0;
}

int main(int argc, char *argv[]){
	struct sigaction act ={{0}};
	sigset_t set;
	int blocked = 0;

	if(argc!=5){
		fprintf(stderr,"Invalid number of arguments\n");
		fprintf(stderr,"Input should be ./prog1 -m M -b B\n");
		fprintf(stderr,"M= max reps, B= SIGUSR1 block\n");
	}

	act.sa_handler = handler;
	if(sigaction(SIGUSR1, &act, NULL)==-1){
		fprintf(stderr, "seting signal action failed\nExiting\n");
		return 1;
	}
	
	const int reps = atoi(argv[2]);
	const pid_t pid = getpid();

	if(atoi(argv[4])){
		//block sigusr1
		if(sigemptyset(&set)==-1){
			fprintf(stderr, "emptying signal set failed\nExiting\n");
			return 1;
		}
		if(sigaddset(&set, SIGUSR1)==-1){
			fprintf(stderr, "adding signal to set failed\nExiting\n");
			return 1;
		}
		if(sigprocmask(SIG_BLOCK, &set, NULL)==-1){
			fprintf(stderr, "setting signal mask failed\nExiting\n");
			return 1;
		}
		blocked = 1;
	}

	while(i<reps){

		if(blocked && i>(reps-1)/2){
			//unblock sigusr1
			blocked = 0;
			if(sigprocmask(SIG_UNBLOCK, &set, NULL)==-1){
				fprintf(stderr, "setting signal mask failed\nExiting\n");
				return 1;
			}
		}

		printf("pid=%d, counter=%d, reps=%d\n", pid, i, reps);
		i++;
		sleep(5);
	}

	return 0;
}