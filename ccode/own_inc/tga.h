// file    : rga.h
// author  : rb
// purpose : header file for tga.c
// date    : 250224
// last    : 261015
//

#ifndef _TGA_H_
#define _TGA_H_

// -- defines 

// -- structs 
// TGA header
typedef struct 
{
  uint8_t  id_length;        // length of ID field
  uint8_t  color_map_type;   // color map type
  uint8_t  image_type;       // image type
  uint16_t color_map_origin; // color map origin
  uint16_t color_map_length; // color map length
  uint8_t  color_map_depth;  // color map depth
  uint16_t x_origin;         // X origin of the image
  uint16_t y_origin;         // Y origin of the image
  uint16_t width;            // width of the image
  uint16_t height;           // height of the image
  uint8_t  pixel_depth;      // pixel depth
  uint8_t  image_descriptor; // image descriptor
} tga_header_t;

// -- protoypes
void tga_dump (uint8_t *tga_body, uint16_t width, uint16_t height);

void tga_header_generate (tga_header_t *header, uint16_t width, uint16_t height);
void tga_header_dump     (tga_header_t *header);
void tga_header_dump_raw (tga_header_t *header);

void tga_image_dump (tga_header_t *header, uint16_t width, uint16_t height, uint8_t *body);

void uart1_putc_raw (int c);

#endif















