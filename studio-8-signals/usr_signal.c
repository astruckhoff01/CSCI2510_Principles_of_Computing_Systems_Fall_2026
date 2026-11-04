// Anna Struckhoff
// November 3, 2026
// Optional exercise: a child sends SIGUSR1 after finishing its work.
// The parent responds after its handler sets a flag.

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

volatile sig_atomic_t received_usr1 = 0;

void receive_usr1( int signum ){
  (void) signum;
  received_usr1 = 1;
}

int main( void ){
  if ( signal( SIGUSR1, receive_usr1 ) == SIG_ERR ) {
    perror( "Error installing SIGUSR1 handler" );
    return -1;
  }

  pid_t child = fork();
  if ( child == -1 ) {
    perror( "Could not fork" );
    return -1;
  }
  if ( child == 0 ) {
    // child does some "work" then notifies the parent it is ready
    printf( "Child: doing some work...\n" );
    sleep( 1 );
    printf( "Child: done, sending SIGUSR1 to parent.\n" );
    if ( kill( getppid(), SIGUSR1 ) == -1 ) {
      perror( "Error sending SIGUSR1" );
      return -1;
    }
    return 0;
  }

  // Check the flag between sleeps.
  printf( "Parent: waiting for SIGUSR1 from the child...\n" );
  while ( received_usr1 == 0 ) {
    sleep( 1 );
  }
  printf( "Parent: got SIGUSR1, now doing my work.\n" );

  while ( waitpid( child, NULL, 0 ) == -1 ) {
    if ( errno == EINTR ) {
      continue;
    }
    perror( "Error waiting for child" );
    return -1;
  }
  return 0;
}
