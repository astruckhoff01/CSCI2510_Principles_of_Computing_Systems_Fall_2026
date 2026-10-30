// Anna Struckhoff
// October 27, 2026
// Binary run-length encoding: one count byte followed by one base pattern.
// Usage: rle input output pattern_length mode (0 = encode, 1 = decode)

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void fail(const char *operation)
{
    perror(operation);
    exit(-1);
}

static void invalid(const char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(-1);
}

static long number(const char *text)
{
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0') {
        invalid("Expected an integer argument.");
    }
    return value;
}

// Accumulate short reads; only the final pattern may contain fewer bytes.
static size_t read_block(int fd, unsigned char *data, size_t capacity)
{
    size_t filled = 0;
    while (filled < capacity) {
        ssize_t received = read(fd, data + filled, capacity - filled);
        if (received < 0) {
            if (errno == EINTR) {
                continue;
            }
            fail("read");
        }
        if (received == 0) {
            break;
        }
        filled += (size_t)received;
    }
    return filled;
}

static void write_block(int fd, const unsigned char *data, size_t length)
{
    size_t written = 0;
    while (written < length) {
        ssize_t sent = write(fd, data + written, length - written);
        if (sent < 0) {
            if (errno == EINTR) {
                continue;
            }
            fail("write");
        }
        if (sent == 0) {
            errno = EIO;
            fail("write made no progress");
        }
        written += (size_t)sent;
    }
}

static void encode(int input, int output, size_t width)
{
    // Reserve byte zero for the counter so a record needs just one write.
    unsigned char *run = malloc(width + 1);
    unsigned char *lookahead = malloc(width + 1);
    if (run == NULL || lookahead == NULL) {
        fail("malloc");
    }

    size_t run_length = read_block(input, run + 1, width);
    unsigned int count = 1;

    while (run_length != 0) {
        size_t next_length = read_block(input, lookahead + 1, width);
        if (next_length == run_length && count < 255 &&
            memcmp(run + 1, lookahead + 1, run_length) == 0) {
            ++count;
            continue;
        }

        run[0] = (unsigned char)count;
        write_block(output, run, run_length + 1);

        // Reuse the lookahead as the next run instead of copying its bytes.
        unsigned char *swap = run;
        run = lookahead;
        lookahead = swap;
        run_length = next_length;
        count = 1;
    }

    free(run);
    free(lookahead);
}

static void decode(int input, int output, size_t width)
{
    unsigned char *pattern = malloc(width);
    if (pattern == NULL) {
        fail("malloc");
    }

    unsigned char count;
    while (read_block(input, &count, 1) != 0) {
        if (count == 0) {
            invalid("Invalid encoded file: a run cannot have a zero count.");
        }
        size_t length = read_block(input, pattern, width);
        if (length == 0) {
            invalid("Invalid encoded file: count byte has no pattern.");
        }
        if (length < width && count != 1) {
            invalid("Invalid encoded file: repeated incomplete final pattern.");
        }
        // Write each repetition directly; no expanded-run allocation needed.
        for (unsigned int repetition = 0; repetition < count; ++repetition) {
            write_block(output, pattern, length);
        }
    }

    free(pattern);
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s input output pattern_length mode\n", argv[0]);
        invalid("Use mode 0 to compress or mode 1 to decompress.");
    }

    long length = number(argv[3]);
    long mode = number(argv[4]);
    if (length < 1 || length > SSIZE_MAX - 1) {
        invalid("Pattern length must be a positive supported integer.");
    }
    if (mode != 0 && mode != 1) {
        invalid("Mode must be 0 or 1.");
    }

    int input = open(argv[1], O_RDONLY);
    if (input < 0) {
        fail("open input");
    }
    // Check an existing destination before O_TRUNC can erase its contents.
    struct stat source, destination;
    if (fstat(input, &source) < 0) {
        fail("fstat");
    }
    if (stat(argv[2], &destination) == 0) {
        if (source.st_dev == destination.st_dev &&
            source.st_ino == destination.st_ino) {
            invalid("Input and output must be different files.");
        }
    } else if (errno != ENOENT) {
        fail("stat output");
    }
    int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
                      S_IRUSR | S_IWUSR);
    if (output < 0) {
        fail("open output");
    }

    if (mode == 0) {
        encode(input, output, (size_t)length);
    } else {
        decode(input, output, (size_t)length);
    }

    int input_closed = close(input);
    int input_error = errno;
    int output_closed = close(output);
    if (input_closed < 0) {
        errno = input_error;
        fail("close input");
    }
    if (output_closed < 0) {
        fail("close output");
    }
    return 0;
}
