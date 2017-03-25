#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int wrap_stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);

#define JC_QRENCODE_IMPLEMENTATION
#include "jc_qrencode.h"


static void Usage()
{
    printf("Usage: qrencode [options] [text]\n");
    printf("\t-i <inputfile>\t\tA file containing text\n");
    printf("\t-o <outputfile.png>\n");
    printf("\t-w <width>\n");
    printf("\t-h <height>\n");
}

static char g_Text[8192];
static uint32_t g_TextLength = 0;

/*
static void read_input(char* buffer, uint32_t buffersize, uint32_t* length)
{
    ssize_t num_read = 0;
    uint32_t l = 0;
    while( (num_read = read(STDIN_FILENO, &buffer[l], 32)) > 0 )
    {
        l += num_read;
        if( l > buffersize )
        {
            *length = 0xFFFFFFFF;
            return;
        }
    }
    *length = l;
}
*/

static int save_image(JCQRCode* qr, const char* path)
{
    int32_t size = qr->size;

    int32_t border = 2;
    int32_t scale = 8;
    int32_t newsize = scale*(size + 2 * border);
    uint8_t* large = (uint8_t*)malloc( newsize * newsize );

    memset(large, 255, newsize*newsize);

    for( int y = 0; y < size*scale; ++y )
    {
        for( int x = 0; x < size*scale; ++x )
        {
            uint8_t module = qr->data[(y/scale)*256 + (x/scale)];
            large[(y + scale*border) * newsize + x + scale*border] = module;
        }
    }

    int result = wrap_stbi_write_png(path, newsize, newsize, 1, large, newsize);
    free(large);
    
    if(result)
        printf("Wrote to %s\n", path);
    else
        printf("Failed to write to %s\n", path);
    return result;
}


static void read_input(const char* path, char* buffer, uint32_t buffersize, uint32_t* length)
{
    FILE* file = 0;
    if( path )
    {
        if( strcmp(path, "-") == 0 )
            file = stdin;
        else
            file = fopen(path, "rb");
        if( !file )
        {
            fprintf(stderr, "Failed to open %s for reading\n", path);
            *length = 0;
            return;
        }

        /*
        fseek(file, 0, SEEK_END);
        size_t size = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);
        count = (int)(size / sizeof(jcv_point));
        */

        uint32_t l = 0;
        while( !feof(file) )
        {
            size_t num_read = fread((void*)&buffer[l], 1, 64, file);
            l += num_read;
            if( l > buffersize )
            {
                *length = 0xFFFFFFFF;
                fclose(file);
                return;
            }
        }
        *length = l;
        buffer[buffersize-1] = 0;
        buffer[l] = 0;
        fclose(file);
    }
}

int main(int argc, const char** argv)
{
    int width = 512;
    int height = 512;
    const char* inputfile = 0;
    const char* outputfile = "out.png";

    if( argc == 1 )
    {
        Usage();
        return 1;
    }

    int firsttext = 1;
    for( int i = 1; i < argc; ++i )
    {
        if(strcmp(argv[i], "-i") == 0)
        {
            if( i+1 < argc )
                inputfile = argv[++i];
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-o") == 0)
        {
            if( i+1 < argc )
                outputfile = argv[++i];
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-w") == 0)
        {
            if( i+1 < argc )
                width = (int)atol(argv[++i]);
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-h") == 0)
        {
            if( i+1 < argc )
                height = (int)atol(argv[++i]);
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0)
        {
            Usage();
            return 0;
        }
        else
        {
            if( firsttext )
            {
                g_TextLength = 0;
                g_Text[0] = 0;
            }
            else
            {
                if( g_TextLength + 1 >= sizeof(g_Text) )
                {
                    fprintf(stderr, "Too much input text\n");
                    return 1;
                }
                g_Text[g_TextLength++] = ' ';
            }

            size_t len = strlen(argv[i]);
            if( g_TextLength + len >= sizeof(g_Text) )
            {
                fprintf(stderr, "Too much input text\n");
                return 1;
            }

            memcpy(&g_Text[g_TextLength], argv[i], len);
            g_TextLength += len;
            firsttext = 0;
        }
    }

    if( inputfile != 0 )
    {
        g_TextLength = 0;
        read_input(inputfile, g_Text, sizeof(g_Text), &g_TextLength);

        if( g_TextLength == 0 )
        {
            fprintf(stderr, "Too little input text\n");
            return 1;
        }
        if( g_TextLength > sizeof(g_Text) )
        {
            fprintf(stderr, "Too much input text\n");
            return 1;
        }
    }

    printf("TEXT: '%s'\n", g_Text);


    JCQRCode* qr = jc_qrencode((const uint8_t*)g_Text, strlen(g_Text), JC_QRE_ERROR_CORRECTION_LEVEL_QUARTILE);
    if( !qr )
    {
        fprintf(stderr, "Failed to encode text\n");
        return 1;
    }

    save_image(qr, outputfile);
    free(qr);

    return 0;
}
