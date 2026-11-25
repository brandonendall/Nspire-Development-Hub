"""Feed physical SDK key identifiers to the same scanner linked into the TNS."""
from pathlib import Path
import os
import re
import subprocess
from tempfile import TemporaryDirectory

with TemporaryDirectory() as tmp:
    root = Path(tmp)
    source = Path("native/text_input.c").read_text()
    keys = sorted(set(re.findall(r"KEY_NSPIRE_\w+", source)))
    (root / "libndls.h").write_text(
        "typedef int t_key; enum {" + ",".join(keys) +
        "}; extern int isKeyPressed(int);\n")
    (root / "check.c").write_text("""
#include "libndls.h"
#include "app.h"
#include "text_input.h"
#include <assert.h>
static int key=-1;
int isKeyPressed(int k){return k==key;}
static int press(int k){key=-1;eh16_textkey();key=k;return eh16_textkey();}
int main(void){
    assert(press(KEY_NSPIRE_QUESEXCL)==MARKS);
    assert(!eh16_textkey());
    assert(eh16_key_event(0,press(KEY_NSPIRE_CAT),0)==CATALOG_KEY);
    assert(eh16_key_event(0,press(KEY_NSPIRE_CAT),1)==TI_INPUT);
    assert(eh16_key_event(0,0,0)==0);
    assert(eh16_key_event(0,press(KEY_NSPIRE_LP),1)=='[');
    eh16_key_event(0,0,0);eh16_key_event(0,0,1);eh16_key_event(0,0,0);
    assert(eh16_key_event(0,press(KEY_NSPIRE_RP),0)==']');
    assert(eh16_key_event(0,press(KEY_NSPIRE_RP),0)==')');
    assert(eh16_key_event(0,press(KEY_NSPIRE_S),1)==SAVE);
    assert(eh16_key_event(0,press(KEY_NSPIRE_QUESEXCL),0)==MARKS);
}
""")
    subprocess.run([os.environ.get("CC", "cc"), "-std=gnu99", "-I.", "-Inative",
                    "-I" + tmp, "native/text_input.c", root / "check.c", "-o", root / "check"], check=True)
    subprocess.run([root / "check"], check=True)
print("PASS: physical key identifiers, press edges, held and tapped Ctrl, punctuation and catalog.")
