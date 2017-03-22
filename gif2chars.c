//arg 1:	input file
//arg 2: 	output file 
//arg 3: 	width
//arg 4: 	height  
#include <stdio.h>
#include <stdlib.h>
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

struct color{
	char red;
	char green;
	char blue;
}color;

struct color * read_color_table(char*,uint);
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
	fread(buf,fsize,1,inp);
	printf("sizeF %l",fsize);
	
	for(int j =0;j<152;j++){
		for(int i =0;i<30;i++)
			if(*buf!=0x21)
				printf(KRED " %2x" KNRM,*buf++);
				else
			printf(KNRM" %2x",*buf++);

	printf("\n");
	}

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

	unsigned char color_table_flag = hdr_pck_fld>>7;
	uint color_res = (hdr_pck_fld>>4)&0b00000111;
	//bool
	unsigned char sort_flag = hdr_pck_fld&(1<<3)!=0;
	uint exp = hdr_pck_fld&0b00000111;
	uint size = 4<<exp;//three color channels

	unsigned char bg_color = *(buf+11);
	unsigned char pixel_ratio = *(buf+12);


	struct color *color_table;
	unsigned char *b2 = buf + 13;

	if(color_table_flag){
		color_table = read_color_table(b2,size);
		printf("global tab size %i\n",size);
	}
	/*
	while(*b2){
		for(int i =0;i<30;i++)
			if(*b2==0x21)
				printf(KRED " %2x" KNRM,*b2++);
				else
		printf(" %2x",*b2++);

	printf("\n");
	}
	*/

	//graphic extension variables
	char block_size,ext_pck_fld;
	unsigned short delay_time;
	char transp_color_index;

	//image descriptor variables
	unsigned short img_left,img_top,img_wdt,img_hgt;
	unsigned char desc_pck_fld;

	struct color * local_color_table;

	unsigned char min_code_size;
	unsigned short img_block_size;
	while(*b2!=0x3b){
		//printf("%2x\n",*b2);
		switch(*b2++){
			//extension
			case 33:
				switch(*b2++){
				printf("a\n");
					//graphic extension (e.g. animation)
					case 249:
						block_size=*b2++;
						ext_pck_fld=*b2++;
						delay_time=*b2++; delay_time+=(*b2++)<<8;//assuming little endian
						transp_color_index = *b2++;
						b2++;
						printf("a\n");
					break;
					//application extension 
					case 255:
					break;
					//comment extension
					case 254:
					break;
					//plaintext extension
					case 1:
					b2+=*b2;
					break;
				}
			break;
			//image descriptor
			case 44:
			img_left = *b2++; img_left+=(*b2++)<<8;
			img_top = *b2++; img_top+=(*b2++)<<8;
			img_wdt = *b2++; img_wdt+=(*b2++)<<8;
			img_hgt = *b2++; img_hgt+=(*b2++)<<8;
			desc_pck_fld = *b2++;
			//local color table
			if(desc_pck_fld>>7){
				exp = hdr_pck_fld&0b00000111;
				size = 4<<exp;
				local_color_table = read_color_table(b2,size);
			}else
				local_color_table = color_table;
			break;

			//image data
			default:
				min_code_size=*b2++;
				data_block_size=*b2++;
				while(*b2!=0){
					*b2++;
					

					break;
				}
			break;
			
		}
	}


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
struct color * read_color_table(char * buf,uint size){
	struct color *color_table = malloc(size*3);
	printf("size %i\n",size);
	for(int i =0;i<size;i++){
		//printf("R:%hhu G:%hhu B:%hhu\n",*(buf),*(buf+1),*(buf+2));
		(*(color_table+i)).red=*(buf++);
		(*(color_table+i)).blue=*(buf++);
		(*(color_table+i)).green=*(buf++);
	}
	return color_table;//color_table; 
}
