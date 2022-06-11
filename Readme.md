# FreeBSD Term Project

## Features
* Uses OS level features such as groups, ACL and pipes.
* Provides a pleasing UI for the purpose of editing marks of a student.
* Supports real-time updation of marks of all students.
* Supports upto 1000 faculty and 1000 students simultaneously running the application.
* Provides version control for the marks of each student.
* Provides features like average and maximum/minimum along with an option to export the marks as CSV.
* Contains dtrace script for logging and debugging.

## Getting Started
1. Run ```make setup``` to create the users with appropriate permissions
2. Run ```make install``` to compile all the executables
3. Run ```make clean``` to clean up everything
