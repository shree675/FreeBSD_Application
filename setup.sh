#!/bin/bash


# printf "\ninstalling python....\n"

#pkg install python3


pw groupadd hod
pw groupadd faculty
pw groupadd student
pw groupadd all_hfs 

printf "Groups created : hod, faculty, student.\n"

# NOTE : while creating users, the corresponding home dirs are not created
# as there is no use

# force the password to be the same as username

pw useradd -n hod -G all_hfs -w yes

printf "how many faculty : "
read num_faculty

printf "how many students : "
read num_students


# ------ data files creation --------------

mkdir data
cd data

let "i = num_faculty"

while [ $i -gt 0 ]
do
	let "j = num_students"
	while [ $j -gt 0 ]
	do

		mkdir "data$i$j"

		# This is being over-written by the following permissions (640)
		# chmod 655 "data$i$j"

		cd "data$i$j"
		git init --shared=group
		git config user.email "user@gmail.com"
		git config user.name "user"
		touch "data$i$j.txt"
		# deny access for others
		chmod -R 640 . 

		echo "0" > "data$i$j.txt"
		git add .
		git commit -m "Initializing data$i$j file."

		cd ..

		let "j -= 1"
	done

	let "i -= 1"
done

cd ..

printf "The data files are created in the \"data\" folder\n"

# ----------------------------------------------------------


disk_name=`mount | head -n1 | awk '{printf $1;}'`

mount -o acls $disk_name

# create faculty users, add them to faculty group 
# give permissions to data files
let "i = num_faculty"
while [ $i -gt 0 ]
do
	pw useradd -n "faculty$i" -G faculty,all_hfs -w yes

	let "j = num_students"
	while [ $j -gt 0 ]
	do
		setfacl -R -m u:"faculty$i":rwx "./data/data$i$j"
		setfacl -m u:"faculty$i":rw- "./data/data$i$j/data$i$j.txt"

		# Setting permissions for admin and hod here only.
		setfacl -R -m u:hod:rwx "./data/data$i$j/"
		setfacl -m u:hod:rw- "./data/data$i$j/data$i$j.txt"

		let "j -= 1"
	done

	let "i -= 1"
done

# create student users, add them to student group 
# give permissions to data files
let "temp_i = num_students"
while [ $temp_i -gt 0 ]
do
	let "i = $num_students - $temp_i + 1"

	printf "Enter student$i's full name : "
	read full_name

	pw useradd -n "student$i" -G student,all_hfs -w yes -c $full_name


	let "j = num_faculty"
	while [ $j -gt 0 ]
	do
		setfacl -m u:"student$i":r-x "./data/data$j$i"
		setfacl -m u:"student$i":r-- "./data/data$j$i/data$j$i.txt"

		let "j -= 1"
	done

	let "temp_i -= 1"
done

# --------------------Creating named pipes------------------

mkdir pipes

let "i = num_students"
while [ $i -gt 0 ]
do
	mkfifo "pipes/p_student$i"

	setfacl -m u:"student$i":rw- "pipes/p_student$i"
	setfacl -m u:"hod":rw- "pipes/p_student$i"

	let "i -= 1"
done




# ----------------------------------------------------------


printf "Created users and set permissions for all the files.\n"
