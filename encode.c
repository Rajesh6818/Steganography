#include <stdio.h>
#include "encode.h"
#include<string.h>
#include "types.h"
#define MAGIC_STRING "#*"
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
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr,0,SEEK_END);
      uint a=ftell(fptr);
      rewind(fptr);
      return a;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    int len=strlen(argv[2]);
    if(argv[2][len-1]=='p' && argv[2][len-2]=='m' && argv[2][len-3]=='b' && argv[2][len-4]=='.')
    {
        encInfo->src_image_fname=argv[2];
    }
    else
    return e_failure; 
    //step1 -> check source file name having .bmp present or not
            // no -> return e_failure
            // yes -> store source file name into encInfo->src_image_fname


    int len1=strlen(argv[3]);
    if(argv[3][len1-1]=='t' && argv[3][len1-2]=='x' && argv[3][len1-3]=='t' && argv[3][len1-4]=='.')
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,".txt");
    }
    else
    return e_failure;
    //step2 -> check secret file having extn or not
            // no -> return e_failure
            // yes -> store secret file name into encInfo->src_image_fname

    if(argv[4]!=NULL)
    {
        // step3 -> check optional file is passed or not
        int len=strlen(argv[4]);
        if(argv[4][len-1]=='p' && argv[4][len-2]=='m' && argv[4][len-3]=='b' && argv[4][len-4]=='.')
        {
            encInfo->stego_image_fname=argv[4];
        }
        else
        return e_failure;
    }
    else
    {
        encInfo->stego_image_fname="stego.bmp";
    }
    return e_success;
            // yes -> check the file having .bmp or not
                    // no -> return e_failure
                    //yes -> store the file name into encInfo->stego_image_fname
            // no -> store default name to encInfo->stego_image_fname = "stego.bmp";
    //step4 -> return e_success
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if(encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
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

Status check_capacity(EncodeInfo *encInfo)
{
    // step1 -> encInfo->image_capacity =get_image_size_for_bmp(source_file_pointer)

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    
    // step2 -> find secret file size encInfo -> size_secret_file = get_file_size(secret file pointer)
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // step3 -> compare encInfo->image_capacity > 16 + 32 + 32 + 32 + 54 + (encInfo -> size_secret_file * 8)
            //yes -> return e_success
            // no -> return e_failure
    if(encInfo->image_capacity>=(16+32+32 +32+54+(encInfo->size_secret_file *8)))
    {
        return e_success;
    }
    else
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{

    char str[54];
    //step1 -> rewind file pointer 0th pos
    rewind(fptr_src_image);
    //step2 -> read 54 bytes from source file(use fread)
    fread(str,1,54,fptr_src_image);
    //step3 -> write the 54 bytes to stego image file(from buffer)
    fwrite(str,1,54,fptr_stego_image);
    return e_success;
    // return e_success

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //step1 ->  read 8 bytes of buffer from source image
    //step2 -> call encode_bytes_to_lsb(magic_string[0], buffer)
    for(int i=0;i<2;i++)
    {
        char str[8];
        fread(str,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], str);
        fwrite(str,1,8,encInfo->fptr_stego_image);
    }
    return e_success;
    //step3 -> store the buffer to stego image file
            // do this upto magic string size time (step1)

    // return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    //step1 -> read 32 bytes of buffer from source image
    char str[32];
    fread(str,1,32,encInfo->fptr_src_image);
    encode_size_to_lsb(size,str);
    fwrite(str,1,32,encInfo->fptr_stego_image);
    return e_success;

    //step2 -> call encode_size_to_lsb(size, buffer)

    //step3 -> store the buffer to stego image file

    // return e_success.
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    //step1 -> read 8 bytes of buffer from source image file

    //step2 -> call encode_byte_to_lsb(file_extn[0], buffer)

    //step3 -> store the buffer into stego image file
            // repeat this upto size of extn size
    int len=strlen(file_extn);
    for(int i=0;i<len;i++)
    {
        char str[8];
        fread(str,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],str);
        fwrite(str,1,8,encInfo->fptr_stego_image);
    }

    return e_success;

    // return e_success
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{   
    //step1 -> read 32 bytes of buffer from source image
    int c=file_size;
    //rewind(encInfo->fptr_secret);

    
    char str[32];  
    fread(str,1,32,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,str);
    fwrite(str,1,32,encInfo->fptr_stego_image);
    
    return e_success;
    

    //step2 -> call encode_size_to_lsb(file_size, buffer)

    //step3 -> store the buffer to stego image file

    // return e_success.
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //step1 -> read the secret data into one data_buffer
    rewind(encInfo->fptr_secret);
    int i=0;
    char ch;
    while(i<encInfo->size_secret_file)
    {
        ch=fgetc(encInfo->fptr_secret);
        char str[8];
        fread(str,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,str);
        fwrite(str,1,8,encInfo->fptr_stego_image);
        i++;
    }
    return e_success;


    //step2 -> read 8 bytes of buffer from source image file

    //step3 -> call encode_byte_to_lsb(data_buffer[0], buffer)

    //step4 -> store the buffer into stego image file
            // repeat this upto size of secret file(step2)

    // return e_success.
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    /*char ch;
    while((ch=getc(fptr_src))!=EOF)
    {
        fputc(ch,fptr_dest);
    }
    */
    char str[8];
    while(!feof(fptr_src))
    {
        fread(str,8,1,fptr_src);
        fwrite(str,8,1,fptr_dest);
    }
    return e_success;

    // run a loop upto reaching EOF
    // read buffer from source image file
    // store into stego image file.
    

    // return e_success.
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
   // logic to encode the data.
   int j=0;
   for(int i=7;i>=0;i--)
   {
        if(((unsigned)data>>i)&1)
        {
            image_buffer[j]=image_buffer[j]|1;
            j++;
        }
        else
        {
            image_buffer[j]=image_buffer[j]&~1;
            j++;
        }
   }
   return e_success;

}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    //logic to encode the size.
    int j=0;
    for(int i=31;i>=0;i--)
    {
        if((size>>i)&1)
        imageBuffer[j]=imageBuffer[j]|1;
        else
        imageBuffer[j]=imageBuffer[j]&~1;
        j++;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    //step1 -> call open file(encInfo)
            // e_failure -> print error msg and return e_failure
            // e_success -> print success msg goto next step
    int ret=open_files(encInfo);
    if(ret)
    {
        printf("\nSuccess,All files opened\n");
    }
    else
    {
        printf("File open Failed\n ");
        return e_failure;
    }

    // step2 -> call check_capacity(encInfo)
            // e_failure -> print error msg and return e_failure
            // e_success -> print success msg goto next step
    if(!check_capacity(encInfo))
    {
        printf("check capacity failed\n");
        return e_failure;
    }
    else
    {
        printf("check capacity Success\n");
    }
    
    // step3 -> call copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
            // print success msg and goto next step
    ret=copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if(ret)
    {
        printf("Header copied Successfully\n");
    }

    // step4 -> call encode_magic_string(MAGIC_STRING,encInfo)
            // print success msg and goto next step
    ret=encode_magic_string(MAGIC_STRING,encInfo);
    if(ret)
    {
        printf("Encode magic string Succesfully\n");
    }
    

    ret=encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo);
    if(ret)
    {
        printf("Secrete file extension size Encoded\n");
    }
    // step5 -> call encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)
            // print success msg and goto next step
    
    ret=encode_secret_file_extn(encInfo -> extn_secret_file, encInfo);
    if(ret)
    {
        printf("Secrete file extension Encoded\n");
    }
    // step6 -> call encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)
            // print success msg and goto next step

    ret=encode_secret_file_size(encInfo -> size_secret_file, encInfo);
    if(ret)
    {
        printf("secret file size Encodded\n");
    }
    //step7 -> call encode_secret_file_size(encInfo -> size_secret_file, encInfo)
            // print success msg and goto next step
    
    ret=encode_secret_file_data(encInfo);
    if(ret)
    {
        printf("secret file data Encoded\n");
    }
    //step8 -> call encode_secret_file_data(encInfo)
            // print success msg and goto next step

    ret=copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if(ret)
    {
        printf("Remaining data copied Successfully\n");
    }
    return e_success;
    //step9 -> call copy_remaining_img_data(pass two file pointers)
            // print success msg and goto next step

    //step10 -> return e_seccess.
}
