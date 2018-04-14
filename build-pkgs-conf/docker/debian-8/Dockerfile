FROM debian:8

RUN apt-key adv --fetch-keys http://packages.nixys.ru/debian/repository.gpg.key && \
	echo "deb [arch=amd64] http://packages.nixys.ru/debian/ jessie main" > /etc/apt/sources.list.d/packages.nixys.ru.list && \
	apt-get update; apt-get install -y libevent-nxs libssl1.0.0 make gcc libmysqlclient-dev libncurses5-dev libxml2-dev libjansson-dev libssl-dev libcurl4-openssl-dev dh-make rsync
