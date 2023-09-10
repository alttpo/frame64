
#include <assert.h>
#include <string.h>

#include "outgoing.h"

struct frame64wr *frame64wr_init(
    struct frame64wr *s,
    void *opaque,
    frame64wr_write_cb write_data
) {
    assert(s);
    assert(write_data);

    memset(s, 0, sizeof(struct frame64wr));
    s->opaque = opaque;
    s->write_data = write_data;
    s->p = &s->buf[1];

    return s;
}

void frame64wr_reset(struct frame64wr *s) {
    assert(s);

    memset(s->buf, 0, 64);
    s->p = &s->buf[1];
}

bool frame64wr_send(struct frame64wr *s, uint8_t chn, bool fin) {
    assert(s);
    assert(chn <= 1);

    // compute length:
    size_t len = frame64wr_len(s);
    assert(len <= 63);

    // compute header byte:
    s->buf[0] = ((fin & 1) << 7) | ((chn & 1) << 6) | (len & 63);

    // send frame out:
    assert(s->write_data);
    long n;
    if (!s->write_data(s, s->buf, len + 1, &n)) {
        return false;
    }

    // reset pointer:
    s->p = s->buf + 1;

    return true;
}
