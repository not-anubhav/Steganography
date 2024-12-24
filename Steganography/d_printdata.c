#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"


void d_printdata(DecodeInfo *decInfo)
{
    printf("Source Image Name: %s\n", decInfo->src_image_fname);
    printf("Secret File Name: %s\n", decInfo->secret_fname);
    printf("Secret File Extension Size: %d\n", decInfo->file_extn_size);
    printf("Secret extension: %s\n", decInfo->extn_secret_file);
    printf("Secret File Size: %ld\n", decInfo->size_secret_file);
}