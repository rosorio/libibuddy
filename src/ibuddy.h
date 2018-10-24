/*
 * Copyright (c) 2010, Rodrigo OSORIO 
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef IBBUDY_H
#define IBBUDY_H

/*
 * Global definitions
 */
/* I-Buddy head colors */
#define IBUDDY_HEAD_RED    0x01
#define IBUDDY_HEAD_GREEN  0x02
#define IBUDDY_HEAD_BLUE   0x04
/* I-Buddy wing state */
#define IBUDDY_WING_UP     0x00
#define IBUDDY_WING_DOWN   0x01
/* I-Buddy boddy flick */
#define IBUDDY_BODY_LEFT   0x00
#define IBUDDY_BODY_RIGHT  0x01
/* I-Buddy heart state */
#define IBUDDY_HEART_OFF   0x00
#define IBUDDY_HEART_ON    0x01
/* I-Buddy identification */
#define IBUDDY_VENDOR_ID  0x1130
#define IBUDDY_PRODUCT_ID 0x0001

typedef void * ibuddy_ref;

/*
 * Exported functions 
 */

/* Set a specific product/vendor ID for the I-Buddy */
void ibuddy_set_product_id(unsigned int vendor_id, unsigned int product_id);

/* Get the number of I-Buddy devices connected */
int ibuddy_get_device_count();

/* Open the I-Buddy device */
void * ibuddy_open(unsigned short ref);

/* Close the I-Buddy device */
void ibuddy_close(ibuddy_ref * id);

/* Set I-Buddy sate */
void ibuddy_set_head_color(ibuddy_ref * id, unsigned short color);
void ibuddy_set_wing_state(ibuddy_ref * id, unsigned short state);
void ibuddy_set_body_state(ibuddy_ref * id, unsigned short state);
void ibuddy_set_hearth_state(ibuddy_ref * id, unsigned short state);
/* Get I-Buddy state */
const unsigned short ibuddy_get_head_color(ibuddy_ref * id);
const unsigned short ibuddy_get_wing_state(ibuddy_ref * id);
const unsigned short ibuddy_get_body_state(ibuddy_ref * id);
const unsigned short ibuddy_get_heat_state(ibuddy_ref * id);

void ibuddy_set_all_state(ibuddy_ref * id,unsigned short head_color,
                                      unsigned short wing_state,
                                      unsigned short body_state,
                                      unsigned short hearth_state,
                                      int            delay);

/* Do action */
void ibuddy_do_action(ibuddy_ref * id);
/* Reset buddy state (head,wings,body,heart) */
void ibuddy_reset_state(ibuddy_ref * id);
/* I-buddy test function */
void ibbudy_full_test(ibuddy_ref * id);

/* Ibuddy facilitators */
void ibuddy_flick_body(ibuddy_ref * id,int number,int delay);
void ibuddy_blink_heart(ibuddy_ref * id,int number,int delay);
void ibuddy_flap_wings(ibuddy_ref * id,int number,int delay);

void __attribute__ ((constructor)) ibuddy_init(void);
void __attribute__ ((destructor))  ibuddy_fini(void);


#endif
