#include "app.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "catalog_index.h"

const CatalogEntry *catalog_entries;
int catalog_count,catalog_index,catalog_group;
char catalog_filter[40];
const int catalog_group_count = sizeof catalog_groups / sizeof catalog_groups[0];
int catalog_browsing_modules;
static const struct {const char *name,*syntax;} common[]={
    {"print","print(value, ...)"},{"input","input([prompt])"},{"open","open(file[, mode])"},{"len","len(obj)"},
    {"range","range(start, stop[, step])"},{"int","int(value)"},{"float","float(value)"},
    {"str","str(value)"},{"list","list(iterable)"},{"dict","dict(...)"},{"sum","sum(iterable)"},
    {"min","min(iterable)"},{"max","max(iterable)"},{"abs","abs(number)"},
    {"type","type(obj)"},{"dir","dir([obj])"},{"help","help([obj])"}
};
static int qualified(void) {
    return catalog_group && strcmp(catalog_groups[catalog_group], "builtins");
}
static void load_group(void) {
    catalog_index = 0;
    catalog_filter[0] = 0;
    catalog_entries = catalog_members + catalog_ranges[catalog_group].offset;
    catalog_count = catalog_ranges[catalog_group].count;
}
void catalog_open(void){
    app.origin=app.page;
    app.page=CATALOG;
    catalog_group=0;
    catalog_browsing_modules=1;
    catalog_filter[0]=0;
    catalog_count=0;
}
void catalog_name(char *out,size_t size){
    const char *name=catalog_entries[catalog_index].name;
    if(!qualified())snprintf(out,size,"%s",name);
    else snprintf(out,size,"%s.%s",catalog_groups[catalog_group],name);
}
const char *catalog_signature(void){
    if(!qualified())for(size_t i=0;i<sizeof common/sizeof common[0];i++)
        if(!strcmp(catalog_entries[catalog_index].name,common[i].name))return common[i].syntax;
    static const struct { const char *name, *signature; } signatures[] = {
        {"display.fill_rect", "display.fill_rect(x,y,w,h,rgb)"},
        {"display.draw_string", "display.draw_string(x,y,text[,rgb[,font]])"},
        {"keys.keydown", "keys.keydown(key)"}, {"time.sleep_ms", "time.sleep_ms(ms)"},
        {"time.ticks_diff", "time.ticks_diff(a, b)"},
        {"numpy.array", "numpy.array(value[, dtype])"}, {"numpy.dot", "numpy.dot(a, b)"},
        {"numpy.matmul", "numpy.matmul(a, b)"}, {"numpy.transpose", "numpy.transpose(a)"},
        {"numpy.norm", "numpy.norm(vector)"}, {"numpy.normalize", "numpy.normalize(vector)"},
        {"os.getcwd", "os.getcwd()"}, {"os.chdir", "os.chdir(path)"},
        {"os.listdir", "os.listdir([path])"}, {"os.mkdir", "os.mkdir(path)"},
        {"json.dump", "json.dump(obj, file)"}, {"json.load", "json.load(file)"},
        {"json.dumps", "json.dumps(obj)"}, {"json.loads", "json.loads(text)"},
        {"math.sqrt", "math.sqrt(x)"}, {"math.sin", "math.sin(radians)"},
        {"math.cos", "math.cos(radians)"}, {"math.tan", "math.tan(radians)"},
        {"math.atan2", "math.atan2(y, x)"}, {"math.floor", "math.floor(x)"},
        {"math.ceil", "math.ceil(x)"}, {"math.log", "math.log(x[, base])"},
        {"math.exp", "math.exp(x)"}, {"math.pow", "math.pow(x, y)"},
        {"display.get_pixel", "display.get_pixel(x, y)"},
        {"display.set_pixel", "display.set_pixel(x, y[, rgb])"},
        {"display.clear_screen", "display.clear_screen([rgb])"},
        {"display.show_screen", "display.show_screen()"},
        {"gint.dclear", "gint.dclear(rgb565)"}, {"gint.dpixel", "gint.dpixel(x, y, rgb565)"},
        {"gint.drect", "gint.drect(x1, y1, x2, y2, rgb565)"},
        {"gint.dtext", "gint.dtext(x, y, rgb565, text)"}, {"gint.dupdate", "gint.dupdate()"},
        {"gint.keydown", "gint.keydown(key)"}, {"ion.keydown", "ion.keydown(key)"},
        {"display.color", "display.color(r, g, b)"}, {"time.ticks_ms", "time.ticks_ms()"},
        {"time.sleep", "time.sleep(seconds)"},
        {"pygame.draw.rect", "pygame.draw.rect(surface,color,rect[,width])"},
        {"pygame.draw.line", "pygame.draw.line(surface,color,start,end[,width])"},
        {"pygame.display.set_mode", "pygame.display.set_mode([size,flags,depth])"},
        {"pygame.display.flip", "pygame.display.flip()"},
        {"pygame.display.update", "pygame.display.update([rect])"},
        {"pygame.event.pump", "pygame.event.pump()"}, {"pygame.event.poll", "pygame.event.poll()"},
        {"pygame.event.get", "pygame.event.get([types])"}, {"pygame.event.post", "pygame.event.post(event)"},
        {"pygame.event.clear", "pygame.event.clear()"}, {"pygame.key.get_pressed", "pygame.key.get_pressed()"},
        {"pygame.image.load", "pygame.image.load(source[,namehint])"},
        {"pygame.font.Font", "pygame.font.Font(file,size)"}, {"pygame.time.Clock", "pygame.time.Clock()"},
        {"pygame.sprite.Sprite", "pygame.sprite.Sprite(*groups)"},
        {"pygame.sprite.Group", "pygame.sprite.Group(*sprites)"},
        {"pygame.init", "pygame.init()"}, {"pygame.quit", "pygame.quit()"},
        {"pygame.Surface", "pygame.Surface(size[, flags, depth])"},
        {"pygame.Rect", "pygame.Rect(x, y, width, height)"},
        {"gc.collect", "gc.collect()"}, {"gc.mem_free", "gc.mem_free()"},
        {"array.array", "array.array(typecode[, iterable])"},
        {"io.BytesIO", "io.BytesIO([data])"}, {"io.StringIO", "io.StringIO([text])"},
        {"random.randint", "random.randint(a, b)"}, {"random.choice", "random.choice(seq)"},
        {"struct.pack", "struct.pack(fmt, values...)"}, {"struct.unpack", "struct.unpack(fmt, data)"},
    };
    char name[80];
    catalog_name(name, sizeof name);
    for (size_t i = 0; i < sizeof signatures / sizeof signatures[0]; i++)
        if (!strcmp(name, signatures[i].name)) return signatures[i].signature;
    return NULL;
}
static void find_prefix(void) {
    size_t len = strlen(catalog_filter);
    int count = catalog_browsing_modules ? catalog_group_count : catalog_count;
    for (int i = 0; i < count; i++) {
        const char *name = catalog_browsing_modules ? catalog_groups[i] : catalog_entries[i].name;
        if (strncasecmp(name, catalog_filter, len)) continue;
        if (catalog_browsing_modules) catalog_group = i;
        else catalog_index = i;
        return;
    }
    strcpy(app.status, "No matching name");
}

void catalog_key(int k) {
    if (k == CATALOG_KEY || k == MENU) { app.page = app.origin; return; }
    if (!catalog_browsing_modules && (k == LEFT || k == ESC)) {
        catalog_browsing_modules = 1;
        catalog_filter[0] = 0;
        return;
    }
    if (k == ESC) { app.page = app.origin; return; }
    int count = catalog_browsing_modules ? catalog_group_count : catalog_count;
    int *selection = catalog_browsing_modules ? &catalog_group : &catalog_index;
    size_t len = strlen(catalog_filter);
    if (k == 8 && len) {
        catalog_filter[len-1] = 0;
        find_prefix();
    } else if ((k >= 'a' && k <= 'z') || (k >= 'A' && k <= 'Z') ||
               (k >= '0' && k <= '9') || k == '_' || k == '.') {
        if (len+1 < sizeof catalog_filter) {
            catalog_filter[len] = k;
            catalog_filter[len+1] = 0;
            find_prefix();
        }
    } else if ((k == UP || k == DOWN) && count) {
        catalog_filter[0] = 0;
        *selection = (*selection + count + (k == UP ? -1 : 1)) % count;
    } else if ((k == ENTER || k == RIGHT) && catalog_browsing_modules) {
        if (strncasecmp(catalog_groups[catalog_group], catalog_filter, len)) return;
        catalog_browsing_modules = 0;
        load_group();
    } else if (k == INDENT && !catalog_browsing_modules && qualified()) {
        char statement[80];
        snprintf(statement, sizeof statement, "import %s", catalog_groups[catalog_group]);
        app.page = app.origin;
        app_insert_text(statement);
    } else if (k == ENTER && count) {
        if (strncasecmp(catalog_entries[catalog_index].name, catalog_filter, len)) return;
        char value[80];
        catalog_name(value, sizeof value);
        if (catalog_entries[catalog_index].callable) strcat(value, "(");
        app.page = app.origin;
        app_insert_text(value);
    }
}
