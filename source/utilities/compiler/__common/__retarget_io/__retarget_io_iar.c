/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifdef __USE_COMMON_RETARGET_IO_IAR_C__
#undef __USE_COMMON_RETARGET_IO_IAR_C__

#if VSF_USE_POSIX == ENABLED
SECTION(".vsf.utilities.stdio.iar.__open")
int __open(const char *path_name, int flags, mode_t mode)
{
    return open(path_name, flags, mode);
}

SECTION(".vsf.utilities.stdio.iar.__close")
void __close(int handle)
{
    close(handle);
}

SECTION(".vsf.utilities.stdio.iar.__lseek")
off_t __lseek(int handle, off_t offset, int whence)
{
    return lseek(handle, offset, whence);
}

#   if VSF_USE_LINUX == ENABLED

#       if !(VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED)
SECTION(".vsf.utilities.stdio.iar.__exit")
void __exit(int status)
{
    // exit process is not supported, can exit current thread only
    vsf_thread_exit();
}
#       endif

#       if !(VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED)

//! statement is unreachable
#pragma diag_suppress=pe111

SECTION(".vsf.utilities.stdio.iar.remove")
int remove(const char * pathname)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

SECTION(".vsf.utilities.stdio.iar.rename")
int rename(const char *old_filename, const char *new_filename)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}
#       endif

#   endif

#endif      // VSF_USE_POSIX

SECTION(".vsf.utilities.stdio.iar.__write")
size_t __write(int handle, const unsigned char *buf, size_t buf_size)
{
    return __vsf_stdio_write(handle, buf, buf_size);
}

SECTION(".vsf.utilities.stdio.iar.__read")
size_t __read(int handle, unsigned char *buf, size_t buf_size)
{
    return __vsf_stdio_read(handle, buf, buf_size);
}

#endif      // __USE_COMMON_RETARGET_IO_IAR_C__
