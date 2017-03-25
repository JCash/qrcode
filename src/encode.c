#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
                inputfile = argv[i+1];
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-o") == 0)
        {
            if( i+1 < argc )
                outputfile = argv[i+1];
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-w") == 0)
        {
            if( i+1 < argc )
                width = (int)atol(argv[i+1]);
            else
            {
                Usage();
                return 1;
            }
        }
        else if(strcmp(argv[i], "-h") == 0)
        {
            if( i+1 < argc )
                height = (int)atol(argv[i+1]);
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

/*
    size_t imagesize = (size_t)(width*height*3);
    unsigned char* image = (unsigned char*)malloc(imagesize);
    memset(image, 0, imagesize);

    unsigned char color_pt[] = {255, 255, 255};
    unsigned char color_line[] = {220, 220, 220};

    if( mode == 0 )
    {
        jcv_diagram diagram;
        memset(&diagram, 0, sizeof(jcv_diagram));
        jcv_diagram_generate(count, (const jcv_point*)points, width, height, &diagram );

        // If you want to draw triangles, or relax the diagram,
        // you can iterate over the sites and get all edges easily
        const jcv_site* sites = jcv_diagram_get_sites( &diagram );
        for( int i = 0; i < diagram.numsites; ++i )
        {
            const jcv_site* site = &sites[i];

            srand((unsigned int)site->index); // for generating colors for the triangles

            unsigned char color_tri[3];
            unsigned char basecolor = 120;
            color_tri[0] = basecolor + (unsigned char)(rand() % (235 - basecolor));
            color_tri[1] = basecolor + (unsigned char)(rand() % (235 - basecolor));
            color_tri[2] = basecolor + (unsigned char)(rand() % (235 - basecolor));

            const jcv_graphedge* e = site->edges;
            while( e )
            {
                draw_triangle( &site->p, &e->pos[0], &e->pos[1], image, width, height, 3, color_tri);
                e = e->next;
            }
        }

        // If all you need are the edges
        const jcv_edge* edge = jcv_diagram_get_edges( &diagram );
        while( edge )
        {
            draw_line((int)edge->pos[0].x, (int)edge->pos[0].y, (int)edge->pos[1].x, (int)edge->pos[1].y, image, width, height, 3, color_line);
            edge = edge->next;
        }

        jcv_diagram_free( &diagram );
    }

    // flip image
    int stride = width*3;
    uint8_t* row = (uint8_t*)malloc((size_t)stride);
    for( int y = 0; y < height/2; ++y )
    {
        memcpy(row, &image[y*stride], (size_t)stride);
        memcpy(&image[y*stride], &image[(height-1-y)*stride], (size_t)stride);
        memcpy(&image[(height-1-y)*stride], row, (size_t)stride);
    }

    char path[512];
    sprintf(path, "%s", outputfile);
    stbi_write_png(path, width, height, 3, image, stride);
    printf("wrote %s\n", path);

    free(image);

*/

    return 0;
}
