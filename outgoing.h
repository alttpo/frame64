#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

struct frame64wr;

typedef bool (*frame64wr_write_cb)(struct frame64wr *s, uint8_t *buf, size_t size, long *n);

struct frame64wr {
    uint8_t *p;

    uint8_t buf[64];

    void *opaque;

// required callbacks:
    frame64wr_write_cb write_data;
};

struct frame64wr *frame64wr_init(
    struct frame64wr *s,
    void *opaque,
    frame64wr_write_cb write_data
);

void frame64wr_reset(struct frame64wr *s);

bool frame64wr_send(struct frame64wr *s, uint8_t chn, bool fin);

static inline size_t frame64wr_len(struct frame64wr *s) {
    return s->p - (s->buf + 1);
}

static inline bool frame64wr_append(struct frame64wr *s, uint8_t x) {
    assert(s);
    assert(frame64wr_len(s) <= 63);

    if (frame64wr_len(s) + 1 > 63) {
        return false;
    }

    *s->p++ = x;

    return true;
}

static inline bool frame64wr_append_bytes(struct frame64wr *s, const uint8_t *buf, size_t len) {
    assert(s);
    assert(buf);
    assert(len <= 63);

    assert(frame64wr_len(s) <= 63);

    if (frame64wr_len(s) + len > 63) {
        return false;
    }

    memcpy(s->p, buf, len);
    s->p += len;

    return true;
}
