// Anna Struckhoff
// October 29, 2026
// Reads a line of input, splits it into tokens with strtok(), packs the tokens
// into a my_argv[] array, and runs the result as a program with execvp().
// This is the springboard for Lab 2.

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define bufferSize 256

int main( void ){
  char buffer[ bufferSize ];

  printf( "Enter a command: " );
  fflush( stdout );
  if ( fgets( buffer, bufferSize, stdin ) == NULL ) {
    if ( ferror( stdin ) ) {
      perror( "Error reading input" );
      return 1;
    }
    return 0;
  }

  // Reject a line that does not fit rather than run a shortened command.
  if ( strchr( buffer, '\n' ) == NULL ) {
    int next = getchar();
    if ( ferror( stdin ) ) {
      perror( "Error reading input" );
      return 1;
    }
    if ( next != '\n' && next != EOF ) {
      fprintf( stderr, "Input is too long (maximum %d characters).\n", bufferSize - 1 );
      return 1;
    }
  }

  // strip the trailing newline that fgets() leaves on the end
  buffer[ strcspn( buffer, "\n" ) ] = '\0';

  int max_args = 15;
  int max_argv_size = max_args + 2;   // one for argv[0], one for the NULL terminator
  char* cmd;
  char* my_argv[ max_argv_size ];

  // the command is always the first token
  cmd = strtok( buffer, " " );
  if ( cmd == NULL ) {
    return 0;
  }
  my_argv[0] = cmd;

  // each remaining token fills the next slot, then a NULL caps the array
  int i = 1;
  char* res = strtok( NULL, " " );
  while ( res != NULL && i < max_argv_size - 1 ) {
    my_argv[i] = res;
    i++;
    res = strtok( NULL, " " );
  }
  my_argv[i] = NULL;
  if ( res != NULL ) {
    fprintf( stderr, "Too many arguments (maximum %d).\n", max_args );
    return 1;
  }

  execvp( cmd, my_argv );

  // only reached if execvp() fails
  perror( "Error executing command" );
  return 1;
}
