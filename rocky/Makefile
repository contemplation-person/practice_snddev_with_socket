.PHONY: all build run fclean re

all: client_build client_run

build:
	docker build -t alti_db ./altibase_db
server_run:
	docker run -it -v "./altibase_db:/altibase" alti_db
	-p 20300:20300

server_start:
	docker start 44e05 && docker exec -it 44e05 /bin/zsh

client_build:
	docker build -t alti_client .

client_run:
	docker run -it \
	-v "./altibase_db:/altibase" \
	-v "./server:/server" \
	-p 4242:4242 \
	alti_client

client_start:
	docker start 0186e2 && docker exec -it 0186e2 /bin/zsh

fclean:
	docker rmi altibase_db

re: fclean all
