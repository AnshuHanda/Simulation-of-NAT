/******************************************************************************************************************************
*
* NAME: thread_main.c
*
* DESCRIPTION: This function works as Timer for NAT simulation. It continuously checks a flag and if it is set it 
*		resets the counter otherwise when counter elapses it frees the memory and returns the address to file.
*             
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

#include "header.h"

pthread_mutex_t counter = PTHREAD_MUTEX_INITIALIZER;              //Mutex initializer

void *pthread_main(void *arg)
{
	FILE *log_ptr = fopen("../files/timer_log", "w+");
	if(NULL == log_ptr)
	{
		perror("Error opening log file for timer\n");
		pthread_exit(NULL);
	}
	
	extern U32_INT g_trace_level;
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Thread begins\n"));
	extern struct node *head;
	U32_INT count = 60, len;
	struct node *trv = NULL;
	SCHAR *inter = NULL;
	struct pthread *pthread_ptr = (struct pthread *) arg;
	struct node *temp = NULL;
	pthread_ptr->flag = 0;                                        //resets the flag 
	inter = (SCHAR *)calloc(LINE_LEN, sizeof(SCHAR));
	if(NULL == inter)
	{
	perror("Memory allocation fails");
	pthread_exit(NULL);
	}


	while(1)
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "SLEEP Initiated\n"));
		sleep(1);					//sleep for 1 second
		count -= 1;					//decrement counter
		if(count == 0)
		{
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MUTEX locked\n"));
			pthread_mutex_lock(&counter);		//lock mutex
			rewind(pthread_ptr->f_ptr);		//rewind file pointer to start
			trv = head;
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Checking if only one entry in tabel\n"));
			if(trv->next == NULL)			//if one entry in table
			{
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Reading data from file\n"));
			  	while(NULL != fgets(inter, LINE_LEN, pthread_ptr->f_ptr))   //checking for address in file 
                                {
                                        if(!(strncmp(inter, pthread_ptr->pub_ip, MAX)))		//function to change the flag to free
                                        {
                                                len = strlen(inter);
                                                inter[len - 2] = FREE;
                                                fseek(pthread_ptr->f_ptr, -2, SEEK_CUR);
                                                fprintf(pthread_ptr->f_ptr, "%s", &inter[len - 2]);
                                                break;
                                        }
                                }

				free(head);
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Entry freed\n"));
				head = NULL;
				rewind(pthread_ptr->f_ptr);               //rewind the file pointer
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "File pointer rewinded\n"));
				printf("Entry deleted\n");
				pthread_mutex_unlock(&counter);			//release the mutex
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Mutex unlocked\n"));		
				break;
			}
	
			else 
			{
				if(head == pthread_ptr->ptr)
				{
					A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Head is entry to be deleted\n"));
					trv = head;
					head = head->next;
				}
				else 
				{
					A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Searching for the entry to be deleted\n"));
					while(trv != (pthread_ptr->ptr))     //traversing the table till the corrosponding entry found
					{
						temp = trv;
						trv = trv->next;
					}
					
					temp->next = pthread_ptr->ptr->next;
				}

				free(trv);
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Entry freed\n"));
				rewind(pthread_ptr->f_ptr);
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "File pointer rewinded\n"));
				inter = (SCHAR *)calloc(LINE_LEN, sizeof(SCHAR));
	
				while(NULL != fgets(inter, LINE_LEN, pthread_ptr->f_ptr))     //checking for address in file
				{
					A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Reading data from file\n"));
					if(!(strncmp(inter, pthread_ptr->pub_ip, MAX)))    //comparing and changing flag to free
					{
						len = strlen(inter);
						inter[len - 2] = FREE;
						fseek(pthread_ptr->f_ptr, -2, SEEK_CUR);
						fprintf(pthread_ptr->f_ptr, "%s", &inter[len - 2]);
						break;
					}
				}
			  
				rewind(pthread_ptr->f_ptr);
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "File pointer rewinded\n"));
				printf("Entry deleted\n");
				pthread_mutex_unlock(&counter);							//release mutex
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Mutex unlocked\n"));		
				break;
			}
		}
		
		else if(pthread_ptr->flag == 1)                   //checking for flag
		{
			printf("Counter reset\n");
			pthread_ptr->flag = 0;
			count = 180;					//reseting counter
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s","COUNTER reset\n"));
		}
	}

	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "Thread exited normally\n"));
	free(inter);
	pthread_exit(NULL);
}	
