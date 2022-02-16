#!/bin/bash


printf "\ninstalling python....\n"

#pkg install python3

printf "\ngroups being created: admin, hod, faculty, student\n"

#pw groupadd admin
#pw groupadd hod
#pw groupadd faculty
#pw groupadd student

printf "\nfinished creating groups....\n"

printf "\ntime to add the users.....\n"




printf "admin username : "
read username
#pw user add -n $username -G admin


printf "hod username : "
read username
#pw user add -n $username -G hod




printf "how many faculty : "
read num_faculty

printf "how many students : "
read num_students

mkdir data

# ------ data files creation --------------
let "i = num_faculty"

while [ $i -gt 0 ]
do
	let "j = num_students"
	while [ $j -gt 0 ]
	do
		touch "./data/data$i$j.txt"
		let "j -= 1"
	done

	let "i -= 1"
done



while [ $num_faculty -gt 0 ]
do
	# add user to the faculty group, give permissions to files he can access
	#pw user add -n "faculty$num_faculty" -G faculty
	let "num_faculty -= 1"
done


while [ $num_students -gt 0 ]
do
	# add user to the student group, give permissions to files he can access
	#pw user add -n "student$num_students" -G student
	let "num_students -= 1"
done


# set permissions






