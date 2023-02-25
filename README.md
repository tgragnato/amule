aMule
=====

![aMule](amule.png)

aMule is an eMule-like client for the eDonkey and Kademlia networks.

Overview
--------

aMule is a multi-platform client for the ED2K file sharing network and based on
the windows client eMule. aMule started in August 2003, as a fork of xMule,
which is a fork of lMule.

aMule currently supports Linux, FreeBSD, OpenBSD, Windows, MacOS X and X-Box on
both 32 and 64 bit computers.

aMule is intended to be as user friendly and feature rich as eMule and to
remain faithful to the look and feel of eMule so users familiar with either
aMule or eMule will be able switch between the two easily.

Since aMule is based upon the eMule codebase, new features in eMule tend to
find their way into aMule soon after their inclusion into eMule so users of
aMule can expect to ride the cutting-edge of ED2k clients.


Features
--------

* an all-in-one app called `amule`.
* a daemon app called `amuled`. It's amule but with no interface.
* a client for the server called `amulegui` to connect to a local or distant
  amuled.
* `amuleweb` to access amule from a web browser.
* `amulecmd` to access amule from the command line.


Compiling
---------

In general, compiling aMule should be as easy as running `cmake .` and `make`.
