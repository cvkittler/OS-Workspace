README

Author: Charles Kittler

prolific.c "uses a for loop to spawn and wait for children"
    prolific startes off by opening a file and reading one string (the seed)
    that seed is then converted from a string to an int and then used to seed rand()
    rand() is used to generate a random number of childrean 10-15
    to generate random numbers for the childrean a for loop is used to populate an array with random numbers
    then the childrean are forked in a for loop that has an if statement  for the child and parent
        -Child
            The child section of the if statement reads the random number that it 
            corrisponds too from the array previously mentioned and then waits that long
        -Parent 
            The parent waits for the child prosses to finish then forks off another child

generation.c "generation.c uses recurstion to make the childen"
    The prosses to get a seed from the file is the same as prolific.c 
    once the number of generations is randomly picked that number is passed into
    a recursive helper function (spawnChild()) the function passes the depth left to 
    recurse each time is resurses. if the 'counter' is zero (max depth) then the child exits 
    with an exit code of 0. otherwise the prosses spawns another child and waits for it
    to return a value, then that prosses returns that +1

explorer.c "uses a switch case and number generator to change directorys"
    startes off by opening a file and reading one string (the seed)
    that seed is then converted from a string to an int and then used to seed rand()
    rand() is used to generate a random number for each of the 5 children it will spawn
    then in a for loop a random number is accesed and converted to a switch case key
    the value determans what directory to switch to, the directory is then checked and 
    a child is spawned and the child runs a command while the parent waits. When the child
    terminates then the parent continues in the for loop

slug.c "reads command line inputs to know what file to read the seed from"
    slug.c takes the argument that was passed in and combines it with some other text using
    sprintf() to get the correct file path. Then the prosses of opening and reading the
    seed from a file is the same as in prolific.c the sleep time is determaned by the equation
    "(rand() % 4) + 1" and the coin flip is determaned by "rand() % 2" the value from the sleep
    is passed directly into a sleep() call and the coinflip value is used to determan what 
    command to run using an if else statement

slugrace.c "generates children in a for loop and monitors them in a while loop"
    slugrace starts off by spawning 4 children and and starting a different slug 
    each time a child is forked its id is recorded in an array. once all 4 children are
    spawned the clock is started and then the parent enters a while loop. In that loop
    the parent cyclese between, checking which of the children have finished and their last state,
    then by checking if the last state was still running and the current state is not running
    then there is a check if all children are done if they are exit else print out
    the children who are still running

    the the current and last state of each child is recorded in an array.