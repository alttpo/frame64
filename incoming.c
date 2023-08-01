
#include <assert.h>
#include <string.h>
#include "incoming.h"

struct frame_incoming *frame_incoming_init(struct frame_incoming *s) {
    memset(s, 0, sizeof(struct frame_incoming));
    return s;
}

int frame_incoming_read(struct frame_incoming *s) {
    // read available bytes:
    assert(s->read_more);
    int n = s->read_more(s->rbuf + s->rt, 64 - s->rt);
    if (n < 0) {
        // error:
        return n;
    }
    if (n == 0) {
        // EOF?
        return n;
    }

    s->rt += n;

    while (s->rh < s->rt) {
        if (!s->rf) {
            // [7654 3210]
            //  fcll llll   f = final frame of message
            //              c = channel (0..1)
            //              l = length of frame (0..63)
            s->rx = s->rbuf[s->rh++];
            // determine length of frame:
            s->rl = s->rx & 63;
            // read the frame header byte:
            s->rf = true;
        }

        if (s->rh + s->rl > s->rt) {
            // not enough data for frame:
            return n;
        }

        // handle this current frame:
        uint8_t f = (s->rx >> 7) & 1;
        uint8_t c = (s->rx >> 6) & 1;
        assert(s->frame_received);
        s->frame_received(c, f, s->rl, s->rbuf + s->rh);

        s->rf = false;
        s->rh += s->rl;

        if (s->rh >= s->rt) {
            // buffer is empty:
            s->rh = 0;
            s->rt = 0;
            return n;
        }

        // remaining bytes begin the next frame:
        memmove(s->rbuf, s->rbuf + s->rh, s->rt - s->rh);
        s->rt -= s->rh;
        s->rh = 0;
    }

    return n;
}
