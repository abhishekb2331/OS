#include<unistd.h>
#include<sys/utsname.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
int iter=0;
int last=0;
int last_char,cpid;
int no_of_process_que=0;
char initial_path[100];
void split_argument(char instruction[],char **argument) 	//Function to split the arguments according to space and tabs
{
	int i=0;
	while(instruction[i]!='\0')				//checking If the string has ended
	{
		while((instruction[i]==' ')||(instruction[i]=='\t'))
		{
			instruction[i]='\0';			//replacing whitespaces with null characters
			i++;
		}
		*argument++=&instruction[i];			//pointer to the starting pt of arguments
		last_char++;                                    //keeps list of number of arguments
		while((instruction[i]!='\0')&&(instruction[i]!=' ')&&(instruction[i]!='\t'))
		{
			i++;
		}
	}
	*argument='\0';                                 //indicating that the arguments have ended
}
void print_status(int pid)                              //function to display process info
{
	cpid=pid;                                       //process id
	char present_dir[100];
	getcwd(present_dir,100);			//contains the address of current working directory
	int i;
	char temp_dir[100],temp_dir2[100];
	strcpy(temp_dir,"/proc/");			//Contains info about all process running
	int tmp=pid;
	char process_id[100];
	for(i=0;tmp>0;i++)     				//Converting Process id to string
	{
		process_id[i]=tmp%10+'0';
		tmp=tmp/10;
	}
	process_id[i]='\0';
	tmp=strlen(process_id);
	char temp_id[100];
	int l=0;
	for(i=tmp-1;i>=0;i--)
	{
		temp_id[l++]=process_id[i];
	}
	temp_id[l]='\0';				//process id is in temp_id
	strcpy(temp_dir2,temp_id);
	strcat(temp_dir,temp_dir2);
	int err=chdir(temp_dir);			//Going to the directory of the process
	if(err==0)
	{
		FILE *fb=fopen("status","r");		//opening the file with required information
		char *c=NULL;
		int n;
		n=0;
//		if(fb==-1)
//		{
//			write(1,"Process is not present\n",23);
//		}
//		else
//		{
///			while(read(fb,c,1)>0)
//			{
//				write(1,c,1);
//			}
			while(getline(&c,&n,fb)>0)	//reading line by line
			{
			if(strncmp("Name",c,4)==0)
			{
				write(1,c,strlen(c));
			}
			else if(strncmp("State",c,5)==0) //comparing first 5 characters with 'State'
			{
				write(1,c,strlen(c));
				//printf("%s\n",c);
			}
			else if(strncmp("Pid",c,3)==0)
			{
				write(1,c,strlen(c));
				//printf("%s\n",c);
			}
			else if(strncmp("Vmsize",c,6)==0)
			{
				write(1,c,strlen(c));
				//printf("%s\n",c);
			}
			}
		//}
		fclose(fb);
		//char path[100],path2[100];
		char path2[100];
		char path[100];
		char temp_exepath[100];
		int flag=0;
		for(i=0;i<sizeof(temp_exepath);i++)
		{
			temp_exepath[i]='\0';
		}
		sprintf(path2,"%s/exe",temp_dir);
		readlink(path2,temp_exepath,sizeof(temp_exepath));
//		n=strlen(path);
//		path[n]='\0';
//		printf("%d\n",n);
		//printf("%s %s\n",temp_exepath,initial_path);
		i=0;
		if(strncmp(initial_path,temp_exepath,strlen(initial_path))==0)	//comparing if current_working _dir(cwd) contains initial_path(home directory)
		{
			flag=1;
			for(i=0;i<strlen(initial_path);i++)
			{
				if(initial_path[i]!=temp_exepath[i])
					break;
				i++;
			}
		}
		int k=0;
		while(i<strlen(temp_exepath))		//adding the remaining uncommon string in path
		{
			path[k++]=temp_exepath[i];
			i++;
		}
//		printf("%d\n",k);
		path[k]='\0';
		write(1,"Executable path -- ",19);
		if(flag==1)
			write(1,"~",1);
		write(1,path,strlen(path));
		write(1,"\n",1);
	}
	else						//Displaying Error if process not present
	{
		write(1,"Process is not present\n",23);
	}
	chdir(present_dir);				//Going to the initial directory
	return;
}
void sig_handler(int signo)				//Function to check the signals
{
	if (signo == SIGUSR1)
		write(1,"received SIGUSR1\n",17);
	else if (signo == SIGKILL)
		write(1,"received SIGKILL\n",17);
	else if (signo == SIGSTOP)
		write(1,"received SIGSTOP\n",17);
	else if (signo == SIGINT)
		write(1,"received SIGINT\n",16);
	else if (signo == SIGCHLD)
		write(1,"received SIGCHLD\n",16);
	else if (signo == SIGQUIT)
		write(1,"received SIGQUIT\n",16);
	else if (signo == SIGTSTP)			//For ctrl-Z
	{
//		int pid=getpid();
		if(last==1)
		{
		write(1,"recieved SIGTSTP\n",17);
		kill(cpid,SIGTSTP);			//Stops the process
		}
		else
		{
			write(1,"no process\n",11);
		}
	}
}

int main()
{
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)	//For Signal Handling
		write(1,"can't catch SIGUSR1\n",20);
	if (signal(SIGKILL, sig_handler) == SIG_ERR)
//		write(1,"can't catch SIGKILL\n",20);
	if (signal(SIGSTOP, sig_handler) == SIG_ERR)
//		write(1,"can't catch SIGSTOP\n",20);
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		write(1,"can't catch SIGSTOP\n",20);
	if (signal(SIGTSTP, sig_handler) == SIG_ERR)
		write(1,"can't catch SIGSTOP\n",20);
	if (signal(SIGQUIT, sig_handler) == SIG_ERR)
		write(1,"can't catch SIGSTOP\n",20);
	if (signal(SIGCHLD, sig_handler) == SIG_ERR)
		write(1,"can't catch SIGSTOP\n",20);

	int l,l_read,host_len,i,flag;
	int status,processid_que[100];
	char process_que[100][100];
	char *ptr;
	struct utsname unamedata;
	uname(&unamedata);
	char host_name[100],domain_name[100],path[100],instruction[100],current_working_dir[100],temp[100];
	char *argument[100];
	int fd[100][2];
	path[0]=' ';
	path[1]='\0';
	host_len=gethostname(host_name,100);     	//To get the hostname
	//getdomainname(domain_name,100);
	//	printf("<%s@%s:~%s>",host_name,unamedata.sysname,path);
	//	chdir(getenv("HOME"));     //no home
	getcwd(initial_path,100);			 //To Set the current working directory as home directory
	for(i=0;i<100;i++)
		instruction[i]='\0';
	while(1)
	{
		flag=0;
		getcwd(current_working_dir,100);	//Getting the present working directory
		//		strcpy(initial_path,getenv("HOME"));
		//		printf("^\n");
		i=0;
		if(strncmp(initial_path,current_working_dir,strlen(initial_path))==0)	//comparing if current_working _dir(cwd) contains initial_path(home directory)
		{
			flag=1;
			for(i=0;i<strlen(initial_path);i++)
			{
				if(initial_path[i]!=current_working_dir[i])
					break;
				i++;
			}
		}
		int k=0;
		while(i<strlen(current_working_dir))		//adding the remaining uncommon string in path
		{
			path[k++]=current_working_dir[i];
			i++;
		}
		path[k]='\0';
		write(1,"<",1);
		write(1,host_name,strlen(host_name)+1);
		write(1,"@",1);
		write(1,unamedata.sysname,sizeof(unamedata.sysname));
		write(1,":",1);
		if(flag==1)
			write(1,"~",1);
		write(1,path,strlen(path));
		write(1,">",1);
		l_read=read(0,instruction,100);
		int newtmp;
		while((newtmp=waitpid(-1,&status,WNOHANG))>0)		//Checking if any process has ended
//			if(newtmp>0)
			{
				//			printf("lol\n");
				for(i=0;i<no_of_process_que;i++)		//Checking which process it was
				{
					if(processid_que[i]==newtmp)
					{
						break;
					}
				}
				char new_temp[100];
				strcpy(new_temp,process_que[i]);
				for(i=i+1;i<no_of_process_que;i++)		//removing that process from the job list
				{
					processid_que[i-1]=processid_que[i];
					strcpy(process_que[i-1],process_que[i]);
				}
				no_of_process_que--;				//decrementing the number of process in job list
				write(1,new_temp,strlen(new_temp));		//process name
				write(1," with pid ",10);

				//	int tmp1[1];
				//	tmp1[0]=pid;
				//	write(1,&pid,sizeof(pid));
				//	puts(pid);
				int tmp=newtmp;
				char process_id[100];
				for(i=0;tmp>0;i++)			//converting process id to a string
				{
					process_id[i]=tmp%10+'0';
					tmp=tmp/10;
				}
				process_id[i]='\0';
				tmp=strlen(process_id);
				char temp_id[100];
				l=0;
				for(i=tmp-1;i>=0;i--)
				{
					temp_id[l++]=process_id[i];
				}
				temp_id[l]='\0';
				write(1,temp_id,strlen(temp_id));	//printing the process id
				write(1," exited successfully\n",21);
			}
		//getcwd(current_working_dir,100);
		//	ptr=strstr(current_working_dir,getenv("HOME"));
		//	printf("%s\n",path);
		//printf("%s\n",current_working_dir);
		//printf("%c",instruction[l_read -1]);
		instruction[l_read -1]='\0';
		last_char=0;
///		printf("arg:%s\n",instruction);
		split_argument(instruction,argument);
//		printf("arg:%s\n",argument[0]);
		//		printf("%d\n",last_char);
		//		i=1;
		//		while(argument[i]!='\0')
		//		{
		//			strcat(temp,current_working_dir);
		//			strcat(temp,argument[i]);
		//			argument[i]=temp;
		//			i++;
		//		}
		if(last_char>=1)			//checks if number of characters are atleast 1
		{
//			printf("%s\n",argument[0]);
			if(strcmp("cd",argument[0])==0)		//checking if the command is 'cd'
			{
				int err;
				//	execvp("/bin/cd",argument);
				//	execvp(*argument,argument);
				//	printf("YES %s\n",argument[1]);
				//			execl("/bin/cd",NULL)
				//	printf("%s\n",argument[1]);
				if(argument[1]!='\0')
				{
				if(strcmp("~",argument[1])==0)
				{
					err=chdir(initial_path);
				}
				else
				{
					err=chdir(argument[1]);		//changing directory
				}
				if(err==-1 && (strcmp(argument[0],"..")!=0))
				{
					write(1,"Folder is not present\n",22);
				}
				}
			}
			else if(strcmp("quit",argument[0])==0)		//checking if the command is 'quit'
			{
//				printf("ENTERED\n");
				//			printf("broke\n");
				return;
				break;
			}
			else if(strcmp("jobs",argument[0])==0)		//checking if the command is 'jobs'
			{
				//			printf("^\n");
				int r;
				for(r=0;r<no_of_process_que;r++)	
				{
					int tmp=processid_que[r];	//contains process id
					char process_id[100];   	//converting process id to string
					for(i=0;tmp>0;i++)
					{
						process_id[i]=tmp%10+'0';
						tmp=tmp/10;
					}
					process_id[i]='\0';
					tmp=strlen(process_id);
					char temp_id[100];
					int l=0;
					for(i=tmp-1;i>=0;i--)
					{
						temp_id[l++]=process_id[i];
					}
					temp_id[l]='\0';
					int p=r+1;			//contains job number
					char temp_no[100];		//converting job number to a string
					for(i=0;p>0;i++)
					{
						temp_no[i]=p%10+'0';
						p=p/10;
					}
					temp_no[i]='\0';
					char temp_no_final[100];
					l=0;
					for(i=i-1;i>=0;i--)
					{
						temp_no_final[l++]=temp_no[i];
					}
					temp_no_final[l]='\0';
					write(1,"[",1);
					write(1,temp_no_final,strlen(temp_no_final));	//printing job number
					write(1,"] ",2);
					write(1,process_que[r],strlen(process_que[r]));	//printing process name
					write(1," [",2);
					write(1,temp_id,strlen(temp_id));		//printing process id
					write(1,"]\n",2);
					//				printf("[%s] %s [%s]\n",temp_no_final,process_que[r],temp_id);
				}
			}
			else if(strcmp("kjob",argument[0])==0)		//checking if the command is 'kjob'
			{
				int job_id=atoi(argument[1]);		//contains job id
				if(job_id>no_of_process_que)
				{
					write(1,"No such process\n",16);
				}
				else
				{
				int kill_no=atoi(argument[2]);		//contains process id
				int err=kill(processid_que[job_id-1],kill_no);	//kills the process
				int r=job_id-1;
				if(err==-1)
				{
					int tmp=processid_que[r];	//contains process id
					char process_id[100];		//converting process id to a string
					for(i=0;tmp>0;i++)
					{
						process_id[i]=tmp%10+'0';
						tmp=tmp/10;
					}
					process_id[i]='\0';
					tmp=strlen(process_id);
					char temp_id[100];
					int l=0;
					for(i=tmp-1;i>=0;i--)
					{
						temp_id[l++]=process_id[i];
					}
					temp_id[l]='\0';
					int p=r+1;			//job number	
					char temp_no[100];		//converting job number to a string
					for(i=0;p>0;i++)	
					{
						temp_no[i]=p%10+'0';
						p=p/10;
					}
					temp_no[i]='\0';
					char temp_no_final[100];
					l=0;
					for(i=i-1;i>=0;i--)
					{
						temp_no_final[l++]=temp_no[i];
					}
					temp_no_final[l]='\0';
					//write(1,"Process cannot be killed\n",25);
					write(1,"Process ",8);
					write(1,process_que[r],strlen(process_que[r]));		//prints process name
					write(1," with pid ",10);
					write(1,temp_no_final,strlen(temp_no_final)); 		//prints process id
					write(1," cannot be killed\n",18);
				}
				}
			}
			else if(strcmp("overkill",argument[0])==0)		//checking if the command is overkill
			{
				int err;
				int r;
				for(r=0;r<no_of_process_que;r++)
				{
					err=kill(processid_que[r],9);		//kills the process
					if(err==-1)
					{
						int tmp=processid_que[r];	//contains process id
						char process_id[100];		//converting process id to a string
						for(i=0;tmp>0;i++)
						{
							process_id[i]=tmp%10+'0';
							tmp=tmp/10;
						}
						process_id[i]='\0';
						tmp=strlen(process_id);
						char temp_id[100];
						int l=0;
						for(i=tmp-1;i>=0;i--)
						{
							temp_id[l++]=process_id[i];
						}
						temp_id[l]='\0';
						int p=r+1;			//converting job no. to a string
						char temp_no[100];
						for(i=0;p>0;i++)
						{
							temp_no[i]=p%10+'0';
							p=p/10;
						}
						temp_no[i]='\0';
						char temp_no_final[100];
						l=0;
						for(i=i-1;i>=0;i--)
						{
							temp_no_final[l++]=temp_no[i];
						}
						temp_no_final[l]='\0';
						write(1,"Process ",8);
						write(1,process_que[r],strlen(process_que[r]));		//contains process name
						write(1," with pid ",10);
						write(1,temp_no_final,strlen(temp_no_final));		//contains process id
						write(1," cannot be killed\n",18);
					}
				}
				no_of_process_que=0;
			}
			else if(strcmp("pinfo",argument[0])==0)		//checking if the process is 'pinfo'
			{
				int tmp_pid;
				if(argument[1]=='\0')
				{
					tmp_pid=getpid();
					print_status(tmp_pid);
				}
				else
				{
					tmp_pid=atoi(argument[1]);
					print_status(tmp_pid);
				}
			}
			else if(strcmp("fg",argument[0])==0)		//checking if the process is 'fg'
			{
				int job_id=atoi(argument[1]);
				if(job_id>no_of_process_que)
				{
					write(1,"No such process\n",16);
				}
				else
				{
				int process_id=processid_que[job_id-1];
				kill(process_id,SIGCONT);
				wait(process_id,SIGCONT);
				for(i=job_id;i<no_of_process_que;i++)
				{
					processid_que[i-1]=processid_que[i];
					strcpy(process_que[i-1],process_que[i]);
				}
				no_of_process_que--;
				}
			}
			else
			{
				pid_t pid;
				pid=fork();				//create a new process
//				iter=1;
				last=1;
				cpid=pid;
				if(pid<0)				//error in creation
				{
					perror("Child not created\n");
				}
				else if(pid==0)				//child process
				{
//					iter=1;
			//	cpid=getpid();
					if((strlen(argument[last_char-1])==1)&&(argument[last_char-1][0]=='&'))	//checking if the last argument is '&'
					{
						*(argument +last_char-1)='\0';
					}
					int err=execvp(*argument,argument);		//run the command in argument[0]
					if(err==-1)
					{
						write(1,"Process cannot be executed\n",27);
						exit(0);
					}
				}
				else				//parents process
				{
//					iter=1;
					if((strlen(argument[last_char-1])==1)&&(argument[last_char-1][0]=='&'))	//checking if the last argument is '&'
					{
						//incrementing the number of process in job list
						(processid_que[no_of_process_que])=pid;
						//					*processid_que++;
						strcpy((process_que[no_of_process_que]),argument[0]);
						i=1;
						while(i<(last_char-1))
						{
							sprintf(process_que[no_of_process_que],"%s %s",process_que[no_of_process_que],argument[i]);
//							strcat(process_que[no_of_process_que],argument[i]);
								i++;
						}
						//					*process_que++;
						//					printf("%d %s\n",processid_que[no_of_process_que],process_que[no_of_process_que]);
						no_of_process_que++;
					}
					else
					{
					//	while(wait(&status)!=pid);
						iter=cpid;
						waitpid(cpid,&status,WUNTRACED);	//returns if a child has stopped	
						iter=0;
						if(WIFSTOPPED(status))		//returns true if the child process was stopped by delivery of a signal
						{				//adding the process to job list
						(processid_que[no_of_process_que])=pid;
						strcpy((process_que[no_of_process_que]),argument[0]);
						i=1;
						while(i<(last_char))
						{
							sprintf(process_que[no_of_process_que],"%s %s",process_que[no_of_process_que],argument[i]);
							//strcat(process_que[no_of_process_que],argument[i]);
								i++;
						}
						no_of_process_que++;
						}
					}
				}
			}
		}
		//write(1,instruction,l_read);
		//		printf("%s\n",getcwd());
		argument[0]='\0';
		for(i=0;i<100;i++)
			instruction[i]='\0';
	}
	//scanf("%s",instruction);
	//printf("%s",instruction);
	//fork();
	//	execl("/bin/ls","-la",NULL);
	return 0;
}
