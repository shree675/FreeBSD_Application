#!/bin/bash


printf "\ninstalling python....\n"

#pkg install python3

printf "\ngroups being created: admin, hod, faculty, student\n"

pw groupadd admin
pw groupadd hod
pw groupadd faculty
pw groupadd student

printf "\nfinished creating groups....\n"

printf "\ntime to add the users.....\n"


while true
do
	printf "\nSelect the group to which you want to add user to? \n1. admin\n2. hod\n3. faculty\n4. student\n5. stop creating users\n";
	read option
	case $option in
		1)
			printf "username : "
			read username
			pw user add -n $username -G admin
		;;
		2)	
			printf "username : "
			read username
			pw user add -n $username -G hod
		;;
		3)
			printf "username : "
			read username
			pw user add -n $username -G faculty
		;;
		4)
			printf "username : "
			read username
			pw user add -n $username -G student
		;;
		5)
			break
		;;
		*)
			printf "select a valid option\n"
		;;
	esac
done

# set permissions
mount -o 






