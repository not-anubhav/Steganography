#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"


void e_printdata(EncodeInfo *encInfo)
{
    printf("Source Image Name: %s\n", encInfo->src_image_fname);
    printf("Image Capacity: %d\n", encInfo->image_capacity);
    printf("Bits per pixel: %d\n", encInfo->bits_per_pixel);
    printf("Secret File Name: %s\n", encInfo->secret_fname);
    printf("Secret extension: %s\n", encInfo->extn_secret_file);
    printf("Secret File Size: %ld\n", encInfo->size_secret_file);
    printf("Stego Image Name: %s\n", encInfo->stego_image_fname);
}
