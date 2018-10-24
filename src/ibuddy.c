
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

#include "ibuddy.h"

#include <errno.h>
#include <libusb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "ibuddy_internal.h"


unsigned char MSG_IBUDDY_SETUP[] = {0x21, 0x09, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00};
unsigned char MSG_IBUDDY_ACT[]   = {0x55, 0x53, 0x42, 0x43, 0x00, 0x40, 0x02};

unsigned int ibuddy_vendor_id  = IBUDDY_VENDOR_ID;
unsigned int ibuddy_product_id = IBUDDY_PRODUCT_ID;
/* Time limits */
#define IBUDDY_MINIMAL_WAIT_MS   100 
#define IBUDDY_HEAD_WAIT_MS      300 
#define IBUDDY_HEART_WAIT_MS     180 
#define IBUDDY_BODY_WAIT_MS      250 
#define IBUDDY_FLAP_WAIT_MS      140 
/* Time maniulation macros */
#define TIME_NS_TO_MS(ns) (ns / 1000000)
#define TIME_MS_TO_NS(ms) (ms * 1000000)
#define TIME_SE_TO_MS(se) (se * 1000)
#define TIME_MS_TO_SEC(ms)(ms / 1000)
#define TIME_MS_MOD_SEC(ms)(ms % 1000)

#define MAX(a,b) (a>b)?a:b


#define IBUDDY_FLICK_LEFT_BIT       0
#define IBUDDY_FLICK_RIGHT_BIT      1
#define IBUDDY_WING_DOWN_BIT        2
#define IBUDDY_WING_UP_BIT          3
#define IBUDDY_HEAD_RED_BIT         4
#define IBUDDY_HEAD_GREEN_BIT       5
#define IBUDDY_HEAD_BLUE_BIT        6
#define IBUDDY_HEART_BIT            7        

#define IBUDDY_REQUEST              0x09
#define IBUDDY_REQUEST_TYPE         0x21
#define IBUDDY_REQUEST_VALUE        0x01
#define IBUDDY_REQUEST_INDEX        0x01

void microsleep(int delay)
{
  struct timespec ts;

  if(delay <= 0)
    return;

  memset(&ts,0,sizeof(struct timespec));
  ts.tv_sec  = TIME_MS_TO_SEC(delay);
  ts.tv_nsec = TIME_MS_TO_NS(TIME_MS_MOD_SEC(delay));

  /* Sleep ! */
  nanosleep(&ts,NULL);
}

void ibuddy_reset_message(struct ibuddy_context_t * context)
{
  context->action_message = 0xFF;
}

void ibuddy_change_bit_value(struct ibuddy_context_t * context, int number, int state)
{

  if(state) /* Clear the bit */
    context->action_message &= ~(1 << number);
  else /* Set the bit */
    context->action_message |= (1 << number);
}

int ibuddy_send_message(struct ibuddy_context_t * context,unsigned int new_delay)
{
  int nbytes;
  unsigned char buffer[8];
  unsigned int delay = MAX(IBUDDY_MINIMAL_WAIT_MS,new_delay);

  memcpy ((void *)buffer, MSG_IBUDDY_ACT, 7);
  buffer[7] = context->action_message;

  /* Send the header */
  nbytes = libusb_control_transfer (context->handle,
  IBUDDY_REQUEST_TYPE,
  IBUDDY_REQUEST,
  IBUDDY_REQUEST_VALUE,
  IBUDDY_REQUEST_INDEX,
  (uint8_t *)MSG_IBUDDY_SETUP, 8, 500);  

  if(nbytes < 0)
    return errno;

  /* Send the action message */
  nbytes = libusb_control_transfer (context->handle,
  IBUDDY_REQUEST_TYPE,
  IBUDDY_REQUEST,
  IBUDDY_REQUEST_VALUE,
  IBUDDY_REQUEST_INDEX,
  (uint8_t *)buffer, 8, 500);

  if(nbytes < 0)
    return errno;

  /* Do a minimal sleep if need */
  if(delay)
    microsleep(delay);

  return EXIT_SUCCESS;
}

/*
 * Exported functions
 */
void ibuddy_set_head_color(ibuddy_ref * id, unsigned short color)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;
  
  ibuddy_change_bit_value(context,IBUDDY_HEAD_RED_BIT,   color & IBUDDY_HEAD_RED);
  ibuddy_change_bit_value(context,IBUDDY_HEAD_GREEN_BIT, color & IBUDDY_HEAD_GREEN);
  ibuddy_change_bit_value(context,IBUDDY_HEAD_BLUE_BIT,  color & IBUDDY_HEAD_BLUE);

  if(context->auto_action)
    ibuddy_send_message(context,IBUDDY_HEAD_WAIT_MS);
}

void ibuddy_set_wing_state(ibuddy_ref * id, unsigned short state)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;

  ibuddy_change_bit_value(context,IBUDDY_WING_DOWN_BIT, (IBUDDY_WING_UP == state)?0:1 );
  ibuddy_change_bit_value(context,IBUDDY_WING_UP_BIT,   (IBUDDY_WING_UP != state)?0:1 );

  if(context->auto_action)
    ibuddy_send_message(context,IBUDDY_FLAP_WAIT_MS);
}

void ibuddy_set_body_state(ibuddy_ref * id, unsigned short state)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;

  ibuddy_change_bit_value(context,IBUDDY_FLICK_LEFT_BIT,  (IBUDDY_BODY_LEFT != state)?0:1 );
  ibuddy_change_bit_value(context,IBUDDY_FLICK_RIGHT_BIT, (IBUDDY_BODY_LEFT == state)?0:1 );
  
  if(context->auto_action)
    ibuddy_send_message(context,IBUDDY_BODY_WAIT_MS);
}

void ibuddy_set_heart_state(ibuddy_ref * id, unsigned short state)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;

  ibuddy_change_bit_value(context,IBUDDY_HEART_BIT,(IBUDDY_HEART_OFF==state)?0:1);

  if(context->auto_action)
    ibuddy_send_message(context,IBUDDY_HEART_WAIT_MS);
}

const unsigned short ibuddy_get_head_color(ibuddy_ref * id)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;
  unsigned short color = 0;

  color |= context->action_message & IBUDDY_HEAD_RED_BIT?   IBUDDY_HEAD_RED   : 0;
  color |= context->action_message & IBUDDY_HEAD_GREEN_BIT? IBUDDY_HEAD_GREEN : 0;
  color |= context->action_message & IBUDDY_HEAD_BLUE_BIT?  IBUDDY_HEAD_BLUE  : 0;
  
  return color;
}

const unsigned short ibuddy_get_wing_state(ibuddy_ref * id)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;
  unsigned short state = 0;
  
  if(context->action_message & IBUDDY_WING_UP_BIT)
    state = IBUDDY_WING_UP;
  if(context->action_message & IBUDDY_WING_DOWN_BIT)
    state = IBUDDY_WING_DOWN;

  return state;
}

const unsigned short ibuddy_get_body_state(ibuddy_ref * id)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;
  unsigned short state = 0;
  
  if(context->action_message & IBUDDY_FLICK_LEFT_BIT)
    state = IBUDDY_BODY_LEFT;
  if(context->action_message & IBUDDY_FLICK_RIGHT_BIT)
    state = IBUDDY_BODY_RIGHT;

  return state;
}

const unsigned short ibuddy_get_heat_state(ibuddy_ref * id)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;

  return context->action_message & IBUDDY_HEART_BIT ? IBUDDY_HEART_ON : IBUDDY_HEART_OFF;
}

void buddy_set_product_id(unsigned int vendor_id, unsigned int product_id)
{
 ibuddy_vendor_id  = vendor_id;
 ibuddy_product_id = product_id;
}

void ibuddy_reset_state(ibuddy_ref * id)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;

  ibuddy_reset_message(context);  

  if(context->auto_action)
    ibuddy_send_message(context,IBUDDY_MINIMAL_WAIT_MS);
}

int ibuddy_get_device_count()
{
  struct libusb_device **devs;
  struct libusb_device_descriptor info;
  int count, i,ibuddy_cnt = 0;

  count=libusb_get_device_list(NULL,&devs);

  if (count > 0){
    for(i=0;i<count;i++){
      libusb_get_device_descriptor(devs[i],&info);

      if(info.idVendor == ibuddy_vendor_id 
      && info.idProduct == ibuddy_product_id)
        ibuddy_cnt++;
    }

    libusb_free_device_list(devs,1);
  } else {
    return -1;
  }
  
  return ibuddy_cnt;
}

void * ibuddy_open(unsigned short ref) 
{
  struct ibuddy_context_t * context = NULL;
  struct libusb_device **devs;
  struct libusb_device_descriptor info;
  int count, i,ibuddy_cnt = 0;
  libusb_device_handle * handle;

  count=libusb_get_device_list(NULL,&devs);

  if(count <= 0)
    return NULL;

  for(i=0;i<count;i++){
    libusb_get_device_descriptor(devs[i],&info);
    
    if(info.idVendor == ibuddy_vendor_id
    && info.idProduct == ibuddy_product_id) {

      if(ibuddy_cnt == ref) {
        context = (struct ibuddy_context_t *)malloc(sizeof(struct ibuddy_context_t));
  
        if(!context)
          return NULL;
  
        memset(context,0,sizeof(struct ibuddy_context_t));

        context->auto_action = 1;
 	ibuddy_reset_message(context);

        if(libusb_open(devs[i],&(context->handle)) == 0) {
          libusb_free_device_list(devs,1);
          return (void *)context;
        }
          
      }
      ibuddy_cnt++;
    }
  }

  libusb_free_device_list(devs,1);
  return NULL;
}

void ibuddy_close(ibuddy_ref * id)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;
  
  /* Close the device handle and free the context */
  libusb_close(context->handle);
  free(context);
}

void ibuddy_flick_body(ibuddy_ref * id,int number,int delay)
{
  int i;
  for(i=0; i < number ; i++){
    ibuddy_set_body_state(id, IBUDDY_BODY_LEFT);                             
    ibuddy_set_body_state(id, IBUDDY_BODY_RIGHT);                           
    microsleep(delay); 
  }
}

void ibuddy_flap_wings(ibuddy_ref * id,int number,int delay)
{
  int i;
  for(i=0; i < number ; i++){
    ibuddy_set_wing_state(id, IBUDDY_WING_UP);   
    ibuddy_set_wing_state(id, IBUDDY_WING_DOWN);   
    microsleep(delay); 
  }
}

void ibuddy_blink_heart(ibuddy_ref * id,int number,int delay)
{
  int i;
  for(i=0; i < number ; i++){
    ibuddy_set_heart_state(id, IBUDDY_HEART_ON);                           
    ibuddy_set_heart_state(id, IBUDDY_HEART_OFF);                             
    microsleep(delay); 
  }
}

void ibuddy_set_all_state(ibuddy_ref * id,unsigned short head_color,
                                      unsigned short wing_state,
                                      unsigned short body_state,
                                      unsigned short hearth_state,
                                      int            delay)
{
  struct ibuddy_context_t * context = (struct ibuddy_context_t *)id;
  int save_auto_sate = context->auto_action;

  /* deactivate the auto action */
  context->auto_action = 0;

  ibuddy_set_head_color(id,head_color);
  ibuddy_set_wing_state(id,wing_state);
  ibuddy_set_body_state(id,body_state);
  ibuddy_set_heart_state(id,hearth_state);

  ibuddy_send_message(context,delay);   

  context->auto_action = save_auto_sate;
}

/*
 * This a full demo of the i-buddy capabilities
 */
void ibuddy_full_test(ibuddy_ref * id)
{
  /* reset i-buddy */
  ibuddy_reset_state(id);
  /* change head color */
  ibuddy_set_head_color(id,IBUDDY_HEAD_RED);                   /* 001 */
  ibuddy_set_head_color(id,IBUDDY_HEAD_GREEN);                 /* 010 */
  ibuddy_set_head_color(id,IBUDDY_HEAD_RED|IBUDDY_HEAD_GREEN); /* 011 */
  ibuddy_set_head_color(id,IBUDDY_HEAD_BLUE);                  /* 100 */
  ibuddy_set_head_color(id,IBUDDY_HEAD_RED|IBUDDY_HEAD_BLUE);  /* 101 */
  ibuddy_set_head_color(id,IBUDDY_HEAD_GREEN|IBUDDY_HEAD_BLUE);/* 110 */
  ibuddy_set_head_color(id,
  IBUDDY_HEAD_RED|IBUDDY_HEAD_GREEN|IBUDDY_HEAD_BLUE);         /* 111 */
  ibuddy_set_head_color(id,0);

  microsleep(200);

  /* bink heart */
  ibuddy_blink_heart(id, 5, 0);
  /* Reset State */
  ibuddy_flick_body(id, 5, 0);
  /* Reset State */
  ibuddy_reset_state(id);
  /* flap wings */
  ibuddy_flap_wings(id, 5, 0);
}

/*
 * Initialization functions
 */

void __attribute__ ((constructor)) ibuddy_init(void) {
  /* Init libusb */
  libusb_init(NULL);
}

void __attribute__ ((destructor))  ibuddy_fini(void) {
  /* Uninit libusb */
  libusb_exit(NULL);
}
