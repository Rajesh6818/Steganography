#define ENCODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding source Image to txt file
 */

typedef struct _decodeInfo
{  
    /* Stego Image Info */
    char *stego_image_fname; 
    FILE *fptr_stego_image;  

    char *msg_fname;
    FILE *fptr_msg_fname;

    char *magic_str;
    int secrf_ext_size;
    char *secrf_ext;
    int secr_file_size;

} DecodeInfo;

/* decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
//Status open_files(DecodeInfo *decInfo);

/* check capacity */
//Status check_capacity(DecodeInfo *decInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/*decode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char *data, char *image_buffer);

// decode a size to lsb
Status decode_size_to_lsb(int *size, char *imageBuffer);