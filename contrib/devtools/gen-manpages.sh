#!/bin/bash

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

BPQD=${BPQD:-$SRCDIR/bpqd}
BPQCLI=${BPQCLI:-$SRCDIR/bpq-cli}
BPQTX=${BPQTX:-$SRCDIR/bpq-tx}
BPQQT=${BPQQT:-$SRCDIR/qt/bpq-qt}

[ ! -x $BPQD ] && echo "$BPQD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
BTCVER=($($BPQCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for bpqd if --version-string is not set,
# but has different outcomes for bpq-qt and bpq-cli.
echo "[COPYRIGHT]" > footer.h2m
$BPQD --version | sed -n '1!p' >> footer.h2m

for cmd in $BPQD $BPQCLI $BPQTX $BPQQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${BTCVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${BTCVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
