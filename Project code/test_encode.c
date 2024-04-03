#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    
    if(argv[1] != NULL)
    {
    int ret = check_operation_type(argv[1]);
	if ( ret == e_encode )
	{
	    //sleep(1);
	    printf("INFO : Selected Encoding\n");
	    sleep(1);
	    if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	    {
		//sleep(1);
		printf("INFO : Validation completed\n");
		sleep(1);
		
		if(do_encoding(&encInfo) == e_success)
		{
		    for(int i=0; i<=100; i++)
		    {
			if(i < 100)
			printf("\r\033[0;31mEncoding[%d%%]",i);
			else
			printf("\r\033[0;32mEncoding[%d%%]",i);
			usleep(5000);
			fflush(stdout);
		    }
		    printf("\n");
		    printf("\033[0m"); // Reset color and print percentage
		    printf("Encoding is done\n");
		}
	    }
	    else
	    {
		printf("Error: validation failed\n");
	    }
	}
	else if(ret == e_decode)
	{
	    printf("INFO : Selected decoring\n");
	    sleep(1);
	    if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	    {
		//sleep(1);
		printf("INFO : Validation completed\n");
		sleep(1);
		
		if(do_decoding(&decInfo) == e_success)
		{
		    for(int i=0; i<=100; i++)
		    {
			if(i < 100)
			printf("\r\033[0;31mDecoding[%d%%]",i);
			else
			printf("\r\033[0;32mDecoding[%d%%]",i);
			usleep(5000);
			fflush(stdout);
		    }
		    printf("\n");
		    printf("\033[0m"); // Reset color and print percentage
		    printf("Decoding is done\n");
		}
		else
		{
		    printf("Error: decoding validation failed\n");
		}
	    }
	    
	}
	else
	{
	    printf("Error: pass valid option\n");
	}
    }
    else
    {
	printf("Please give the option -e or -d\n");
    }

    return 0;
}
OperationType check_operation_type(char *argv)
{
    if(strcmp(argv,"-e") == 0)
    {
	return e_encode;
    }
    else if(strcmp(argv,"-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }


}
