/* EndraGBA ARv3 single, aligned RAM assignments, in emulated memory. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

static uint32_t addresses[10], values[10];

static unsigned char widths[10], once[10];
static char current_rom[512];

static int add_code(const char *name, uint32_t a, uint32_t b, int single_use)
{
  const uint32_t delta = 0x9e3779b9u;
  uint32_t sum = delta * 32u;
  unsigned i;
  if (num_cheats >= 10 || !*name || strlen(name) > 16) return 0;
  for (i = 0; i < 32; ++i) {
    b -= ((a << 4) + 0xc0efaad5u) ^ (a + sum) ^ ((a >> 5) + 0x42712c57u);
    a -= ((b << 4) + 0x7aa9648fu) ^ (b + sum) ^ ((b >> 5) + 0x7fae6994u);
    sum -= delta;
  }
  unsigned op = a >> 24;
  if (op != 0 && op != 2 && op != 4) return 0;
  unsigned width = 1u << (op / 2);
  if ((a & (width - 1)) || (width < 4 && (b >> (width * 8)))) return 0;
  a = (a & 0xfffffu) | ((a << 4) & 0x0f000000u);
  if (!((a >= 0x02000000u && a < 0x02040000u) ||
        (a >= 0x03000000u && a < 0x03008000u))) return 0;
  /* The legacy menu later uses this label in a printf format string. */
  for (const char *p = name; *p; ++p)
    if (!isalnum((unsigned char)*p) && !strchr(" -_.'", *p)) return 0;
  strcpy((char *)cheats[num_cheats].cheat_name, name);
  widths[num_cheats] = width;
  once[num_cheats] = single_use;
  addresses[num_cheats] = a;
  values[num_cheats++] = b;
  return 1;
}

static int hexword(const char *s, uint32_t *v)
{
  uint32_t n = 0;
  for (int i = 0; i < 8; ++i) {
    unsigned c = (unsigned char)s[i];
    if (!isxdigit(c)) return 0;
    n = n * 16u + (isdigit(c) ? c - '0' : (unsigned)tolower(c) - 'a' + 10u);
  }
  *v = n;
  return 1;
}

static void load_ar3(const char *rom, const char *id)
{
  char path[512], line[100], expected[20];
  FILE *fp;
  memset(cheats, 0, sizeof(cheats));
  num_cheats = 0;
  if (!strcmp(id, "AU2E")) add_code("Chaos Ring", 0xcbd43a43u, 0x720490c7u, 1);
  size_t n = strlen(rom);
  if (n >= sizeof(current_rom)) { current_rom[0] = 0; return; }
  if (rom != current_rom) memcpy(current_rom, rom, n + 1);
  if (n >= 4 && !strcasecmp(rom+n-4, ".tns")) n -= 4;
  while (n && rom[n-1] != '.' && rom[n-1] != '/') --n;
  if (!n || rom[n-1] != '.' || n + sizeof("cht.tns") > sizeof(path)) return;
  memcpy(path, rom, n);
  strcpy(path+n, "cht.tns");
  fp = fopen(path, "r");
  if (!fp) return;
  snprintf(expected, sizeof(expected), "GAME %.4s", id);
  if (!fgets(line, sizeof(line), fp)) goto done;
  line[strcspn(line, "\r\n")] = 0;
  if (strcmp(line, expected)) goto done;
  while (num_cheats < 10 && fgets(line, sizeof(line), fp)) {
    char *eq;
    uint32_t a, b;
    if (!strchr(line, '\n') && !feof(fp)) {
      int c;
      while ((c = fgetc(fp)) != EOF && c != '\n') {}
      continue;
    }
    line[strcspn(line, "\r\n")] = 0;
    eq = strchr(line, '=');
    if (!eq) continue;
    *eq++ = 0;
    if (strlen(eq) == 17 && eq[8] == ' ' &&
        hexword(eq, &a) && hexword(eq + 9, &b)) add_code(line + (line[0] == '!'), a, b, line[0] == '!');
  }
done:
  fclose(fp);
}

static void apply_ar3(void)
{
  for (unsigned i = 0; i < num_cheats; ++i) {
    if (!cheats[i].cheat_active) continue;
    switch (widths[i]) {
      case 1: write_memory8(addresses[i], values[i]); break;
      case 2: write_memory16(addresses[i], values[i]); break;
      case 4: write_memory32(addresses[i], values[i]); break;
    }
    if (once[i]) cheats[i].cheat_active = 0;
  }
}
