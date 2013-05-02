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
