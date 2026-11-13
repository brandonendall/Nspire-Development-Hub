#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT="${1:-tools/ndless-smoke}"
IMAGE="${NSPIRE_BUILD_IMAGE:-bensuperpc/ndless:latest}"
case "$PROJECT" in
  ""|/*|*..*|*" "*|*"'"*|*"\""*) echo "Unsafe project directory: $PROJECT" >&2; exit 2 ;;
esac
if [ ! -f "$ROOT/$PROJECT/Makefile" ]; then
  echo "No Makefile under $PROJECT" >&2
  exit 2
fi
command -v docker >/dev/null || { echo "Docker is required; or use GitHub Actions" >&2; exit 1; }
echo "Using image: $IMAGE"
docker run --rm --entrypoint /bin/sh \
  --mount "type=bind,source=$ROOT,target=/work" \
  --workdir "/work/$PROJECT" "$IMAGE" \
  -c 'set -eu; command -v nspire-gcc; command -v genzehn; make -j2'
