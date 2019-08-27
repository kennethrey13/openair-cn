
USER_EMAIL=$(shell git config --get user.email)

EPC_VERSION=1.0.1.3
CONF_VERSION=0.11.0
DB_VERSION=0.9.12
TARGET_DIR=./BUILD/
LIB_DIR=./LIBRARIES/

build_deps:
	sudo apt-get install autoconf automake bison build-essential cmake cmake-curses-gui doxygen doxygen-gui flex pkg-config git libconfig-dev libgcrypt20-dev libidn2-0-dev libidn11-dev default-libmysqlclient-dev libpthread-stubs0-dev libsctp1 libsctp-dev libssl-dev libtool openssl nettle-dev nettle-bin php python-pexpect castxml guile-2.0-dev libgmp-dev libhogweed4 libgtk-3-dev libxml2 libxml2-dev mscgen check python libgnutls28-dev python-dev unzip libmnl-dev libevent-dev ruby ruby-dev rubygems
	sudo gem install --no-ri --no-rdoc fpm

libraries:
	make -C $(LIB_DIR) all

libraries_deb:
	make -C $(LIB_DIR) all_deb

dir:
	mkdir -p $(TARGET_DIR)

hss: dir
	./oaienv; ./scripts/build_hss

mme: dir
	./oaienv; ./scripts/build_mme

spgw: dir
	./oaienv; ./scripts/build_spgw

epc: hss mme spgw
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/local/etc/oai/hss.conf \
		--config-files /usr/local/etc/oai/mme.conf \
		--config-files /usr/local/etc/oai/spgw.conf \
		--config-files /usr/bin/spgw_nat.sh \
		--config-files /usr/local/etc/oai/freeDiameter/acl.conf \
		--config-files /usr/local/etc/oai/freeDiameter/hss_fd.conf \
		--config-files /usr/local/etc/oai/freeDiameter/mme_fd.conf \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface EPC" \
		--url "https://github.com/uw-ictd/openair-cn" \
		--name colte-epc \
		--version $(EPC_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'default-libmysqlclient-dev, libconfig9, libsctp1, colte-freediameter, colte-liblfds, colte-libgtpnl, colte-db, colte-conf, libevent-openssl-2.0-5, libevent-pthreads-2.0-5' \
		--replaces 'colte-hss, colte-mme, colte-spgw' \
		--after-install ./package/epc/postinst \
		--after-remove ./package/epc/postrm \
		./BUILD/oai_hss=/usr/bin/ \
		./BUILD/mme=/usr/bin/ \
		./BUILD/spgw=/usr/bin/ \
		./package/epc/spgw_nat.sh=/usr/bin/ \
		./package/epc/hss.conf=/usr/local/etc/oai/hss.conf \
		./package/epc/mme.conf=/usr/local/etc/oai/mme.conf \
		./package/epc/spgw.conf=/usr/local/etc/oai/spgw.conf \
		./package/epc/colte-epc.service=/etc/systemd/system/colte-epc.service \
		./package/epc/colte-hss.service=/etc/systemd/system/colte-hss.service \
		./package/epc/colte-mme.service=/etc/systemd/system/colte-mme.service \
		./package/epc/colte-spgw.service=/etc/systemd/system/colte-spgw.service \
		./package/epc/colte-spgw_nat.service=/etc/systemd/system/colte-spgw_nat.service \
		./package/epc/oai=/usr/local/etc/colte/oai \
		./package/epc/freeDiameter=/usr/local/etc/oai/

conf: dir
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/bin/colte/roles/configure/vars/main.yml \
		--maintainer sevilla@cs.washington.edu \
		--description "Configuration Tools for CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--name colte-conf \
		--version $(CONF_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'ansible, python-mysqldb, colte-db' \
		--after-install ./colteconf/postinst \
		--after-remove ./colteconf/postrm \
		./colteconf/colteconf=/usr/bin/ \
		./colteconf/coltedb=/usr/bin/ \
		./colteconf/colte=/usr/bin/ \
		./colteconf/config.yml=/usr/local/etc/colte/config.yml

db: dir
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "Sample database for use with CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--name colte-db \
		--version $(DB_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'default-mysql-server, default-mysql-client' \
		--after-install ./package/db/postinst \
		--after-remove ./package/db/postrm \
		./package/db/sample_db.sql=/usr/local/etc/colte/sample_db.sql

all: epc db conf

package-clean:
	rm colte*\.deb

build-clean:
	echo "build-clean does nothing right now..."
	# ./scripts/build_hss -c
	# ./scripts/build_mme -c
	# ./scripts/build_spgw -c

clean: package-clean build-clean
