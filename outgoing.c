
#include <assert.h>
#include <string.h>

#include "outgoing.h"

struct frame_outgoing *frame_outgoing_init(struct frame_outgoing *s) {
    assert(s);

    memset(s, 0, sizeof(struct frame_outgoing));

    return s;
}

int frame_outgoing_send(struct frame_outgoing *s, uint8_t chn, bool fin) {
    assert(s);
    assert(s->len <= 63);
    assert(chn <= 1);

    // compute header byte:
    s->sbuf[0] = ((fin & 1) << 7) | ((chn & 1) << 6) | (s->len & 63);

    // send frame out:
    assert(s->write_data);
    int res = s->write_data(s->sbuf, s->len+1);
    if (res < 0) {
        return res;
    }

    s->len = 0;

    return res;
}

bool frame_outgoing_append(struct frame_outgoing *s, uint8_t x) {
    assert(s);
    assert(s->len <= 63);

    if (s->len + 1 > 63) {
        return false;
    }

    s->sbuf[1 + s->len++] = x;

    return true;
}

bool frame_outgoing_append_bytes(struct frame_outgoing *s, uint8_t *buf, uint8_t len) {
    assert(s);
    assert(s->len <= 63);
    assert(buf);
    assert(len <= 63);

    if (s->len + len > 63) {
        return false;
    }

    memcpy(&s->sbuf[1 + s->len], buf, len);
    s->len += len;

    return true;
}
