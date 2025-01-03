#ifndef DECODE_H
#define DECODE_H
#include "types.h"

/* 
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define MAX_HEADER_SIZE 54
#define MAX_FILENAME_SIZE 256

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname; /* Store the source image name*/
    FILE *fptr_src_image; /* Store the source image */

    /* Secret File Info */
    char *secret_fname; /*Store the secret file name */
    FILE *fptr_secret; /*Store the secret file address*/
    uint file_extn_size; /*Store the size of the secret file extension*/
    char extn_secret_file[MAX_FILE_SUFFIX]; /*Store the extension of secret file*/
    char secret_data[MAX_SECRET_BUF_SIZE]; /*Store the data present inside the secret file*/
    long size_secret_file; /*Store the size of the secret file*/

    char *magic_str;

} DecodeInfo;

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_image_file(DecodeInfo *decInfo);

Status open_secret_file(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(char *data, uint size, FILE *fptr_src_image);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode a size from LSB of image data array */
Status decode_size_from_lsb(uint *size, char *buffer);

/*Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);
#endif