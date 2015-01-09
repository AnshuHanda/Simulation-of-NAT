/******************************************************************************************************************************
*
* NAME: server.c
*
* DESCRIPTION: This function works as server for NAT simulation. It reads data from a file containing public IP addresses.
*              Calls function fro IP extraction, space identification, mapping table update(NAT table) and finally sends 
*              the data to server by checking some conditions.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

#include "header.h"

FILE *log_ptr = NULL;
U32_INT g_trace_level = DETAILED_TRACE;
struct node *head = NULL;

S32_INT main(S32_INT argc, SCHAR *argv[])
{
	log_ptr = fopen("../files/server_log", "w+");
	if(NULL == log_ptr)
	{
		perror("error opening server log file");
		exit(EXIT_FAILURE);
	}	
	
	FILE *f_ptr = NULL;
        f_ptr = fopen("../files/public_ip_pool", "r+");                //Opening file containing publuic IPs
        if(NULL == f_ptr)                                  //error checking
        {
                perror("Opening of public IP file fails");
                exit(EXIT_FAILURE);
	}

	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "SERVER PROGRAM BEGINS\n"));
	U32_INT value = 1, choice, map;
	S32_INT sd, connsd, ret;
	U32_INT rlen, slen;
	SCHAR *s_ip, *d_ip;
	S32_INT sip_space, dip_space;
	socklen_t len;
	SCHAR *sendbuf = NULL, *recvbuf = NULL;

	struct sockaddr_in serveraddr, clientaddr;
	memset(&serveraddr, '0', sizeof(serveraddr));
	memset(&clientaddr, '0', sizeof(clientaddr));
	
	//populating socket structure
	serveraddr.sin_family = AF_INET;                   
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sd = socket(AF_INET, SOCK_STREAM, 0);              //creating socket
	if(0 > sd)                               //error check
	{
		perror("Socket creation fails");
		exit(EXIT_FAILURE);
	}

	ret = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));          //binding socket to structure
	if(0 > ret)
	{
		perror("Bind fails");
	}
	ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
	if(0 > ret)
	{
		perror("Setsockopt fails");
		exit(EXIT_FAILURE);
	}
	
	ret = listen(sd, MAX_Q);                          //listening for connections
	if(0 > ret) 				//error check
	{
		perror("Listen fails");
		exit(EXIT_FAILURE);
	}

	len = sizeof(clientaddr);      
	
	while(1)
	{
		printf("Press 1 to display NAT table or any other key to skip\n");
		scanf("%d",&choice);
		if(choice == 1)
		{
			ret = display_table(head);
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "TABLE DISPLAYED\n"));
			if(0 > ret)
			{
				perror("Display table function fails");
				exit(EXIT_FAILURE);
			}	
		}
		
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "SOCKET LISTENING\n"));
		printf("Listening for connection\n");
		connsd = accept(sd, (struct sockaddr *)&clientaddr, &len);     //Accepting connection
		
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CONNECTION ACCEPTED\n"));

		ret = myread(connsd, &rlen, sizeof(rlen));             //reading rlen
		if(0 > ret)                                //error check
		{
			perror("Myread for rlen fails");
			exit(EXIT_FAILURE);
		}

		rlen = ntohs(rlen);                         //network to host byte order conversion
		if(rlen != IP_HEADER_LEN)                   //checking if data that will be received is a valid IP header
		{
			printf("Received data not a valid IP header\n");
			ret = close(connsd);
			if(0 > ret)                                //error check
			{
				perror("closing of connsd fails");
				exit(EXIT_FAILURE);
			}

			continue;
		}

		recvbuf = (SCHAR *)calloc(rlen + 1, sizeof(SCHAR));  //memory allocation for receiving data
		if(NULL == recvbuf)          //error check
		{
			perror("Memory allocation fails for recvbuf");
			exit(EXIT_FAILURE);
		}
		memset(recvbuf, '\0', rlen);

		ret  = myread(connsd, recvbuf, rlen);      //reading data
		if(0 > ret)                                //error check
		{
			perror("Myread for buffer fails");
			exit(EXIT_FAILURE);
		}
		recvbuf[ret] = '\0';

		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DATA RECEIVED FROM CLIENT:  "));
		A_TRACE(DETAILED_TRACE, (log_ptr, "%s\n", recvbuf));	
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED IP EXTRACT WITH SOURCE IP\n"));
		s_ip = ip_extract(recvbuf, 0);      //function call for extracting sourc IP
		if(NULL == s_ip)
		{
			perror("IP extract function fails");
			exit(EXIT_FAILURE);
		}
		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED FUNCTION IP EXTRACT WITH DESTINATION IP \n"));
		d_ip = ip_extract(recvbuf, 1);    // function call for extracting destination IP
		if(NULL == d_ip)
		{
			perror("IP extraction function fails");
			exit(EXIT_FAILURE);
		}

		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED SPACE IDENTIFIER FOR SOURCE IP\n"));
		sip_space = space_identifier(s_ip);   //IP space identification for source IP 
		if(0 > sip_space)                                //error check
		{
			perror("space identifier function fails");
			exit(EXIT_FAILURE);
		}

		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED SPACE IDENTIFIER FOR DESTINATION IP\n"));
		dip_space = space_identifier(d_ip);   //IP space identification for destination IP
		if(0 > dip_space)                                //error check
		{
			perror("space identifier function fails");
			exit(EXIT_FAILURE);
		}

		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "CALLED FUNCTION MAPPING TABLE UPDATE\n"));
		sendbuf = mapping_table_update(s_ip, d_ip, sip_space, dip_space, f_ptr, &map, &head);//fuction call for updating NAT table
	
		switch(map)                            //checking if NAT applied or not
		{
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s","SWITCHING MAPPING FLAG\n"));
			case 0:                       //No nat required
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FLAG IS 0\n"));
				slen = strlen("S_ip private, d_ip private NAT not required\n");
                       		slen = htons(slen);

                        	ret = mywrite(connsd, &slen, sizeof(slen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for slen fails");
					exit(EXIT_FAILURE);
				}


                        	slen = ntohs(slen);                                //sending message
                        	ret = mywrite(connsd, "S_ip private, d_ip private NAT not required\n", slen);
				if(0 > ret)                                //error check
				{
					perror("Mywrite for message fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MESSAGE SENT TO CLIENT\n"));

				rlen = strlen(recvbuf);
                        	rlen = htons(rlen);
                        	ret = mywrite(connsd, &rlen, sizeof(rlen));   
				if(0 > ret)                                //error check
				{
					perror("Mywrite for rlen fails");
					exit(EXIT_FAILURE);
				}

				recvbuf[rlen] = '\0';
                        	rlen = ntohs(rlen);
                        	ret = mywrite(connsd, recvbuf, rlen);       //sending receved data as it is
				if(0 > ret)                                //error check
				{
					perror("Myread for recvbuf fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DATA SENT TO CLIENT: "));
				A_TRACE(DETAILED_TRACE, (log_ptr, "%s\n", recvbuf));
				break;
					
			case 1:                              //NAT applied
                		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FLAG IS 1\n"));
		                recvbuf = packet_modifier(recvbuf, sendbuf, map); //function call for packet modification
				if(NULL == recvbuf)
				{
					perror("Packet modifier function fails");
					exit(EXIT_FAILURE);
				}	
                                slen = strlen("NAT applied\n");
                                slen = htons(slen);

                                ret = mywrite(connsd, &slen, sizeof(slen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for slen fails");
					exit(EXIT_FAILURE);
				}


                                slen = ntohs(slen);                       //sending message
                                ret = mywrite(connsd, "NAT applied\n", slen);
				if(0 > ret)                                //error check
				{
					perror("Mywrite for Message fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MESSAGE SENT TO CLIENT\n"));

				rlen = strlen(recvbuf);
                                rlen = htons(rlen);
                                ret = mywrite(connsd, &rlen, sizeof(rlen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for rlen fails");
					exit(EXIT_FAILURE);
				}


                                rlen = ntohs(rlen);
                                ret = mywrite(connsd, recvbuf, rlen);          //sending modified IP packet
				if(0 > ret)                                //error check
				{
					perror("Mywrite for recvbuf fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "DATA SENT TO CLIENT: "));
                                A_TRACE(DETAILED_TRACE, (log_ptr, "%s\n", recvbuf));
                                break;
		
			 case 2:                            //Situation NOT possible
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FLAG IS 2\n"));
                                slen = strlen("Packet dropped because S_ip public, d_ip private not possible\n");
                                slen = htons(slen);

                                ret = mywrite(connsd, &slen, sizeof(slen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for slen fails");
					exit(EXIT_FAILURE);
				}


                                slen = ntohs(slen);                               //sending message
                                ret = mywrite(connsd, "Packet dropped because S_ip public, d_ip private not possible\n", slen);
				if(0 > ret)                                //error check
				{
					perror("Mywrite for message fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MESSAGE SENT TO CLIENT\n"));

				rlen = 0;
                                rlen = htons(rlen);
                                ret = mywrite(connsd, &rlen, sizeof(rlen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for rlen fails");
					exit(EXIT_FAILURE);
				}

                                break;

			case 3:	                                //NO public IP available
				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FLAG IS 3\n"));
				slen = strlen("Packed dropped because no public IP is free\n");
                        	slen = htons(slen);

                        	ret = mywrite(connsd, &slen, sizeof(slen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for slen fails");
					exit(EXIT_FAILURE);
				}


                 	        slen = ntohs(slen);                           //sending message
                       		ret = mywrite(connsd, "Packet dropped because no public IP is free\n", slen);
				if(0 > ret)                                //error check
				{
					perror("Mywrite for message fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MESSAGE SENT TO CLIENT\n"));

				rlen = 0;
                        	rlen = htons(rlen);
                        	ret = mywrite(connsd, &rlen, sizeof(rlen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for rlen fails");
					exit(EXIT_FAILURE);
				}

				break;
		
			 case 4:                                      //NO entry for given Public Source IP
                                A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FLAG IS 4\n"));
				slen = strlen("Packed dropped because no entry found for sourcr public\n");
                                slen = htons(slen);

                                ret = mywrite(connsd, &slen, sizeof(slen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for slen fails");
					exit(EXIT_FAILURE);
				}


                                slen = ntohs(slen);                           //sending message
                                ret = mywrite(connsd, "Packet dropped because no entry found for source public\n", slen);
				if(0 > ret)                                //error check
				{
					perror("Mywrite for message fails");
					exit(EXIT_FAILURE);
				}

				A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MESSAGE SENT TO CLIENT\n"));

				rlen = 0;
                                rlen = htons(rlen);
                                ret = mywrite(connsd, &rlen, sizeof(rlen));
				if(0 > ret)                                //error check
				{
					perror("Mywrite for rlen fails");
					exit(EXIT_FAILURE);
				}

                                break;
	
			default:
				break;
		}

		A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "SERVER FINISHED SERVING CLIENT\n"));
		ret = close(connsd);                       //closing connection
		if(0 > ret)                                //error check
		{
			perror("closing of connsd fails");
			exit(EXIT_FAILURE);
		}
	
		free(recvbuf);                  //free buffer
		free(sendbuf);
		free(s_ip);
		free(d_ip);
	}

	ret = close(sd);                  //closing server socket
	if(0 > ret)                                //error check
	{
		perror("closing of sd fails");
		exit(EXIT_FAILURE);
	}

 	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "SERVER PROGRAM ENDS\n"));
	ret = fclose(log_ptr);
	if(0 > ret)                                //error check
	{
		perror("closing of log file fails");
		exit(EXIT_FAILURE);
	}

	fclose(f_ptr);
	return EXIT_SUCCESS;
}
