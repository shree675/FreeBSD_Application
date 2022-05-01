install: utils.c application.c faculty_interface.c student_interface.c
	make student
	make faculty
	make hod
	make editor
	gcc -o application application.c utils.c -g

setup: setup.sh
	sudo bash setup.sh

student: student_interface.c utils.c
	gcc -o student_interface student_interface.c utils.c -g

faculty:
	gcc -o faculty_interface faculty_interface.c utils.c -g

hod:
	gcc -o hod_interface hod_interface.c utils.c -g

editor:
	gcc -o text_editor text_editor.c -g
	
dtrace:
	dtrace -o log.txt -s trace.d

clean: clean.sh
	sudo ./clean.sh
	rm application student_interface hod_interface faculty_interface
