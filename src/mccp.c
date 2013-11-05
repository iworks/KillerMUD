#ifdef MCCP
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "merc.h"
#include "telnet.h"

/*
 * Ported to SMAUG by Garil of DOTDII Mud
 * aka Jesse DeFer <dotd@dotd.com>  http://www.dotd.com
 *
 * revision 1: MCCP v1 support
 * revision 2: MCCP v2 support
 * revision 3: Correct MMCP v2 support
 * revision 4: clean up of write_to_descriptor() suggested by Noplex@CB
 *
 * See the web site below for more info.
 */

/*
 * mccp.c - support functions for mccp (the Mud Client Compression Protocol)
 *
 * see http://homepages.ihug.co.nz/~icecube/compress/ and README.Rom24-mccp
 *
 * Copyright (c) 1999, Oliver Jowett <icecube@ihug.co.nz>.
 *
 * This code may be freely distributed and used if this copyright notice is
 * retained intact.
 */


bool	write_to_descriptor	    args( ( DESCRIPTOR_DATA *d, char *txt, int length ) );

void *zlib_alloc(void *opaque, unsigned int items, unsigned int size)
{
    return calloc(items, size);
}

void zlib_free(void *opaque, void *address)
{
    DISPOSE(address);
}

bool process_compressed(DESCRIPTOR_DATA *d)
{
    int iStart = 0, nBlock, nWrite, len;

    if (!d->out_compress)
        return TRUE;

    // Try to write out some data..
    len = d->out_compress->next_out - d->out_compress_buf;

    if (len > 0)
    {
        // we have some data to write
        for (iStart = 0; iStart < len; iStart = nWrite)
        {
            nBlock = UMIN (len - iStart, 4096);
            if ((nWrite = write(d->descriptor, d->out_compress_buf + iStart, nBlock)) < 0)
            {
                if (errno == EAGAIN ||
                    errno == ENOSR)
                    break;

                return FALSE;
            }

            if (!nWrite)
                break;
        }

        if (iStart)
        {
            // We wrote "iStart" bytes
            if (iStart < len)
                memmove(d->out_compress_buf, d->out_compress_buf + iStart, len - iStart);

            d->out_compress->next_out = d->out_compress_buf + len - iStart;
        }
    }

    return TRUE;
}

char enable_compress[] =
{
    IAC, SB, TELOPT_COMPRESS, WILL, SE, 0
};
char enable_compress2[] =
{
    IAC, SB, TELOPT_COMPRESS2, IAC, SE, 0
};

bool compressStart(DESCRIPTOR_DATA *d, unsigned char telopt)
{
    z_stream *s;

    if (d->out_compress)
        return TRUE;

    CREATE(s, z_stream, 1);
    CREATE(d->out_compress_buf, unsigned char, COMPRESS_BUF_SIZE);

    s->next_in = NULL;
    s->avail_in = 0;

    s->next_out = d->out_compress_buf;
    s->avail_out = COMPRESS_BUF_SIZE;

    s->zalloc = zlib_alloc;
    s->zfree  = zlib_free;
    s->opaque = NULL;

    if (deflateInit(s, 9) != Z_OK)
    {
        DISPOSE(d->out_compress_buf);
        DISPOSE(s);
        return FALSE;
    }

    if (telopt == TELOPT_COMPRESS)
        write_to_descriptor(d, enable_compress, 0);
    else if (telopt == TELOPT_COMPRESS2)
        write_to_descriptor(d, enable_compress2, 0);
    else
        bug("compressStart: bad TELOPT passed", 0);

    d->compressing = telopt;
    d->out_compress = s;

    return TRUE;
}

bool compressEnd(DESCRIPTOR_DATA *d)
{
    unsigned char dummy[1];

    if (!d->out_compress)
        return TRUE;

    d->out_compress->avail_in = 0;
    d->out_compress->next_in = dummy;

    if (deflate(d->out_compress, Z_FINISH) != Z_STREAM_END)
        return FALSE;

    if (!process_compressed(d)) /* try to send any residual data */
        return FALSE;

    deflateEnd(d->out_compress);
    DISPOSE(d->out_compress_buf);
    DISPOSE(d->out_compress);
    d->compressing = 0;

    return TRUE;
}
#endif
