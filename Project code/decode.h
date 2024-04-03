#ifndef DECODE_H
#define DECODE_H

#include "types.h"	//contains user defined types
/* 
 * Structure to store information required for
 * decoding output.bmp to output file
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* output Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    long size_extn;
    long size_secret_file;
    char secret_file_extn[MAX_FILE_SUFFIX];
    char secret_file_data[MAX_SECRET_BUF_SIZE];
    

    /*output file Info*/
    char stego_decode_fname[100];
    FILE *fptr_decode_file;
    

} DecodeInfo;


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decode(DecodeInfo *decInfo);

/* deccoding Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *image_buffer);

/* convert bits to char*/
char converting_bits_to_char(char *lsb_buff);

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode size from LSB of image data array */
Status decode_size_from_lsb(char *image_buffer);

/* convert bits to integer*/
long converting_bits_to_int(char *lsb_size_buff);

/* Decode secret file extenstion */
Status decode_secret_file_extn(long extn_size,DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(long secret_size, FILE *fptr_stego_fname, FILE *fptr_decode_fname);
#endif
