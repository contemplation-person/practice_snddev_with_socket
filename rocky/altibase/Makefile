
.PHONY: all build run fclean re

all: build run

build:
	docker build -t db .

run:
	docker run --name alti_db -it -p 20300:20300 -v "./DB:/DB" db
start:
	docker start alti_db && docker exec -it alti_db /bin/zsh

fclean:
	docker rmi altibase_db

re: fclean all
