1) to create the executable, navigate to its containing directory and type 'make' in the terminal
2) to run the executable, navigate to its containing directory and type './test_goatmalloc' in the terminal
3) to delete the executable, navigate to its containing directory and type 'make clean'

NOTE!!!: I've edited the original test_goatmalloc.c file to include the following:
	1) Included a reference to the global file pointer variable fp (line 10)
	2) Made sure the fp variable is properly initialized inside the main function (lines 453-455)
	3) All printf statements were changed to fprintf (except line 467)
	
	4) On lines 348 and 349 the assignemnt operators were changed to the appropriate comparison operators (see forum thread)
	5) All test cases are uncommented 
	
Without changes 1) 2) and 3) the code would still run, however it will be harder to decipher which lines correspond to which test case in output_ref.txt
