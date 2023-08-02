#pragma once

#include <stdint.h>
#include <stdbool.h>

struct frame_incoming;
typedef bool (*frame_incoming_read_more)(struct frame_incoming *s, uint8_t *buf, long size, long *n);
typedef void (*frame_incoming_received)(struct frame_incoming *s, uint8_t *buf, uint8_t len, uint8_t chn, bool fin);

struct frame_incoming {
    uint8_t rbuf[64];   // recv frame data
    uint8_t rh;         // head
    uint8_t rt;         // tail

    bool    rf;         // read frame header?
    uint8_t rx;         // frame header byte
    uint8_t rl;         // frame length

    void *opaque;

// required callbacks:
    frame_incoming_read_more read_more;
    frame_incoming_received received;
};

struct frame_incoming *frame_incoming_init(
    struct frame_incoming *s,
    void *opaque,
    frame_incoming_read_more read_more,
    frame_incoming_received received
);

bool frame_incoming_read(struct frame_incoming *s);
