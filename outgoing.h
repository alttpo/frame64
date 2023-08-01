#pragma once

#include <stdint.h>
#include <stdbool.h>

struct frame_outgoing {
    uint8_t sbuf[64];
    uint8_t len;

// required callbacks:
    int (*write_data)(uint8_t *buf, int size);
};

struct frame_outgoing *frame_outgoing_init(struct frame_outgoing *s);
int frame_outgoing_send(struct frame_outgoing *s, uint8_t chn, bool fin);
bool frame_outgoing_append(struct frame_outgoing *s, uint8_t x);
bool frame_outgoing_append_bytes(struct frame_outgoing *s, uint8_t *buf, uint8_t len);
