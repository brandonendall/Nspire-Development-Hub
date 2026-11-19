#include "overlay.h"
#include "display.h"

void eh7_overlay(uint16_t *f, const EhUniversalUI *ui) {
    if (!ui->notice.text[0]) return;
    EhPalette p = eh_palette(ui->apps->appearance);
    eh_box(f,6,201,308,18,p.accent);
    eh_box(f,7,202,306,16,p.panel);
    eh_fit_text(f,12,206,ui->notice.text,p.ink,37);
}
