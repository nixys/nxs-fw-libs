FROM centos:7

ADD packages.nixys.ru.repo /etc/yum.repos.d/packages.nixys.ru.repo
RUN rpm --import http://packages.nixys.ru/centos/repository.gpg.key && \
	yum install -y make gcc rpm-build rpmdevtools libevent-nxs openssl-devel mariadb-devel ncurses-devel libxml2-devel jansson-devel libcurl-devel rsync
RUN rpmdev-setuptree
