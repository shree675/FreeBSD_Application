syscall::open:entry
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor"/
{	
	printf("gid:%d, executable:%s, pid:%d, uid:%d, %s:%s",gid,execname,pid,uid,probefunc,copyinstr(arg0));
	printf("\n----------------------------------------------------\n");
}

syscall::access:entry
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor"/
{	
	printf("executable:%s, pid:%d, uid:%d, %s:%s",execname,pid,uid,probefunc,copyinstr(arg0));
	printf("\n----------------------------------------------------\n");
}

proc:::exec
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor"/
{	
	printf("%s",stringof(args[0]));
	printf("\n----------------------------------------------------\n");
}

proc:::exit
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor"/
{	
	printf("%d",args[0]);
	printf("\n----------------------------------------------------\n");
}

syscall::read:entry, syscall::write:entry
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor" && execname!="sh"/
{
	printf("exec:%s", execname);
	printf("\n----------------------------------------------------\n");

}

syscall:::entry
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor"/
{
	@syscalls[probefunc]=count();
}

proc:::create
/execname=="application" || execname=="student_interface" || execname=="faculty_interface" || execname=="hod_interface" || execname=="text_editor"/
{
	printf("fork in %s",execname);
	printf("\n----------------------------------------------------\n");
}
