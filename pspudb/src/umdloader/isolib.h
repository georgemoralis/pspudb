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

#ifndef _CDVDLIB_H
#define _CDVDLIB_H



#ifndef __WIN32__
#	ifdef WIN32
#		define __WIN32__
#	endif
#endif

#if defined(__WIN32__)
#	pragma pack(1)
#endif

struct TocEntry
{
	u32	fileLBA;
	u32 fileSize;
	u8	fileProperties;
	u8	padding1[3];
	u8	filename[128+1];
	u8	date[7];
#if defined(__WIN32__)
};
#else
} __attribute__((packed));
#endif

#if defined(__WIN32__)
#	pragma pack()
#endif

bool ISOFS_findfile(char* fname, struct TocEntry* tocEntry);

#endif // _CDVDLIB_H
