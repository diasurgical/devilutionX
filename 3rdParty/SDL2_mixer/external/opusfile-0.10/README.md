# Opusfile

[![Travis Build Status](https://travis-ci.org/xiph/opusfile.svg?branch=master)](https://travis-ci.org/xiph/opusfile)
[![Jenkins Build Status](https://mf4.xiph.org/jenkins/buildStatus/icon?job=opusfile-autotools)](https://mf4.xiph.org/jenkins/view/opus/job/opusfile-autotools/)
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/xiph/opusfile?branch=master&svg=true)](https://ci.appveyor.com/project/rillian/opusfile)

The opusfile and opusurl libraries provide a high-level API for
decoding and seeking within .opus files on disk or over http(s).

opusfile depends on libopus and libogg.
opusurl depends on opusfile and openssl.

The library is functional, but there are likely issues
we didn't find in our own testing. Please give feedback
in #opus on irc.freenode.net or at opus@xiph.org.

Programming documentation is available in tree and online at
https://opus-codec.org/docs/
