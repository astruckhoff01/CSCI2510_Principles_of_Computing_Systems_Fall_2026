// Anna Struckhoff
// November 3, 2026
// Exercise 5: the caller and sleeper both use the default SIGINT action.

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main( void ){
  printf( "Forking sleeper...\n" );
  fflush( stdout );

  pid_t ret = fork();
  if ( ret == -1 ) {
    perror( "Could not fork" );
    return -1;
  }
  if ( ret == 0 ) {
    char* cmd = "./sleep";
    char* myargv[] = { "sleep", NULL };
    execvp( cmd, myargv );
    perror( "Error calling execvp" );
    _exit( 127 );
  }

  printf( "Waiting for sleeper %ld...\n", (long) ret );
  fflush( stdout );
  if ( waitpid( ret, NULL, 0 ) == -1 ) {
    perror( "Error waiting for sleeper" );
    return -1;
  }
  printf( "Parent finished waiting and returned successfully!\n" );
  return 0;
}
