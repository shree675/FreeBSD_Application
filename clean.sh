# TODO : delete all the users without asking for input

pw groupdel -n hod
pw groupdel -n faculty
pw groupdel -n student
pw groupdel -n all_hfs

printf "All the groups are deleted.\n"

pw userdel -n hod

printf "how many faculty : "
read num_faculty

printf "how many students : "
read num_students


let "i = num_faculty"
while [ $i -gt 0 ]
do
	pw userdel -n "faculty$i"

	let "i -= 1"
done

let "i = num_students"
while [ $i -gt 0 ]
do
	pw userdel -n "student$i"

	let "i -= 1"
done

printf "All the users are deleted.\n"



rm -r data

rm -r pipes

printf "All the data is cleaned.\n"

