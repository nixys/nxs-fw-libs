# Nixys Framework Libraries

## Introduction

Nxs-fw-libs provides a set of libraries to help developers create C programs. *nxs-fw-libs* is a part of Nixys Framework.

Simple example to work with strings:

```c
...

int main()
{
	nxs_string_t str1, str2;

	nxs_string_init(&str1);
	nxs_string_init(&str2);

	nxs_string_printf(&str1, "world");
	nxs_string_printf(&str2, "Hello %r", &str1);
	nxs_string_char_add_char(&str2, (u_char)'!');

	nxs_log_write_console(&process, "%r", &str2);

	nxs_string_free(&str1);
	nxs_string_free(&str2);

	return 0;
}
```

The amount of allocated memory for strings **str1** and **str2** will be dynamically changed.

Other examples for using *nxs-fw-libs* modules will be added in the future.

We recomend to use [Nixys Framework Control Tools](https://github.com/nixys/nxs-fw-ctl) to create C applications based on Nixys Framework Libraries.

## Install in Debian

1.  Add Nixys repository key:

    ```
    wget -O - http://packages.nixys.ru/debian/repository.gpg.key | apt-key add -
    ```

2.  Add the repository. Currently Debian wheezy, jessie and stretch are available:

    ```
    echo "deb [arch=amd64] http://packages.nixys.ru/debian/ wheezy main" > /etc/apt/sources.list.d/packages.nixys.ru.list
    ```

    ```
    echo "deb [arch=amd64] http://packages.nixys.ru/debian/ jessie main" > /etc/apt/sources.list.d/packages.nixys.ru.list
    ```

    ```
    echo "deb [arch=amd64] http://packages.nixys.ru/debian/ stretch main" > /etc/apt/sources.list.d/packages.nixys.ru.list
    ```

3.  Make an update:

    ```
    apt-get update
    ```

4.  Choose and install needed nxs-fw-libs version. For example:

    ```
    apt-get install nxs-fw-libs-1.13
    ```

## Install in CentOS

1.  Add Nixys repository key:

    ```
    rpm --import http://packages.nixys.ru/centos/repository.gpg.key
    ```

2.  Add the repository. Currently CentOS 6 and 7 are available:

    ```
    cat <<EOF > /etc/yum.repos.d/packages.nixys.ru.repo
    [packages.nixys.ru]
    name=Nixys Packages for CentOS \$releasever - \$basearch
    baseurl=http://packages.nixys.ru/centos/\$releasever/\$basearch
    enabled=1
    gpgcheck=1
    gpgkey=http://packages.nixys.ru/centos/repository.gpg.key
    EOF
    ```

3.  For CentOS 6 (to install `jansson-devel`):

    ```
    yum install -y epel-release
    ```

4.  Choose and install needed nxs-fw-libs version. For example:

    ```
    yum install nxs-fw-libs-1.13
    ```
