/*
This file is part of pcsp.

pcsp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

pcsp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pcsp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "types.h"
#include "umdimageloader.h"
#include <cstdio>
#include <cstring>

#include "ext\zlib\include\zlib.h"


FILE *f;
int umdloader_imagetype;
unsigned int filesize;
enum ImageTypes
{
   PSP_ISO,
   PSP_CSO
};
void umdimageloader::reboot(const char *filename)
{
	FILE *temp = fopen(filename,"rb");
	u32 id;
	fread(&id,4,1,temp);
	fclose(temp);
	if(id == 0x4f534943) //'CISO'
	{
       umdloader_imagetype=PSP_CSO;
	}
	else
	{
       umdloader_imagetype=PSP_ISO;
	}

    if(umdloader_imagetype==PSP_CSO)
	{
         load_cso(filename);
	}
	else 
	{
         load_iso(filename);
	}
  
}
void umdimageloader::load_iso(const char *filename)
{
   	f = fopen(filename, "rb");
	fseek(f,0,SEEK_END);
	filesize = ftell(f);
	fseek(f,0,SEEK_SET);
}
//cso specific
u32 *csoIndex;
int indexShift;
u32 blockSize;
int numBlocks;

void umdimageloader::load_cso(const char *filename)
{
   	f = fopen(filename, "rb");
	CISO_H hdr;
	fread(&hdr, 1, sizeof(CISO_H), f);
	if (memcmp(hdr.magic, "CISO", 4) != 0)
	{
		//warnf(isoFileSystem,"CSO header doesn't appear valid");
	}
	else
	{
		//infof(isoFileSystem,"CSO header valid continue normal");
	}
	if (hdr.ver > 1)
	{
		//warnf(isoFileSystem,"CSO version >1");
	}

	int hdrSize = hdr.header_size;
	blockSize = hdr.block_size;
	if (blockSize != 0x800)
	{
		//errorf(isoFileSystem,"Unsupported blockSize! (!=0x800)");
	}
	indexShift = hdr.align;
	u64 totalSize = hdr.total_bytes;
	numBlocks = (int)(totalSize / blockSize);
//	infof(isoFileSystem,"CSO hdrSize=%i numBlocks=%i align=%i", hdrSize, numBlocks, indexShift);

	int indexSize = numBlocks + 1;

	csoIndex = new u32[indexSize];
	fread(csoIndex, 4, indexSize, f);
}
int umdimageloader::read_block(u8 *outPtr,int blockNumber,int size)
{
    if(umdloader_imagetype==PSP_CSO)
	{
         return read_cso_block(outPtr,blockNumber,size);
	}
	else 
	{
         return read_iso_block(outPtr,blockNumber,size);
	}
}
int umdimageloader::read_iso_block(u8 *outPtr,int blockNumber,int size)
{
	fseek(f, blockNumber * 2048, SEEK_SET);
	fread(outPtr, 2048, size, f);
	return 1;
	
}
u8 inbuffer[4096]; //too big
z_stream z;
u8 inbuffer2[4096]; //too big
int umdimageloader::read_cso_block(u8 *outPtr,int blockNumber,int size)
{
	while(size>0)
	{
		u32 idx = csoIndex[blockNumber];
		u32 idx2 = csoIndex[blockNumber+1];

		int plain = idx & 0x80000000;

		idx = (idx & 0x7FFFFFFF) << indexShift;
		idx2 = (idx2 & 0x7FFFFFFF) << indexShift;

		u32 compressedReadPos = idx;
		u32 compressedReadSize = idx2 - idx;

		if (plain)
		{
			fseek(f, compressedReadPos, SEEK_SET);
			fread(outPtr, compressedReadSize, 1, f);

			if(compressedReadSize<2048)
				memset(outPtr+compressedReadSize, 0, 2048-compressedReadSize);
		}
		else
		{
			fseek(f, compressedReadPos, SEEK_SET);
			fread(inbuffer, compressedReadSize, 1, f);

			memset(outPtr, 0, 2048);
			z.zalloc = Z_NULL;
			z.zfree = Z_NULL;
			z.opaque = Z_NULL;
			if(inflateInit2(&z, -15) != Z_OK)
			{
//				errorf(isoFileSystem,"deflateInit : %s\n", (z.msg) ? z.msg : "???");
				return 1;
			}
			z.avail_in = compressedReadSize;
			z.next_out = outPtr;
			z.avail_out = blockSize;
			z.next_in = inbuffer;

			int status = inflate(&z, Z_FULL_FLUSH);
			if(status != Z_STREAM_END)
				//if (status != Z_OK)
			{
//				errorf(isoFileSystem,"block %d:inflate : %s[%d]\n", blockNumber, (z.msg) ? z.msg : "error", status);
				return 1;
			}
			u32 cmp_size = blockSize - z.avail_out;
			if(cmp_size != blockSize)
			{
//				errorf(isoFileSystem,"block %d : block size error %d != %d\n", blockNumber, cmp_size, blockSize);
				return 1;
			}

			if (inflateEnd(&z) != Z_OK)
			{
			//	errorf(isoFileSystem,"cannot end inflation");
			}
		}

		outPtr+=2048;
		blockNumber++;
		size--;
	}
   return 1;
}
void umdimageloader::shutdown()
{
	if(f!=NULL)
        fclose(f);//close image file
  if(umdloader_imagetype==PSP_CSO) 
	  delete [] csoIndex;
}
