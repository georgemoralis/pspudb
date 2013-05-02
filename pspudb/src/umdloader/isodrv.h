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
#pragma once

#include "isolib.h"

/* Filing-system exported functions */
void ISOFS_init();
int ISOFS_open(char *name, int mode);
int ISOFS_lseek(int fd, int offset, int whence);
int ISOFS_read( int fd, char * buffer, int size );
int ISOFS_write( int fd, char * buffer, int size );
int ISOFS_close( int fd);
u32 ISOFS_getfilesize(char *name);
