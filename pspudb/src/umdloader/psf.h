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

struct Psf_header
{
    unsigned char   magic[4];
    unsigned int    version;
    unsigned int    keyTableOffset;
    unsigned int    valueTableOffset;
    unsigned int    indexEntryCount;
};

struct Psf_section
{
    unsigned short   keyOffset;
    unsigned char    padding;
    unsigned char    dataType; /* string=2, integer=4, binary=0 */
    unsigned int     dataSize;
    unsigned int     dataSizePadded;
    unsigned int     valueOffset;
};

struct Psf_info 
{
    char          *category;
    char          *disc_id;
    char          *disc_version;
    char          *psp_system_version;
    char          *title;
    unsigned int   bootable;
    unsigned int   parental_level;
    unsigned int   region;
    unsigned int   disc_number;
    unsigned int   disc_total;

    Psf_info()
    :   category(0)
    ,   disc_id(0) 
    ,   disc_version(0)
    ,   psp_system_version(0)
    ,   title(0) 
    ,   bootable(0) 
    ,   parental_level(0) 
    ,   region(0)
    ,   disc_number(0)
    ,   disc_total(0)
    {
    }

    ~Psf_info();
};

namespace psf 
{
    bool load_psf(u8 *fileptr);
    void store_psf_string(u8 *key, char *value);
    void store_psf_integer(u8 *key, u32 value);
    bool is_loaded();

    extern u8 *pointer;
};

extern Psf_info psfinfo;

