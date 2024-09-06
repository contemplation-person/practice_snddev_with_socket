FROM rockylinux:8

COPY EMG_DB.tar.gz EMG_DB.tar.gz 
COPY install.sh install.sh

RUN yum update -y && yum install -y gcc make vim git tmux zsh ncurses-compat-libs net-tools wget &&\
    sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

RUN chmod 755 install.sh

ENV ALTIBASE_HOME=/opt/altibase-HDB-server-6.5.1
ENV ALTIBASE_PORT_NO=20300
ENV ALTIBASE_NLS_USE=KSC5601

CMD ["zsh"]

EXPOSE 20300
