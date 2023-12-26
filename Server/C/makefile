COMPILER=/bin/clang
PATH=/var/tmp/SRBNP

all: deploy

deploy:
	/bin/make main
	/bin/sh _service_deploy
main:
	if [ ! -d "$(PATH)" ]; then /bin/mkdir $(PATH); fi
	$(COMPILER) *.c */*.c -o $(PATH)/SRBNP.run