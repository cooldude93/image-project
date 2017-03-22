//arg 1:	input file
//arg 2: 	output file 
//arg 3: 	width
//arg 4: 	height  
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
static unsigned char * b2;
static uint loop_length;
static bool loop;
struct color{
	char red;
	char green;
	char blue;
}color;

struct color * read_color_table(uint);
void main(int argc, char **argv){
	//read in bitmap
	FILE *inp = fopen(*(argv+1),"rb");
	if(!inp){
		fprintf(stderr,"file %s does not exist \n",*(argv+1));
		return;
	}
	fseek(inp,0,SEEK_END);
	long fsize = ftell(inp);
	fseek(inp,0,SEEK_SET);
	unsigned char* buf = malloc(fsize+1);
	*(buf+fsize)='\0';
	fread(buf,fsize,1,inp);
	printf("sizeF %x\n",fsize);
	/*
	for(int j =0;j<152;j++){
		for(int i =0;i<30;i++)
			if(*buf!=0x21)
				printf(KRED " %2x" KNRM,*buf++);
				else
			printf(KNRM" %2x",*buf++);

	printf("\n");
	}
	*/
	//strcmp would do but this is like, minimally faster I think 
	if(*buf!='G'||*(buf+1)!='I'||*(buf+2)!='F'){
		fprintf(stderr,"file %s is not a GIF \n",*(argv+1));
		return;
	}

	unsigned char version[3] = {*(buf+3),*(buf+4),*(buf+5)};


	uint canvas_wdt = *(buf+7)+(*(buf+8))<<8;
	uint canvas_hgt = *(buf+9)+(*(buf+10))<<8;

	//global color table flag
	// |
	// | color resolution per channel - 1 e.g 111 = 8bits per color channel, 000= 1bit etc
	// | | | | 
	// | | | | sort flag for global color table 1 = decreasing importance
	// | | | | |	
	// | | | | | exponent -1 for the size of the global color table e.g. gct_size = 2^(X+1) where X = these three bits
	// | | | | | | | |
	// 0 0 0 0 0 0 0 0

	unsigned char hdr_pck_fld = *(buf+10);
	//bool
	printf("packed field:%x\n",hdr_pck_fld,hdr_pck_fld);

	unsigned char color_table_flag = hdr_pck_fld>>7;
	uint color_res = (hdr_pck_fld>>4)&0b00000111;
	//bool
	unsigned char sort_flag = (hdr_pck_fld&(1<<3))!=0;
	uint exp = hdr_pck_fld&0b00000111;
	uint size = 2<<exp;//three color channels

	unsigned char bg_color = *(buf+11);
	unsigned char pixel_ratio = *(buf+12);
	printf("color tab flag:%i\ncolor_resolution:%i\nsorted:%i\nexponent:%i\nbg:%xpixel ratio:%x\n",color_table_flag,color_res,sort_flag,exp,bg_color,pixel_ratio);

	struct color *color_table;
	 b2 = buf + 13;
	//buf += 13;
	if(color_table_flag){
		color_table = read_color_table(size);
		printf("global tab size %i\n",size);
		
	}
	
	//graphic extension variables
	char ext_pck_fld;
	unsigned short delay_time;
	char transp_color_index;

	//image descriptor variables
	unsigned short img_left,img_top,img_wdt,img_hgt;
	unsigned char desc_pck_fld;

	struct color * local_color_table;

	unsigned char min_code_size;
	unsigned char data_block_size;
	int i,a;

	while(b2-buf<=fsize){
		printf("loop pointer:%2x, pos:%x",*b2,b2-buf);
		switch(*b2++){
			//extension
			case 0x21:
				switch(*b2++){
					case 0xf9:
					printf("graphic control extension\n");
					data_block_size=*b2++;//04 bytes in a gce
					ext_pck_fld=*b2++;
					delay_time=*b2++; 
					delay_time+=(*b2++)<<8;//assuming little endian
					transp_color_index = *b2++;
					b2++;//00 terminator
					break;
					//application extension 
					case 255:
					printf("application extension\n");
					data_block_size = *b2++;
					char netscape[11]="NETSCAPE2.0";
					bool unk = false;
					for(i=0;i<data_block_size;i++){
						printf("%c",*b2);
						if(*b2++!=netscape[i]){
							if(!unk)
							printf("unknown application extension\n");
							unk = true;
						}

					}
					printf("\n");
					if(unk){
						data_block_size=*b2++;
						while(data_block_size!=0)
						for(i=0;i<data_block_size;i++){
							printf("%c",*b2++);
						}
						data_block_size=*b2++;
						printf("\n");

					}
					else{
						b2++;//03 for anim
						b2++;//01 for anim
						loop = true;
						loop_length=*b2++;loop_length+=(*b2++)<<7;
						b2++;//00 block terminator
					}
					printf("loop length:%i\n",loop_length);
					break;
					//comment extension
					case 254:
					data_block_size =*b2++;
					while(data_block_size!=0){
						for(i=0;i<data_block_size;i++){
								printf("%c",*b2++);
						}
					data_block_size=*b2++;
					}
					printf("\n*******end comment*********\n");
					break;
					//plaintext extension
					case 1:
					b2+=*b2;
					break;
					default:
					printf("unknown extension\n");
				}
			break;
			//image descriptor
			case 0x2c:
			printf("image descriptor\n");
			img_left = *b2++; img_left+=(*b2++)<<8;
			img_top = *b2++; img_top+=(*b2++)<<8;
			img_wdt = *b2++; img_wdt+=(*b2++)<<8;
			img_hgt = *b2++; img_hgt+=(*b2++)<<8;

			desc_pck_fld = *b2++;
			printf("img_left:%i\n",img_left);
			printf("img_top:%i\n",img_top);
			printf("img_wdt:%i\n",img_wdt);
			printf("img_hgt:%i\n",img_hgt);
			//local color table
			if(desc_pck_fld>>7){
				exp = hdr_pck_fld&0b00000111;
				size = 4<<exp;
				local_color_table = read_color_table(size);
			}else
				local_color_table = color_table;
			break;

			//image data
			default:
				printf("image data\n");
				b2--;
				min_code_size=*b2++;
				data_block_size=*b2++;
				printf("min_code_size:%x\n",min_code_size);
				long image_block_size=data_block_size;
				char *block=b2;
				int min_size = 2<<min_code_size;
				while(data_block_size!=0){
					printf("block_size:%2x\n",data_block_size);
					for(int k = 0; k<data_block_size;k++){
						printf("%c",*b2++);
					}
					data_block_size=*b2++;
					
					image_block_size+=data_block_size;
				}
				printf("\nendofimagedata\n");
				printf("img_size:%i",image_block_size);
				break;
			
		}
	}
	printf("\nendchar:%2x\npos:%2x",*(--b2),b2-buf);

	//TODO these are within normal uint size ranges
	//uint outp_wdt = strtoul(*(argv+3),NULL,10);
	//uint outp_hgt = strtoul(*(argv+4),NULL,10);
	uint gw,gh;
	uint frames;

	//uint framesize = width*height;

	free(buf);
	fclose(inp);
	//try to read output file
	//inject byte array
	//catch 
	//create file and dump characters
	
}
struct color * read_color_table(uint size){
	struct color *color_table = malloc(size*3);
	printf("size %i\n",size);
	for(int i =0;i<size;i++){
		//printf("R:%hhu G:%hhu B:%hhu\n",*(b2),*(b2+1),*(b2+2));
		(*(color_table+i)).red=*(b2++);
		(*(color_table+i)).blue=*(b2++);
		(*(color_table+i)).green=*(b2++);
	}
	return color_table;//color_table; 
}
