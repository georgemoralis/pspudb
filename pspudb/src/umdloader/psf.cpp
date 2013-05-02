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
#include <string.h>
#include "types.h"
#include "psf.h"

#ifdef EMULATOR
#include "emulator/log.h"
#else
#define errorf(name, fmt, ...)
#define infof(name,fmt,...)
#define loader "empty"
#endif


char PsfMagic[] = "\0PSF";

Psf_info psfinfo;

u8 *psf::pointer = 0;

Psf_info::~Psf_info()
{
	if (psf::pointer) delete[] psf::pointer;
}

bool psf::load_psf(u8 *fileptr)
{
	if (psf::pointer) delete[] psf::pointer;

	psf::pointer = fileptr;

	struct Psf_header   *psfheader;
	struct Psf_section  *psfsections, *sect;

	u8   *psflabels;
	u8   *psfdata;
	//u8   strbuf[512];
	//u32  dsize;
	u32 i;

	//step1: check the first 4 bytes is it's a psf
	if (::memcmp(PsfMagic, psf::pointer, 4))
	{
		errorf(loader, "This file isn't a PSF file! [PSF Magic == 0x%08x]\n", *(u32 *)PsfMagic);
		return false;
	}
	psfheader   = (struct Psf_header *)psf::pointer;
	psfsections = (struct Psf_section *)(psf::pointer + sizeof(struct Psf_header));
	psflabels   = fileptr + psfheader->keyTableOffset;
	psfdata     = fileptr + psfheader->valueTableOffset;

	for (i = 0, sect = psfsections; i < psfheader->indexEntryCount; i++, sect++)
	{
		switch(sect->dataType)
		{
		case 2:   /* string*/
			//dsize = sect->dataSize < 512 ? sect->dataSize : 512-1;
			//strncpy((char*)strbuf, (const char*), dsize);
			//strbuf[dsize] = '\0';
			infof(loader, "%-16s = %s", &psflabels[sect->keyOffset], (char *)&psfdata[sect->valueOffset]);
			store_psf_string(&psflabels[sect->keyOffset], (char *)(&psfdata[sect->valueOffset]));
			break;
		case 4: /* Integer word */
			infof(loader, "%-16s = %d", &psflabels[sect->keyOffset], *(unsigned int*)&psfdata[sect->valueOffset]);
			store_psf_integer(&psflabels[sect->keyOffset], *(unsigned int*)&psfdata[sect->valueOffset]);
			break;

		case 0: /* binary data ? */
			infof(loader, "%-16s = BINARY DATA", &psflabels[sect->keyOffset]);
			break;

		default: /* Dunno yet */
			infof(loader, "%-16s = UNKNOWN VALUE", &psflabels[sect->keyOffset]);
			break;
		}
	}
	return true;
}

void psf::store_psf_string(u8 *key , char *value)
{
	if(!strcmp("CATEGORY", (const char*)key))       { psfinfo.category           = value; return; }
	if(!strcmp("DISC_ID", (const char*)key))        { psfinfo.disc_id            = value; return; }
	if(!strcmp("DISC_VERSION", (const char*)key))   { psfinfo.disc_version       = value; return; }
	if(!strcmp("PSP_SYSTEM_VER", (const char*)key)) { psfinfo.psp_system_version = value; return; }
	if(!strcmp("TITLE", (const char*)key))          { psfinfo.title              = value; return; }

	//if anything of above doesn't return then we don't have it on structure
	errorf(loader, "%-16s = %s can't store in structure", key, value);
}
void psf::store_psf_integer(u8 *key, u32 value)
{
	if(!strcmp("BOOTABLE", (const char*)key))       { psfinfo.bootable       = value; return; }
	if(!strcmp("PARENTAL_LEVEL", (const char*)key)) { psfinfo.parental_level = value; return; }
	if(!strcmp("REGION", (const char*)key))         { psfinfo.region         = value; return; }
	if(!strcmp("DISC_NUMBER", (const char*)key))    { psfinfo.disc_number    = value; return; }
	if(!strcmp("DISC_TOTAL", (const char*)key))     { psfinfo.disc_total     = value; return; }
	//if anything of above doesn't return then we don't have it on structure
	errorf(loader, "%-16s = %d can't store in structure", key, value);
}
bool psf::is_loaded()
{
	return !!psf::pointer;
}