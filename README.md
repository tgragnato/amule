# aMule

[![C/C++ CI and CodeQL](https://github.com/tgragnato/amule/actions/workflows/ccpp.yml/badge.svg)](https://github.com/tgragnato/amule/actions/workflows/ccpp.yml)

![aMule](src/icons/amule.svg)

aMule is an eMule-like client for the eDonkey and Kademlia networks.

## Overview

aMule is a multi-platform client for the eD2k / Kad file-sharing network, originally a fork of the Windows client eMule (via xMule and lMule).
aMule started in August 2003.

Supported platforms today: Linux, FreeBSD, OpenBSD, NetBSD, macOS, on both x86_64 and ARM64.

aMule aims to stay close to eMule in look-and-feel so users moving between the two have minimal friction. New eMule protocol-level features are generally adopted into aMule shortly after.

## Features

* `amule` — all-in-one GUI client.
* `amuled` — headless daemon, no GUI.
* `amulegui` — remote GUI; connects to a local or remote `amuled` over the
  EC (External Connection) protocol.
* `amulecmd` — interactive CLI for a running `amuled`.
* `amuleapi` — REST API for a running `amuled`.

## Building from source

In general, compiling aMule should be as easy as running `cmake -B build` and `cmake --build build -j"$(nproc)"`.

## Documentation

* [User guide](docs/README.md)
* [Building and installing from source](docs/INSTALL.md)
* [Installing with Homebrew](docs/INSTALL_BINARIES.md)
* [amuleapi quick start](docs/QUICKSTART-AMULEAPI.md)
* [REST API reference](docs/api/REFERENCE.md)
* [API events](docs/api/EVENTS.md)
* [External Connections protocol](docs/EC_Protocol.md)
* [External Connections authentication](docs/EC_SRP.md)
* [eD2k protocol extensions](docs/ED2K_EXTENSIONS.md)
* [IPv6 server login and source exchange](docs/IPv6_Server_Sources.md)
* [NAT traversal protocol](docs/NAT_T_Protocol.md)
* [Chat spam protection](docs/ChatSpamFilter.md)
* [Logging format](docs/LOGGING.md)
* [Online signature specification](docs/amulesig.md)
* [Media metadata extraction](docs/MEDIA_PROBE.md)
* [Tray icon backends](docs/TrayIcon.md)
* [Download write-buffer limits](docs/WriteBufferLimits.md)
* [Translations](docs/translations.md)
* [Third-party components](docs/THIRDPARTY.md)
* [Authors](docs/AUTHORS.md)
* [Manual pages](docs/man/)
