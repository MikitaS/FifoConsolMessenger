#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>


enum CodeComand
{
	NotAnyComand = 0,
	ComandShowHistory = 1,
	ComandStopChat = 2,
	ComandHelp = 3,
	ComandClear = 4
};

const int BufferSize = 100;

int 	StartChat			(int FifoStatus, size_t fd1, size_t fd2);
int 	WriteInFifo			(int FifoStatus, size_t fd1, size_t fd2);
int 	ReadFromFifo		(int FifoStatus, size_t fd1, size_t fd2);
int 	CheckComand			(const char *buffer);
void 	HelpComandFunction	(void);
void 	ClearComandFunction	(void);

void ClearComandFunction(void)
{
	//printf("\033[2J");
	system("clear");
}


void HelpComandFunction(void)
{
	printf("_____INFO_____\nComands:\n\t/history\n\t/help\n\t/stop\n\t/clear\nAttention: don't write messages with the first symbol \"/\"\nAttention: message's length shouldn't be more than 100 symbols\n______\n");
}

int CheckComand(const char *buffer)
{
	assert(buffer);
	
	if(!strcmp(buffer, "/stop"))
		return ComandStopChat;
	else if(!strcmp(buffer, "/history"))
		return ComandShowHistory;
	else if(!strcmp(buffer, "/help"))
		return ComandHelp;
	else if(!strcmp(buffer, "/clear"))
		return ComandClear;
	else
		return NotAnyComand;
}

int WriteInFifo(int FifoStatus, size_t fd1, size_t fd2)
{
	int comand = 0, CycleStatus = 1;
	char *buffer = 0;
	buffer = (char*)calloc(BufferSize, sizeof(char));
	assert(buffer);
	
	if(FifoStatus == 0)
	{
		while(CycleStatus > 0)
		{
			gets(buffer);
			if(*buffer == '/')
			{
				comand = CheckComand(buffer);
				if(comand == ComandHelp)
					HelpComandFunction();
				else if(comand == ComandClear)
					ClearComandFunction();
				else if(comand == ComandStopChat)
				{
					CycleStatus = 0;
					write(fd1, "/1", BufferSize);
					write(fd2, "/1", BufferSize);
				}
				
			}
			else
				write(fd1, buffer, BufferSize);
			
		}
	}
	else
		while(CycleStatus > 0)
		{
			gets(buffer);
			if(*buffer == '/')
			{
				comand = CheckComand(buffer);
				if(comand == ComandHelp)
					HelpComandFunction();
				else if(comand == ComandClear)
					ClearComandFunction();
				else if(comand == ComandStopChat)
				{
					CycleStatus = 0;
					write(fd1, "/2", BufferSize);
					write(fd2, "/2", BufferSize);
				}
				
			}
			else
				write(fd2, buffer, BufferSize);
		}
	
	free(buffer);
}

int ReadFromFifo(int FifoStatus, size_t fd1, size_t fd2)
{
	int CycleStatus = 1;
	char *buffer = 0;
	buffer = (char*)calloc(BufferSize, sizeof(char));
	assert(buffer);
	
	if(FifoStatus == 0)
	{
		while(CycleStatus > 0)
		{
			read(fd2, buffer, BufferSize);
			if(*buffer == '/')
			{
				if(!strcmp(buffer, "/1"))
					CycleStatus = 0;
				else
					printf("/_______\n/YOU COMPANION LEAVED THE CHAT\n/_______");
			}
			else
				printf("Another process:\t%s\n",buffer);
		}
	}
	else
		while(CycleStatus > 0)
		{
			read(fd1, buffer, BufferSize);
			if(*buffer == '/')
			{
				if(!strcmp(buffer, "/2"))
					CycleStatus = 0;
				else
					printf("/_______\n/YOU COMPANION LEAVED THE CHAT\n/_______");
			}
			else
				printf("Another process:\t%s\n",buffer);
		}
	
	free(buffer);
}

int StartChat(int FifoStatus, size_t fd1, size_t fd2)
{
	pid_t ForkId = fork();
	
	if(ForkId == 0)
	{
		ReadFromFifo(FifoStatus, fd1, fd2);
	}
	else
	{
		WriteInFifo(FifoStatus, fd1, fd2);
		close(fd1);
		close(fd2);
		exit(0);
	}
	
	return 1;
}

int main()
{
	size_t mknodstatus = -1, fd = -1, mknodstatus2 = -1, fd2 = -1;
	
	umask(0);
	mknodstatus = mknod("./fifo.fifo",S_IFIFO | 0666, 0);
	fd = open("./fifo.fifo", O_RDWR);
	
	if(mknodstatus == -1 && fd == -1) 
		printf("Error, fifo file 1 didn't created\n");
	else if(mknodstatus == 0 && fd == -1)
		printf("Error, fifo file 1 was created, but it can't be opened\n");
	else if(mknodstatus == 0 && fd != -1)
		printf("Done, fifo file 1 was opened\n");
	else
		printf("Done, fifo file 1 was created and opened\n");
		
	umask(0);
	mknodstatus2 = mknod("./fifo2.fifo",S_IFIFO | 0666, 0);
	fd2 = open("./fifo2.fifo", O_RDWR);
	
	if(mknodstatus2 == -1 && fd2 == -1)
		printf("Error, fifo file 2 didn't created\n");
	else if(mknodstatus2 == 0 && fd2 == -1)
		printf("Error, fifo file 2 was created, but it can't be opened\n");
	else if(mknodstatus2 == 0 && fd2 != -1)
		printf("Done, fifo file 2 was opened\n");
	else
		printf("Done, fifo file 2 was created and opened\n");
	
	printf("______\nWELCOME TO CHAT, TO KNOW MORE INFORMATION USE COMAND \"/help\"\n");

	StartChat(mknodstatus, fd, fd2);
	
	printf("Chat was finished\n");
	
	if(!mknodstatus)
	{
		system("rm fifo.fifo fifo2.fifo");
	}
	
	close(fd);
	close(fd2);
	return 0;
}
