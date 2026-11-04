// Anna Struckhoff
// November 3, 2026
// Starts a sleeper child and handles SIGINT in the parent.
// The child keeps the default interrupt action.

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

// Report the interrupt with write() and preserve the interrupted errno.
void ignore_sigint( int signum ){
  int saved_errno = errno;
  const char message[] = "\nCaller is ignoring SIGINT and waiting for the sleeper.\n";
  (void) signum;
  (void) write( STDOUT_FILENO, message, sizeof( message ) - 1 );
  errno = saved_errno;
}

int main( void ){
  if ( signal( SIGINT, ignore_sigint ) == SIG_ERR ) {
    perror( "Error installing SIGINT handler" );
    return -1;
  }

  printf( "Forking sleeper...\n" );
  fflush( stdout );

  pid_t ret = fork();
  if( ret == -1 ){
    perror( "Could not fork" );
    return -1;
  }

  if( ret == 0 ){ // Child
    // Restore normal interrupts in the child.
    if ( signal( SIGINT, SIG_DFL ) == SIG_ERR ) {
      perror( "Error restoring SIGINT" );
      _exit( 1 );
    }
    char* cmd = "./sleep";
    char* myargv[] = { "sleep", NULL };
    execvp( cmd, myargv );
    perror( "Error calling execvp" );
    _exit( 127 );
  }

  // Parent
  printf( "Waiting for sleeper %ld...\n", (long) ret );
  fflush( stdout );

  int status;
  while ( waitpid( ret, &status, 0 ) == -1 ) {
    if ( errno == EINTR ) {
      continue;  // continue waiting if a signal interrupted waitpid()
    }
    perror( "Error waiting for sleeper" );
    return -1;
  }
  if ( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 ) {
    return 1;
  }

  printf( "Parent finished waiting and returned successfully!\n" );
  return 0;
}
