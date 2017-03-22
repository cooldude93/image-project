//arg 1:input file;arg 2 output file; arg 3 width arg; 4 height  
#include <stdio.h>
#include <stdlib.h>

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
	//TODO these are within normal uint size ranges
	uint width = strtoul(*(argv+3),NULL,10);
	uint height = strtoul(*(argv+4),NULL,10);
	printf("%i, %i\n",width, height);
	uint framesize = width*height;

	uint framesize = width*height<<3;

	char* buf = malloc(fsize+1);
	fread(buf,fsize,1,inp);


	fclose(inp);
	//try to read output file
	//inject byte array
	//catch 
	//create file and dump characters


}