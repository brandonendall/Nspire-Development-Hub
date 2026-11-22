#ifndef EH016_TEXT_INPUT_H
#define EH016_TEXT_INPUT_H
/* Nonblocking QWERTY press event. ASCII printable or 8 for delete.
 * Uses Ndless keypad API; returns a character only on a fresh key press.
 * Call on every frame (also outside Rename) to track key releases.
 * Never launches OS text dialogs or touches Ndless hooks. */
int eh16_textkey(void);
int eh16_key_event(int navigation,int text,int ctrl);
#endif
