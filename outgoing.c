
#include <assert.h>
#include <string.h>

#include "outgoing.h"

struct frame_outgoing *frame_outgoing_init(
    struct frame_outgoing *s,
    void *opaque,
    frame_outgoing_write_data_cb write_data
) {
    assert(s);
    assert(write_data);

    memset(s, 0, sizeof(struct frame_outgoing));
    s->opaque = opaque;
    s->write_data = write_data;
    s->p = &s->buf[1];

    return s;
}

bool frame_outgoing_send(struct frame_outgoing *s, uint8_t chn, bool fin) {
    assert(s);
    assert(chn <= 1);

    // compute length:
    size_t len = frame_outgoing_len(s);
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
