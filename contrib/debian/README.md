
Debian
====================
This directory contains files used to package bpqd/bpq-qt
for Debian-based Linux systems. If you compile bpqd/bpq-qt yourself, there are some useful files here.

## bpq: URI support ##


bpq-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install bpq-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your bpq-qt binary to `/usr/bin`
and the `../../share/pixmaps/bpq128.png` to `/usr/share/pixmaps`

bpq-qt.protocol (KDE)

