#### MAKEFILES
BUILD_DIR = ../BUILD
BUILD_PATH = $(realpath $(BUILD_DIR))

FD_VERSION=1.2.4
ASN1C_VERSION=0.9.2
GTPNL_VERSION=0.9.3
LFDS_VERSION=0.9.4

build:
	mkdir -p $(BUILD_DIR)

freediameter: build
	mkdir -p freediameter/build
	cd freediameter/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DDISABLE_PEER_EXPIRY:BOOL=ON ../
	make -C freediameter/build
	make -C freediameter/build DESTDIR=$(BUILD_PATH)/freediameter install

# starting in git clone https://github.com/spencersevilla/asn1c.git
asn1c: build
	cd asn1c; export DESTDIR=$(BUILD_PATH)/asn1c; ./configure
	cd asn1c; export DESTDIR=$(BUILD_PATH)/asn1c; make
	cd asn1c; export DESTDIR=$(BUILD_PATH)/asn1c; make install

# starting in git clone https://github.com/spencersevilla/libgtpnl.git
# git clone git://git.osmocom.org/libgtpnl;
# git reset --hard 345d687
# patch --verbose -p1  < {{ epc_dir }}/build/tools/libgtpnl.LTE_dedicated_bearer.v0.patch 
gtpnl: build
	cd libgtpnl; autoreconf -fi
	cd libgtpnl; ./configure --prefix=/usr --exec-prefix=/usr
	cd libgtpnl; make
	cd libgtpnl; make DESTDIR=$(BUILD_PATH)/gtpnl install
	export PATH=$(PATH):/sbin; ldconfig -vn $(BUILD_PATH)/gtpnl/usr/lib

# starting in git clone https://github.com/liblfds/liblfds.git
lfds: build
	cd liblfds710/build/gcc_gnumake; make DESTDIR=$(BUILD_PATH)/lfds so_dbg
	cd liblfds710/build/gcc_gnumake; export PATH=$(PATH):/sbin; make DESTDIR=$(BUILD_PATH)/lfds so_install

all: freediameter asn1c gtpnl lfds

freediameter_deb: 
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "freeDiameter 1.2 version known to work with CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-freediameter \
		--version $(FD_VERSION) \
		--package $(BUILD_DIR) \
		$(BUILD_DIR)/freediameter/usr=/

asn1c_deb: 
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "asn1c fork/patch that is known to work with CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-asn1c \
		--version $(ASN1C_VERSION) \
		--package $(BUILD_DIR) \
		$(BUILD_DIR)/asn1c/usr/local=/usr/

gtpnl_deb: 
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "libgtpnl fork that is known to work with CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-libgtpnl \
		--version $(GTPNL_VERSION) \
		--package $(BUILD_DIR) \
		$(BUILD_DIR)/gtpnl/usr=/

lfds_deb: 
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "lfds fork/patch that is known to work with CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-liblfds \
		--version $(LFDS_VERSION) \
		--package $(BUILD_DIR) \
		$(BUILD_DIR)/lfds/usr=/

all_deb: freediameter_deb asn1c_deb gtpnl_deb lfds_deb
