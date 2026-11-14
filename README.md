# aMule

[![C/C++ CI and CodeQL](https://github.com/tgragnato/amule/actions/workflows/ccpp.yml/badge.svg)](https://github.com/tgragnato/amule/actions/workflows/ccpp.yml)

![aMule](src/icons/amule.svg)

aMule is an eMule-like client for the eDonkey and Kademlia networks.

## Overview

aMule is a multi-platform client for the eD2k / Kad file-sharing network,
originally a fork of the Windows client eMule (via xMule and lMule).
aMule started in August 2003.

Supported platforms today: Linux, FreeBSD, OpenBSD, NetBSD, macOS, and
Windows (MSYS2 / mingw-w64), on both x86_64 and ARM64.

aMule aims to stay close to eMule in look-and-feel so users moving between
the two have minimal friction. New eMule protocol-level features are
generally adopted into aMule shortly after.


## Features

* `amule` — all-in-one GUI client.
* `amuled` — headless daemon, no GUI.
* `amulegui` — remote GUI; connects to a local or remote `amuled` over the
  EC (External Connection) protocol.
* `amulecmd` — interactive CLI for a running `amuled`.
* `amuleapi` — REST API for a running `amuled`.


## Building from source

In general, compiling aMule should be as easy as running `cmake -B build` and `cmake --build build -j"$(nproc)"`.
