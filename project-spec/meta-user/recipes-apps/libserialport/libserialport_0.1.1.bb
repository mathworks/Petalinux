CRIPTION = "C++ library for serial port support."
SECTION = "libs"
LICENSE = "LGPLv3+"
HOMEPAGE = "http://sigrok.org/download/source/libserialport"

#SRC_URI = "${SOURCEFORGE_MIRROR}/libserial/libserial-${PV}.tar.gz"
SRC_URI = "https://sigrok.org/download/source/libserialport/libserialport-${PV}.tar.gz"

inherit autotools pkgconfig

SRC_URI[md5sum] = "b93f0325a6157198152b5bd7e8182b51"
SRC_URI[sha256sum] = "4a2af9d9c3ff488e92fb75b4ba38b35bcf9b8a66df04773eba2a7bbf1fa7529d"


