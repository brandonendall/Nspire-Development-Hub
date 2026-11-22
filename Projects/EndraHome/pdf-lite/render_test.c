#include <mupdf/fitz.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    assert(argc == 2);
    fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
    assert(ctx);
    fz_register_document_handler(ctx, &pdf_document_handler);
    fz_document *doc = fz_open_document(ctx, argv[1]);
    assert(fz_count_pages(ctx, doc) == 2);
    for (int n = 0; n < 2; ++n) {
        fz_page *page = fz_load_page(ctx, doc, n);
        fz_pixmap *pix = fz_new_pixmap(ctx, fz_device_rgb(ctx), 200, 200);
        fz_clear_pixmap_with_value(ctx, pix, 255);
        fz_device *dev = fz_new_draw_device(ctx, pix);
        fz_run_page(ctx, page, dev, &fz_identity, NULL);
        fz_drop_device(ctx, dev);
        unsigned char *samples = fz_pixmap_samples(ctx, pix);
        int stride = fz_pixmap_components(ctx, pix), dark = 0, red = 0;
        for (int i = 0; i < 40000; ++i) {
            unsigned char *pixel = samples + stride * i;
            dark += pixel[0] < 100 && pixel[1] < 100 && pixel[2] < 100;
            red += pixel[0] > 180 && pixel[1] < 70 && pixel[2] < 70;
        }
        if (n == 0) assert(dark > 100 && red == 0);
        else assert(red > 1000);
        fz_drop_pixmap(ctx, pix);
        fz_drop_page(ctx, page);
    }
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);
    puts("PASS: rendered text and JPEG image pages from a .pdf.tns file");
}
