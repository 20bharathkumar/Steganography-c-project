#include <stdio.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO : Width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO : Height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    //printf("Entered validation\n");
    if(argv[2] != NULL)
    {
    if(strchr(argv[2],'.') != NULL)
    {
	if(strcmp(strchr(argv[2],'.'),".bmp") == 0)
	{
	    encInfo -> src_image_fname = argv[2];
	    //printf("done storing src_image_fname\n");
	}
	else
	{
	    printf("Error: not valid file\n");
	    return e_failure;
	}

    }
    else
    {
	printf("Error: no extension\n");
	return e_failure;
    }
    }
    else
    {
	printf("Error: give the source image file name\n");
	return e_failure;
    }

    if(argv[3] != NULL)
    {
	if(strchr(argv[3],'.') != NULL)
	{

	   // printf("Entered argv[3]\n");
	    encInfo -> secret_fname = argv[3];
	    char *suffix_start = strchr(argv[3],'.');
	    for(int i=0; i<MAX_FILE_SUFFIX; i++)
	    {
		encInfo -> extn_secret_file[i] = suffix_start[i];
	    }

	}
	else
	{
	    printf("Error: no secret file extension\n");
	    return e_failure;
	}
    }
    else
    {
	printf("Error: srcete_image_fname not given\n");
	return e_failure;
    }

    if(argv[4] != NULL)
    {

	if(strchr(argv[4],'.') != NULL)
	{
	    if(strcmp(strchr(argv[4],'.'),".bmp") == 0)
	    {
		encInfo -> stego_image_fname = argv[4];
	    }
	    else
	    {
		printf("Error: not valid stegofile\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Error: no extension\n");
	    return e_failure;
	}

    }
    else
    {
	//printf("File name is default\n");
	encInfo -> stego_image_fname = "lol.bmp";
    }

    return e_success;
}
/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
	//printf("File opened successfully\n");
	if(check_capacity(encInfo) == e_success)
	{
	    //printf("checking capacity completed\n");
	    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
	    {
		//printf("copying is completed\n");
		if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		{
		    sleep(1);
		    printf("INFO : Encoding magic string done\n");
		    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
		    {
			//printf("Encoding file extn size successfully\n");
			if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
			{
			    sleep(1);
			    printf("INFO : Encoding secret file extension is done\n");
			    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
			    {
				//printf("encoding secret file size is done\n");
				if(encode_secret_file_data(encInfo) == e_success)
				{
				    sleep(1);
				    printf("INFO : Encoding secret file data is done\n");
				    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
				    {
					sleep(1);
					printf("INFO : Remaining image data copied\n");
					sleep(1);
					return e_success;
				    }
				    else
				    {
					printf("Error: remaining image data not copied\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("Error: encoding secret file data failed\n");
				    return e_failure;
				}
			    }
			    else
			    {
				printf("Error: encoding secret file size failed\n");
				return e_failure;
			    }

			}
			else
			{
			    printf("Error: encoding secret file extension failed");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("Error: encoding seccret file size failed\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("Error: encoding magic string failed\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("Error: Copying bmp header failed\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Error: checking capacity failed\n");
	    return e_failure;
	}

    }
    else
    {
	printf("Error: file opening failed\n");
	return e_failure;
    }
    //return e_success;

}
/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo->fptr_secret);
    //printf("file_size: %d\n",get_file_size(encInfo->fptr_secret));
    if(encInfo->image_capacity > (strlen(MAGIC_STRING)+strlen(encInfo -> extn_secret_file)+get_file_size(encInfo->fptr_secret))*8+32+32)
    {
	//printf("bmp file can fit\n");
	return e_success;
    }
    else
    {
	return e_failure;
    }

}

/* Get image size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}
/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char buff[54];
    fread(buff,54,1,fptr_src_image);
    fwrite(buff,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0; i<strlen(magic_string); i++)
    {
	fread(buffer,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(magic_string[i],buffer);
	fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    ("performing byte encoding\n");
    for(int i=0; i<8; i++)
    {
	image_buffer[i] = (image_buffer[i] & 0xfe) |( (data >> (7-i)) & 0x01);
    }
    return e_success;
}

/* Encode secret file extenstion size */
Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/* Encode a size into LSB of image data array */
Status encode_size_to_lsb(long size, char *image_buffer)
{
    //printf("Performing encode size to lsb\n");
    for(int i=0; i<32; i++)
    {
	image_buffer[i] = (image_buffer[i] & 0xfe) |( (size >> (31-i)) & 0x01);
    }
    return e_success;
}
/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char  buffer[8];
    for(int i=0; i<strlen(file_extn); i++)
    {
	fread(buffer,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(file_extn[i],buffer);
	fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file], temp_buff[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);

    for(int i=0; i<encInfo->size_secret_file;i++)
    {
	fread(temp_buff,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(buffer[i],temp_buff);
	fwrite(temp_buff,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
   while ( fread(&ch,1,1,fptr_src) )
   {
	fwrite(&ch,1,1,fptr_dest);
    
   }

    return e_success;
}
