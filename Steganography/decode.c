#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include "color.h"

/* Function Definitions */

/* Decode Secret File Data From Source Image
 * Input: Source Image file ptr
 * Output: Copies Data of From Destination Image Into Secret File
 * Description: Call decode data from image function to decode the Data of Destination Image 
 * into Secret File Character by Character
 * Return Values : e_success and e_failure
 */

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char data_buffer[decInfo->size_secret_file + 1]; // Take 1 extra space to store NULL character
    if(decode_data_from_image(data_buffer, decInfo->size_secret_file, decInfo->fptr_src_image) == e_success)
    {
        data_buffer[decInfo->size_secret_file] = '\0'; // Add NULL character at the last index
        int write = fwrite(data_buffer, sizeof(char), decInfo->size_secret_file, decInfo->fptr_secret); // Write the data into Secret File
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
/* Function Definitions */

/* Decode Secret File Size from Source Image
 * Input: Destination Image file ptr
 * Output: Number of character bytes encoded inside the source image for secret file
 * Description: Read 32 Bytes of Data from Source Image, Decode the Size of Secret File
 * from those 32 Bytes of Data by Calling decode size from lsb Function
 * Return Values : e_success and e_failure
 */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    uint size = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int); // Size is 32 Bytes
    char buffer[size]; // Create an array buffer of size 32 bytes
    uint file_size = 0;
    int read = fread(buffer,sizeof(char), size, decInfo->fptr_src_image); // Read 32 Bytes from source Image
    if(read < size)
    {
        printf(RED "Error Reading File Size\n" RESET);
        return e_failure;
    }
    decode_size_from_lsb(&file_size, buffer); // Decode the size of secret file encoded inside the source image

    if(file_size <= 0)
    {
        printf(RED "Error: Invalid File Size\n" RESET);
        return e_failure;
    }
    decInfo->size_secret_file = file_size;
    return e_success;
}
/* Function Definitions */

/* Decode Secret File Extension from Source Image
 * Input: Secret File Extension Size, Source Image file ptr
 * Output: Copies Extension Encoded Inside Source Image to extn secret file in structure
 * Description: Call decode data to image function to decode the Extension of Secret File from Destination Image
 * Character by Character
 * 
 * Return Values : e_success and e_failure
 */
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

/* Function Definitions */

/* Decode Unsigned Integer Size From Buffer Array Containing 32 bytes of Image Data Bytes 
 * Input: Size to Decode and Buffer Containing 32 Bytes of Image Data
 * Output: Gets the last bit from each byte inside the buffer and convert that
 * 32 bits into size
 * Description: Performs BitWise Operations Like Get, Clear, Set and Left Shift
 * For Getting Last Byte of Data from Buffer Array of Source Image
 * Return Values : e_success
 */
Status decode_size_from_lsb(uint *size, char *buffer)
{
    for (int i = 0; i < 32; i++)
    {
        *size = *size | (buffer[i] & (1)) << (31 - i);
    }
    return e_success;
}
/* Function Definitions */

/* Decode Secret File Extension Size from Source Image
 * Input: Source Image file ptr
 * Output: Decodes Extension Size of Secret File From Source Image and Stores it
 * inside the file_extn_size in Structure
 * Description: Read 32 Bytes of Data from Source Image, Decode the Size of Extension
 * in those 32 Bytes of Data by Calling size from lsb
 * Return Values : e_success and e_failure
 */
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

/* Function Definitions */
/* Decode Character Data Bytes From Buffer Array Containing 8 bytes of Image Data Bytes 
 * Input: Character Data Bytes and Buffer Containing 8 Bytes of Image Data to Decode
 * Output: Changes the Buffer Data Bytes and Stores Character Bytes Decoded 
 * From Source Image  
 * Description: Performs BitWise Operations Like Get, Set and Left Shift
 * For Getting LSB from each bytes of Data from Buffer Array of Source Image
 * Return Values : e_success
 */
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
/* Function Definitions */

/* Decode Data Bytes From Source Image
 * Input: Character Data, Character Data Size, Source Image file ptr
 * Output: Decode Character Bytes of Data From Source Image For 
 * MAGIC STRING, Secret File Extension and Secret File Data
 * Description: Read 8 Bytes of Data from Source Image, Decode the Character
 * Encoded Inside those 8 Bytes
 * Return Values : e_success and e_failure
 */
Status decode_data_from_image(char *data, uint size, FILE *fptr_src_image)
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

/* Function Definitions */

/* Decode Magic string From Source Image
 * Input: Magic string, Source Image file ptr
 * Output: Decodes Magic String From Source Image After 54 Bytes of Header data
 * Description: Call decode data to image function to decode the magic string character 
 * based on the length of magic string and compare the decoded magic string with user
 * magic string. If Both the Magic String Matched then Continue Decoding Process,
 * else Return Failure and Abort Decoding
 * Return Values : e_success and e_failure
 */

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_src_image, MAX_HEADER_SIZE, SEEK_SET);
    uint size = strlen(magic_string);
    char buffer[size + 1];
    
    if(decode_data_from_image(buffer, size, decInfo->fptr_src_image) == e_success)
    {
        buffer[size] = '\0';
        if (strcmp(buffer, magic_string) == 0)
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
/* Function Definitions */
/* 
 * Get File pointers for o/p files
 * Inputs: Secret file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
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
/* Function Definitions */
/* 
 * Get File pointers for i/p
 * Inputs: Src Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
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
/* Function Definitions */
/*
* Validate Command Line Arguments
* Inputs: Command Line arguments
* Output: source image name, Output secret file name, Output secret file extension
* Return Values: e_success or e_failure
*/
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
            strcpy(decInfo->secret_fname, "Secret_Message");
        }
        return e_success;
    }
    else
    {
        printf(RED "Source Image File is not of type .bmp\n" RESET);
        return e_failure;
    }
}
/* Function Definitions */
/* 
 * Perform Decoding
 * Inputs: Call each Functions one by one to perform the decoding task
 * Output: Information on Status of Function Call
 * Return Value: e_success or e_failure, on file errors
 */
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_image_file(decInfo) == e_success)
    {
        sleep(1);
        printf(BGREEN"[INFO] opened IMAGE File Successfully\n"RESET);
        if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            sleep(1);
            printf(BBLUE "[INFO] Magic String Verified Successfully\n" RESET);
            if(decode_secret_file_extn_size(decInfo) ==e_success)
            {
                sleep(1);
                printf(BMAGENTA "[INFO] Secret File Extension Size Decoded Successfully\n" RESET);
                if(decode_secret_file_extn(decInfo) == e_success)
                {
                    sleep(1);
                    printf(BCYAN "[INFO] Secret File Extension Decoded Successfully\n" RESET);
                    if(open_secret_file(decInfo) == e_success)
                    {
                        sleep(1);
                        printf(BGREEN"[INFO] opened SECRET File Successfully\n"RESET);
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            sleep(1);
                            printf(BMAGENTA "[INFO] Secret File Size Decoded Successfully\n" RESET);
                            if(decode_secret_file_data(decInfo) == e_success)
                            {
                                sleep(1);
                                printf(BCYAN "[INFO] Secret File Data Decoded Successfully\n" RESET);
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