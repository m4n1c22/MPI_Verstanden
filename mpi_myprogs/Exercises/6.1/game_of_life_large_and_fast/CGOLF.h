#include <stdio.h>
#include "zlib.h"

typedef struct CGOLF
{
	int mode;
	z_stream strm;
	FILE * file;
	char * filePath;
	int blockSize;
	unsigned char * buffer;
}CGOLF; /* Compressed Game Of Life File */

