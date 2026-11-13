#ifndef ULTRAZIP_HOST_LIBNDLS_H
#define ULTRAZIP_HOST_LIBNDLS_H
#define SCR_TYPE_INVALID 0
#define SCR_320x240_565 1
enum { KEY_NSPIRE_ESC=1, KEY_NSPIRE_ENTER, KEY_NSPIRE_UP,
       KEY_NSPIRE_DOWN, KEY_NSPIRE_LEFT, KEY_NSPIRE_RIGHT,
       KEY_NSPIRE_MENU, KEY_NSPIRE_1, KEY_NSPIRE_2, KEY_NSPIRE_3 };
int lcd_init(int type);
void lcd_blit(const void *buffer, int type);
void wait_no_key_pressed(void);
void wait_key_pressed(void);
int isKeyPressed(int key);
#endif
