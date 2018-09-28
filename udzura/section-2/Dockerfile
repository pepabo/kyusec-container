FROM ubuntu:xenial
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get -q -y update && apt-get -q -y install apache2
EXPOSE 80
CMD ["/usr/sbin/apache2ctl", "-D", "FOREGROUND"]
