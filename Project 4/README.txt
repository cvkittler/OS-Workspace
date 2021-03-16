General:
all the command line info is parsed and flag variables set in order to let the future code run
the jobs are read from the specified file and then stored in an intager array
a switch case is used to chose the algorithum that is run

FIFO: "First In First Out"
The work load is initilized from the array and then then exacuted in that order

SJF: "Shortest Job First"
before initilizing bubble sort is used to get the job array in order from smallest to largest and then the 
workload is initilized in that order and exacuted

RR: "Round Roben"
the round roben is done by initilizing the workload in the arrival order, and then uses a while loop
to cycle thround all the jobs doing some amount of the remaining work. all the metadata about the job
is stored in the job struct (a LinkedList node with extra data) once all jobs are done the while loop
exits and thats the end

Analisis:
all analisis data is stored in the job struct, start time, end time, job size, amount finished and the next node
this acumulated and prinited in a while loop and once done collecting all the data the averages are also printed