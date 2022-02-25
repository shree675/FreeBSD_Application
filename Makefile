setup: setup.sh
	sudo bash ./setup.sh

student: student_interface.c utils.c
	gcc -o student_interface student_interface.c utils.c

install: utils.c application.c
	gcc -o application application.c utils.c

clean: clean.sh
	sudo ./clean.sh