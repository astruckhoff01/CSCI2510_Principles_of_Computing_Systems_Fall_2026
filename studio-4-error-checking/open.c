#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define bufferSize 200

int main( int argc, char* argv[] ){
  if ( argc != 2 ) {
    fprintf( stderr, "usage: %s <filename>\n", argv[0] );
    return -1;
  }

  int fd = open( argv[1], O_RDONLY );
  if ( fd == -1 ) {                   
    perror( "Error opening file" );
    return -1;
  }

  char buffer[ bufferSize ];
  ssize_t bytesRead;

  while ( ( bytesRead = read( fd, buffer, bufferSize ) ) > 0 ) {
    if ( write( STDOUT_FILENO, buffer, bytesRead ) == -1 ) {
      perror( "Error writing to standard output" );
      close( fd );
      return -1;
    }
  }

  if ( bytesRead == -1 ) {
    perror( "Error reading file" );
    close( fd );
    return -1;
  }

  if ( close( fd ) == -1 ) {
    perror( "Error closing file" );
    return -1;
  }

  return 0;
}
