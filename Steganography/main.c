#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "color.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;

    if(argc >= 3)
    {
        if ( check_operation_type(argv) == e_encode)
        {
            sleep(1);
            printf(BRED"[INFO] You have selected encoding process\n"RESET);
            if(argc <= 5)
            {
                if( read_and_validate_encode_args(argv, &encInfo) == e_success)
                {
                    if( do_encoding(&encInfo) == e_success)
                    {
                        sleep(1);
                        printf(BGREEN"[INFO] Encoding Done Successfully\n"RESET);
                        e_printdata(&encInfo);
                    }
                    else
                    {
                        e_failure;
                    }
                }
                else
                {
                    e_failure;
                }
            }
            else
            {
                e_failure;
            }
        }
        if ( check_operation_type(argv) == e_decode)
        {
            sleep(1);
            printf(BRED"[INFO] You have selected decoding process\n"RESET);
            if(argc <= 4)
            {
                if( read_and_validate_decode_args(argv, &decInfo) == e_success)
                {
                    if( do_decoding(&decInfo) == e_success)
                    {
                        sleep(1);
                        printf(BGREEN"[INFO] Decoding Done Successfully\n"RESET);
                        d_printdata(&decInfo);
                    }
                    else
                    {
                        e_failure;
                    }
                }
                else
                {
                    e_failure;
                }
            }
            else
            {
                return e_failure;
            }
        }
    }
    else
    {
        return e_failure;
    }
}
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0) /*Compare and check the argv[1] == -e*/
    {
        return e_encode; /*If true then return e_encode*/
    }
    else if (strcmp(argv[1], "-d") == 0) /*Compare and check the argv[1] == -d*/
    {
        return e_decode; /*If true then return e_decode*/
    }
    else{
        return e_unsupported; /*For any other arguments return e_unsupported*/
    }
    
}