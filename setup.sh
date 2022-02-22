#!/bin/bash


# printf "\ninstalling python....\n"

#pkg install python3


pw groupadd hod
pw groupadd faculty
pw groupadd student

printf "Groups created : hod, faculty, student.\n"

# NOTE : while creating users, the corresponding home dirs are not created
# as there is no use

# force the password to be the same as username

pw useradd -n hod -d /home/hod -g hod -w yes

printf "how many faculty : "
read num_faculty

printf "how many students : "
read num_students






# ------ data files creation --------------

mkdir data
git init ./data

let "i = num_faculty"

while [ $i -gt 0 ]
do
	let "j = num_students"
	while [ $j -gt 0 ]
	do
		touch "./data/data$i$j.txt"
		echo "0" >> "./data/data$i$j.txt"
		let "j -= 1"
	done

	let "i -= 1"
done

cd data
git add .
git commit -m "initializing all data files"
cd ..

printf "The data files are created in the \"data\" folder\n"

# ----------------------------------------------------------



mount -o acls /dev/ada0s1a

# create faculty users, add them to faculty group 
# give permissions to data files
let "i = num_faculty"
while [ $i -gt 0 ]
do
	pw useradd -n "faculty$i" -G faculty -w yes

	let "j = num_students"
	while [ $j -gt 0 ]
	do
		setfacl -m u:"faculty$i":rw- "./data/data$i$j.txt"

		# Setting permissions for admin and hod here only.

		setfacl -m u:hod:rw- "./data/data$i$j.txt"

		let "j -= 1"
	done

	let "i -= 1"
done

# create student users, add them to student group 
# give permissions to data files
let "i = num_students"
while [ $i -gt 0 ]
# TODO: option to enter full names of students
do
	pw useradd -n "student$i" -G student -w yes


	let "j = num_faculty"
	while [ $j -gt 0 ]
	do
		setfacl -m u:"student$i":r-- "./data/data$j$i.txt"

		let "j -= 1"
	done

	let "i -= 1"
done

printf "Created users and set permissions for all the files.\n"



