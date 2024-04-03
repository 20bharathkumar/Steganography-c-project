
#include <stdio.h>
#include <unistd.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>


Status open_files_for_decode(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

	return e_failure;
    }

    // output file
    //decInfo->fptr_decode_file = fopen(decInfo->stego_decode_fname,"w");
    return e_success;
}

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] != NULL)
    {
	if(strchr(argv[2],'.') != NULL)
	{
	    if(strcmp(strchr(argv[2],'.'), ".bmp") == 0)
	    {
		decInfo->stego_image_fname = argv[2];
	    }
	    else
	    {
		printf("Error invalid extension\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Error invalid file type\n");
	    return e_failure;
	}
    }
    else
    {
	printf("Error: argv[2] not passed");
	return e_failure;
    }

    if(argv[3] != NULL)
    {
	//decInfo->stego_decode_fname = 
	strcpy(decInfo->stego_decode_fname,strtok(argv[3],"."));
    }
    else
    {
	//printf("default name will be stored\n");
	//decInfo->stego_decode_fname = 
	strcpy(decInfo->stego_decode_fname,"output");

    }
    return e_success;
}


/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_files_for_decode(decInfo) == e_success)
    {
	//printf("Successfully opening file is done\n");
	if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	{
	    sleep(1);
	    printf("INFO : Decoding magic string is successfull\n");
	    if(decode_secret_file_extn_size(decInfo) == e_success)
	    {
		//printf("decodeing secret file extension is done\n");
		if(decode_secret_file_extn(decInfo->size_extn,decInfo) == e_success)
		{
		    sleep(1);
		    printf("INFO : Decoding secret file extension and concating is done\n");
		    if(decode_secret_file_size(decInfo) == e_success)
		    {
			//printf("decoding secret file size is done\n");
			if(decode_data_from_image(decInfo->size_secret_file, decInfo->fptr_stego_image, decInfo->fptr_decode_file) == e_success)
			{
			    sleep(1);
			    printf("INFO : Decoding secret file data is completed\n");
			    return e_success;
			}
			else
			{
			    printf("Error: decoding secret file data failed\n");
			}
		    }
		    else
		    {
			printf("Error: decoding secret file size failed\n");
		    }
		}
		else
		{
		    printf("Error: decoding secret file extension failed\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("Error: decodeing secret file extension is failed\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Error:decoding magic string is unsuccessfull\n");
	    return e_failure;
	}
    }
    else
    {
	printf("Error:Opening file failed\n");
	return e_failure;
    }
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    char buffer[8];
    char ch[strlen(magic_string)];
    for(int i=0; i<strlen(magic_string); i++)
    {
	fread(buffer,8,1,decInfo->fptr_stego_image);
	decode_byte_from_lsb(buffer);
	ch[i] = converting_bits_to_char(buffer);
    }
    ch[strlen(magic_string)] = '\0';

    if(strcmp(magic_string,ch) == 0)
    {
	//printf("magic string and decoded strings are equal\n");
	    return e_success;
    }
    else
    {
	printf("Error: Magic string and decoded strings are not equal\n");
	return e_failure;
    }

}
/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *image_buffer)
{
    ("performing byte decoding\n");
    for(int i=0; i<8; i++)
    {
	image_buffer[i] = (image_buffer[i] & 0x01);
    }
    return e_success;
}

/* converting bits to character */
char converting_bits_to_char(char *lsb_buff)
{
    char ch = 0;
    for(int i=0; i<8; i++)
    {
	ch = (lsb_buff[i]<<(7-i))|ch;
    }
    return ch;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
   
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer);
   
    decInfo->size_extn = converting_bits_to_int(buffer);

    return e_success;
}

/* Decode size from LSB of image data array */
Status decode_size_from_lsb(char *image_buffer)
{
    //("performing size decoding\n");
    for(int i=0; i<32; i++)
    {
	image_buffer[i] = (image_buffer[i] & 0x01);
    }
    return e_success;
}

/* converting bits to integer */
long converting_bits_to_int(char *lsb_size_buff)
{
    long li = 0;
    for(int i=0; i<32; i++)
    {
	li = (lsb_size_buff[i]<<(31-i))|li;
    }
    return li;
}

/* Decode secret file extenstion */
Status decode_secret_file_extn(long extn_size,DecodeInfo *decInfo)
{
    char buffer[8];
    char ch[extn_size];
    
    for(int i=0; i<extn_size; i++)
    {
	fread(buffer,8,1,decInfo->fptr_stego_image);
	decode_byte_from_lsb(buffer);
	decInfo->secret_file_extn[i] = converting_bits_to_char(buffer);
    }
    
    strcpy(decInfo->stego_decode_fname,strcat(decInfo->stego_decode_fname,decInfo->secret_file_extn));
    decInfo->fptr_decode_file = fopen(decInfo->stego_decode_fname,"w");
    
    return e_success;
}

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer);
   
    decInfo->size_secret_file = converting_bits_to_int(buffer);

    return e_success;

}

/* Decode function, which does the real decoding */
Status decode_data_from_image(long secret_size, FILE *fptr_stego_fname, FILE *fptr_decode_fname)
{
    char buffer[8];
    char ch = 0;
    for(int i=0; i<secret_size; i++)
    {
	fread(buffer,8,1,fptr_stego_fname);
	decode_byte_from_lsb(buffer);
	ch = converting_bits_to_char(buffer);
	fwrite(&ch,1,1,fptr_decode_fname);
    }
    return e_success;
}

