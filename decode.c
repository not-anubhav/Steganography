#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include "color.h"

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char data_buffer[decInfo->size_secret_file + 1];
    if(decode_data_from_image(data_buffer, decInfo->size_secret_file, decInfo->fptr_src_image) == e_success)
    {
        data_buffer[decInfo->size_secret_file] = '\0';
        int write = fwrite(data_buffer, sizeof(char), decInfo->size_secret_file, decInfo->fptr_secret);
        if(write < decInfo->size_secret_file)
        {
            printf(RED "Error Writing Secret File Data\n" RESET);
            return e_failure;
        }
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    uint size = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int);
    char buffer[size];
    uint file_size = 0;
    int read = fread(buffer,sizeof(char), size, decInfo->fptr_src_image);
    if(read < size)
    {
        printf(RED "Error Reading File Size\n" RESET);
        return e_failure;
    }
    decode_size_from_lsb(&file_size, buffer);
    if(file_size <= 0)
    {
        printf(RED "Error: Invalid File Size\n" RESET);
        return e_failure;
    }
    decInfo->size_secret_file = file_size;
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char extn_buffer[decInfo->file_extn_size + 1];
    if(decode_data_from_image(extn_buffer, decInfo->file_extn_size, decInfo->fptr_src_image) == e_success)
    {
        extn_buffer[decInfo->file_extn_size] = '\0';
        strcpy(decInfo->extn_secret_file, extn_buffer);
        if(strlen(decInfo->secret_fname) + strlen(decInfo->extn_secret_file) >= MAX_FILENAME_SIZE)
        {
            printf(RED "Error: File Name too long\n" RESET);
            return e_failure;
        }
        strcat(decInfo->secret_fname, decInfo->extn_secret_file);
        return e_success;
    }
    else
    {
        printf("Error: Decoding Secret File Extension\n");
        return e_failure;
    }
}


Status decode_size_from_lsb(uint *size, char *buffer)
{
    for (int i = 0; i < 32; i++)
    {
        *size = *size | (buffer[i] & (1)) << (31 - i);
    }
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    uint size = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int);
    char buffer[size];
    uint file_extn_size = 0;
    int read = fread(buffer,sizeof(char), size, decInfo->fptr_src_image);
    if(read < size)
    {
        printf(RED "EXTN Decoding Failed\n" RESET);
        return e_failure;
    }
    decode_size_from_lsb(&file_extn_size, buffer);
    if(file_extn_size <= 0)
    {
        printf(RED "Error: Invalid File Extension Size\n" RESET);
        return e_failure;
    }
    decInfo->file_extn_size = file_extn_size;
    return e_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = ch | (image_buffer[i] & (1)) << (7 - i);
    }
    *data = ch;
    return e_success;
}

Status decode_data_from_image(char *data, int size, FILE *fptr_src_image)
{
    char image_buffer[MAX_IMAGE_BUF_SIZE];
    for( int i = 0; i < size; i++)
    {
        int read = fread(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_src_image);
        if( read < MAX_IMAGE_BUF_SIZE)
        {
            printf(RED "Error reading data bytes from source image\n" RESET);
            return e_failure;
        }
        decode_byte_from_lsb(&data[i], image_buffer);
    }
    return e_success;
}


Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_src_image, MAX_HEADER_SIZE, SEEK_SET);
    int size = strlen(MAGIC_STRING);
    char buffer[size + 1];
    
    if(decode_data_from_image(buffer, size, decInfo->fptr_src_image) == e_success)
    {
        buffer[size] = '\0';
        printf("Size of Magic String : %ld\n", strlen(MAGIC_STRING));
        printf("Size of Buffer : %ld\n", strlen(buffer));
        printf("Magic String : %s\n", buffer);
        if (strcmp(buffer, MAGIC_STRING) == 0)
        {
            return e_success;
        }
        else
        {
            printf(RED "Magic String Verification Failed\n" RESET);
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    } 
}


Status open_secret_file(DecodeInfo *decInfo)
{
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if(decInfo->fptr_secret == NULL)
    {
        printf(RED "Error: Unable to open file %s\n" RESET, decInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}

Status open_image_file(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    if(decInfo->fptr_src_image == NULL)
    {
        printf(RED "Error: Unable to open file %s\n" RESET, decInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp((strstr(argv[2], ".")), ".bmp") == 0)
    {
        decInfo->src_image_fname = argv[2];
        if( !(argv[3] == NULL))
        {
            const char* delim = ".";
            if(!(strstr(argv[3], "delim") == NULL))
            {
                decInfo->secret_fname = argv[3];   
            }
            else
            {
                decInfo->secret_fname = strtok(argv[3], delim);
            }
        }
        else
        {
            decInfo->secret_fname = malloc(MAX_FILENAME_SIZE);
            if(decInfo->secret_fname == NULL)
            {
                printf(RED "Error: Memory Allocation Failed\n" RESET);
                return e_failure;
            }
            strcpy(decInfo->secret_fname, "message");
        }
        return e_success;
    }
    else
    {
        printf(RED "Source Image File is not of type .bmp\n" RESET);
        return e_failure;
    }
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_image_file(decInfo) == e_success)
    {
        sleep(1);
        printf(BGREEN"[INFO] opened IMAGE File Successfully\n"RESET);
        if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            sleep(1);
            printf(BBLUE "Magic String Verified Successfully\n" RESET);
            if(decode_secret_file_extn_size(decInfo) ==e_success)
            {
                sleep(1);
                printf(BMAGENTA "Secret File Extension Size Decoded Successfully\n" RESET);
                if(decode_secret_file_extn(decInfo) == e_success)
                {
                    sleep(1);
                    printf(BCYAN "Secret File Extension Decoded Successfully\n" RESET);
                    if(open_secret_file(decInfo) == e_success)
                    {
                        sleep(1);
                        printf(BGREEN"[INFO] opened SECRET File Successfully\n"RESET);
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            sleep(1);
                            printf(BMAGENTA "Secret File Size Decoded Successfully\n" RESET);
                            if(decode_secret_file_data(decInfo) == e_success)
                            {
                                sleep(1);
                                printf(BCYAN "Secret File Data Decoded Successfully\n" RESET);
                                return e_success;
                            }
                            else
                            {
                                return e_failure;
                            }
                        }
                        else
                        {
                            return e_failure;
                        }
                    }
                    else
                    {
                        return e_failure;
                    }
                }
                else
                {
                    return e_failure;
                }
            }
            else
            {
                return e_failure;
            }
            
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }
}