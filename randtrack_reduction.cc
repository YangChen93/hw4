
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "hash.h"
#include "list.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "First Blood",                  /* Team name */

    "Yang Chen",                    /* First member full name */
    "999551783",                 /* First member student number */
    "robbie.chen@mail.utoronto.ca",                 /* First member email address */

    "Zhongyang Xiao",                           /* Second member full name */
    "999944173",                           /* Second member student number */
    "sam.xiao@mail.utoronto.ca"                            /* Second member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

class sample;
class argument;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};


class argument {
public:
	int startpoint;			 /*starting point of each thread*/
	int num_stream;			/*the number of seed streams that each thread have*/
	int hash_table_number;
};

// This instantiates an empty hash table and four independent hash tables for each threads
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;
hash<sample,unsigned> h_individual[4];


void *process (void *ptr);


int main (int argc, char* argv[]){
  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );
  printf( "Student 2 Name: %s\n", team.name2 );
  printf( "Student 2 Student Number: %s\n", team.number2 );
  printf( "Student 2 Email: %s\n", team.email2 );
  printf( "\n" );

  unsigned key;
  sample *s;
  
  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

if (num_threads != 1 & num_threads != 2 & num_threads !=4){
	printf("please enter correct number of threads, it should be 1, 2 or4");
	exit(1);
}

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);
  h_individual[0].setup(14);
  h_individual[1].setup(14);
  h_individual[2].setup(14);
  h_individual[3].setup(14);

//create pthread
	pthread_t thread[num_threads];
	argument *p = new argument[num_threads];
	int i, j, start = 0;


if (num_threads ==2) {
	for (i=0; i<2; i++){
		p[i].startpoint = start;
		p[i].num_stream = 2;
		p[i].hash_table_number = i;
		pthread_create(&thread[i], NULL, process, (void*) &p[i]);
		start +=2;
	}
}

if (num_threads == 4) {
	for (i=0; i<4; i++){
		p[i].startpoint = start;
		p[i].num_stream = 1;
		p[i].hash_table_number = i;
		pthread_create(&thread[i], NULL, process, (void*) &p[i]);
		start +=1;
	}
}

if(num_threads == 1) {
	p[0].startpoint = start;
	p[0].num_stream = 4;
	p[i].hash_table_number = 0;
	pthread_create(&thread[0], NULL, process, (void*)&p[0]);
}

for (i =0; i < num_threads; i++){
	pthread_join(thread[i], NULL);
}

for (i=0; i<num_threads; i++){
     for (j=0; j < SAMPLES_TO_COLLECT; j++) {
          if(s=h_individual[i].lookup(j)){
               sample *tmp;
               if (!(tmp = h.lookup(j))) {
                    // insert a new element for it into the hash table
                    tmp = new sample(j);
			     h.insert(tmp);
		     }

		  // increment the count for the sample
		  tmp->count += s->count;
          }
     }
}

// print a list of the frequency of all samples
h.print();
}


void *process (void *ptr){
  argument *p = (argument*) ptr;
  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  // process streams starting with different initial numbers
  for (i = p->startpoint; i < (p->startpoint + p->num_stream); i++) {
	  rnum = i;

  // collect a number of samples
	  for (j = 0; j < SAMPLES_TO_COLLECT; j++) {

		  // skip a number of samples
		  for (k = 0; k < samples_to_skip; k++) {
			  rnum = rand_r((unsigned int*)&rnum);
		  }

		  // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
		  key = rnum % RAND_NUM_UPPER_BOUND;
		  
		  
		  // if this sample has not been counted before
		  if (!(s = h_individual[p->hash_table_number].lookup(key))) {

			  // insert a new element for it into the hash table
			  s = new sample(key);
			  h_individual[p->hash_table_number].insert(s);
		  }

		  // increment the count for the sample
		  s->count++;		  
	  }
  } 
}
