FROM rockylinux:8

RUN yum update -y && yum install -y gcc make vim git tmux zsh ncurses-compat-libs \
    && sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

EXPOSE 20300