

// int main() {
//    char *messagePtr = "HELLOWORLD!";
//    printf("%s\n", messagePtr);

//    for (int i = 0; i < 11; i++) {
//        char char_print = messagePtr[i];
//        printf("%c\n", char_print);
//    }

//    for(int i = 0 ; i < 11; i++) { 
//        char char_to_print = *(messagePtr + i);
//        printf("%c\n", char_to_print);
//    }

//    while(*messagePtr != '\0') {
//        printf("%c\n", *messagePtr);
//        messagePtr++;
//    }

//    void printReverse( char* string ){
//        int length = 0;
//        while(string[length] != '\0') {
//            length++;
//        }
//        for(int i = length - 1; i >= 0; i--) {
//            printf("%c\n", string[i]);
//        }
//    }
    
//    printReverse(messagePtr);
//    return 0;
//}

#include <stdio.h>
#include <stdlib.h>

        char* reverseString( char* input ){

        //1. First count how many characters are in the input string
        int number_of_chars_in_input = 0;
        while(input[number_of_chars_in_input] != '\0') {
            number_of_chars_in_input++;
        }
        
        //This creates enough space to store the reversed string, plus one more byte
        //for the null terminator
        char* output = (char*)malloc( number_of_chars_in_input+1);

        //2. Copy the input string to the output string in reverse order. There are
        //multiple ways to do this- consider using a counter, or consider using two
        //pointers. 
        for(int i = number_of_chars_in_input - 1; i >= 0; i--) {
            output[number_of_chars_in_input - 1 - i] = input[i];
        }
        output[number_of_chars_in_input] = '\0';

        //REMEMBER THAT YOUR OUTPUT STRING MUST END WITH A NULL TERMINATOR. This is not
        //provided for you automatically- you must put it there!

            return output; 
        }

        int main() {
            char *messagePtr = "Good day!";
            char* reversedMessage = reverseString( messagePtr );
            printf("Reversed string: %s\n", reversedMessage);
            return 0;
        }