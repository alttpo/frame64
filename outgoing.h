#pragma once

#include <stdint.h>
#include <stdbool.h>

struct frame_outgoing;
typedef bool (*frame_outgoing_write_data_cb)(struct frame_outgoing *s, uint8_t *buf, size_t size, long *n);

struct frame_outgoing {
    uint8_t *p;

    uint8_t buf[64];

    void *opaque;

// required callbacks:
    frame_outgoing_write_data_cb write_data;
};

struct frame_outgoing *frame_outgoing_init(
    struct frame_outgoing *s,
    void *opaque,
    frame_outgoing_write_data_cb write_data
);

bool frame_outgoing_send(struct frame_outgoing *s, uint8_t chn, bool fin);

static inline size_t frame_outgoing_len(struct frame_outgoing *s) {
    return s->p - (s->buf + 1);
}

static inline bool frame_outgoing_append(struct frame_outgoing *s, uint8_t x) {
    assert(s);
    assert(frame_outgoing_len(s) <= 63);

    if (frame_outgoing_len(s) + 1 > 63) {
        return false;
    }

    *s->p++ = x;

    return true;
}

static inline bool frame_outgoing_append_bytes(struct frame_outgoing *s, const uint8_t *buf, size_t len) {
    assert(s);
    assert(buf);
    assert(len <= 63);

    assert(frame_outgoing_len(s) <= 63);

    if (frame_outgoing_len(s) + len > 63) {
        return false;
    }

    memcpy(s->p, buf, len);
    s->p += len;

    return true;
}
