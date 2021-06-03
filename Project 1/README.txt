//make commands

1) To make all the executables type "make all" in terminal
2) To make the individual executables simply type "make executable_name"
	eg. to make the prolific executable simply type "make prolific"
3) To remove all the executables type "make clean"


TESTING
	1) Prolific Parent
	The code was very straightforward, and testing was performed by comparing output of my program to that of the professor.
	I also used the opportunity to test the read_seed() and get_random_number() functions. 
		-read_seed() was tested simply by inputing different seeds in the seed.txt file
		-get_random_number() was tested by inserting the following loop after srand(seed); on line 17: 
			for (int j = 0; j < 100; ++j) {
				printf("%i\n", get_random_number(lowerLimit, upperLimit));
			}
		What was tested is that the get_random_number produced values between the upper and lower limit (inclusive)
		And that get_random_number produced different results when different seed value was read from seed.txt

	2) Lifespan Generation
 	Similar to Prolific Parent scenarion, testing was performed by comparing the output of my program to that of the professor until they matched.
 	I've made sure the lifespan count is always >= 0, and that the parent reads the exit code of the child, rather than calculate it itself

	3) The Explorer
	Output of the code was checked against the code output provided by the professor.
	The list of directories was checked by inserting the following loop after srand(seed); on line 27:
	for (int j = 0; j < numOfDirectories; ++j) {
				printf("%s\n", directories[j]);
			}
	4) The Slug
	First the Slug was checked whether it only takes 1, 2, 3 and 4 as input parameters.
	Then it was checked that the Slug can return all combinations of coin_flip and num_of_seconds using different seed values 
	Lastly the output was checked agains the one provided by the professor.
	
	5)The Slug Race
	The outputs of was compared with teacher's output
	It seems as though the last child will return at the correct timing while the remaining children will be late by 0.25 seconds
	It was made sure the parent didn't force execution of specific order by using different seed values
	
	
