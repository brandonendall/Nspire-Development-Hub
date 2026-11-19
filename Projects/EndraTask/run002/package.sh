#!/usr/bin/env bash
set -euo pipefail

project=Projects/EndraTask
out=dist/EndraTask_Run002
assembly=$(mktemp -d)
trap 'rm -rf "$assembly"' EXIT

mkdir -p "$out" "$assembly"/{endratask/run001,endratask/run002,nkits/TaskManager/src,nkits/common}
cp "$project/run002/EndraTask_Run002.tns" "$project/run002/README.md" nkits/LICENSE "$out/"
cp "$project/run001/"{Makefile,README.md,probe.c,probe.h,test_probe.c,native-readonly.patch} "$assembly/endratask/run001/"
cp "$project/run002/"{Makefile,README.md,main.c} "$assembly/endratask/run002/"
cp nkits/TaskManager/src/{native.c,tasks.c,tasks.h} "$assembly/nkits/TaskManager/src/"
cp nkits/common/{ui.c,ui.h,draw.c,draw.h} "$assembly/nkits/common/"
cp nkits/LICENSE "$assembly/"
tar -czf "$out/EndraTask_Run002_Source.tar.gz" -C "$assembly" .
(cd "$out" && sha256sum EndraTask_Run002.tns EndraTask_Run002_Source.tar.gz > SHA256SUMS && sha256sum -c SHA256SUMS)
