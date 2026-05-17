// Name     - Y.Rajesh 
// Batch id - 25036A

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include<string.h>
#include "types.h"

OperationType check_operation_type(char *);         //check operation function declaration.

int main(int argc, char *argv[])
{
    int a=check_operation_type(argv[1]);            //To check Operation Type ie Encode or Decode.
    if(a==e_encode)
    {
        if(argc>3)                                  //To check Enough arguments passed to encode.
        {
        EncodeInfo encInfo;
        int ret=read_and_validate_encode_args(argv,&encInfo);   //To read and validate the arguments.
        if(ret==e_failure)
        {
            printf("Error\n");
            return e_failure;
        }
        if(do_encoding(&encInfo))                   //Enters to do_encoding function to Encode the data.
        {
            printf("Encoding Done\n\n");            //If encoding done successfully,prints encoding done message.
        }
        else
        {
            printf("Encoding Failed\n");            //if encoding failed,prints encoding done message.
            return 1;
        }
        }
        else
        {
            printf("Enter valid arguments\n");
        return e_failure;
        }
    }
    else if(a==e_decode)                            //if user enters decoding option,it comes here.
    {
        if(argc>2)
        {
        DecodeInfo decInfo;
        int ret=read_and_validate_decode_args(argv,&decInfo);       //Read and validate the arguments for decoding process.
        if(ret==e_failure)
        {
            printf("Error\n");
            return e_failure;
        }
        if(do_decoding(&decInfo))                       //Enters decoding function and performs decoding
        {
            printf("Decoding done\n\n");                //If decoding is success,Prints decoding done message.
        }
        else
        {
            printf("Decoding Failed\n");                //If decoding failed,prints decoding failed message.
        }
        }
        else
        {
            printf("Enter valid arguments\n");          //If arguments not valid,prints this message.
        return e_failure;
        }
    }
    else
    {
        printf("pass valid mode\n");                    //if ivalid mode entered,program will terminate.
        return e_failure;
    } 
}

OperationType check_operation_type(char *symbol)        //Function to check the operation type ie its encode or decode.
{
    if(!(strcmp(symbol,"-e")))                          //comparing argv[1] equals to -e ie for encoding.
    {
        return e_encode;
    }
    else if(!(strcmp(symbol,"-d")))                     //comparing argv[1] equals to -d ie for decoding.
    {
        return e_decode;
    }
    else
    return e_unsupported;                               //if argv[1] not equals to -e and -d, returns e_unsupported.
}

//step1 -> check_operation_type(argv[1])

    //step2 -> check the return value == e_encode
            // declare structure variable EncodeInfo encInfo
            // read_and_validate_encode_args(pass command line arg, &encInfo) == e_success or e_failure
            // e_failure -> print error msg and stop the program
            // e_success -> next step.
        // call do_encoding(&encInfo);
            //e_failure -> print error msg and stop the program
            //e_success -> print success msg and stop the program 

    //step3 -> return value == e_decode
            // --
    //step3 -> return value == e_unsupported
            // --> print invalid arg
            // -e -> encode
            // -d  -> decode
