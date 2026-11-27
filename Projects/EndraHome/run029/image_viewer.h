#ifndef EH018_IMAGE_VIEWER_H
#define EH018_IMAGE_VIEWER_H
#include "../gba-ui/logic.h"
#include <stdint.h>
#define EH18_IMAGE_FILE_LIMIT (5u*1024u*1024u)
#define EH18_IMAGE_PIXELS_LIMIT (1024u*1024u)
#define EH19_IMAGE_DECODE_PIXELS_LIMIT (2u*1024u*1024u)
#define EH18_IMAGE_VIEW_TOP 0
#define EH18_IMAGE_VIEW_HEIGHT 240
typedef struct {
 uint16_t *pixels; /* RGB565, owned; NULL unless image loaded */
 int width,height,zoom,fit_zoom,pan_x,pan_y;
 char error[48];
} Eh18Image;
int eh_app_icon_draw(uint16_t *frame,int slot,const char *app,int x,int y);
int eh18_is_image_name(const char *path);
int eh18_image_open(Eh18Image *image,const char *path);
void eh18_image_close(Eh18Image *image);
int eh18_image_zoom(Eh18Image *image,int direction);
int eh18_image_key(Eh18Image *image,EhKey key);
int eh18_image_pan(Eh18Image *image,int dx,int dy);
void eh18_image_render(uint16_t *frame,const Eh18Image *image);
/* Ndless edge-detected zoom input: multiply or plus=in, divide or minus=out. */
int eh18_zoomkey(void);
#endif
