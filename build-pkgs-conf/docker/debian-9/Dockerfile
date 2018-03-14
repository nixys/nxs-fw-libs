FROM debian:9

RUN apt-get update; apt-get install -y gnupg && \
	apt-key adv --fetch-keys http://packages.nixys.ru/packages.nixys.ru.gpg.key && \
	echo "deb [arch=amd64] http://packages.nixys.ru/debian/ stretch main" > /etc/apt/sources.list.d/packages.nixys.ru.list && \
	apt-get update; apt-get install -y libevent-nxs libssl1.0.2 make gcc libmariadbclient-dev libncurses5-dev libxml2-dev libjansson-dev libssl1.0-dev libcurl4-openssl-dev dh-make rsync
