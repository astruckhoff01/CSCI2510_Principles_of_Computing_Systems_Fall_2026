#include <unistd.h>
#define bufferSize 10

int main() {
    char buffer[bufferSize];
    
    while(1){
        ssize_t bytesRead = read(STDIN_FILENO, buffer, bufferSize);
        if (bytesRead <= 0) {
            break;
        }
        write(STDOUT_FILENO, buffer, bytesRead);
    }
    return 0;
}