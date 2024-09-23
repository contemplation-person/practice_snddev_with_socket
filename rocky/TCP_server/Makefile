
.PHONY: all build run fclean re

all: build run

build:
	docker build -t tcp_server .

run:
	docker run --name tcp_svr -it -p 4242:4242 -v "./server:/tcp_server" tcp_server 
start:
	docker start tcp_svr && docker exec -it tcp_svr /bin/zsh

fclean:
	docker rmi altibase_db

re: fclean all
