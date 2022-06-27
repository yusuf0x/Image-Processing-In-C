#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <math.h>
#include <stdint.h>
#include <math.h>
#include "Bmp.h"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

struct Image openFile(const char *filename){
    FILE *fp=fopen(filename,"rb");
    fread(header.name,2,1,fp);
    fread(&header.size,3*sizeof(int),1,fp);
    printf("%c %c\n",header.name[0],header.name[1]);
    if ((header.name[0]!='B') || (header.name[1]!='M'))
    {
        fclose(fp);
        exit(1);
    }
    printf("size %d\n",header.size);
    printf("offset %d\n",header.image_offset);
    fread(&dibheader,sizeof(DIB_header),1,fp);
    printf("header size :%d\nwidth:%d\nheight:%d\ncompression:%d\n",dibheader.header_size,dibheader.width,dibheader.height,dibheader.compression);
    if ((dibheader.header_size!=40) || (dibheader.compression!=0) || (dibheader.bitsperpixel!=24))
    {
        fclose(fp);exit(1);
    }
    fseek(fp,header.image_offset,SEEK_SET);
    struct Image pic=readImage(fp,abs(dibheader.height),dibheader.width);
    fclose(fp);
    return pic;
}

struct Image readImage(FILE *fp,int height,int width){
    struct Image pic;
    pic.rgb = (struct RGB**)malloc(height*sizeof(void*));
    pic.height = height;
    pic.width = width;
    int bytestored,numOfrgb;
    bytestored = ((24 *width +31)/32)*4 ;
    numOfrgb = bytestored/sizeof(struct RGB)+1;
    for (int i = height-1; i >= 0; i--)
    {
        pic.rgb[i] = (struct RGB*)malloc(width*sizeof(struct RGB));
        fread(pic.rgb[i],1,bytestored,fp);
    }
    return pic;
}
int createBmpImage(struct BMP_header header,DIB_header dibheader ,struct Image pic){
    FILE *fpw = fopen("new.bmp","w");
    if(fpw==NULL) return 1;
    fwrite(header.name,2,1,fpw);
    fwrite(&header.size,3*sizeof(int),1,fpw);
    fwrite(&dibheader,sizeof(DIB_header),1,fpw);
    for (int i = pic.height-1; i >= 0; i--)
    {
        fwrite(pic.rgb[i], ((24 *pic.width +31)/32)*4,1,fpw);
    }
    fclose(fpw);
    return 0;
}

void freeImage(struct Image pic){
    int i;
    for (int i = pic.height-1; i >= 0; i--) free(pic.rgb[i]);
    free(pic.rgb);
}
unsigned char grayscale(struct RGB rgb){
    float avg = (((float)rgb.red + rgb.green+rgb.blue) / 3);
     int roundi = round(avg);
    return avg;
}
struct Image RGB_TO_GrayScale(struct Image pic){
    int i,j;
    for (int i = 0; i < pic.height; ++i)
    {
        for (int j= 0;j < pic.width; ++j)
        {
            pic.rgb[i][j].red=pic.rgb[i][j].green=pic.rgb[i][j].blue=grayscale(pic.rgb[i][j]);
        }
    }
    return pic;
}

void IMageToText(struct Image img){
    unsigned char c;
    int i,j;
    int d=0;
    char textpixel[] = {'@','#','%','O','a','-','.',' '};
    for (i = 0; i < img.height; ++i)
    {
        for (j = 0; j < img.width; ++j)
        {
            c =  grayscale(img.rgb[i][j]);
            d = c/32;
            printf("%c",textpixel[7-d]);
        }
        printf("\n");
    }
}

struct Image edges(int height, int width, struct Image image)
{
    struct RGB tmp[height][width];
    // struct

    //gx matrix
    int Gx[3][3] =
    {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    //gy matrix
    int Gy[3][3] =
    {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    for (int row = 0; row < height; row ++)
    {
        for (int col = 0; col < width; col ++)
        {
            int count = 0;

            //x axis
            int xaxis[] = {row - 1, row, row + 1};
            // y axis
            int yaxis[] = {col - 1, col, col + 1};

            // flaot vals for gx rgb
            float Gx_R = 0, Gx_G = 0, Gx_B = 0;

            // flaot vals for gy rgb
            float Gy_R = 0, Gy_G = 0, Gy_B = 0;

            for (int r = 0; r < 3; r++)
            {
                for (int c = 0; c < 3; c++)
                {
                    int curRow = xaxis[r];
                    int curCol = yaxis[c];

                    if (curRow >= 0 && curRow < height && curCol >= 0 && curCol < width)
                    {
                        // RGBTRIPLE pixel = image[curRow][curCol];
                        struct RGB pixel = image.rgb[curRow][curCol];
                        // matrix for gx_rgb * the gx vals
                        Gx_R += Gx[r][c] * pixel.red;
                        Gx_G += Gx[r][c] * pixel.green;
                        Gx_B += Gx[r][c] * pixel.blue;

                        // matrix for gy_rgb * the gy vals
                        Gy_R += Gy[r][c] * pixel.red;
                        Gy_G += Gy[r][c] * pixel.green;
                        Gy_B += Gy[r][c] * pixel.blue;


                    }
                }
            }

            //sqrt of the vals of gx and gy rgb then roud it
            int final_red = round(sqrt((Gx_R * Gx_R) + (Gy_R * Gy_R)));
            int final_green = round(sqrt((Gx_G * Gx_G) + (Gy_G * Gy_G)));
            int final_blue = round(sqrt((Gx_B * Gx_B) + (Gy_B * Gy_B)));

            // if the value more than 255 then cap it to 255
            if (final_red > 255)
            {
                final_red = 255;
            }
            if (final_green > 255)
            {
                final_green = 255;
            }
            if (final_blue > 255)
            {
                final_blue = 255;
            }

            //update vals in the tmp
            tmp[row][col].red = final_red;
            tmp[row][col].green = final_green;
            tmp[row][col].blue = final_blue;


        }
    }

    // updating the vals into the new image output
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j ++)
        {
            image.rgb[i][j] = tmp[i][j];
        }
    }
    return image;
}
struct Image logicAND(struct Image im1, struct Image im2)
{

    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            new.rgb[i][j].blue = im1.rgb[i][j].blue & im2.rgb[i][j].blue;
            new.rgb[i][j].green = im1.rgb[i][j].green & im2.rgb[i][j].green;
            new.rgb[i][j].red = im1.rgb[i][j].red & im2.rgb[i][j].red;
        }
    }

    return new;
}
struct Image logicNOT(struct Image im1)
{

    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            new.rgb[i][j].blue = (im1.rgb[i][j].blue==0) ? 255 : 0 ;
            new.rgb[i][j].green = (im1.rgb[i][j].green==0) ? 255 : 0;
            new.rgb[i][j].red = (im1.rgb[i][j].red==0) ? 255 : 0;
        }
    }

    return new;
}
struct Image logicNAND(struct Image im1,struct Image im2){
    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            new.rgb[i][j].blue = !(im1.rgb[i][j].blue & im2.rgb[i][j].blue);
            new.rgb[i][j].green = !(im1.rgb[i][j].green & im2.rgb[i][j].green);
            new.rgb[i][j].red = !(im1.rgb[i][j].red & im2.rgb[i][j].red);
        }
    }
    return new;
}
struct Image logicOR(struct Image im1,struct Image im2){
    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            new.rgb[i][j].blue = im1.rgb[i][j].blue | im2.rgb[i][j].blue;
            new.rgb[i][j].green = im1.rgb[i][j].green | im2.rgb[i][j].green;
            new.rgb[i][j].red = im1.rgb[i][j].red | im2.rgb[i][j].red;
        }
    }

    return new;
}
struct Image logicXOR(struct Image im1,struct Image im2){
    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            new.rgb[i][j].blue = im1.rgb[i][j].blue ^ im2.rgb[i][j].blue;
            new.rgb[i][j].green = im1.rgb[i][j].green ^ im2.rgb[i][j].green;
            new.rgb[i][j].red = im1.rgb[i][j].red ^ im2.rgb[i][j].red;
        }
    }

    return new;
}
struct Image subtraction(struct Image im1,struct Image im2){
    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            unsigned int  pixel = im1.rgb[i][j].blue - im2.rgb[i][j].blue;
            new.rgb[i][j].blue = (unsigned int )max(pixel, 0);
            pixel = im1.rgb[i][j].green - im2.rgb[i][j].green;
            new.rgb[i][j].green = (unsigned int )max(pixel, 0);
            pixel = im1.rgb[i][j].red - im2.rgb[i][j].red;
            new.rgb[i][j].red =(unsigned int )max(pixel, 0);
        }
    }

    return new;
}
struct Image addition(struct Image im1,struct Image im2){
    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            unsigned int  pixel = im1.rgb[i][j].blue + im2.rgb[i][j].blue;
            new.rgb[i][j].blue = (unsigned int )min(pixel, 255);
            pixel = im1.rgb[i][j].green + im2.rgb[i][j].green;
            new.rgb[i][j].green = (unsigned int )min(pixel, 255);
            pixel = im1.rgb[i][j].red + im2.rgb[i][j].red;
            new.rgb[i][j].red =(unsigned int )min(pixel, 255);
        }
    }

    return new;
}
struct Image multiplication(struct Image im1 , double factor){
    struct Image new;
    for (int i = 0; i < im1.height; i++)
    {
        for (int j = 0; j < im1.width; j++)
        {
            unsigned int  pixel = im1.rgb[i][j].blue * factor;
            new.rgb[i][j].blue = (unsigned int )min(pixel, 255);
            pixel = im1.rgb[i][j].green * factor;
            new.rgb[i][j].green = (unsigned int )min(pixel, 255);
            pixel = im1.rgb[i][j].red * factor;
            new.rgb[i][j].red =(unsigned int )min(pixel, 255);
        }
    }

    return new;
}
struct Image reflect(int height, int width, struct Image image)
{
    // swaping 2 vals [end to start]
    for (int i = 0; i < height; i++)
    {
        //only till width / 2 because we dont want it to swap again
        for (int j = 0; j < (int)width / 2 ; j ++)
        {
            int tmpblue = image.rgb[i][j].blue;
            image.rgb[i][j].blue = image.rgb[i][width - j - 1].blue;
            image.rgb[i][width - j - 1].blue = tmpblue;

            int tmpgreen = image.rgb[i][j].green;
            image.rgb[i][j].green = image.rgb[i][width - j - 1].green;
            image.rgb[i][width - j - 1].green = tmpgreen;

            int tmpred = image.rgb[i][j].red;
            image.rgb[i][j].red = image.rgb[i][width - j - 1].red;
            image.rgb[i][width - j - 1].red = tmpred;

        }
    }
    return image;
}
void printHelp(){
    printf("--------------------Help---------------------\n"
           "- To apply NOT operation on an image: ./Bmp not infile outfile\n"
           "- To apply EDGES operation on an image: ./Bmp edges infile outfile\n"
           "- To apply reflect operation on an image: ./Bmp reflect infile outfile\n"
           "- To apply grayscale operation on an image: ./Bmp grayscale infile outfile\n"
           "- To ADD two images: ./Bmp  infile1 add infile2 outfile\n"
           "- To SUB two images: ./Bmp  infile1 sub infile2 outfile\n"
           "- To AND two images: ./Bmp  infile1 and infile2 outfile\n"
           "- To NAND two images: ./Bmp  infile1 nand infile2 outfile\n"
           "- To OR two images: ./Bmp  infile1 or infile2 outfile\n"
           "- To XOR two images: ./Bmp  infile1 xor infile2 outfile\n"
           "- To MUL an image by factor: ./Bmp  factor mul infile outfile \n"
           "---------------------------------------------\n");
}



struct Image runOperation(int argc,char **argv){
    switch (argc)
    {
    case 3:
        return runOp3(argv);
        break;

    case 4:
        return runOp4(argv);
        break;
    default:
        printHelp();
    }
}

struct Image runOp3(char **argv){

    if (strcmp(argv[1], "not") == 0)
    {
        printf("Applying NOT operation on image\n");
        struct Image pic1 = openFile(argv[2]);
        pic1 = logicNOT(pic1);
        return pic1 ;
    } 
     else if (strcmp(argv[1], "edges") == 0)
    {
        printf("Applying EDGES operation on image\n");
        struct Image pic1 = openFile(argv[2]);
        pic1 = edges(pic1.height,pic1.width,pic1);
        return pic1 ;
    } 
      else if (strcmp(argv[1], "reflect") == 0)
    {
        printf("Applying reflect operation on image\n");
        struct Image pic1 = openFile(argv[2]);
        pic1 = reflect(pic1.height,pic1.width,pic1);
        return pic1 ;
    } 
      else if (strcmp(argv[1], "grayscale") == 0)
    {
        printf("Applying reflect operation on image\n");
        struct Image pic1 = openFile(argv[2]);
        pic1 = RGB_TO_GrayScale(pic1);
        return pic1 ;
    } 
}
struct Image runOp4(char **argv){
    if (strcmp(argv[2], "and") == 0)
    {
        struct Image pic1 = openFile(argv[1]);
        struct Image pic2 = openFile(argv[3]);
        return logicAND(pic1, pic2);
    }
    else if (strcmp(argv[2], "nand") == 0)
    {
        struct Image pic1 = openFile(argv[1]);
        struct Image pic2 = openFile(argv[3]);
        return logicNAND(pic1, pic2);
    }
    else if (strcmp(argv[2], "or") == 0)
    {
        struct Image pic1 = openFile(argv[1]);
        struct Image pic2 = openFile(argv[3]);
        return logicOR(pic1, pic2);
    }
    else if (strcmp(argv[2], "xor") == 0)
    {
        struct Image pic1 = openFile(argv[1]);
        struct Image pic2 = openFile(argv[3]);
        return logicXOR(pic1, pic2);
    }
    else if (strcmp(argv[2], "add") == 0)
    {
        struct Image pic1 = openFile(argv[1]);
        struct Image pic2 = openFile(argv[3]);
        return addition(pic1, pic2);
       
    }
    else if (strcmp(argv[2], "sub") == 0)
    {
        struct Image pic1 = openFile(argv[1]);
        struct Image pic2 = openFile(argv[3]);
        return subtraction(pic1, pic2);
        
    }
    else if (strcmp(argv[1], "mult") == 0)
    {
        struct Image pic1 = openFile(argv[3]);
        double factor = atof(argv[2]);
        return multiplication(pic1, factor);
    }
    else{
        printHelp();
        struct Image new;
        return new;
    }
}
int main(int argc, char const *argv[])
{

    struct Image out = runOperation(argc, argv);
    if (out.rgb == NULL)
    {   
        exit(1);
    }
    createBmpImage(header,dibheader,out);
    freeImage(out);
    return 0;
}

































