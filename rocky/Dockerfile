FROM rockylinux:8

RUN yum update -y && yum install -y gcc make vim git tmux zsh ncurses-compat-libs \
    && sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
RUN yum install -y net-tools
RUN yum install -y wget && wget http://support.altibase.com/fileDownload.do?gubun=etc&pro_ver=44&cate=2&os=LINUX-X86&p_bit=64bit 
ENV ALTIBASE_HOME=/opt/altibase-HDB-server-6.5.1


EXPOSE 4242