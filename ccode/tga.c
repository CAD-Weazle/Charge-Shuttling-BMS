// file    : tga.c
// author  : rb/AI
// purpose : create TGA image from LCD pixel data
// date    : 250224
// last    : 250225
//

#include "includes.h"

// TGA header data
tga_header_t tga_header;


// dump TGA header only - binary for logging to image file, image data is written in 'lcd.c'
void tga_dump (uint8_t *body, uint16_t width, uint16_t height)
{
  // generate TGA image header
  tga_header_generate (&tga_header, width, height); 

  // dump TGA header
  tga_header_dump_raw (&tga_header);
}

// generate TGA image header
void tga_header_generate (tga_header_t *header, uint16_t width, uint16_t height)
{
  // initialize the TGA header
  header->id_length        = 0;
  header->color_map_type   = 0;
  header->image_type       = 2;        // uncompressed true color
  header->color_map_origin = 0;
  header->color_map_length = 0;
  header->color_map_depth  = 0;
  header->x_origin         = 0;
  header->y_origin         = 0;
  header->width            = width;
  header->height           = height;
  header->pixel_depth      = 24;       // 24 bits per pixel (RGB)
  header->image_descriptor = 0;
}

// dump TGA image header - binary for logging to image file
void tga_header_dump_raw (tga_header_t *header)
{
  uart1_putc_raw (header->id_length);
  uart1_putc_raw (header->color_map_type);
  uart1_putc_raw (header->image_type);
  uart1_putc_raw ((uint8_t)((header->color_map_origin>>0) & 0xff));
  uart1_putc_raw ((uint8_t)((header->color_map_origin>>8) & 0xff));
  uart1_putc_raw ((uint8_t)((header->color_map_length>>0) & 0xff));
  uart1_putc_raw ((uint8_t)((header->color_map_length>>8) & 0xff));
  uart1_putc_raw (header->color_map_depth);
  uart1_putc_raw ((uint8_t)((header->x_origin>>0) & 0xff));
  uart1_putc_raw ((uint8_t)((header->x_origin>>8) & 0xff));
  uart1_putc_raw ((uint8_t)((header->y_origin>>0) & 0xff));
  uart1_putc_raw ((uint8_t)((header->y_origin>>8) & 0xff));
  uart1_putc_raw ((uint8_t)((header->width>>0) & 0xff));
  uart1_putc_raw ((uint8_t)((header->width>>8) & 0xff));
  uart1_putc_raw ((uint8_t)((header->height>>0) & 0xff));
  uart1_putc_raw ((uint8_t)((header->height>>8) & 0xff));
  uart1_putc_raw (header->pixel_depth);
  uart1_putc_raw (header->image_descriptor);
}

// dump TGA image header - ASCII for debug
void tga_header_dump (tga_header_t *header)
{
  printf ("id_length       : %d\n", header->id_length);
  printf ("color_map_type  : %d\n", header->color_map_type);
  printf ("image_type      : %d\n", header->image_type);
  printf ("color_map_origin: %d\n", header->color_map_origin);
  printf ("color_map_length: %d\n", header->color_map_length);
  printf ("color_map_depth : %d\n", header->color_map_depth);
  printf ("x_origin        : %d\n", header->x_origin);
  printf ("y_origin        : %d\n", header->y_origin);
  printf ("width           : %d\n", header->width);
  printf ("height          : %d\n", header->height);
  printf ("pixel_depth     : %d\n", header->pixel_depth);
  printf ("image_descriptor: %d\n", header->image_descriptor);
}

// raw mode 'putc' for writing binary data to terminal
void uart1_putc_raw (int c)
{
  // write character
  USART1->TDR = c;
  USART1->CR1 |= USART_CR1_TXEIE;

  // relax a bit, give UART time to write character
  usleep (100);
}