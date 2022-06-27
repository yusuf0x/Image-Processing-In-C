
struct BMP_header{
    char name[2];
    unsigned int size;
    int garbage;
    unsigned int image_offset;
};

typedef struct
{
    unsigned int   header_size;
    unsigned int   width;
    unsigned int   height;
    unsigned short int   colorplanes;
    unsigned short int   bitsperpixel;
    unsigned int  compression;
    unsigned int  image_size;
    int temp[4];
     // unsigned int   biXPelsPerMeter;
     // unsigned int   biYPelsPerMeter;
     // unsigned int  biClrUsed;
     // unsigned int  biClrImportant;
}
DIB_header;
struct RGB {
    unsigned int blue;
    unsigned int green;
    unsigned int red;
};
struct Image { 
    int height;
    int width;
    struct RGB **rgb;
};


struct BMP_header header;
DIB_header dibheader;

struct Image openFile(const char *filename);
void IMageToText(struct Image img);
int createBmpImage(struct BMP_header header,DIB_header dibheader ,struct Image pic);
struct Image RGB_TO_GrayScale(struct Image pic);
unsigned char grayscale(struct RGB rgb);
void freeImage(struct Image pic);
struct Image readImage(FILE *fp,int height,int width);
struct Image edges(int height, int width, struct Image image);
struct Image logicAND(struct Image im1, struct Image im2);
struct Image logicNOT(struct Image im1);
struct Image logicNAND(struct Image im1,struct Image im2);
struct Image logicOR(struct Image im1,struct Image im2);
struct Image logicXOR(struct Image im1,struct Image im2);
struct Image subtraction(struct Image im1,struct Image im2);
struct Image addition(struct Image im1,struct Image im2);
struct Image multiplication(struct Image im1 , double factor);
void printHelp();
struct Image runOperation(int argc,char **argv);
struct Image runOp3(char **argv);
struct Image runOp4(char **argv);
struct Image reflect(int height, int width, struct Image image);
