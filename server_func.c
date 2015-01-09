/******************************************************************************************************************************
*
* NAME: ip_extract
*
* DESCRIPTION: This function is used for reading data.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

#include "header.h"

SCHAR *ip_extract(SCHAR *buffer, U32_INT type)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "START OF IP EXTRACT FUNCTION\n"));
	if(IP_HEADER_LEN != strlen(buffer))
	{
		perror("Not a valid input\n");
		return NULL;
	}

	U32_INT i;
	for (i=0 ; i<IP_HEADER_LEN; )        //lower case conversion
                {
                        buffer[i] = tolower(buffer[i]);
                        i += 1;
                }

	SCHAR *temp = NULL;
	temp = (SCHAR *)calloc(MAX + 1, sizeof(SCHAR));                  //memrory allocation for temporary storage of data
	if(NULL == temp)					//error check
	{
		perror("Memory allocation for temp fails");
		return NULL;
	}
	memset(temp, '\0', MAX + 1); 

	if(0 == type)                                                //if source IP to extract
	{
		for(i = SIP_START; i < DIP_START; i++)			//loop for copying s_ip
		{
			temp[i - SIP_START] = buffer[i];
		}
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP EXTRACT FUNCTION SUCCESSFULLY RETURNED\n"));
		return temp;
	}

	else if(1 == type)						//if destination IP to extract
	{	
		for(i = DIP_START; i < DIP_START + MAX; i++)		//loop for copying d_ip
		{
			temp[i - DIP_START] = buffer[i];
		}

		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP EXTRACT FUNCTION SUCCESSFULLY RETURNED\n"));
		return temp;
	}
	
	else
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP EXTRACT FUNCTION SUCCESSFULLY RETURNED\n"));
		exit(EXIT_FAILURE);
	}
}

/******************************************************************************************************************************
*
* NAME: space_identifier
*
* DESCRIPTION: This function is used for reading data.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

S32_INT space_identifier(SCHAR *temp)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP SPACE IDENTIFIER FUNCTION STARTS\n"));
	if(MAX != strlen(temp))
	{
		perror("Not a valid input\n");
		return EXIT_FAILURE;
	}

	if( ('0' == temp[0]) && ('a' == temp[1] ))             //class A private space check
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP SPACE IDENTIFIER FUNCTION SUCCESSFULLY RETURNED\n"));
		return PRIVATE;
	}
	
	else if( ('a' == temp[0]) && ('c' == temp[1]) && ('1' == temp[2]))        //class B private space check
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP SPACE IDENTIFIER FUNCTION SUCCESSFULLY RETURNED\n"));
		return PRIVATE;
	}

	else if( ('c' == temp[0]) && ('0' == temp[1]) && ('a' == temp[2]) && ('8' == temp[3]))   //class C private space check
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP SPACE IDENTIFIER FUNCTION SUCCESSFULLY RETURNED\n"));
		return PRIVATE;
	}
	
	else               				//if no condition true return PUBLIC
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "IP SPACE IDENTIFIER FUNCTION SUCCESSFULLY RETURNED\n"));
		return PUBLIC;
	}
} 

/******************************************************************************************************************************
*
* NAME: mapping_table_update
*
* DESCRIPTION: This function updates the NAT table.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

SCHAR *mapping_table_update(SCHAR *s_ip, SCHAR *d_ip, U32_INT sip_space, U32_INT dip_space, FILE *f_ptr, U32_INT *mapping, struct node **ptr)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MAPPING TABLE UPDATE FUNCTION STARTS\n"));
	if((MAX != strlen(s_ip)) || (MAX != strlen(d_ip)))
	{
		perror("Not a valid input\n");
		exit(EXIT_FAILURE);
	}

	SCHAR *inter = NULL;	
		
	if( (sip_space == PUBLIC) && (dip_space == PRIVATE) )          //condition check
	{
		*mapping = 2;                                           //mapping not applied
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MAPPING TABLE UPDATE FUNCTION SUCCESSFULLY RETURNED\n"));
		return NULL;						
	}

	else if( (sip_space == PRIVATE) && (dip_space == PRIVATE))     //condition check
	{
		inter = (SCHAR *)calloc(2*MAX + 1, sizeof(SCHAR));
		if(NULL == inter)
		{
			perror("Memory allocation for inter fails");
			exit(EXIT_FAILURE);
		}	
		memset(inter, '\0', 2*MAX + 1);
		strncpy(inter, s_ip, MAX);			       //copy s_ip to buffer
		strncat(inter, d_ip, MAX);				// append d_ip to buffer
		*mapping = 0;						//mapping not applied
	}
	
	else if( (sip_space == PRIVATE) && (dip_space == PUBLIC))        //condition check
	{
		inter = file_update(s_ip, d_ip, f_ptr, ptr);                   //Calling function for updating file and table
		if(NULL == inter)					//If no public IP is available
		{
			*mapping = 3;
		}
		
		else							//Returned pointer contains address of updated IP
		{
			*mapping = 1;
		}
	}

	else if( (sip_space == PUBLIC) && (dip_space == PUBLIC))               //last condition check when both destination and source IP are public
	{
		inter = table_search(s_ip, d_ip, *ptr);                          //Searching for an entry in table function call
		if(NULL == inter)					//If no entry found
		{
			*mapping = 4;
		}
	
		else                                                  //returned pointer contains a valid entry
		{
			*mapping = 1;
		}
	}
	
	else
	{
		perror("Not valid ip space\n");
		exit(EXIT_FAILURE);
	}
		
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MAPPING TABLE UPDATE FUNCTION SUCCESSFULLY RETURNED\n"));
	return inter;	
}

/******************************************************************************************************************************
*
* NAME: packet_modifier
*
* DESCRIPTION: This function replaces the older IP addresses with updated ones.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

SCHAR *packet_modifier(SCHAR *readbuf, SCHAR *sendbuf, U32_INT map_update)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "PACKET MODIFIER FUNCTION SUCCESSFULLY RETURNED\n"));
	if((IP_HEADER_LEN != strlen(readbuf)) || (2*MAX != strlen(sendbuf)))
	{
		perror("Not valid packets\n");
		return NULL;
	}
	U32_INT i = 0;
		
	for(i = SIP_START; i < DIP_START + MAX; i++)              //appending source and destination IP to original packet
	{
		readbuf[i] = sendbuf[i - SIP_START];
	}
	
	readbuf[IP_HEADER_LEN] = '\0';
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "PACKET MODIFIER FUNCTION SUCCESSFULLY RETURNED\n"));
	return readbuf;
}

/******************************************************************************************************************************
*
* NAME: display_table
*
* DESCRIPTION: This function displays the NAT table in presentable format.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

S32_INT display_table(struct node *ptr)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DISPLAY TABLE FUNCTION STARTS\n"));
	struct node *trv = NULL;
	U32_INT *buffer;
	buffer = (U32_INT *)malloc(sizeof(U32_INT));         //memory allocation to store returned address from format_conversion
	if(NULL == buffer)                    //error check
	{
		perror("Memory allocation for buffer fails");
		exit(EXIT_FAILURE);
	}
	memset(buffer, 0, MAX);			//reset memory
	U32_INT j = 0;

	printf("################################### NAT TABLE ##############################################\n");
	printf("Source Private IP              Destination Public IP                 Source Public IP\n");
	printf("-------------------------------------------------------------------------------------\n");
	trv = ptr;
	if(NULL == ptr)
	{
		printf("NO entry in table\n");
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DISPLAY TABLE FUNCTION SUCCESSFULLY RETURNED\n"));
		return EXIT_SUCCESS;
	}

	else 
	{
		while(trv != NULL)                                      //Check only for Occupied Public addresses
		{
			buffer = format_conversion(trv->spr_ip);                  //call function format_conversion
			for(j=0;j < (MAX/2); j++)                                   //display source private IP address
			{
				printf("%d", buffer[j]);
				if(j == (MAX/2) - 1)
				{
					break;
				}
				printf(".");    
			}
	
			printf("                      ");

			buffer = format_conversion(trv->dpu_ip);                    //call function format_conversion
                	for(j=0;j < (MAX/2); j++)					//display destination public IP address
                	{										
                        	printf("%d", buffer[j]);
                        	if(j == (MAX/2) - 1)
                        	{
                               		break;
                        	}
                        	printf(".");
                	}

                	printf("                      ");

			buffer = format_conversion(trv->pub_ip);                    //call function format_conversion
                	for(j=0;j < (MAX/2); j++)					//display source public IP address
                	{
                        	printf("%d", buffer[j]);
                        	if(j == (MAX/2) - 1)
                        	{
                                	break;
                        	}
                        	printf(".");
                	}

                	printf("                 ");
			printf("\n");
			trv = trv -> next;
		}
		
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DISPLAY TABLE FUNCTION SUCCESSFULLY RETURNED\n"));
		return EXIT_SUCCESS;
	}
	
	return EXIT_FAILURE;
}

/******************************************************************************************************************************
*
* NAME: format_conversion
*
* DESCRIPTION: This function converts the characters string passed to it into hexadecimal array.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

U32_INT *format_conversion(SCHAR *ptr)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FORMAT CONVERSION FUNCTION STARTS\n"));
	if(NULL == ptr)	
	{
		perror("Not valid data for conversion\n");
		return NULL;
	}

	U32_INT *display, i = 0, count = 0;
	display = (U32_INT *)calloc((MAX/2) + 1, sizeof(U32_INT));              //allocating memory for storing converted data.
	if(NULL == display)				//error check
	{
		perror("Memory allocation for display fails");
		return NULL;
	}
	memset(display, '0', sizeof(display));				//reset memory
	SCHAR *ptr_1;
	ptr_1 = (SCHAR *)calloc(MAX + 1, sizeof(SCHAR));			//memory allocation for storing passed string
	if(NULL == ptr_1)					//error check
	{
		perror("Memrory allocation fails");
		return NULL;
	}
	memset(ptr_1, '\0', sizeof(ptr_1));				//memory reset
	strncpy(ptr_1, ptr, MAX); 					//copying passed string to current string
	
       	for(i = 0; i < MAX; )						//copying 2 elemnts at a time and converting it to hexa
	{			
		sscanf(&ptr[i], "%02x", &display[count]);
		i += 2;
		count += 1;
	}

	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FORMAT CONVERSION FUNCTION SUCCESSFULLY RETURNED\n"));
	return display;
}
 




/******************************************************************************************************************************
*
* NAME: file_update 
*
* DESCRIPTION: This function replaces the older IP addresses with updated ones. It reads IP from a file if they are free 
*	       assigns the same to given source private IP.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

SCHAR *file_update(SCHAR *s_ip, SCHAR *d_ip,FILE *f_ptr, struct node **ptr)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FILE UPDATE FUNCTION STARTS\n"));
	if((MAX != strlen(s_ip)) || (MAX != strlen(d_ip)))
	{
		perror("Not a valid input\n");
		exit(EXIT_FAILURE);
	}

	if(NULL == f_ptr)
	{
		perror("File not opened\n");
		exit(EXIT_FAILURE);
	}

	U32_INT len;
	pthread_t tid;
	struct pthread *pthread_ptr = NULL;
	struct node *temp = NULL;
	struct node *trv = NULL;
	SCHAR inter[LINE_LEN];
	SCHAR *buffer = NULL;
	buffer = (SCHAR *)calloc(2*MAX + 1, sizeof(SCHAR));
	if(NULL == buffer)
	{
		perror("Memory allocation for buffer fails");
		exit(EXIT_FAILURE);
	}	
	memset(buffer, '\0', sizeof(buffer));
	rewind(f_ptr);	
       	temp = (struct node *)malloc(sizeof(struct node));              //memory allocation for structure to store table contents
       	if(NULL == temp)					//error check
	{
		perror("Memory allocation fails");
		exit(EXIT_FAILURE);
	}

	trv = *ptr;
	while(trv != NULL)
	{
		if(NULL == trv)
		{
			break;
		}

		else if(!(strncmp(trv->spr_ip, s_ip, MAX)))
		{
			trv->ptr->flag = 1;
			strncpy(buffer, trv->pub_ip, MAX);
			strcat(buffer, d_ip);
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FILE UPDATE FUNCTION SUCCESSFULLY RETURNED\n"));
			return buffer;
		}
		
		trv = trv->next;
	}

	while(NULL != fgets(inter, LINE_LEN, f_ptr))			//reading file
	{
		len = strlen(inter);
		if(inter[len - 2] == FREE)				//if free exit loop
                {
                	break;
                }
	
		if(NULL == f_ptr)
		{
			if(feof(f_ptr))
			{
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FILE UPDATE FUNCTION RETURNED WITH READ ERROR\n"));
				return NULL;
			}
		}
       	}

	if(inter[len - 2] == OCCUPIED)
	{
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FILE UPDATE FUNCTION RETURNED SUCCESSFULLY\n"));
		return NULL;
	}

	fseek(f_ptr, -2, SEEK_CUR);					//Updating flag of IP to occupied
        inter[len - 2] = OCCUPIED;
      	fprintf(f_ptr, "%s",&inter[len - 2]);

      	strncpy(temp->pub_ip, inter, MAX); 				//populating Table 
 	strncpy(temp->spr_ip, s_ip, MAX);
     	strncpy(temp->dpu_ip, d_ip, MAX);
     	temp->next = NULL;
	pthread_ptr = (struct pthread *)malloc(sizeof(struct pthread));
	if(NULL == pthread_ptr)
	{
		perror("Memory allocation fails");
		exit(EXIT_FAILURE);
	}

	temp->ptr = pthread_ptr;
	if(NULL == *ptr)						//checking if Ist entry of table
      	{
  	      *ptr = temp;
    	}

	else
	{
		trv = *ptr;
		while(trv->next != NULL)
		{
			trv = trv->next;
		}
		
		trv->next = temp;
	}

	pthread_ptr->ptr = temp;
	strncpy(pthread_ptr->spr_ip, s_ip, MAX);
	strncpy(pthread_ptr->pub_ip, temp->pub_ip, MAX);
	pthread_ptr->flag = 1;
	pthread_ptr->f_ptr = f_ptr;
	pthread_create(&tid, NULL, pthread_main, (void *)pthread_ptr);

    	strncpy(buffer, temp->pub_ip, MAX);            //if true copy pub_ip assigned in table to buffer
      	strncat(buffer, temp->dpu_ip, MAX);                      //append destination address to buffer
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FILE UPDATE FUNCTION SUCCESSFULLY RETURNED\n"));
    	return buffer;
}

/******************************************************************************************************************************
*
* NAME: table_search
*
* DESCRIPTION: This function replaces the older IP addresses with updated ones. It searches for given source IP in table
*             if entry found returns its corrosponding private IP.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 200614 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

SCHAR *table_search(SCHAR *s_ip, SCHAR *d_ip, struct node *ptr)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "TABLE SEARCH FUNCTION STARTS\n"));
	if((MAX != strlen(s_ip)) || (MAX != strlen(d_ip)))
	{
		perror("Not a valid input\n");
		exit(EXIT_FAILURE);
	}

	struct node *trv = NULL;
	SCHAR *buffer;
	buffer = (SCHAR *)calloc(2*MAX + 1, sizeof(SCHAR));
	if(NULL == buffer)
	{
		perror("Memory allocation fails");
		exit(EXIT_FAILURE);
	}
	memset(buffer, '\0', sizeof(buffer));

	trv = ptr;
	while(trv != NULL)                                                 //treversing the table
	{
		if(!(strncmp(trv->pub_ip, d_ip, MAX)))                           //if entry found
		{
			strncpy(buffer, s_ip, MAX);                        //copy s_ip to buffer
			strncat(buffer, trv->spr_ip, MAX);			//append d_ip to it
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "TABLE SEARCH FUNCTION SUCCESSFULLY RETURNED\n"));
			return buffer;
		}

		trv = trv->next;
	}

	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "TABLE SEARCH FUNCTION SUCCESSFULLY RETURNED\n"));
	return NULL;
}
