#include <stdio.h>
#include "decode.h"
#include<string.h>
#include "types.h"



Status open_files_dec(DecodeInfo *decInfo)
{
    // opening Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    // opening message file to decode data
    decInfo->fptr_msg_fname = fopen(decInfo->msg_fname, "w");
    // Do Error handling
    if (decInfo->fptr_msg_fname == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->msg_fname);

        return e_failure;
    }
    return e_success;

}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //step1 -> check stego file name having .bmp present or not
            // no -> return e_failure
            // yes -> store source file name into encInfo->stego_image_fname
    int len=strlen(argv[2]);
    if(argv[2][len-1]=='p' && argv[2][len-2]=='m' && argv[2][len-3]=='b' && argv[2][len-4]=='.')
    {
        decInfo->stego_image_fname=argv[2];
    }
    else
    return e_failure; 
    
    //check whether the output file is passed or not.
    //If not passed create a output.txt file
    if(argv[3]!=NULL)
    {
        int len=strlen(argv[3]);
        if(argv[3][len-1]=='t' && argv[3][len-2]=='x' && argv[3][len-3]=='t' && argv[3][len-4]=='.')
        {
            decInfo->msg_fname=argv[3];
        }
        else
        return e_failure;
    }
    else
    {
        decInfo->msg_fname="output.txt";
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    //skip 54 bytes and start decode
    //now decode magic string.
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    char ch[2];
    for(int i=0;i<2;i++)
    {
        char str[8];
        fread(str,1,8,decInfo->fptr_stego_image);
        decode_byte_to_lsb(&ch[i],str);
    }
    decInfo->magic_str=ch;
    return e_success;
}
Status decode_byte_to_lsb(char *data, char *image_buffer)
{
    //Logic to decode bytes from lsb
    int j=7;
    for(int i=0;i<8;i++)
    {
        if(image_buffer[i]&1)
        {
            *data=*data|(1<<j);
        }
        else
        {
            *data=*data&~(1<<j);
        }
        j--;
    }
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    //Logic to decode secrete file extension size
    int n;
        char str[32];
        fread(str,1,32,decInfo->fptr_stego_image);
        decode_size_to_lsb(&n,str);
    
    decInfo->secrf_ext_size=n;
    //printf("%d\n\n",decInfo->secrf_ext_size);
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    //Logic to decode secrete file extension
    char ch[4];
    for(int i=0;i<decInfo->secrf_ext_size;i++)
    {
        char str[8];
        fread(str,1,8,decInfo->fptr_stego_image);
        decode_byte_to_lsb(&ch[i],str);
    }
    decInfo->secrf_ext=ch;
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    //logic to decode secret file size.
    int n;
        char str[32];
        fread(str,1,32,decInfo->fptr_stego_image);
        decode_size_to_lsb(&n,str);
    
    decInfo->secr_file_size=n;
    //printf("%d\n\n",decInfo->secr_file_size);
    return e_success;

}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    //Logic to decode secret file data
    for(int i=0;i<decInfo->secr_file_size;i++)
    {
        char ch;
        char str[8];
        fread(str,1,8,decInfo->fptr_stego_image);
        decode_byte_to_lsb(&ch,str);
        fprintf(decInfo->fptr_msg_fname,"%c",ch);
    }
    return e_success;
}

Status decode_size_to_lsb(int *size, char *imageBuffer)
{
    //Logic for decoding lsb bit
    int i=31;
    for(int j=0;j<32;j++)
    {
        if(imageBuffer[j]&1)
        {
            *size=*size|(1<<i);
        }
        else
        {
            *size=*size&~(1<<i);
        }
        i--;
    }


}
    
Status do_decoding(DecodeInfo *decInfo)
{
    //step1 -> call open file(decInfo)
            // e_failure -> print error msg and return e_failure
            // e_success -> print success msg goto next step
    int ret=open_files_dec(decInfo);
    if(ret==e_success)
    {
        printf("\nFiles opened succesfully\n");
    }
    else
    {
        printf("File opening failed\n");
        return e_failure;
    }

    //call decode_magic_string(MAGIC_STRING,decInfo)
    // print success msg and goto next step
    if(decode_magic_string(decInfo))
    {
        printf("Magic string Decoded\n");
    }
    else
    {
        printf("Magic string decode failed\n");
        return e_failure;
    }

    //call decode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), decInfo)
    // print success msg and goto next step
    if(decode_secret_file_extn_size(decInfo))
    {
        printf("Secret file extension size Decoded\n");
    }
    else
    {
        printf("Secret file extension size Decode failed\n");
        return e_failure;
    }

    //call decode_secret_file_extn(decInfo -> extn_secret_file, decInfo)
    // print success msg and goto next step
    if(decode_secret_file_extn(decInfo))
    {
        printf("Secret file extension Decoded\n");
    }   
    else
    {
        printf("Secret file extension Decode failed\n");
        return e_failure;
    }

    //call encode_secret_file_size(encInfo -> size_secret_file, decInfo)
    // print success msg and goto next step
    if(decode_secret_file_size(decInfo))
    {
        printf("Secret file size Decoded\n");
    }
    else
    {
        printf("Secret file size decode failed\n");
    }

    //decode secret message from thee stego file. 
    if(decode_secret_file_data(decInfo))
    {
        printf("Secret file Data Decoded\n");
    }
    else
    {
        printf("Secret file Data decode Failed\n");
        return e_failure;
    }
    






}