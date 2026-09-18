#!/usr/bin/env bash
set -eu

usage() {
  echo "Usage: $0 <mlfq|mmap> [destination]" >&2
  exit 2
}

[ "$#" -ge 1 ] && [ "$#" -le 2 ] || usage

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
repo_root=$(CDPATH= cd -- "$script_dir/.." && pwd)
lab=$1

case "$lab" in
  mlfq)
    upstream_url="https://github.com/mit-pdos/xv6-riscv.git"
    upstream_commit="e90b2575ae6efd40927fedb2425a1fc54ffa23df"
    patch_file="$repo_root/labs/03-xv6-mlfq-scheduler/patches/mlfq.patch"
    default_destination="$repo_root/build/xv6-mlfq"
    ;;
  mmap)
    upstream_url="git://g.csail.mit.edu/xv6-labs-2022"
    upstream_commit="9cc6b8345397c1f06cc93ed3fbaa20709cb1984e"
    patch_file="$repo_root/labs/04-xv6-memory-mapping/patches/mmap.patch"
    default_destination="$repo_root/build/xv6-mmap"
    ;;
  *)
    usage
    ;;
esac

destination=${2:-$default_destination}
case "$destination" in
  /*) ;;
  *) destination="$repo_root/$destination" ;;
esac

if [ -e "$destination" ]; then
  echo "Destination already exists: $destination" >&2
  echo "Choose a new path or remove the existing generated tree yourself." >&2
  exit 1
fi

mkdir -p "$(dirname -- "$destination")"
git clone "$upstream_url" "$destination"
git -C "$destination" checkout --detach "$upstream_commit"
git -C "$destination" apply --check "$patch_file"
git -C "$destination" apply "$patch_file"

echo "Prepared $lab at $destination"
echo "Upstream commit: $upstream_commit"

