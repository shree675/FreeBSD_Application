# TODO : delete all the users without asking for input

pw groupdel -n admin
pw groupdel -n hod
pw groupdel -n faculty
pw groupdel -n student

printf "All the groups are deleted.\n"

pw userdel -n admin
pw userdel -n hod

printf "how many faculty : "
read num_faculty

printf "how many students : "
read num_students


# adding all faculty users to faculty group ,give permissions to files he can access
let "i = num_faculty"
while [ $i -gt 0 ]
do
	pw userdel -n "faculty$i"

	let "i -= 1"
done

# adding all student users to student group, give permissions to files he can access
let "i = num_students"
while [ $i -gt 0 ]
do
	pw userdel -n "student$i"

	let "i -= 1"
done

printf "All the users are deleted.\n"



rm ./data/*.txt
rmdir data

printf "All the data is cleaned.\n"

