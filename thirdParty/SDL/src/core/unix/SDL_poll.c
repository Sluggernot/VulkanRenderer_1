/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2023 Sam Lantinga <slouken@libsdl.org>

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
*/

#include "../../SDL_internal.h"

#include "SDL_poll.h"

#ifdef HAVE_POLL
#include <poll.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <errno.h>

int SDL_IOReady(int fd, int flags, int timeoutMS)
{
    int result;

    SDL_assert(flags & (SDL_IOR_READ | SDL_IOR_WRITE));

    /* Note: We don't bother to account for elapsed time if we get EINTR */
    do {
#ifdef HAVE_POLL
        struct pollfd info;

        info.fd = fd;
        info.events = 0;
        if (flags & SDL_IOR_READ) {
            info.events |= POLLIN | POLLPRI;
        }
        if (flags & SDL_IOR_WRITE) {
            info.events |= POLLOUT;
        }
        result = poll(&info, 1, timeoutMS);
#else
        fd_set rfdset, *rfdp = NULL;
        fd_set wfdset, *wfdp = NULL;
        struct timeval tv, *tvp = NULL;

        /* If this assert triggers we'll corrupt memory here */
        SDL_assert(fd >= 0 && fd < FD_SETSIZE);

        if (flags & SDL_IOR_READ) {
            FD_ZERO(&rfdset);
            FD_SET(fd, &rfdset);
            rfdp = &rfdset;
        }
        if (flags & SDL_IOR_WRITE) {
            FD_ZERO(&wfdset);
            FD_SET(fd, &wfdset);
            wfdp = &wfdset;
        }

        if (timeoutMS >= 0) {
            tv.tv_sec = timeoutMS / 1000;
            tv.tv_usec = (timeoutMS % 1000) * 1000;
            tvp = &tv;
        }

        result = select(fd + 1, rfdp, wfdp, NULL, tvp);
#endif /* HAVE_POLL */

    } while (result < 0 && errno == EINTR && !(flags & SDL_IOR_NO_RETRY));

    return result;
}

/* vi: set ts=4 sw=4 expandtab: */
