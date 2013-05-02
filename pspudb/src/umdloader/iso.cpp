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

#include "types.h"
#include <string.h>
#include "umdimageloader.h"
#include "iso.h"
#include "isodrv.h"

struct dir_toc_data{
	unsigned int start_LBA;
	unsigned int num_sectors;
	unsigned int num_entries;
	unsigned int current_entry;
	unsigned int current_sector;
	unsigned int current_sector_offset;
	unsigned int inc_dirs;
	unsigned char extension_list[128+1];
};

//static u8 cdVolDescriptor[2048];
static struct dir_toc_data getDirTocData;
static struct cdVolDesc CDVolDesc;

extern s32 cdvdDirectReadSector(s32 first, s32 mode, char *buffer);

void _splitpath2(const char *constpath, char *dir, char *fname)
{
	// 255 char max path-length is an ISO9660 restriction
	// we must change this for Joliet or relaxed iso restriction support
	static char pathcopy[1024+1];

	char* slash;

	strncpy(pathcopy, constpath, 1024);
	pathcopy[1024]=0;

    slash = strrchr (pathcopy, '/');

	// if the path doesn't contain a '/' then look for a '\'
    if (!slash)
		slash = strrchr (pathcopy,  (int)'\\');

	// if a slash was found
	if (slash != NULL)
	{
		// null terminate the path
		slash[0] = 0;
		// and copy the path into 'dir'
		strncpy(dir, pathcopy, 1024);
		dir[255]=0;

		// copy the filename into 'fname'
		strncpy(fname, slash+1, 128);
		fname[128]=0;
	}
	else
	{
		dir[0] = 0;

		strncpy(fname, pathcopy, 128);
		fname[128]=0;
	}

}

// Copy a TOC Entry from the CD native format to our tidier format
void TocEntryCopy(struct TocEntry* tocEntry, struct dirTocEntry* internalTocEntry){
	int i;
	int filenamelen;

	tocEntry->fileSize = internalTocEntry->fileSize;
	tocEntry->fileLBA = internalTocEntry->fileLBA;
	tocEntry->fileProperties = internalTocEntry->fileProperties;
	memcpy(tocEntry->date, internalTocEntry->dateStamp, 6);
	tocEntry->date[6]=0;

	if (CDVolDesc.filesystemType == 2){
		// This is a Joliet Filesystem, so use Unicode to ISO string copy

		filenamelen = internalTocEntry->filenameLength/2;

		if (!(tocEntry->fileProperties & 0x02)){
			// strip the ;1 from the filename
//			filenamelen -= 2;//(Florin) nah, do not strip ;1
		}

		for (i=0; i < filenamelen; i++)
			tocEntry->filename[i] = internalTocEntry->filename[(i<<1)+1];

		tocEntry->filename[filenamelen] = 0;
	}
	else{
		filenamelen = internalTocEntry->filenameLength;

		if (!(tocEntry->fileProperties & 0x02)){
			// strip the ;1 from the filename
//			filenamelen -= 2;//(Florin) nah, do not strip ;1
		}

		// use normal string copy
		strncpy((char*)tocEntry->filename,(char*)internalTocEntry->filename,128);
		tocEntry->filename[filenamelen] = 0;
	}
}

// Check if a TOC Entry matches our extension list
bool TocEntryCompare(char* filename, char* extensions)
{
	static char ext_list[129];

	char* token;

	char* ext_point;

	strncpy(ext_list,extensions,128);
	ext_list[128]=0;

	token = strtok( ext_list, " ," );
	while( token != NULL )
	{
		// if 'token' matches extension of 'filename'
		// then return a match
		ext_point = strrchr(filename,'.');

		if (strnicmp(ext_point, token, strlen(token)) == 0)
			return true;

		/* Get next token: */
		token = strtok( NULL, " ," );
	}

	// If not match found then return FALSE
	return false;

}

/**************************************************************
* The functions below are not exported for normal file-system *
* operations, but are used by the file-system operations, and *
* may also be exported  for use via RPC                       *
**************************************************************/

bool ISOFS_GetVolumeDescriptor(void)
{
	// Read until we find the last valid Volume Descriptor
	int volDescSector;

	static struct cdVolDesc localVolDesc;

	for (volDescSector = 16; volDescSector<20; volDescSector++)
	{
		umdimageloader::read_block((u8*)&localVolDesc,volDescSector,1);

		// If this is still a volume Descriptor
		if (strncmp((char*)localVolDesc.volID, "CD001", 5) == 0)
		{
			if ((localVolDesc.filesystemType == 1) ||
				(localVolDesc.filesystemType == 2))
			{
				memcpy(&CDVolDesc, &localVolDesc, sizeof(struct cdVolDesc));
			}
		}
		else
			break;
	}

	return true;
}

bool ISOFS_findfile(char* fname, struct TocEntry* tocEntry)
{
	static char filename[128+1];
	static char pathname[1024+1];
	static char toc[2048];
	char* dirname;

	static struct TocEntry localTocEntry;	// used for internal checking only

	bool found_dir;

	int num_dir_sectors;
	int current_sector;

	int dir_lba;

	struct dirTocEntry* tocEntryPointer;

	_splitpath2(fname, pathname, filename);

	// Find the TOC for a specific directory
	if (!ISOFS_GetVolumeDescriptor())
	{
		return false;
	}

	// Read the TOC of the root directory
	if (umdimageloader::read_block((u8*)toc,CDVolDesc.rootToc.tocLBA,1) < 0)
	{
		return false;
	}

	// point the tocEntryPointer at the first real toc entry
	tocEntryPointer = (dirTocEntry*)toc;

	num_dir_sectors = (tocEntryPointer->fileSize+2047) >> 11;	//round up fix
	current_sector = tocEntryPointer->fileLBA;

	tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);
	tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);


	localTocEntry.fileLBA = CDVolDesc.rootToc.tocLBA;
	// while (there are more dir names in the path)
	dirname = strtok( pathname, "\\/" );

	while( dirname != NULL )
	{
		found_dir = false;

		while(1)
		{
			if ((tocEntryPointer->length == 0) || (((char*)tocEntryPointer-toc)>=2048))
			{
				num_dir_sectors--;

				if (num_dir_sectors > 0)
				{
					// If we've run out of entries, but arent on the last sector
					// then load another sector

					current_sector++;
					if (umdimageloader::read_block((u8*)toc,current_sector,1) < 0)
					{
						return false;
					}
					tocEntryPointer = (dirTocEntry*)toc;
				}
				else
				{
					// Couldnt find the directory, and got to end of directory
					return false;
				}
			}


			if (tocEntryPointer->fileProperties & 0x02)
			{
				TocEntryCopy(&localTocEntry, tocEntryPointer);

				// If this TOC Entry is a directory,
				// then see if it has the right name
				if (strcmp(dirname,(char*)localTocEntry.filename) == 0)
				{
					// if the name matches then we've found the directory
					found_dir = true;
					break;
				}
			}

			// point to the next entry
			tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);
		}

		// If we havent found the directory name we wanted then fail
		if (!found_dir)
		{
			return false;
		}

		// Get next directory name
		dirname = strtok( NULL, "\\/" );

		// Read the TOC of the found subdirectory
		if (umdimageloader::read_block((u8*)toc,localTocEntry.fileLBA,1) < 0)
		{
			return false;
		}

		num_dir_sectors = (localTocEntry.fileSize+2047) >> 11;	//round up fix
		current_sector = localTocEntry.fileLBA;

		// and point the tocEntryPointer at the first real toc entry
		tocEntryPointer = (dirTocEntry*)toc;
		tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);
		tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);
	}

	tocEntryPointer = (dirTocEntry*)toc;

	num_dir_sectors = (tocEntryPointer->fileSize+2047) >> 11;	//round up fix
	dir_lba = tocEntryPointer->fileLBA;

	tocEntryPointer = (dirTocEntry*)toc;
	tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);
	tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);

	int filesize	= strlen(filename);
	bool match1		= (filename[filesize-2] == ';');

	while (num_dir_sectors > 0)
	{
		while(tocEntryPointer->length != 0)
		{
			// Copy the CD format TOC Entry to our format
			TocEntryCopy(&localTocEntry, tocEntryPointer);

			int local_toc_filesize	= strlen((char*)localTocEntry.filename);

			bool same_file	= (strnicmp((char*)localTocEntry.filename, filename, strlen(filename)) == 0);
			bool match2		= (localTocEntry.filename[local_toc_filesize-2] == ';');
			bool match3		= (strnicmp((char*)localTocEntry.filename, filename, filesize-2) == 0);
				
			if (same_file || match1 && match2 && match3) 
			{
				// if the filename matches then copy the toc Entry
				tocEntry->fileLBA = localTocEntry.fileLBA;
				tocEntry->fileProperties = localTocEntry.fileProperties;
				tocEntry->fileSize = localTocEntry.fileSize;

				strcpy((char*)tocEntry->filename, (char*)localTocEntry.filename);
				memcpy((char*)tocEntry->date, (char*)localTocEntry.date, 7);

				return true;
			}

			tocEntryPointer = (dirTocEntry*)(((char*)tocEntryPointer)+tocEntryPointer->length);
		}

		num_dir_sectors--;

		if (num_dir_sectors > 0)
		{
			dir_lba++;

			if (umdimageloader::read_block((u8*)toc,dir_lba,1) < 0)
			{
				return false;
			}

			tocEntryPointer = (dirTocEntry*)toc;
		}
	}

	return false;
}
