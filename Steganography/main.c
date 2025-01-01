/*Documentation
* NAME : Anubhav Anand
* DATE : 01-01-2025
* DESCRIPTION : LSB IMAGE STEGANOGRAPHY
* •	The application accept an image file say .bmp along with the a text file which contains the message to be steged
* •	It then analyzes the size of the message file and the data part of the .bmp file to check whether the messsage could fit in the provided .bmp image
* •	After the Image Capacity Check is Successfull, It stegs a magic string inside output image which is used to protect it from unknown access
* •	The appliaction also provides a option to decrpt the output encoded image file
* •	This is a command line application and all the options has to be passed as a command line argument
* SAMPLE INPUT :
* ./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file (optional)]
* ./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file (optional)]

* SAMPLE OUTPUT (ENCODING):
* ✓[INFO] You have selected encoding process
* INFO: Output File not mentioned. Creating stego.bmp as default
* INFO: Opening required files
* INFO: Opened beautiful.bmp Successfully
* INFO: Opened secret.txt Successfully
* INFO: Opened stego.bmp Successfully
* ✓[INFO] Done
* ✓[INFO] ## Encoding Procedure Started ##
* width = 1024
* height = 768
* INFO: Checking for secret.txt size
* INFO: Done. Not Empty
* INFO: Checking for beautiful.bmp capacity to handle secret.txt
* INFO: Done. Found OK
* ✓[INFO] Check Capacity Done
* INFO: Copying Image Header
* ✓[INFO] Copying BMP Header Successfully
* INFO: Encoding Magic String Signature
* ✓[INFO] MAGIC STRING Encoded Successfully
* INFO: Encoding secret.txt File Extenstion Size
* ✓[INFO] Secret File Extension Size Encoded Successfully
* INFO: Encoding secret.txt File Extenstion
* ✓[INFO] Secret File Extension Encoded Successfully
* INFO: Encoding secret.txt File Size
* ✓[INFO] Secret File Size Encoded SuccessFully
* INFO: Encoding secret.txt File Data
* ✓[INFO] Secret File Data Encoded Successfully
* INFO: Copying Left Over Data
* ✓[INFO] Remaining Image Data Copied Successfully
* ✓[INFO] ## Encoding Done Successfully ##

* SAMPLE OUTPUT (DECODING):

* ✓[INFO] You have selected decoding process
* ✓[INFO] opened IMAGE File Successfully
* ✓[INFO] Magic String Verified Successfully
* ✓[INFO] Secret File Extension Size Decoded Successfully
* ✓[INFO] Secret File Extension Decoded Successfully
* ✓[INFO] opened SECRET File Successfully
* ✓[INFO] Secret File Size Decoded Successfully
* ✓[INFO] Secret File Data Decoded Successfully
* ✓[INFO] Decoding Done Successfully
*/

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
    /* Validate Number of Command Line Arguments Passed */
    if(argc >= 3) 
    {
        if ( check_operation_type(argv) == e_encode) /* Check the Operation Type Based on the flag passed from Command Line,
        if -e then operation type is encoding */
        {
            sleep(1);
            printf(BRED"[INFO] You have selected encoding process\n"RESET);
            if(argc > 3 && argc <= 5) /* Number of Command Line Arguments Required For Encoding Operation*/
            {
                if( read_and_validate_encode_args(argv, &encInfo) == e_success) /* Call the read and validate function to validate
                Command Line Arguments Passed, If the Function return e_success then perform encoding operation*/
                {
                    if( do_encoding(&encInfo) == e_success) 
                    {
                        sleep(1);
                        printf(BGREEN"[INFO] ## Encoding Done Successfully ##\n"RESET);
                    }
                    else
                    {
                        printf(RED "Encoding Failed\n" RESET);
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
                printf(RED "Invalid Number of Arguments Passed for Encoding\n" RESET); /* Print Error Message if Number of Argument passed
                for encoding is less than 4 and greater than 5 */
                e_failure;
            }
        }
        if ( check_operation_type(argv) == e_decode) /* Check the Operation Type Based on the flag passed from Command Line,
        if -d then operation type is decoding */
        {
            sleep(1);
            printf(BRED"[INFO] You have selected decoding process\n"RESET);
            if(argc <= 4) /* Number of Command Line Arguments Required For Decoding Operation*/
            {
                if( read_and_validate_decode_args(argv, &decInfo) == e_success) /* Call the read and validate function to validate
                Command Line Arguments Passed, If the Function return e_success then perform decoding operation*/
                {
                    if( do_decoding(&decInfo) == e_success)
                    {
                        sleep(1);
                        printf(BGREEN"[INFO] Decoding Done Successfully\n"RESET);
                    }
                    else
                    {
                        printf(RED "Decoding Failed\n" RESET);
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
                printf(RED "Invalid Number of Arguments Passed for Decoding\n" RESET); /* Print Error Message if Number of Argument passed
                for decoding is less than 4 and greater than 2*/
                return e_failure;
            }
        }
        if( check_operation_type(argv) == e_unsupported ) /* Check the Operation Type Based on the flag passed from Command Line,
        if anything other than -e or -d is passed then operation type is unsupported */
        {
            sleep(1);
            printf(RED "Unsupported Operation Type Selected\n" RESET); /* Print Error Message on Terminal*/
            return e_failure;
        }
    }
    else
    {
        printf(RED "Invalid Number of Arguments Passed\n" RESET);
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