
#include <assert.h>
#include <string.h>
#include "incoming.h"

struct frame_incoming *frame_incoming_init(
    struct frame_incoming *s,
    void *opaque,
    frame_incoming_read_more read_more,
    frame_incoming_received received
) {
    assert(s);
    assert(read_more);
    assert(received);

    memset(s, 0, sizeof(struct frame_incoming));
    s->opaque = opaque;
    s->read_more = read_more;
    s->received = received;

    return s;
}

bool frame_incoming_read(struct frame_incoming *s) {
    // read available bytes:
    assert(s->read_more);
    long n;
    if (!s->read_more(s, s->rbuf + s->rt, 64 - s->rt, &n)) {
        // error:
        return false;
    }
    if (n == 0) {
        // EOF?
        return false;
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
            return true;
        }

        // handle this current frame:
        uint8_t f = (s->rx >> 7) & 1;
        uint8_t c = (s->rx >> 6) & 1;
        assert(s->frame_received);
        s->received(s, s->rbuf + s->rh, s->rl, c, f);

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
