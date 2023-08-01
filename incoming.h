#pragma once

#include <stdint.h>
#include <stdbool.h>

struct frame_incoming {
    uint8_t rbuf[64];   // recv frame data
    uint8_t rh;         // head
    uint8_t rt;         // tail

    bool    rf;         // read frame header?
    uint8_t rx;         // frame header byte
    uint8_t rl;         // frame length

// required callbacks:
    int (*read_more)(uint8_t *buf, int size);
    void (*frame_received)(uint8_t chn, bool fim, uint8_t len, uint8_t *buf);
};

struct frame_incoming *frame_incoming_init(struct frame_incoming *s);
int frame_incoming_read(struct frame_incoming *s);
