
.PHONY: all build run fclean re

all: build run

build:
	docker build -t tcp_server .

run:
	docker run -it -p 4242:4242 -v "./server:/tcp_server" tcp_server 
start:
	docker start 3b8a6  && docker exec -it  3b8a6   /bin/zsh

fclean:
	docker rmi altibase_db

re: fclean all
