// C Program for Message Queue (Reader Process)
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#define MAXWORDS 100000
#define MAXLEN 100
// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;
char *words[MAXWORDS];  
int numWords = 0;
int main()
{
	key_t key;
	int msgid;

    // ftok to generate unique key - main server
    key = ftok(getenv("HOME"), 1);
  
  	int counter=3;

   	// msgget creates a message queue and returns identifier
   	msgid = msgget(key, 0666 | IPC_CREAT);
  	//printf("Key %d Msgid %d\n", key, msgid);
	
	char line[MAXLEN];
	FILE *fp = fopen("dictionary.txt","r");
	if(!fp)
	{
		puts("dictionary.txt cannot be open");
		exit(1);
	}
	int i = 0;
	
	while(fgets(line,MAXLEN,fp))
	{
		words[i] = (char *) malloc (strlen(line)+1);
		strcpy(words[i],line);
		i++;
	}
	numWords = i;
	printf("%d words read \n",numWords);
	srand(getpid() + time(NULL) + getuid());
	while (1) 
	{
    	// msgrcv to receive message
    	msgrcv(msgid, &message, sizeof(message), 1, 0);
    	// to destroy the message queue
    	//msgctl(msgid, IPC_RMID, NULL);
  		if(fork() ==0)
		{
    	// display the message
    //	printf("Data Received is : %s \n", message.mesg_text);
		int clientPid = atoi(message.mesg_text);

		//use clientPid to come up with key & msgid to respond
		int key2 = ftok(getenv("HOME"), clientPid);
    	int msgid2 = msgget(key2, 0666 | IPC_CREAT);
		
		int npid = getpid();
		message.mesg_type = 1;
		sprintf(message.mesg_text, "%d", npid);
    	msgsnd(msgid2, &message, sizeof(message), 0);
    //	printf("Data Sent is : %s\n", message.mesg_text);
		
		int key3 = ftok(getenv("HOME"),npid);
		int msgid3 = msgget(key3,0666 | IPC_CREAT);	

		//gametime
		srand(getpid() + time(NULL) + getuid);
		char *word = words[rand()%numWords];

		char *cptr1 = strchr(word, '\n');
		if(cptr1)
		{
			*cptr1 = '\0';
		}
		printf("Word chosen %s\n",word);

		int count = 0;
		for(int x = 0;x<MAXLEN;x++)
		{
			if(word[x] == NULL)
			{
				break;
			}
			count++;
		}

		char display[count];
		for(int y = 0;y<count;y++)
		{
			display[y] = '*';
		}
		sprintf(message.mesg_text,"(Guess) Enter letter for word: %s > ",display);
		msgsnd(msgid2,&message,sizeof(message),0);
		int wrongGuesses = 0;
		int finished = 0;
		int unexposed = count;
		while(1)
		{
			msgrcv(msgid3,&message,sizeof(message),1,0);

			char guess = message.mesg_text[0];

			int found = 0;
			int already = 0;
			
			for(int x = 0;x < count;x++)
			{
				if(guess == word[x])
				{
					found = 1;
					if(guess == display[x])
					{
						already =1;
					}
					else
					{

						display[x] = guess;
						unexposed--;
						
					}
				}
			}	
		
			if(unexposed == 0)
			{
				sprintf(message.mesg_text,"The word is %s! You missed %d times\n",word,wrongGuesses);
				msgsnd(msgid2,&message,sizeof(message),0);
				finished = 1;
			//	break;

			}
			if(found == 0 && already == 0)
			{
				sprintf(message.mesg_text, "%c is not in the word \n(Guess) Enter letter for word: %s > ",guess,display);
				msgsnd(msgid2,&message,sizeof(message),0);
				wrongGuesses++;
			}
			else if(already ==1)
			{
				
					sprintf(message.mesg_text, "%c is already in the word \n(Guess) Enter letter for word: %s > ",guess,display);
					msgsnd(msgid2,&message,sizeof(message),0);
			}
			else
			{
				if(already==0 && finished ==0)
				{
					sprintf(message.mesg_text,"(Guess) Enter letter for word: %s > ",display);
					msgsnd(msgid2,&message,sizeof(message),0);
				}
			}
		//counter += 2;
		}
  	} 
}  
    return 0;
	}

