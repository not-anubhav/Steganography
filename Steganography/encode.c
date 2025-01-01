#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "color.h"

/* Function Definitions */

/* Copy the remaining data bytes in Destination Image
 * Input: Source and Destination Image file ptr
 * Output: Copies Remaining data bytes till End of File Into Destination Image
 * Description: After copying the secret file data, Encode the remaining data bytes present
 * in Source Image Till it reaches End of File into Destination Image
 * Return Values : e_success
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    sleep(1);
    printf(YEL "INFO: Copying Left Over Data\n" RESET);
    char ch;
    while(fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}
/* Function Definitions */

/* Encode Secret File Data in Destination Image
 * Input: Secret File Data, Secret File Data Size, Source and Destination Image file ptr
 * Output: Copies Data of Secret File Into Destination Image
 * Description: Call encode data to image function to Encode the Data of Secret File Character by Character
 * into Destination Image
 * Return Values : e_success and e_failure
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    sleep(1);
    printf(YEL "INFO: Encoding %s File Data\n" RESET, encInfo->secret_fname);
    rewind(encInfo->fptr_secret);
    char image_data[encInfo->size_secret_file + 1]; // Take 1 extra space to store NULL character
    fread(image_data, sizeof(char), encInfo->size_secret_file, encInfo->fptr_secret);
    image_data[encInfo->size_secret_file] = '\0'; // Add NULL character at the last index
    uint len = strlen(image_data);
    /*Remove Newline Character From the End*/
    if (len > 0 && image_data[len - 1] == '\n')
    {
        image_data[len - 1] = '\0';
        len--;
    }
    if(encode_data_to_image(image_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        printf(RED "Error Encoding Secret File Data\n" RESET);
        return e_failure;
    }
    
}
/* Function Definitions */

/* Encode Secret File Size in Destination Image
 * Input: Secret File Size, Source and Destination Image file ptr
 * Output: Copies Size of Secret File Into Destination Image
 * Description: Read 32 Bytes of Data from Source Image, Encode the Size of Secret File
 * in those 32 Bytes of Data by Calling encode size to lsb Function
 * Return Values : e_success and e_failure
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    sleep(1);
    printf(YEL "INFO: Encoding %s File Size\n" RESET, encInfo->secret_fname);
    uint size = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int); // Size is 32 Bytes
    char buffer[size]; // Create an array buffer of size 32 bytes
    int read = fread(buffer, sizeof(char), size, encInfo->fptr_src_image); // Read 32 Bytes from source Image
    if(read < size)
    {
        printf(RED "Error Reading File Size\n" RESET);
        return e_failure;
    }
    encode_size_to_lsb(file_size, buffer); // Encode the lsb of read bytes with secret file size
    int write = fwrite(buffer, sizeof(char), size, encInfo->fptr_stego_image); // Write 32 bytes inside Destination image
    if(write < size)
    {
        printf(RED "Error Writing File Size\n" RESET);
        return e_failure;
    }
    return e_success;
}
/* Function Definitions */

/* Encode Secret File Extension in Destination Image
 * Input: Secret File Extension, Secret File Extension Size, Source and Destination Image file ptr
 * Output: Copies Extension of Secret File Into Destination Image
 * Description: Call encode data to image function to Encode the Extension of Secret File Character by Character
 * into Destination Image
 * Return Values : e_success and e_failure
 */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    sleep(1);
    printf(YEL "INFO: Encoding %s File Extenstion\n" RESET, encInfo->secret_fname);
    int extn_size = strlen(file_extn); // Size of Secret File Extension
    if(encode_data_to_image(file_extn, extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) // Encode the Extension of Secret File in Destination Image
    {
        return e_success;
    }
    else
    {
        printf(RED "Error Encoding Secret File Extension\n" RESET);
        return e_failure;
    }
}
/* Function Definitions */

/* Encode Unsigned Integer Size Into Buffer Array Containing 32 bytes of Image Data Bytes 
 * Input: Size to Encode and Buffer Containing 32 Bytes of Image Data
 * Output: Changes the Buffer Data Bytes From LSB Side of Source Image
 * with Size From MSB Side to Encode In Destination Image  
 * Description: Performs BitWise Operations Like Get, Clear, Set, Left Shift and Right Shift
 * For Getting Last Byte of Data from Buffer Array of Source Image the Clearing that Byte
 * and Setting it with MSB of Size to be Encoded for 32 times
 * Return Values : e_success
 */
Status encode_size_to_lsb(uint size, char *buffer)
{
    for( int i = 0; i < 32; i++)
    {
        buffer[i] = ((buffer[i] & (~1)) | ((size & (1 << (31 - i))) >> (31 - i)));
    }
    return e_success;
}
/* Function Definitions */

/* Encode Secret File Extension Size in Destination Image
 * Input: Secret File Extension Size, Source and Destination Image file ptr
 * Output: Copies Extension Size of Secret File Into Destination Image
 * Description: Read 32 Bytes of Data from Source Image, Encode the Size of Extension
 * in those 32 Bytes of Data by Calling size to lsb and using fwrite function
 * Return Values : e_success and e_failure
 */
Status encode_secret_file_extn_size(long file_extn_size, EncodeInfo *encInfo)
{
    sleep(1);
    printf(YEL "INFO: Encoding %s File Extenstion Size\n" RESET, encInfo->secret_fname);
    uint size = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int); // Size is 32 Bytes
    char buffer[size]; // Create an array buffer of size 32 bytes
    int read = fread(buffer,sizeof(char), size, encInfo->fptr_src_image); // Read 32 Bytes from source Image
    if(read < size)
    {
        printf(RED "EXTN Encoding Failed\n" RESET);
        return e_failure;
    }
    if( encode_size_to_lsb(file_extn_size, buffer) == e_success) // Encode the lsb of read bytes with file extn size
    {
        int write = fwrite(buffer, sizeof(char), size, encInfo->fptr_stego_image); // Write 32 bytes inside Destination image
        if(write < size)
        {
            printf(RED "EXTN Encoding Failed\n" RESET);
            return e_failure;
        }
        return e_success;
    }

}
/* Function Definitions */

/* Encode Character Data Bytes Into Buffer Array Containing 8 bytes of Image Data Bytes 
 * Input: Character Data Bytes to Encode and Buffer Containing 8 Bytes of Image Data
 * Output: Changes the Buffer Data Bytes From LSB of Source Image
 * with Character Data Bytes From MSB to Encode In Destination Image  
 * Description: Performs BitWise Operations Like Get, Clear, Set, Left Shift and Right Shift
 * For Getting Last Byte of Data from Buffer Array of Source Image the Clearing that Byte
 * and Setting it with MSB of Data to be Encoded
 * Return Values : e_success
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for( int i = 0; i < 8; i++)
    {
        image_buffer[i] = ((image_buffer[i] & (~1)) | ((data & (1 << (7 - i))) >> (7 - i)));
    }
    return e_success;
}
/* Function Definitions */

/* Encode Data Bytes Into Destination Image
 * Input: Character Data, Character Data Size, Source and Destination Image file ptr
 * Output: Copies Character Bytes of Data into Destination Image For 
 * MAGIC STRING, Secret File Extension and Secret File Data
 * Description: Read 8 Bytes of Data from Source Image, Encode the Character Data into 8 Bytes 
 * and Write the Encoded 8 Bytes Inside Destination Image For Size times 
 * Return Values : e_success and e_failure
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[MAX_IMAGE_BUF_SIZE]; // Declare a character array of Size 8 bytes
    
    for( int i = 0; i < size; i++) 
    {
        int read = fread(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_src_image); // Read 8 Bytes from source Image
        if(read < MAX_IMAGE_BUF_SIZE)
        {
            printf(RED "Error reading data bytes from source image\n" RESET);
            return e_failure;
        }
        encode_byte_to_lsb(data[i], image_buffer); // Convert the read bytes lsb with Character bytes
        int write = fwrite(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_stego_image); // Encode the Converted Bytes Inside the Destination Image

        if(write < 8)
        {
            printf(RED "Error writing data bytes to stego image\n" RESET);
            return e_failure;
        }
    }
    return e_success;
}
/* Function Definitions */

/* Encode Magic string into destination Image
 * Input: Magic string, Magic String Size, Source and Destination Image file ptr
 * Output: Copies Magic String into Destination Image After 54 Bytes of Header data
 * Description: Call encode data to image function to encode the magic string character 
 * based on the length of magic string into destination image
 * Return Values : e_success and e_failure
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    sleep(1);
    printf(YEL "INFO: Encoding Magic String Signature\n" RESET);
    int size = strlen(MAGIC_STRING);
    if(encode_data_to_image(MAGIC_STRING, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        printf(RED "Error Encoding MAGIC STRING\n" RESET);
        return e_failure;
    }
}
/* Function Definitions */

/* Copy BMP header from source bmp image to destination stego image
 * Input: Source and Destination Image file ptr
 * Output: Copies 54 Bytes of Header Data From src to dest image
 * Description: Read 54 bytes of data from source bmp image and write the data to destination stego image
 * Return Values : e_success and e_failure
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    sleep(1);
    printf(YEL "INFO: Copying Image Header\n" RESET);
    rewind(fptr_src_image); // Rewind the File Position Indicator to 0th Position
    char buffer[MAX_HEADER_SIZE];
    int read = fread(buffer, sizeof(char), MAX_HEADER_SIZE, fptr_src_image); // Read 54 Bytes from the Source Image
    if(read < MAX_HEADER_SIZE)
    {
        printf(RED "Error Reading bmp header\n" RESET);
        return e_failure;
    }
    int write = fwrite(buffer, sizeof(char), MAX_HEADER_SIZE, fptr_dest_image); // Write read Bytes in the Destination Image
    if(write < MAX_HEADER_SIZE)
    {
        printf(RED "Error Writing bmp header\n" RESET);
        return e_failure;
    }
    return e_success;
}
/* Function Definitions */

/* Check image capacity
 * Input: EncodeInfo Structure members image capacity, secret file size, secret file extension size and MACRO MAGIC STRING
 * Output: image capacity of source image, size of secret file, magic string length and extension size of secret file
 * Description: Call Functions to get image capacity, file size and store the values inside the EncodeInfo Data Members
 * Return Values : e_success and e_failure
 */
Status check_capacity(EncodeInfo* encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image, encInfo); // Get Source Image Size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // Get Secret File Size
    sleep(1);
    printf(YEL "INFO: Checking for %s capacity to handle %s\n" RESET, encInfo->src_image_fname, encInfo->secret_fname);
    int MAGIC_STRING_SIZE = strlen(MAGIC_STRING); // Size of Magic String
    int extn_secret_file_size = strlen(encInfo->extn_secret_file); // Size of Secret file extension
    uint file_size = 4; // Maximum File size
    uint capacity = (MAX_HEADER_SIZE) + ((MAGIC_STRING_SIZE + file_size + extn_secret_file_size 
    + encInfo->size_secret_file + MAX_FILE_SUFFIX) * MAX_IMAGE_BUF_SIZE);
    if(encInfo->image_capacity > capacity) // Check if Image capacity is greater than the calculated Capacity
    {
        sleep(1);
        printf(GRN "INFO: Done. Found OK\n" RESET);
        return e_success;
    }
    else
    {
        sleep(1);
        printf(RED "ERROR: %s doesn't have the capacity to encode %s\n" RESET, encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }
}
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_file_size(FILE* fptr_secret)
{
    sleep(1);
    printf(YEL "INFO: Checking for secret.txt size\n" RESET);
    fseek(fptr_secret, 0, SEEK_END);
    uint size = ftell(fptr_secret);
    if (size > 0)
    {
        sleep(1);
        printf(GRN "INFO: Done. Not Empty\n" RESET);
        return size;
    }
    sleep(1);
    printf(RED "INFO: Done. Empty\n" RESET);
    return size;
}
uint get_image_size_for_bmp(FILE *fptr_image, EncodeInfo *encInfo)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    printf(RED "width = %u\n" RESET, width);
    printf(RED "height = %u\n" RESET, height);

    // Read bit pixel
    fseek(fptr_image, 2L, SEEK_CUR);
	fread(&encInfo->bits_per_pixel, sizeof(short), 1, fptr_image);
	rewind(fptr_image);

    // Convert bits per pixel to bytes per pixel
    uint bytes_per_pixel = encInfo->bits_per_pixel / 8;

    // Return image capacity
    return width * height * bytes_per_pixel;
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
    sleep(1);
    printf(YEL "INFO: Opening required files\n" RESET);
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n" RESET, encInfo->src_image_fname);

    	return e_failure;
    }
    sleep(1);
    printf(GRN "INFO: Opened %s Successfully\n" RESET, encInfo->src_image_fname);
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n" RESET, encInfo->secret_fname);

    	return e_failure;
    }
    sleep(1);
    printf(GRN "INFO: Opened %s Successfully\n" RESET, encInfo->secret_fname);
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n" RESET, encInfo->stego_image_fname);

    	return e_failure;
    }
    sleep(1);
    printf(GRN "INFO: Opened %s Successfully\n" RESET, encInfo->stego_image_fname);
    // No failure return e_success
    return e_success;
}
/*
* Validate Command Line Arguments
* Inputs: Command Line arguments
* Output: source image name, secret file name, secret file extension, output image name
* Return Values: e_success or e_failure
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp((strstr(argv[2], ".")), ".bmp") == 0) /* Check For Passed Image Format as .bmp */
    {
        encInfo->src_image_fname = argv[2];
        char *extn;
        extn = strstr(argv[3], ".");
        if((strcmp(extn, ".txt") == 0) || (strcmp(extn, ".sh") == 0) || (strcmp(extn, ".c") == 0)) /* Check For Passed Secret File Format as .txt or .c or .sh */
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, extn);
            if (!(argv[4] == NULL)) /* Check Whether Output Image Argument is Passed or not ! */
            {
                if(strcmp((strstr(argv[4], ".")), ".bmp") == 0) /* If Output Image Argument is Passed check if it's Extension is .bmp */
                {
                    encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    sleep(1);
                    printf(RED "Output File name is not of type .bmp\n" RESET);
                    return e_failure;
                }
            }
            else
            {
                sleep(1);
                printf(YEL "INFO: Output File not mentioned. Creating stego.bmp as default\n" RESET);
                encInfo->stego_image_fname = "stego.bmp"; /* If not Passed Create Default File Named STEGO.bmp */
            }
            
        }
        else
        {
            printf(RED "Secret file not of type .txt\n" RESET);
            return e_failure;
        }
    }
    else
    {
        printf(RED "Source Image File is not of type .bmp\n" RESET);
        return e_failure;
    }
    return e_success;
}

/* 
 * Perform Encoding
 * Inputs: Call each Functions one by one to perform the encoding task
 * Output: Information on Status of Function Call
 * Return Value: e_success or e_failure, on file errors
 */

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        sleep(1);
        printf(BGREEN"[INFO] Done\n"RESET);
        printf(BMAGENTA "[INFO] ## Encoding Procedure Started ##\n" RESET);
        if(check_capacity(encInfo) == e_success)
        {
            sleep(1);
            printf(BGREEN"[INFO] Check Capacity Done\n"RESET);
            if( copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                sleep(1);
                printf(BCYAN"[INFO] Copying BMP Header Successfully\n"RESET);
                if( encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    sleep(1);
                    printf(BBLUE"[INFO] MAGIC STRING Encoded Successfully\n"RESET);
                    if( encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                    {
                        sleep(1);
                        printf(BMAGENTA"[INFO] Secret File Extension Size Encoded Successfully\n"RESET);
                        if( encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            sleep(1);
                            printf(BCYAN"[INFO] Secret File Extension Encoded Successfully\n");
                            if( encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                sleep(1);
                                printf(BMAGENTA"[INFO] Secret File Size Encoded SuccessFully\n"RESET);
                                if( encode_secret_file_data(encInfo) == e_success)
                                {
                                    sleep(1);
                                    printf(BCYAN"[INFO] Secret File Data Encoded Successfully\n"RESET);
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        sleep(1);
                                        printf(BRED "[INFO] Remaining Image Data Copied Successfully\n" RESET);
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