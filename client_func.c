/******************************************************************************************************************************
*
* NAME: format_conversion_display
*
* DESCRIPTION: This function converts the data in a presentable format and displays the same.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/

#include "header.h"

S32_INT format_conversion_display(SCHAR *buffer)
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FORMAT CONVERSION PROGRAM BEGINS\n"));

	U32_INT i, count = 0;
	U32_INT display[MAX + 1];
	memset(display, '0', MAX);                     //reset array

	if(NULL == buffer)    //check for data in buffer
	{
		printf("No data found in buffer\n");
		return EXIT_FAILURE;
	}

	printf("IP version............ IPv%d\n", (buffer[0] - ASCII_VAL));     //Display IP version
	
	if((buffer[IP_PROTOCOL] == '0') && (buffer[IP_PROTOCOL + 1] == '6'))    //check for TCP protocol
	{
		printf("IP Protocol is TCP\n");
	}
	else if((buffer[IP_PROTOCOL] == '1') && (buffer[IP_PROTOCOL + 1] == '7'))  //check for UDP protocol
	{
		printf("IP Protocol is UDP\n");
	}
	else
	{
		printf("Unknown Protocol\n");
	}

	for(i = SIP_START; i < DIP_START; )                      //conversion to hex
	{
		sscanf(&buffer[i], "%02x", &display[count]);
		i += 2; 
		count += 1;
	}

	printf("Source ip ...........");              //display sourcr IP
	for(i = 0; i < count; i++)
	{
		printf("%d",display[i]);
		if(i == count - 1)
		{
			break;
		}

		printf(".");
	}
	printf("\n");

	memset(display, '0', sizeof(display));   //setting memory to zero   
	count = 0;

	for(i = DIP_START; i < DIP_START + MAX; )             //conversion to hex
        {
                sscanf(&buffer[i], "%02x", &display[count]);
		i += 2;
                count += 1;
        }

        printf("Destination ip ...........");               //displaying destination IP
        for(i = 0; i < count; i++)
        {
                printf("%d",display[i]);
		if(i == count - 1)
		{
			break;
		}
		printf(".");
	}
        printf("\n");

	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "FORMAT CONVERSION FUNCTION RETURNED SUCCESSFULLY\n"));
	return EXIT_SUCCESS;
}
