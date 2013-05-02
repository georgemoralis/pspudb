#include <string.h>
#include <stdio.h>
#include "types.h"

#include "isolib.h"
#include "iso.h"
#include "isodrv.h"

#include "umdimageloader.h"


struct fdtable{
//int fd;
	int fileSize;
	int LBA;
	int filePos;
};

static struct fdtable fd_table[16];
static int fd_used[16];
static int files_open=0;
static bool inited=false;

#ifdef min
#undef min
#endif
#define min(x,y) (((x)<(y))?(x):(y))

void ISOFS_init()
{
	memset(fd_table, 0, sizeof(fd_table));
	memset(fd_used, 0, 16*sizeof(int));

	inited = true;

	return;
}

int ISOFS_open(char *name, int mode)
{
	int j;
	static struct TocEntry tocEntry;

	if(!inited) ISOFS_init();

	// check if the file exists
   if (!ISOFS_findfile(name, &tocEntry))
   {

      /* if(memcmp(name,"sce_lbn",7)==0) //TODO check it and maybe add more checks??
       {
            char *idx0 = name+7;
            char *idx1 = strchr(name+7,'_');
            int len1 = idx1-idx0;
            if(memcmp(idx1,"_size",5)==0)
            {
                char* idx2 = idx1+5;
                int len2 = strlen(idx2);
                int lba  = strtoul(idx0,NULL,16);
                int size = strtoul(idx2,NULL,16);
                // make up a fake tocEntry
                tocEntry.fileLBA = lba;
                tocEntry.fileSize = size;
            }
            else return -1;
	   }
       else */
	   
		   return -1;

    }

	if(mode != 1) 
	{
		
		return -2;	//SCE_RDONLY
	}

	// set up a new file descriptor
	for(j=0; j < 16; j++) if(fd_used[j] == 0) break;
	if(j >= 16) return -3;

	fd_used[j] = 1;
	files_open++;

	fd_table[j].fileSize = tocEntry.fileSize;
	fd_table[j].LBA = tocEntry.fileLBA;
	fd_table[j].filePos = 0;

   	return j;
}

int ISOFS_lseek(int fd, int offset, int whence)
{
	if ((fd >= 16) || (fd_used[fd]==0))
	{
		return -1;
	}

	switch(whence)
	{
	case SEEK_SET:
		fd_table[fd].filePos = offset;
		break;

	case SEEK_CUR:
		fd_table[fd].filePos += offset;
		break;

	case SEEK_END:
		fd_table[fd].filePos = fd_table[fd].fileSize + offset;
		break;

	default:
		return -1;
	}

	if (fd_table[fd].filePos < 0)
		fd_table[fd].filePos = 0;

	if (fd_table[fd].filePos > fd_table[fd].fileSize)
		fd_table[fd].filePos = fd_table[fd].fileSize;

	return fd_table[fd].filePos;
}

int ISOFS_read( int fd, char *buffer, int size )
{
	int off_sector;

	static char lb[2048];					//2KB

	//Start, Aligned, End
	int  ssector, asector, esector;
	int  ssize=0, asize,   esize;

	if ((fd >= 16) || (fd_used[fd]==0))
	{
		
		return -1;
	}

	// A few sanity checks
	if (fd_table[fd].filePos > fd_table[fd].fileSize)
	{
		// We cant start reading from past the end of the file
		
		return 0;	// File exists but we couldnt read anything from it
	}

	if ((fd_table[fd].filePos + size) > fd_table[fd].fileSize)
		size = fd_table[fd].fileSize - fd_table[fd].filePos;

	// Now work out where we want to start reading from
	asector = ssector = fd_table[fd].LBA + (fd_table[fd].filePos >> 11);
	off_sector = (fd_table[fd].filePos & 0x7FF);
	if (off_sector)
	{
		ssize   = min(2048 - off_sector, size);
		size   -= ssize;
		asector++;
	}
	asize = size & 0xFFFFF800;
	esize = size & 0x000007FF;
	esector=asector + (asize >> 11);
	size += ssize;

	if (ssize)
	{
		if (umdimageloader::read_block((u8*)lb, ssector,1) < 0)
		{
			return 0;
		}
		memcpy(buffer, lb + off_sector, ssize);
	}
	if (asize)
	{
        if (umdimageloader::read_block((u8*)buffer+ssize,asector, asize >> 11)<0)
		{
             return 0;
		}
	}
	if (esize)
	{
		if (umdimageloader::read_block((u8*)lb, esector,1) < 0)
		{
			return 0;
		}
		memcpy(buffer+ssize+asize, lb, esize);
	}

	fd_table[fd].filePos += size;

	return (size);
}

int ISOFS_write( int fd, char * buffer, int size )
{
   if(size == 0) return 0;
   else 		 return -1;
}

int ISOFS_close( int fd)
{
	if ((fd >= 16) || (fd_used[fd]==0)){
		return -1;
	}

	fd_used[fd] = 0;
	files_open--;

    return 0;
}
u32 ISOFS_getfilesize(char *name)
{
	TocEntry tocEntry;
	if (ISOFS_findfile(name, &tocEntry) != true){
		return 0;
	}
	return tocEntry.fileSize;

}

