
#include <assert.h>
#include <string.h>

#include "incoming.h"

struct frame64rd *frame64rd_init(
    struct frame64rd *s,
    void *opaque,
    frame64rd_received received
) {
    assert(s);
    assert(received);

    memset(s, 0, sizeof(struct frame64rd));
    s->opaque = opaque;
    s->received = received;

    return s;
}

bool frame64rd_parse(struct frame64rd *s) {
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
            return true;
        }

        // handle this current frame:
        uint8_t f = (s->rx >> 7) & 1;
        uint8_t c = (s->rx >> 6) & 1;
        assert(s->received);
        if (!s->received(s, s->rbuf + s->rh, s->rl, c, f)) {
            return false;
        }

        s->rf = false;
        s->rh += s->rl;

        if (s->rh >= s->rt) {
            // buffer is empty:
            s->rh = 0;
            s->rt = 0;
            return true;
        }

        // remaining bytes begin the next frame:
        memmove(s->rbuf, s->rbuf + s->rh, s->rt - s->rh);
        s->rt -= s->rh;
        s->rh = 0;
    }

    return true;
}
