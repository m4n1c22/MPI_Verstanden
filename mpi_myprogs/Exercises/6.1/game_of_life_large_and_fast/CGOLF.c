#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
#include "CGOLF.h"

int CGOLF_Init(CGOLF * cgolf,int fileBlockSize)
{
	cgolf->mode=0;
	cgolf->file=NULL;
	cgolf->filePath=NULL;
	cgolf->blockSize=fileBlockSize;
	/* init internal buffer */
	cgolf->buffer=(unsigned char*)malloc(sizeof(char)*cgolf->blockSize);
}

int CGOLF_in_open(CGOLF * cgolf, char * fileName)
{
	/* Mode must be 0 for this operation */
	if (cgolf->mode!=0)
	{
		fprintf("Invalid Operation: Must be in close state. Mode=%i\n",cgolf->mode);
		exit(-1);
		return -1;
	}

	cgolf->mode=2;
	cgolf->filePath=fileName;
	cgolf->file=NULL;
	cgolf->file=fopen(fileName,"r");
	if (feof(cgolf->file)) fprintf(stderr,"FEOF!!\n");
	int error;
	if (error=ferror(cgolf->file)) fprintf(stderr,"FERROR!!\n");
//	perror();
	if (cgolf->file==NULL) 
	{
		fprintf(stderr,"fopen(\"%s\",\"r\") failed\n",fileName);
		exit(-1);
	}
	cgolf->strm.zalloc = Z_NULL;
	cgolf->strm.zfree = Z_NULL;
	cgolf->strm.opaque = Z_NULL;
	cgolf->strm.avail_in = 0;
	cgolf->strm.next_in = cgolf->buffer;
	int ret=inflateInit(&(cgolf->strm));
	if (ret != Z_OK)
	{
		fprintf(stderr,"inflateInit failed\n");
		return ret;
	}

}

int CGOLF_out_open(CGOLF * cgolf,char * fileName)
{
	if (cgolf->mode!=0)
		return -1;
	/* set to write mode */
	cgolf->mode=1;
	cgolf->filePath=fileName;
	cgolf->file=fopen(fileName,"w");
	/* set compression flags */
	cgolf->strm.zalloc = Z_NULL;
	cgolf->strm.zfree = Z_NULL;
	cgolf->strm.opaque = Z_NULL;
	int ret=deflateInit(&(cgolf->strm),Z_DEFAULT_COMPRESSION);
	if (ret!=Z_OK)
		return ret;

	/* set buffer for stream */
	cgolf->strm.avail_in=0;
	cgolf->strm.next_out=cgolf->buffer;
	cgolf->strm.avail_out=cgolf->blockSize;
	cgolf->strm.next_out=cgolf->buffer;
}

int CGOLF_in_read(CGOLF * cgolf,int nElements,unsigned char * elements)
{
	/* setup output buffers */
	int filled=0;
	int dataRead=0;
	int ret;
	do
	{
		/* setup request: try to read the missing elements */
		cgolf->strm.avail_out=nElements-filled;
		cgolf->strm.next_out=elements+filled;
		/* if the input buffer is empty and the file is not done */
		if (cgolf->strm.avail_in==0 && !feof(cgolf->file))
		{
			cgolf->strm.avail_in=fread(cgolf->buffer,1,cgolf->blockSize,cgolf->file);
			cgolf->strm.next_in=cgolf->buffer;
				
			if (cgolf->strm.avail_in<1*cgolf->blockSize && feof(cgolf->file))
				fprintf(stderr,"FEOF!!\n");
			dataRead=1;
		}
		else
		{
			dataRead=0;
		}
		/* try to satisfy request */	
		ret=inflate(&(cgolf->strm),Z_NO_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
		switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&(cgolf->strm));
				return ret;
		}
		/* how much data was written */
		filled=nElements-cgolf->strm.avail_out;
	}while(ret==Z_OK && (filled!=nElements || dataRead));
	
	if (filled!=nElements)
	{
		fprintf(stderr,"Not Sufficient Data\n");
		exit(-1);
		return -1;
	}		
	/* read from the filw until either we have enough for the elements or until eof */
}

int CGOLF_in_close(CGOLF * cgolf)
{
	int ret=inflateEnd(&(cgolf->strm));
	fclose(cgolf->file);
	cgolf->mode=0;
}

int CGOLF_out_write(CGOLF * cgolf,int nElements,unsigned char * elements)
{
	char * buffer=cgolf->buffer;
	int bufferSize=cgolf->blockSize;
	
	cgolf->strm.avail_in=nElements;
	cgolf->strm.next_in=elements;
	/* deflate = compress now */

	int ret;
	while(cgolf->strm.avail_in>0)
	{
		/* compress next block */
		ret=deflate(&(cgolf->strm),Z_NO_FLUSH);	
		if (ret != Z_OK)
			fprintf(stderr,"Deflate failed\n");	
		/* is the out buffer full ? */	
		if (ret == Z_OK && cgolf->strm.avail_out == 0)
		{
			fwrite(buffer,1,cgolf->blockSize,cgolf->file);
			/* reset the available space */
			cgolf->strm.avail_out=cgolf->blockSize;
			cgolf->strm.next_out=cgolf->buffer;
		}
	}
	
}
int CGOLF_out_close(CGOLF * cgolf)
{
	/* we have to flush the output */
	cgolf->strm.avail_in=0;
	unsigned remaining;
	int remainingBits;
	int ret;
	/* as long as the out buffer is not completely free */
	int dataLeft=1;
	ret=deflate(&(cgolf->strm),Z_FULL_FLUSH);
	dataLeft=cgolf->strm.avail_out< cgolf->blockSize;
	while(ret == Z_OK && dataLeft){
		if (ret != Z_OK)
            printf("Deflate failed\n");
		
        /* is the out buffer full ? */
        fwrite(cgolf->buffer,1,cgolf->blockSize,cgolf->file);
		if (cgolf->strm.avail_out==0)
		{
            /* reset the available space */
            cgolf->strm.avail_out=cgolf->blockSize;
			cgolf->strm.next_out=cgolf->buffer;
			/* there may be data left */
			ret=deflate(&(cgolf->strm),Z_FULL_FLUSH);
        }	
		else
		{
			/* Buffer was not fully used, therefore we are done after writing it! */
			dataLeft=0;
			
		}
	}
	deflateEnd(&(cgolf->strm));
	ret=fclose(cgolf->file);	
	if (ret!=0)
	{
		fprintf(stderr,"Fclose failed\n");
	}
	cgolf->mode=0;
	if (cgolf->mode!=1)
		return -1;
}

