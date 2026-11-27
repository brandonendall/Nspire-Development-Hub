/* Included by MicroPython's objstr.c to reuse its string builder. */
typedef struct {
    unichar first;
    unichar last;
    int32_t delta;
} EpCaseRange;

typedef struct {
    unichar point;
    unichar mapped[3];
} EpCaseExpansion;

#include "text_case_data.h"

#define EP_CASE_COUNT(table) (sizeof(table) / sizeof((table)[0]))

static const EpCaseRange *ep_case_range(const EpCaseRange *ranges, size_t count, unichar point) {
    size_t low = 0;
    while (low < count) {
        size_t mid = low + (count - low) / 2;
        if (point < ranges[mid].first) {
            count = mid;
        } else if (point > ranges[mid].last) {
            low = mid + 1;
        } else {
            return &ranges[mid];
        }
    }
    return NULL;
}

static bool ep_is_cased(unichar point) {
    return ep_case_range(ep_cased, EP_CASE_COUNT(ep_cased), point) != NULL;
}

static bool ep_is_case_ignorable(unichar point) {
    return ep_case_range(ep_case_ignorable, EP_CASE_COUNT(ep_case_ignorable), point) != NULL;
}

static bool ep_followed_by_cased(const byte *cursor, const byte *end) {
    while (cursor < end) {
        unichar point = utf8_get_char(cursor);
        if (!ep_is_case_ignorable(point)) {
            return ep_is_cased(point);
        }
        cursor = utf8_next_char(cursor);
    }
    return false;
}

static void ep_append_case(vstr_t *out, unichar point, bool title) {
    const EpCaseRange *range = title
        ? ep_case_range(ep_title_ranges, EP_CASE_COUNT(ep_title_ranges), point)
        : ep_case_range(ep_lower_ranges, EP_CASE_COUNT(ep_lower_ranges), point);
    if (range != NULL) {
        vstr_add_char(out, (int32_t)point + range->delta);
        return;
    }

    const EpCaseExpansion *expansions = title ? ep_title_expansions : ep_lower_expansions;
    size_t high = title ? EP_CASE_COUNT(ep_title_expansions) : EP_CASE_COUNT(ep_lower_expansions);
    size_t low = 0;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        if (point < expansions[mid].point) {
            high = mid;
        } else if (point > expansions[mid].point) {
            low = mid + 1;
        } else {
            for (size_t i = 0; i < EP_CASE_COUNT(expansions[mid].mapped) && expansions[mid].mapped[i]; i++) {
                vstr_add_char(out, expansions[mid].mapped[i]);
            }
            return;
        }
    }
    vstr_add_char(out, point);
}

static mp_obj_t ep_str_case(mp_obj_t self_in, bool title_words) {
    GET_STR_DATA_LEN(self_in, data, len);
    vstr_t out;
    vstr_init(&out, len + 1);
    const byte *end = data + len;
    bool title = true;
    if (mp_obj_is_str(self_in)) {
        bool after_cased = false;
        while (data < end) {
            unichar point = utf8_get_char(data);
            data = utf8_next_char(data);
            if (!title && point == 0x03A3 && after_cased && !ep_followed_by_cased(data, end)) {
                vstr_add_char(&out, 0x03C2);
            } else {
                ep_append_case(&out, point, title);
            }
            bool cased = ep_is_cased(point);
            title = title_words && !cased;
            if (!ep_is_case_ignorable(point)) {
                after_cased = cased;
            }
        }
    } else {
        while (data < end) {
            byte ch = *data++;
            vstr_add_byte(&out, title ? unichar_toupper(ch) : unichar_tolower(ch));
            title = title_words && !unichar_isalpha(ch);
        }
    }
    return mp_obj_new_str_type_from_vstr(mp_obj_get_type(self_in), &out);
}

static mp_obj_t ep_str_title(mp_obj_t self_in) {
    return ep_str_case(self_in, true);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ep_str_title_obj, ep_str_title);

static mp_obj_t ep_str_capitalize(mp_obj_t self_in) {
    return ep_str_case(self_in, false);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ep_str_capitalize_obj, ep_str_capitalize);
