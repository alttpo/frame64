#pragma once

#include <stdint.h>
#include <stdbool.h>

struct frame64rd;

typedef bool (*frame64rd_received)(struct frame64rd *s, uint8_t *buf, uint8_t len, uint8_t chn, bool fin);

struct frame64rd {
    uint8_t rbuf[64];   // recv frame data
    uint8_t rh;         // head
    uint8_t rt;         // tail

    bool rf;         // read frame header?
    uint8_t rx;         // frame header byte
    uint8_t rl;         // frame length

    void *opaque;

// required callbacks:
    frame64rd_received received;
};

struct frame64rd *frame64rd_init(
    struct frame64rd *s,
    void *opaque,
    frame64rd_received received
);

bool frame64rd_parse(struct frame64rd *s);

static inline uint8_t *frame64rd_read_dest(struct frame64rd *s) {
    return s->rbuf + s->rt;
}

static inline size_t frame64rd_read_size(struct frame64rd *s) {
    return 64 - s->rt;
}

static inline void frame64rd_read_complete(struct frame64rd *s, long n) {
    assert(s->rt + n <= 64);
    s->rt += n;
}
