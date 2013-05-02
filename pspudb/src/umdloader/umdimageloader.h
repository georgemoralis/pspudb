// Copyright (c) 2013- PSPUDB Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/georgemoralis/pspudb and http://www.pspudb.com/

//complessed ISO(9660) header format
typedef struct ciso_header
{
	unsigned char magic[4];			// +00 : 'C','I','S','O'                 
	unsigned long header_size;		// +04 : header size (==0x18)            
	unsigned long long total_bytes;	// +08 : number of original data size    
	unsigned long block_size;		// +10 : number of compressed block size 
	unsigned char ver;				// +14 : version 01                      
	unsigned char align;			// +15 : align of index value            
	unsigned char rsv_06[2];		// +16 : reserved                        
} CISO_H;


namespace umdimageloader
{
  void reboot(const char *filename);
  void load_iso(const char *filename);
  void load_cso(const char *filename);
  int read_block(u8 *outPtr,int blockNumber,int size);
  int read_iso_block(u8 *outPtr,int blockNumber,int size);
  int read_cso_block(u8 *outPtr,int blockNumber,int size);
  void shutdown();
};