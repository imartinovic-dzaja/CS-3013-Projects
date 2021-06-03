	Data Structure (found in linkedlist.c and include.h)
To implement all policies a singly-linked linked list was used. The list structure simply kept track of the first and last job in the list. The job structure is the node of the linked list. The elements of the job structure are as follows:
	1) id - 		id of a job
	2) length - 		length of a job
	3) responseTime - 	time it takes to run the job after arrival
	4) turnaroundTime - 	time it takes for the job to fully complete after arrival
	5) waitTime - 		time the process spends waiting after arrival
	6) hasRun - 		boolean value indicating whether the job has run at least once
	7) lastTime - 		time of the system since the last time job has run
	8) next - 		next job in the linked list

There are also a couple of functions associated with the linked list structure:
init_list - 		initializes list

void add_job_to_front - allocates space for a new job node and adds it to the front of the list (it is a helper function for
			add_job_sjf)
			
void add_job_sjf - 	allocates space for a new job, adds the job to a list of jobs, such that the list is in ascending 
			order of job lengths, used for policy implementation of sjf when queueing up jobs
			
void add_job_to_back - 	allocate space for a new job and add it to the back of the list, used for policy implementation 
			of fifo and rr when queueing up jobs
			
void add_job_responseTime_ascending - 	take a (finished) job and add it to the list of (finished) jobs in order of ascending
					 responseTimes, used for implementing analysis parts of all policies 
					 
job* dequeue - 		dequeue the first job on the list (used to remove jobs from the non-completed list in the fifo and 
			sjf policies)
			
job* removeFromList - 	remove the specified job from the list of jobs (used to remove a job from the non-completed list 
			in the rr policy)
			
void print_averages - 	used during analysis part to print the averages for a list; i.e. the average response time,average
			turnaround time and average wait time

	Algorithm
FIFO - jobs are initially read and in the same order added to the notCompleted list of jobs. We start the timer, which keeps its value in the currentTime variable. Then jobs are deuqued one by one from the notCompleted list which simulates them being run to completion. Their respective responseTime, turnaroundTime and waitTime are also calculated using the currentTime variable. After a job "runs to completion" it is added to the completed list, the currentTime variable is updated and the next job is run. After every job runs we enter the analysis part, which simply steps through each job in the completed list and prints its responseTime, turnaroundTime and waitTime. Once done the print_averages function is run on the list of completed jobs to calculate the average values

SJF - jobs are initially read and added to the notCompleted list of jobs in ascending order of length. We start the timer, which keeps its value in the currentTime variable. Then jobs are deuqued one by one from the notCompleted list which simulates them being run to completion. Their respective responseTime, turnaroundTime and waitTime are also calculated using the currentTime variable. After a job "runs to completion" it is added to the completed list, the currentTime variable is updated and the next job is run. After every job runs we enter the analysis part, which simply steps through each job in the completed list and prints its responseTime, turnaroundTime and waitTime. Once done the print_averages function is run on the list of completed jobs to calculate the average values

RR - jobs are initially read and in the same order added to the notCompleted list of jobs. We start the timer, which keeps its value in the currentTime variable. Then we step through each of the jobs in the notCompleted list and do the following: 

1) 	We check whether the job has been previously run, and if not we set its response time to the current time, and set the 
	hasRun flag to 1 (this is because a job may run multiple times, and we only set the response time to the first time 
	the job ran). 
2) 	We update the wait time for the job, by subtracting the lastTime from currentTime (note: lastTime = time of the system 
	since the last time job has run). 
3)	Finally we are able to simulate the job running. For this we have 2 cases:
		a) the remaining length of the job is longer than the time slice
			-we simulate the job running for the time slice by subtracting the time slice from its length
			-we update the currentTime variable using the time slice value
			-we update the lastTime variable for the job
			-we proceed to the next job
		b) the remaining length of the job is shorter or equal to the time slice
			-we simulate the job completing by updating its turnaround time
			-we update the currentTime variable using the time slice value
			-we update the lastTime variable for the job
			-we remove the job from the notCompleted list and add it to the completedList in ascending order of responseTimes(generally the order shouldn't matter, this is done to satisfy the output of the test cases)
			-we proceed to the next job

Once there are no more jobs in the notCompleted list, we proceed to the analysis part, which simply steps through each job in the completed list and prints its responseTime, turnaroundTime and waitTime. Once done the print_averages function is run on the list of completed jobs to calculate the average values
	
	
	Novel Workloads
workload_1.in: 	To make the wait and response times equal for all jobs, all jobs except the last job must have a length <= to the
		length of the time slice (in this case 3)

workload_2.in:	Say we have 5 jobs that come in the following order: a,b,c,d,e. Suppose the following is true about the lengths of
		each job: a>=e>=d>=c>=b. In FIFO the turnaround time for job a is a, job b is a+b, job c is a+b+c and etc. 
		Therefore the average turnaround time is t = (5a+4b+3c+2d+e)/5
		In SJF the jobs will run as b,c,d,e,a, and hence the average turnaround time is t = (5b+4c+3d+2e+a)/5
		Set FIFO = 10 SJF
		(5a+4b+3c+2d+e)/5 = 10*((5b+4c+3d+2e+a)/5)		$$multiply both sides by 5
		5a+4b+3c+2d+e = 50b+40c+30d+20e+10a			$$move everything to LHS
		5a+46b+37c+28d+19e = 0					$$this obviously can be true for a>=e>=d>=c>=b>0
		
		Therefore we need more jobs, at least 11. By repeating the same analysis now with 11 jobs, where the first job is 
		longest and the remaining jobs are all of same length we get:
		(11a+10b+9b+8b+7b+6b+5b+4b+3b+2b+b)/11 = 10*((11b+10b+9b+8b+7b+6b+5b+4b+3b+2b+a)/11)	
		(11a+55b)/11 = 10*((65b + a)/11)	$$multiply by 11
		11a+55b = 10a + 650b
		a = 595b
		Say b = 1, then a = 595
		
workload_3.in:	FIFO and SJF will produce the same everything if the jobs arrive in order of shortest to longest. If we make the jobs
		also shorter or equal to the time slice, then RR would behave as FIFO. Therefore we have to combine the two: 1) make
		the jobs arrive in order of shortest to longest 2) make the jobs shorter or equal to the time slice

workload_4.in	Suppose we have 5 jobs. We need the avg. turnaround time to be over 100. To do this we could make each job take a 
		long time to complete. However since the time slices are quite short this would make the average wait time quite
		high as well. On the other hand if we had only 1 very long job, and 4 very short ones, then the short ones would
		immediately complete with very little wait time, and the long job would proceed executing by itelf, with no wait
		time at all.However since the long job is very long, it offsets the avg. turnaround time massively in its favor.
		Therefore we get a huge avg. turnaround time with a small avg. wait time.
		
workload_5.in	Suppose jobs arrive in the following order a,b,c; where length of a = 3. The response time of a is 0, response time
		of b is a, and c is a+b. Therefore the avg. response time is (2a+b)/3
		Set this equal to 5
		(2a+b)/3 = 5		$$substitue a = 3
		(6+b)/3 = 5		$$multiply by 3
		6+b = 15
		b = 9
		
		Now that we have length of b, let's find the avg. turnaround time. For a the turnaround time is a, for b it is a+b
		and for c it is a+b+c. Therefore the average turnaround time is (3a+2b+c)/3
		Set this equal to 13
		(3a+2b+c)/3 = 13	$$multiply by 3
		3a+2b+c = 39		$$substitute a=3 and b = 9
		9+18+c = 39
		c = 12
		
		Finally a=3, b=9 and c=12
		
		
