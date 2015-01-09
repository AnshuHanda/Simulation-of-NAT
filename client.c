
/**8888888888
ME: client.c
*
* DESCRIPTION: This function works as client for NAT simulation. It reads data from a file specified as command line argument
*              sends the same to server. It calls a function for displaying output.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 2014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

#include "header.h"

FILE *log_ptr = NULL;
U32_INT g_trace_level = DETAILED_TRACE;

/* main starts */
S32_INT main(S32_INT argc, SCHAR *argv[])
{
	log_ptr = fopen("../files/client_log", "w+");
	if(NULL == log_ptr)
	{
		perror("Error opening client log file");
		exit(EXIT_FAILURE);
	}
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CLIENT BEGINS\n"));

	U32_INT rlen, slen;
	U32_INT sd, ret;
	SCHAR *buffer = NULL;
	SCHAR *recvbuf = NULL;
	struct sockaddr_in serveraddr;
	memset(&serveraddr, '0', sizeof(serveraddr));
	FILE *f_ptr = NULL;
	
	if(4 > argc)                              //Error checking for command line arguments
	{
		printf("Usage: %s <server ip_adddr> <port> <ip_packet_filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	f_ptr = fopen(argv[3], "r");              //Opening file for reading
	if(NULL == f_ptr)
	{
		perror("Error opening input file");
		exit(EXIT_FAILURE);
	}

	// populating the socket structure
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);  

	sd = socket(AF_INET, SOCK_STREAM, 0);          //creating socket
	if(0 > sd)
	{
		perror("Socket creation fails");
		exit(EXIT_FAILURE);
	}

	ret = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));    //request for connection to the server
	if(0 > ret)
	{
		perror("Connection fails");
		exit(EXIT_FAILURE);
	}

	buffer = (SCHAR *)calloc(IP_HEADER_LEN + 1, sizeof(SCHAR));         // Allocating memory for reading data from file
	if(NULL == buffer)                                  //error check
	{
		perror("Memory can't be allocated to buffer");
		exit(EXIT_FAILURE);
	}
	memset(buffer, '\0', IP_HEADER_LEN + 1);

	printf("################## CLIENT SIDE DATA #######################\n");
	if(NULL == fgets(buffer, IP_HEADER_LEN + 1, f_ptr))
	{
		printf("Reading error\n");
		exit(EXIT_FAILURE);
	}

	if(IP_HEADER_LEN != strlen(buffer))
	{
		printf("Not a valid IP header\n");
		exit(EXIT_FAILURE);
	}

	if(EXIT_FAILURE == error_check(buffer))
	{
		perror("Not a valid IP\n");
		exit(EXIT_FAILURE);
	}
	slen = strlen(buffer);
	slen = htons(slen);                                                 //host to network byte order conversion
	ret = mywrite(sd, &slen, sizeof(slen));                             //writing slen to socket
	if(0 > ret)
	{
		perror("Mywrite for slen fails");
		exit(EXIT_FAILURE);
	}

	slen = ntohs(slen);
	ret = mywrite(sd, buffer, slen);                                    //writing data to socket
	
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DATA SENT TO SERVER\n"));			   //error handling
	A_TRACE(DETAILED_TRACE, (log_ptr, "%s\n", buffer));

	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED FUNCTION FORMAT CONVERSIO\n"));
	ret = format_conversion_display(buffer);                           //calling function to display sent data
	if(0 > ret)
	{
		perror("format conversion function fails");
		exit(EXIT_FAILURE);
	}

	printf("######################## SERVER SIDE DATA ###################\n");
	ret = myread(sd, &rlen, sizeof(rlen));                             // redaing rlen from socket
	if(0 > ret)
	{
		perror("myread for rlen fails");
		exit(EXIT_FAILURE);
	}
	
	rlen = ntohs(rlen);                                             //network to host byte order conversion

	recvbuf = (SCHAR *)calloc(rlen + 1, sizeof(SCHAR));	
	memset(recvbuf, '\0', strlen(recvbuf) + 1);                      //reseting buffer
	ret = myread(sd, recvbuf, rlen);				       //reading message from socket			
	if(0 > ret)
	{
		perror("Message reading fails");
		exit(EXIT_FAILURE);
	}
	printf("%s\n",recvbuf);
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "RECEIVED MESSAGE FROM SERVER:  "));
	A_TRACE(DETAILED_TRACE, (log_ptr, "%s\n", buffer));
	ret = myread(sd, &rlen, sizeof(rlen));                         //reading rlen from socket
	
	rlen = ntohs(rlen);						//network to host byte order conversion			

	if(rlen > 0)
	{
		memset(buffer, '\0', strlen(buffer) + 1);			//reseting buffer
		ret = myread(sd, buffer, rlen);					//reading data from socket		
		if(0 > ret)
		{
			perror("buffer reading fails");
			exit(EXIT_FAILURE);
		}
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "RECEIVED DATA FROM SERVER:  "));
		A_TRACE(DETAILED_TRACE, (log_ptr, "%s\n", buffer));
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED FUNCTION FORMAT CONVERSION\n"));
		ret = format_conversion_display(buffer);                        //calling function for display
		if(0 > ret)
		{
			perror("Format conversion function fails");
			exit(EXIT_FAILURE);
		}
	}
	free(recvbuf);
	free(buffer);
	ret = close(sd);						//closing socket connection
	if(0 > ret)
	{
		perror("closing of socket descriptor fails");
		exit(EXIT_FAILURE);
	}
						//error check
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "END OF CLIENT PROGRAM\n"));
	ret = fclose(log_ptr);
	if(0 > ret)
	{
		perror("closing of log file fails");
		exit(EXIT_FAILURE);
	}

	ret = fclose(f_ptr);						//closing file pointer
	if(0 > ret)
	{
		perror("closing of input file fails");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
