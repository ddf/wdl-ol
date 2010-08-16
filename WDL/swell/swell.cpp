/* Cockos SWELL (Simple/Small Win32 Emulation Layer for Losers (who use OS X))
   Copyright (C) 2006-2007, Cockos, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
  

    This file implements a few Windows calls using their posix equivilents

  */



#include "swell.h"
#include <sys/types.h>
#include <sys/stat.h>



char *lstrcpyn(char *dest, const char *src, int l)
{
  if (l<1) return dest;

  char *dsrc=dest;
  while (--l > 0)
  {
    char p=*src++;
    if (!p) break;
    *dest++=p;
  }
  *dest++=0;

  return dsrc;
}

void Sleep(int ms)
{
  usleep(ms?ms*1000:100);
}

DWORD GetTickCount()
{
  struct timeval tm={0,};
  gettimeofday(&tm,NULL);
  return tm.tv_sec*1000 + tm.tv_usec/1000;
}


static void intToFileTime(time_t t, FILETIME *out)
{
  unsigned long long a=(unsigned long long)t; // seconds since january 1st, 1970
  a+=(60*60*24*(365*4+1)/4)*(1970-1601); // this is approximate
  a*=1000*10000; // seconds to 1/10th microseconds (100 nanoseconds)
  out->dwLowDateTime=a & 0xffffffff;
  out->dwHighDateTime=a>>32;
}

BOOL GetFileTime(void *fh, FILETIME *lpCreationTime, FILETIME *lpLastAccessTime, FILETIME *lpLastWriteTime)
{
  if (!fh) return 0;
  FILE *fp=(FILE *)fh;
  struct stat st;
  if (fstat(fileno(fp),&st)) return 0;
  
  if (lpCreationTime) intToFileTime(st.st_ctime,lpCreationTime);
  if (lpLastAccessTime) intToFileTime(st.st_atime,lpLastAccessTime);
  if (lpLastWriteTime) intToFileTime(st.st_mtime,lpLastWriteTime);

  return 1;
}