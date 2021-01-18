//# OS_ex8
// OS- semester A- class B2 
// Bar Ifrah-  login:- barif 
// Sharon Levi  login:- sharonlevi 
// Date - January 18st, 2021
**********************************************************************************
ALL PROGRAMS ARE EXECUTED VIA UNIX TERMINAL ALL PROGRAMS COMPILED VIA TERMINAL-
USING COMMAND "gcc -Wall (filename.cc) -o (runfilename) -lpthread -lrt
Programs are executed via terminal. './<runFileName>.
********************************************************************************** 
//ex8a
  //master
// this program creates the random numbers array,open a semaphore 
//and the shared memory.
// after checking that we have 3 different number makers, we look for
// numbers to delete. this is all done by the number finders.
// when the array is all set to be -1, we finish .
// the output will be- how much time it took.
  //number maker
// this program is the number creator for ex8a1, and connect to existed semaphore.
// here we create random numbers that will delete the
// numbers in the shared memory array.
//the numbers creaters proccesses are Synchronized by the semaphore.
// output will be- numbers sent, number deleted.
  // race modes-
// none that we are aware of.
// output will be- 3 subprocesses data(as described), master time.
**********************************************************************************
**********************************************************************************
//ex8b
//this program create 3 threads- 2 servers and one fron end.
//the frond end thread ask for an action from the user:
//'g' for gc action or 'd' for caclculate prime factors.
//after the user input the right numbers, it send it to the right thread and 
//wait for an answer. each servers threads get the input and return the 
//right result- gcd of 2 numbers or prime factors of a number.
//after the front end thread got the answer it print the result to the screen. 
//Compile: gcc -Wall ex7b.c -oex7b -lpthread
//Run: ./ex8b
