#include <stdio.h>
#include <stdlib.h>
#include<math.h>
int height,width,bitdepth;

unsigned char buf[1000*1000*3]; // to store the image data
unsigned char new_data[1000*1000*3];///because i am not able to declare that much big array inside the function
int mean_gray(unsigned char* buf);
void newcopy(unsigned char*,unsigned char*,unsigned char* buf);
void negativecopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void brightcopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void bitimagecopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void crop(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void colorToGray(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void rotate(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void vreflect(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void hreflect(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void blurr_image(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void sepia(unsigned char* header,unsigned char* colorTable,unsigned char* buff);
void translate(unsigned char* header,unsigned char* colorTable ,unsigned char* buf);
void shrink_image(unsigned char* header, unsigned char* colorTable,unsigned char* buf);//works perfectly mainly on 512x512 images
void set_dimensions(unsigned char*, int,int);//enter height and width in the header
int main()
{
	int i;
	char file_name[30];
	printf("Enter filename of image which is to be processed  : ");
	scanf("%s",file_name);
	FILE *streamIn;
        streamIn = fopen(file_name, "rb"); // Read file in bits

    if (streamIn == (FILE *)0) // check if the input file has not been opened successfully.
	{
            printf("File opening error ocurred. Exiting program.\n");
            exit(0);
 	}

 	unsigned char header[54]; // to store the image header
	unsigned char colorTable[1024]; // to store the colorTable, if it exists.


    fread(header, sizeof(unsigned char), 54, streamIn);


 	width = *(int*)&header[18]; // read the width from the image header
 	height = *(int*)&header[22]; // read the height from the image header
	bitdepth = *(int*)&header[28]; // read the bitdepth from the image header

	if(bitdepth <= 8)
	{
	    printf("%d\n",bitdepth);
        fread(colorTable, sizeof(unsigned char), 1024, streamIn);
        for(i=0;i<height*width;i++)
        {
            buf[i]=getc(streamIn);
        }
	}
	else
    {
        printf("%d\n",bitdepth);
        for(i=0;i<height*width;i++)
        {
            buf[2+3*i]=getc(streamIn);   ///Blue
            buf[1+3*i]=getc(streamIn);   ///GREEN
            buf[3*i]=getc(streamIn);   ///RED
        }
    }
    int m;
    printf("You can perform following operations:\n");
        printf("1-NewCopy\n2-NegativeCopy\n3-BrightCopy\n4-Bitimage\n5-Rotate\n6-crop\n7-HorizontalReflection\n8-VerticalReflection\n9-ColorToGray\n10-ApplyingSepia\n11-BlurrImage\n12-Translate\n13-ShrinkImage\n");
        printf("Enter your choice:");
        scanf("%d",&m);
        switch(m)
        {
            case 1: newcopy(header,colorTable,buf);
            break;
            case 2: negativecopy(header,colorTable,buf);
            break;
            case 3: brightcopy(header,colorTable,buf);
            break;
            case 4: bitimagecopy(header,colorTable,buf);
            break;
            case 5: rotate(header,colorTable,buf);
            break;
            case 6: crop(header,colorTable,buf);
            break;
            case 7: hreflect(header,colorTable,buf);
            break;
            case 8: vreflect(header,colorTable,buf);
            break;
            case 9: colorToGray(header,colorTable,buf);
            break;
            case 10: sepia(header,colorTable,buf);
            break;
            case 11: blurr_image(header,colorTable,buf);
            break;
            case 12: translate(header,colorTable,buf);
            break;
            case 13: shrink_image(header,colorTable,buf);
            break;
            default:printf("Enter a valid choice\n");
            break;
        }
 	fclose(streamIn);
 }


void blurr_image(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
{
    int factor; int i,j;
    printf("Enter factor(ODD INTEGER) by which you want to blur : ");
    scanf("%d",&factor);
    float v=1.0/(float)(factor*factor);
    float kernel[factor][factor];
    for(i=0;i<factor;i++)
    {
        for(j=0;j<factor;j++)
        {
            kernel[i][j]=v;
        }
    }
    int mid=factor/2;
    int x,y;
    float sum,r,g,b;
    for(i=0;i<3*height*width;i++)
    {
        new_data[i]=buf[i];
    }
    if(bitdepth<=8)
    {
        for(x=mid;x<height-mid;x++)
        {
            for(y=1;y<width-mid;y++)
            {

                sum=0.0;
                for(i=-mid;i<=mid;++i)
                {
                    for(j=-mid;j<=mid;++j)
                    {
                       sum = sum + (float)kernel[i+mid][j+mid]*buf[(x+i)*width+(y+j)];
                    }
                }
                new_data[x*width + y] = sum;
            }
        }
    }
    else
    {
        for(x=mid;x<height-mid;x++)
        {
            for(y=mid;y<width-mid;y++)
            {
                r=0.0; g=0.0; b=0.0;
                for(i=-mid;i<=mid;i++)
                {
                    for(j=-mid;j<=mid;j++)
                    {
                       r=r+(float)kernel[i+mid][j+mid]*buf[3*(x+i)*width + 3*(y+j)];

                       g=g+(float)kernel[i+mid][j+mid]*buf[3*(x+i)*width + 3*(y+j) + 1];

                       b=b+(float)kernel[i+mid][j+mid]*buf[3*(x+i)*width + 3*(y+j) + 2];
                    }
                }
                new_data[3*x*width + 3*y ]= r;
                new_data[3*x*width + 3*y + 1]= g;
                new_data[3*x*width + 3*y + 2]= b;
            }
        }

    }
    newcopy(header,colorTable,new_data);

}

void crop(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)///Height and width of cropped image should be multiple of 20 or only multiple of 2(190x190 image cannot be formed)
{///250x250 image cannot be formed ..260*260 can be formed..256 can be formed but 258 cannot be formed
    int i,j;
    int new_width,new_height;
    int LLr,LLc,URr,URc;///UR-upper right  LL- lower left
    printf("Enter Lower Left pixel location\nLLr : ");
    scanf("%d",&LLr);
    printf("LLc : ");
    scanf("%d",&LLc);

    printf("Enter Upper Right pixel location\nURr : ");
    scanf("%d",&URr);
    printf("URc : ");
    scanf("%d",&URc);

    new_width = URc - LLc;
    new_height = URr - LLr;

    unsigned char new_header[54];
    for(i=0;i<54;i++)
    {
        new_header[i]=header[i];
    }

    new_header[18] = (new_width) - 256*(new_width >> 8);  ///information of new_height and new_width saved in new_header
    new_header[19] = (new_width >> 8) - 256*(new_width >> 16);
    new_header[20] = (new_width >> 16) - 256*(new_width >> 24);
    new_header[21] = (new_width >> 24);
    new_header[22] = (new_height) - 256*(new_height >> 8);
    new_header[23] = (new_height >> 8) - 256*(new_height >> 16);
    new_header[24] = (new_height >> 16) - 256*(new_height >> 24);
    new_header[25] = (new_height >> 24);
    if(bitdepth<=8)
    {
        for(i=LLr;i<URr;i++)
        {
            for(j=LLc;j<URc;j++)
            {
                new_data[(i-LLr)*(new_width) + j - LLc] = buf[i*width + j];
            }
        }
    }
    else
    {
        for(i=LLr;i<URr;i++)
        {
            for(j=LLc;j<URc;j++)
            {

                new_data[3*(i-LLr)*new_width + 3*(j-LLc)] = buf[3*i*width + 3*j];

                new_data[3*(i-LLr)*new_width + 3*(j-LLc) + 1] = buf[3*i*width + 3*j + 1];

                new_data[3*(i-LLr)*new_width + 3*(j-LLc) + 2] = buf[3*i*width + 3*j + 2];
            }
        }
    }
    width=*(int*)&new_header[18];///storing new height
    height=*(int*)&new_header[22];///storing new width
    newcopy(new_header,colorTable,new_data);
}


void colorToGray(unsigned char* header,unsigned char* colorTable, unsigned char* buf)
{
    int y=0,i;
    if(bitdepth<=8)
    {
        printf("\nAlready Gray\n");
        newcopy(header,colorTable,buf);
    }
    else
    {
        for(i=0;i<height*width;i++)
        {
            y = (buf[3*i]*0.299) + (buf[3*i+1]*0.587) + (buf[3*i+2]*0.114); //conversion formula of rgb to gray

            new_data[3*i]=y;
            new_data[3*i + 1]=y;
            new_data[3*i + 2]=y;
        }
        newcopy(header,colorTable,new_data);
    }
}
void rotate(unsigned char* header,unsigned char* colorTable, unsigned char* buf)
{
    int n;
    printf("Press\n1 for 180degree rotation\n2 for clockwise 90degree rotation\n3 for anti-clockwise 90degree rotation\n");
    scanf("%d",&n);
    while(n!=1 && n!=2 && n!=3)
    {
        printf("Enter valid input : \n");
        scanf("%d",&n);
    }
    int i,j;
    int new_width;
    int new_height;
    unsigned char new_header[54];
    for(i=0;i<54;i++)
    {
        new_header[i]=header[i];
    }

    switch(n)
    {
        case 1:
            new_width = width;
            new_height = height;
        if(bitdepth<=8)
    {
        for(i=0;i<height*width;i++)
        {
            new_data[i] = buf[height*width - i -1];
        }
    }
    else
    {
        for(i=0;i<height*width;i++)
        {
            new_data[3*i] = buf[3*height*width - 3*i -3];
            new_data[3*i+1] = buf[3*height*width - 3*i -2];
            new_data[3*i+2] = buf[3*height*width - 3*i-1];
        }
    }
    break;
    case 2:
        new_width = height;
        new_height = width;
        if(bitdepth<=8)
        {
            for(i=0;i<new_height;i++)
            {
                for(j=0;j<new_width;j++)
                {
                    new_data[i*new_width + j] = buf[(j+1)*(width) - 1 - i ];
                }
            }
        }
        else
        {
            for(i=0;i<new_height;i++)
            {
                for(j=0;j<new_width;j++)
                {
                    new_data[3*i*new_width + 3*j] = buf[3*(j+1)*(width)  - 3*i - 3];
                    new_data[3*i*new_width + 3*j + 1] = buf[3*(j+1)*(width)  - 3*i - 2];
                    new_data[3*i*new_width + 3*j + 2] = buf[3*(j+1)*(width)  - 3*i - 1];
                }
            }
        }
        break;
    case 3:
        new_width = height;
        new_height = width;
        if(bitdepth<=8)
        {
            for(i=0;i<new_height;i++)
            {
                for(j=0;j<new_width;j++)
                {
                    new_data[i*new_width + j] = buf[height*width - (j+1)*width  + i - 1 ];
                }
            }
        }
        else
        {
            for(i=0;i<new_height;i++)
            {
                for(j=0;j<new_width;j++)
                {
                    new_data[3*i*new_width + 3*j] = buf[3*height*width - 3*(j+1)*width  + 3*i - 3 ];
                    new_data[3*i*new_width + 3*j + 1] = buf[3*height*width - 3*(j+1)*width  + 3*i - 2 ];
                    new_data[3*i*new_width + 3*j + 2] = buf[3*height*width - 3*(j+1)*width  + 3*i - 1 ];
                }
            }
        }
        break;
    }
    new_header[18] = (new_width) - 256*(new_width >> 8);  ///information of new_height and new_width saved in new_header
    new_header[19] = (new_width >> 8) - 256*(new_width >> 16);
    new_header[20] = (new_width >> 16) - 256*(new_width >> 24);
    new_header[21] = (new_width >> 24);
    new_header[22] = (new_height) - 256*(new_height >> 8);
    new_header[23] = (new_height >> 8) - 256*(new_height >> 16);
    new_header[24] = (new_height >> 16) - 256*(new_height >> 24);
    new_header[25] = (new_height >> 24);
    height = new_height;
    width = new_width;
    newcopy(new_header,colorTable,new_data);
}


void hreflect(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
{
    int i,j;
    if(bitdepth<=8)
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[i*width + j] = buf[(height-i-1)*width + j];
            }
        }
    }
    else
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[3*i*width + 3*j]=buf[(height-i-1)*3*width + 3*j];
                new_data[3*i*width + 3*j + 1]=buf[(height-i-1)*3*width + 3*j + 1];
                new_data[3*i*width + 3*j + 2]=buf[(height-i-1)*3*width + 3*j + 2];
            }
        }
    }
    newcopy(header,colorTable,new_data);
}


void vreflect(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
{
    int i,j;
    if(bitdepth<=8)
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[i*width + j] = buf[i*width + width - j -1];
            }
        }
    }
    else
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[3*i*width + 3*j]=buf[i*3*width + 3*(width - j -1)];
                new_data[3*i*width + 3*j + 1]=buf[i*3*width + 3*(width - j -1) + 1];
                new_data[3*i*width + 3*j + 2]=buf[i*3*width + 3*(width - j -1) + 2];

            }
        }
    }
    newcopy(header,colorTable,new_data);
}


int mean_gray(unsigned char* buf)
{
    int i,mean=0;
    for(i=0;i<height*width;i++)
    {
        mean=mean+buf[i];
    }
    mean=mean/height/width;
    printf("%d",mean);
    return mean;
}


void newcopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf )
 {
     int i;
     printf("width: %d\n",width);
    printf("height: %d\n",height );
    FILE *fo = fopen("copy.bmp","wb"); // Output File name

	fwrite(header, sizeof(unsigned char), 54, fo);
	if(bitdepth <= 8)
    {
        fwrite(colorTable, sizeof(unsigned char), 1024, fo);
        for(i=0;i<height*width;i++)
        {
            putc(buf[i],fo);
        }
    }
    else
    {
        for(i=0;i<width*height;i++)
        {
            putc(buf[i*3+2],fo);
            putc(buf[i*3+1],fo);
            putc(buf[i*3],fo);
        }
    }
    char new_name[30];
    printf("Enter name of copy  : ");
    scanf("%s",new_name);
	fclose(fo);
	rename("copy.bmp",new_name);
 }


 void negativecopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
 {
     int i,j;
    if(bitdepth<=8)
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[i*width + j] = 255 - buf[i*width + j];
            }
        }
    }
    else
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[3*i*width + 3*j] = 255 - buf[3*i*width + 3*j];

                new_data[3*i*width + 3*j + 1] = 255 - buf[3*i*width + 3*j + 1];

                new_data[3*i*width + 3*j + 2] = 255 - buf[3*i*width + 3*j + 2];
            }
        }

    }
     newcopy(header,colorTable,new_data);
 }


 void brightcopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
 {
    int brightness_factor;
    int temp,i,j;
    printf("Enter brightness factor : ");///How much bright image you want
    scanf("%d",&brightness_factor);
    if(bitdepth<=8)
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                temp=buf[i*width + j] + brightness_factor;
                if(temp<=255)
                new_data[i*width + j] = temp;
                else
                new_data[i*width + j] = 255;
             }
         }
     }
     else
     {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                temp=buf[3*i*width + 3*j] + brightness_factor;
                if(temp<=255)
                new_data[3*i*width + 3*j] = temp;
                else
                new_data[i*3*width + 3*j] = 255;

                temp=buf[3*i*width + 3*j + 1] + brightness_factor;
                if(temp<=255)
                new_data[3*i*width + 3*j + 1] = temp;
                else
                new_data[i*3*width + 3*j + 1] = 255;

                temp=buf[3*i*width + 3*j + 2] + brightness_factor;
                if(temp<=255)
                new_data[3*i*width + 3*j + 2] = temp;
                else
                new_data[i*3*width + 3*j + 2] = 255;
            }
        }
     }
     newcopy(header,colorTable,new_data);
 }


void bitimagecopy(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
{
    int mid=128;
    int temp,i,j;
    if(bitdepth<=8)
    {
        mid=mean_gray(buf);
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                temp=buf[i*width + j];
                if(temp<=mid)
                new_data[i*width + j] = 0;
                else
                new_data[i*width + j] = 255;
            }
        }
     }
     else
     {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                temp=buf[3*i*width + 3*j];
                if(temp<=mid)
                new_data[3*i*width + 3*j] = 0;
                else
                new_data[3*i*width + 3*j] = 255;

                temp=buf[3*i*width + 3*j + 1];
                if(temp<=mid)
                new_data[3*i*width + 3*j + 1] = 0;
                else
                new_data[3*i*width + 3*j + 1] = 255;

                temp=buf[3*i*width + 3*j + 2];
                if(temp<=mid)
                new_data[3*i*width + 3*j + 2] = 0;
                else
                new_data[3*i*width + 3*j + 2] = 255;
            }
        }
     }
     newcopy(header,colorTable,new_data);
}
void sepia(unsigned char* header,unsigned char* colorTable,unsigned char* buf)
{
    if(bitdepth<=8)
    {
        printf("!Error!\n");
        printf("Gray Scale image cannot be processed to this filter\n ");
    }
    else
    {
        int i,r,g,b,j,k;
        for(i=0;i<height*width;i++)
        {
            r = 0;
            g= 0;
            b = 0;
            r = (buf[3*i]*0.393) + (buf[1+3*i]*0.769)	+ (buf[2+3*i]*0.189);
            g = (buf[3*i]*0.349) + (buf[1+3*i]*0.686)	+ (buf[2+3*i]*0.168);
            b = (buf[3*i]*0.272) + (buf[1+3*i]*0.534)	+ (buf[2+3*i]*0.131);

            if(r > 255)
            {				//if value exceeds
                r = 255;
            }
            if(g > 255)
            {
                g = 255;
            }
            if(b > 255)
            {
                b = 255;
            }
            new_data[3*i] = r;
            new_data[3*i + 1] = g;
            new_data[3*i + 2] = b;
        }

    }
    newcopy(header,colorTable,new_data);
}
void translate(unsigned char* header,unsigned char* colorTable ,unsigned char* buf)
 {
    int t;
    int i,j;
    printf("Enter translation factor : ");///How much bright image you want
    scanf("%d",&t);
    if(bitdepth<=8)
    {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[i*width + j]=(buf[i*width + j] + t)%256;
            }
         }
     }
     else
     {
        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                new_data[3*i*width + 3*j]=buf[3*i*width + 3*j] + t;
                new_data[3*i*width + 3*j + 1]=buf[3*i*width + 3*j + 1] + t;
                new_data[3*i*width + 3*j + 2]=buf[3*i*width + 3*j + 2] + t;
            }
        }
     }
     newcopy(header,colorTable,new_data);
 }
void shrink_image(unsigned char* header, unsigned char* colorTable,unsigned char* buf)//works perfectly mainly on 512x512 images
{
    if(height!= width)
    {
        printf("\nThis process doesn't work for Rectangular Images\n");
    }
    int i,j;
    int s;///s=shrinking factor
    printf("Enter Shrinking factor(2 or 4 or 8) : ");
    scanf("%d",&s);
    int new_width=width/s;
    int new_height=height/s;
    unsigned char new_header[54];
    if(bitdepth<=8)
    {
        for(i=0;i<new_height;i++)
        {
            for(j=0;j<new_width;j++)
            {
                new_data[i*(new_width) + j] = buf[s*i*width + s*j];
            }
        }
    }
    else
    {
         for(i=0;i<new_height;i++)
        {
            for(j=0;j<new_width;j++)
            {
                new_data[3*i*(new_width) + 3*j] = buf[3*s*i*width + 3*s*j];
                new_data[3*i*(new_width) + 3*j + 1] = buf[3*s*i*width + 3*s*j + 1];
                new_data[3*i*(new_width) + 3*j + 2] = buf[3*s*i*width + 3*s*j + 2];
            }
        }
    }
    for(i=0;i<54;i++)
    {
        new_header[i]=header[i];
    }
    set_dimensions(new_header,new_width,new_height);
    height=new_height;
    width=new_width;
    newcopy(new_header,colorTable,new_data);
}
void set_dimensions(unsigned char* new_header, int new_width,int new_height)//enter height and width in the header
{
    new_header[18] = (new_width) - 256*(new_width >> 8);  ///information of new_height and new_width saved in new_header
    new_header[19] = (new_width >> 8) - 256*(new_width >> 16);
    new_header[20] = (new_width >> 16) - 256*(new_width >> 24);
    new_header[21] = (new_width >> 24);
    new_header[22] = (new_height) - 256*(new_height >> 8);
    new_header[23] = (new_height >> 8) - 256*(new_height >> 16);
    new_header[24] = (new_height >> 16) - 256*(new_height >> 24);
    new_header[25] = (new_height >> 24);
}
 ///Grayscale image donot have colortable
 ///A colored image with bitdepth <=8(that means image have less than 256 colors) will have a colortable for sure,each pixel will be of 1byte and will contiain an integer(0-255) corresponding to index in colortable
 ///in a colored image with bitdepth=24bits,every pixel value takes 3bytes of memory 1byte each for shades of red,blue and green
 ///Resolution is pixel per inch(DPI as dots per inch)
