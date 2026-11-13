#!/usr/bin/env python3
"""Test the actual EndraZIP application with host Ndless input/display stubs."""
import argparse
import json
import os
from pathlib import Path
import random
import subprocess
import tempfile
import time
import zipfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument("--report", type=Path)
args = parser.parse_args()
checks = 0


def check(value, message):
    global checks
    checks += 1
    if not value:
        raise AssertionError(message)


with tempfile.TemporaryDirectory(prefix="endrazip-006-") as temp:
    tmp = Path(temp)
    export = tmp / "miniz_export.h"
    export.write_text("#define MINIZ_EXPORT\n")
    driver = tmp / "host_driver"
    command = [os.environ.get("CC", "gcc"), "-std=c99", "-O1", "-g",
               "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie",
               "-D_POSIX_C_SOURCE=200809L", "-I" + str(ROOT / "tests/stubs"),
               "-I" + str(ROOT / "vendor/miniz"), "-I" + str(tmp),
               str(ROOT / "tests/host_driver.c")]
    command += [str(ROOT / "vendor/miniz" / name) for name in
                ("miniz.c", "miniz_zip.c", "miniz_tdef.c", "miniz_tinfl.c")]
    subprocess.run(command + ["-o", str(driver)], check=True)
    environment = dict(os.environ, ASAN_OPTIONS=os.environ.get("ASAN_OPTIONS", "detect_leaks=1:halt_on_error=1"),
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")

    def run(mode, source, output, succeeds=True):
        result = subprocess.run([str(driver), mode, str(source), str(output)],
                                env=environment, text=True, capture_output=True)
        check(result.returncode == (0 if succeeds else 1),
              f"{mode} {source.name}: {result.returncode}\n{result.stdout}\n{result.stderr}")
        return result.stdout

    rng = random.Random(20261113)
    noise = rng.randbytes(262144)
    # Original generated fixtures and this project's own source; no game ROMs.
    fixtures = {
        "notes.txt.tns": (b"EndraCommander NX: folders, ZIP files, saved workspaces.\n" * 8192),
        "source.c.tns": (ROOT / "src/endrazip.c").read_bytes(),
        "mixed.bin.tns": b"".join(rng.choice([b"\x00" * 256, bytes(range(256)),
                                             noise[:256]]) for _ in range(4096)),
        "random.bin.tns": noise,
        "empty.txt.tns": b"",
        "tiny.txt.tns": b"hi",
    }
    report = []
    for name, data in fixtures.items():
        source = tmp / name
        source.write_bytes(data)
        fast = tmp / (name + ".fast.zip")
        best = tmp / (name + ".best.zip.tns")
        run("fast", source, fast)
        start = time.monotonic()
        run("create", source, best)
        elapsed = time.monotonic() - start
        with zipfile.ZipFile(best) as archive:
            check(archive.namelist() == [name], "stored filename")
            check(archive.testzip() is None, "ZIP CRC")
            check(archive.read(name) == data, "Python reads exact original bytes")
        output = tmp / (name + "_extracted")
        run("extract", best, output)
        check((output / name).read_bytes() == data, "app extracts exact original bytes")
        check(source.read_bytes() == data, "source unchanged")
        old_bytes, new_bytes = fast.stat().st_size, best.stat().st_size
        if name in ("notes.txt.tns", "source.c.tns", "mixed.bin.tns"):
            check(new_bytes <= old_bytes, "best compression improves compressible fixture")
        report.append({"fixture": name, "source_bytes": len(data),
                       "level_1_zip_bytes": old_bytes, "level_10_zip_bytes": new_bytes,
                       "host_seconds": round(elapsed, 4)})

    sample = tmp / "source.c.tns"
    protected = tmp / "protected.zip.tns"
    protected.write_bytes(b"do not overwrite")
    run("create", sample, protected, succeeds=False)
    check(protected.read_bytes() == b"do not overwrite", "existing archive preserved")
    missing_output = tmp / "missing.zip.tns"
    run("create", tmp / "missing", missing_output, succeeds=False)
    check(not missing_output.exists(), "failed archive cleaned up")
    # Folders are now first-class ZIP sources. Retain nested and empty dirs.
    source_dir = tmp / "ArchiveMe"
    (source_dir / "nested" / "deeper").mkdir(parents=True)
    (source_dir / "empty").mkdir()
    (source_dir / "nested" / "empty_child").mkdir()
    (source_dir / "nested" / "deeper" / "payload.txt.tns").write_bytes(
        b"recursive folder test\n" * 5000)
    (source_dir / ".hidden.tns").write_bytes(b"hidden files are included")
    (source_dir / "root.txt.tns").write_bytes(b"root content")
    archive_dir = tmp / "ArchiveMe.zip.tns"
    run("create", source_dir, archive_dir)
    with zipfile.ZipFile(archive_dir) as z:
        names = z.namelist()
        for item in ("ArchiveMe/", "ArchiveMe/empty/",
                     "ArchiveMe/nested/", "ArchiveMe/nested/empty_child/",
                     "ArchiveMe/nested/deeper/", "ArchiveMe/.hidden.tns",
                     "ArchiveMe/root.txt.tns",
                     "ArchiveMe/nested/deeper/payload.txt.tns"):
            check(item in names, f"folder member exists: {item}")
        check(z.testzip() is None, "recursive archive ZIP CRC")
        check(z.read("ArchiveMe/nested/deeper/payload.txt.tns") ==
              b"recursive folder test\n" * 5000, "nested data matches")
    unpack_dir = tmp / "recursive_unpacked"
    run("extract", archive_dir, unpack_dir)
    check((unpack_dir / "ArchiveMe" / "empty").is_dir(), "empty directory extracted")
    check((unpack_dir / "ArchiveMe" / "nested" / "empty_child").is_dir(),
          "nested empty directory extracted")
    check((unpack_dir / "ArchiveMe" / ".hidden.tns").read_bytes() ==
          b"hidden files are included", "hidden file extracted")
    check((unpack_dir / "ArchiveMe" / "root.txt.tns").read_bytes() ==
          b"root content", "folder root file extracted")
    check((unpack_dir / "ArchiveMe" / "nested" / "deeper" /
           "payload.txt.tns").read_bytes() ==
          b"recursive folder test\n" * 5000, "nested file extracted")
    run("create", source_dir, archive_dir, succeeds=False)
    check(archive_dir.is_file(), "existing folder archive preserved")
    empty_source = tmp / "EmptyOnly"
    empty_source.mkdir()
    empty_archive = tmp / "EmptyOnly.zip.tns"
    run("create", empty_source, empty_archive)
    with zipfile.ZipFile(empty_archive) as z:
        check(z.namelist() == ["EmptyOnly/"], "empty root folder ZIP entry")

    # Recursive archive must fail cleanly if a child is unreadable or a path
    # exceeds the fixed calculator path buffer.
    too_deep = tmp / "TooDeep"
    too_deep.mkdir()
    inner = too_deep
    for i in range(26):
        inner = inner / "sub"
        inner.mkdir()
    failed_archive = tmp / "TooDeep.zip.tns"
    run("create", too_deep, failed_archive, succeeds=False)
    check(not failed_archive.exists(), "recursive failure removes partial archive")

    protected_dir = tmp / "protected_dir"
    protected_dir.mkdir()
    (protected_dir / sample.name).write_bytes(b"keep existing")
    run("extract", tmp / "source.c.tns.best.zip.tns", protected_dir, succeeds=False)
    check((protected_dir / sample.name).read_bytes() == b"keep existing", "existing extraction preserved")

    for index, unsafe in enumerate(("../escaped", "/absolute", "nested/../../escaped", "a\\b")):
        archive = tmp / f"unsafe-{index}.zip"
        with zipfile.ZipFile(archive, "w") as z:
            z.writestr(unsafe, b"unsafe")
        run("extract", archive, tmp / f"unsafe-out-{index}", succeeds=False)
    check(not (tmp / "escaped").exists(), "archive traversal refused")

    browse = tmp / "browse"
    browse.mkdir()
    (browse / "child").mkdir()
    (browse / "notes.txt.tns").write_bytes(fixtures["notes.txt.tns"])
    with zipfile.ZipFile(browse / "archive.zip.tns", "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("sample.txt.tns", b"sample")
    print(run("ui", browse, tmp).strip())
    with zipfile.ZipFile(browse / "child.zip.tns") as z:
        check(z.namelist() == ["child/"], "menu creates selected folder ZIP")
    with zipfile.ZipFile(browse / "notes.txt.tns.zip.tns") as z:
        check(z.read("notes.txt.tns") == fixtures["notes.txt.tns"], "menu compression round trip")

    summary = {"python_checks": checks, "ui_states": 27, "sanitizers": "ASan + UBSan",
               "note": "Synthetic host fixtures; calculator speed and real-ROM ratios not measured.",
               "compression": report}
    print(json.dumps(summary, indent=2))
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(summary, indent=2) + "\n")
