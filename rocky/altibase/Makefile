
.PHONY: all build run fclean re

all: build run

build:
	docker build -t db .

run:
	docker run -it -p 20300:20300 -v "./DB:/DB" db 
start:
	docker start 61fa82 && docker exec -it 61fa82 /bin/zsh

fclean:
	docker rmi altibase_db

re: fclean all
