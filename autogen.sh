#!/bin/sh
autoreconf -i 
if test -f osl/autogen.sh; then
	(cd osl; ./autogen.sh)
fi
if test -f candl/autogen.sh; then
	(cd candl; ./autogen.sh)
fi
if test -f pluto/autogen.sh; then
	(cd candl; ./autogen.sh)
fi
