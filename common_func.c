/******************************************************************************************************************************
*
* NAME: myread
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

S32_INT myread(S32_INT sd, void* buffer, U32_INT n) 
{
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MYREAD FUNCTION BEGINS\n")); 
	S32_INT ret = 0; 
	U32_INT pointer = 0; 
	U32_INT totnoofbytes = 0; 
	while (1) 
	{ 
		ret = read(sd, buffer + pointer, n - totnoofbytes); //reading data will return number of bytes read
		if (0 > ret) 
		{ 
			perror("I am getting error in read!!\n"); 
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MYREAD FUNCTION RETURNED WITH ERROR\n"));
			return EXIT_FAILURE; 
		} 
			 
		totnoofbytes = totnoofbytes + ret;      //incrementing total no of bytes read 
		pointer = pointer + ret; 		//increments the pointer for index 
		if ((totnoofbytes == n) || (ret == 0))  //if there is nothing to read condition check
		{ 
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MYREAD FUNCTION RETURNED SUCCESSFULLY\n"));
			return totnoofbytes; 
		} 
	} 
} 

/******************************************************************************************************************************
*
* NAME: mywrite
*
* DESCRIPTION: This function is used for writing data.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/
 
S32_INT mywrite(S32_INT sd, void* buffer, U32_INT n) 
{ 
	A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MWRITE FUNCTION BEGINS\n")); 
	S32_INT ret = 0; 
	U32_INT pointer = 0; 
	U32_INT totnoofbytes = 0; 
	while (1) 
	{ 
		ret = write(sd, buffer + pointer, n - totnoofbytes); //writing data
		if (0 > ret) 
		{ 
			perror("I am getting error in write!!\n"); 
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MYWRITE FUNCTION RETURNED WITH ERROR\n"));
                        return EXIT_FAILURE; 
		} 
			 
		totnoofbytes = totnoofbytes + ret;    //total no of bytes read update
		pointer = pointer + ret;             //pointer update
 
		if (totnoofbytes == n)               //if there is no data to write condition check
		{ 
			A_TRACE(BRIEF_TRACE, (log_ptr, "%s", "MYWRITE FUNCTION RETURNED SUCCESSFULLY\n"));
			return totnoofbytes; 
		} 
	} 
}

/******************************************************************************************************************************
*
* NAME: error_check
*
* DESCRIPTION: This function is used for validating hex data.
*
* GROUP           DATE                  REFERENCE                      REASON
* TH6_05          20-11-2014               --                          PROJECT
*
* Copyright © 20014 Aricent Inc  All Rights Reserved
*
******************************************************************************************************************************/


U32_INT  error_check(SCHAR *buffer)
{
        U32_INT i = 0;
        U32_INT ret;
        U32_INT len;
        len = strlen(buffer);
        while(i < len)
        {
                switch(buffer[i])
                {
                        case 'g':
                                ret = EXIT_FAILURE;
                                break;
                        case 'h':
                                ret = EXIT_FAILURE;
                                break;
                        case 'i':
                                ret = EXIT_FAILURE;
                                break;
                        case 'j':
                                ret = EXIT_FAILURE;
                                break;
                        case 'k':
                                ret = EXIT_FAILURE;
                                break;
                        case 'l':
                                ret = EXIT_FAILURE;
                                break;
                        case 'm':
                                ret = EXIT_FAILURE;
                                break;
                        case 'n':
                                ret = EXIT_FAILURE;
                                break;
                        case 'o':
                                ret = EXIT_FAILURE;
                                break;
                        case 'p':
                                ret = EXIT_FAILURE;
                                break;
                        case 'q':
                                ret = EXIT_FAILURE;
                                break;
                        case 'r':
                                ret = EXIT_FAILURE;
                                break;
                        case 's':
                                ret = EXIT_FAILURE;
                                break;
                        case 't':
                                ret = EXIT_FAILURE;
                                break;
                        case 'u':
                                ret = EXIT_FAILURE;
                                break;
                        case 'v':
                                ret = EXIT_FAILURE;
                                break;
                        case 'w':
                                ret = EXIT_FAILURE;
                                break;
                        case 'x':
                                ret = EXIT_FAILURE;
                                break;
                        case 'y':
                                ret = EXIT_FAILURE;
                                break;
                        case 'z':
                                ret = EXIT_FAILURE;
                                break;
                        default:
                                ret = EXIT_SUCCESS;
                                break;
                }

                if(ret == EXIT_FAILURE)
                {
                        break;
                }
                i += 1;
        }

        return ret;
}
 
