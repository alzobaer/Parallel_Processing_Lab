#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 15

typedef struct {
    char name[MAX_NAME_LENGTH];
    char phone[MAX_PHONE_LENGTH];
} Contact;

// Function to search for a name in a range of contacts and print matching results
void searchPhonebook(char names[][50], char numbers[][50], char* searchName, int start, int end) {
    for (int i = start; i < end; i++) {
        // Case-sensitive search for the given name in the phonebook
        if (strstr(names[i], searchName) != NULL)
            // Print the matching name and corresponding phone number
            printf("%s\t%s\n", names[i], numbers[i]);
    }
}

int main(int argc, char **argv) {
    char names[1000][50];
    char contacts[1000][50];
    char searchName[50];

    int id, size;
    int line_count = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the rank (id) and size of the MPI communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int MAX_FILES = 10;
    const char* phonebookFiles[MAX_FILES];

    // the 'argc' (the number of command-line arguments) will be 1, as the program name itself is counted as an argument
    if (argc < 3) {
        if (id == 0) {
            // Print an error message if usage is incorrect
            fprintf(stderr, "Usage: %s <phonebook_file1> [<phonebook_file2> ...]\n", argv[0]);
        }
        // Finalize MPI and return an error code
        MPI_Finalize();
        return 1;
    }

    // Store command-line arguments (phonebook filenames) in an array
    for (int i = 1; i < argc; i++) {
        phonebookFiles[i - 1] = argv[i];
    }

    // Read and process phonebook files on the root process (id == 0)
    if (id == 0) {
        char c;
        FILE* file;

        // Loop through each specified phonebook file
        for (int i = 0; i < argc - 1; i++) {
            // Open the current phonebook file for reading
            file = fopen(phonebookFiles[i], "r");
            
            // Check if the file opening was successful
            if (file == NULL) {
                // Print an error message and abort MPI if opening fails
                fprintf(stderr, "Error opening file: %s\n", phonebookFiles[i]);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            // Read lines from the file and store names and contacts in arrays
            while (fscanf(file, "%49[^,],%111[^\n]\n", &names[line_count], &contacts[line_count]) != EOF) {
                line_count += 1;
            }

            fclose(file);       // Close the current file
        }
        // Prompt the user to enter the name to search for
        scanf("%s", searchName);
    }
    
    // Broadcast the line count, search name, and phonebook data to all processes
    MPI_Bcast(&line_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(searchName, 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Broadcast each name and contact to all processes
    for (int i = 0; i < line_count; i++) {
        MPI_Bcast(names[i], 50, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(contacts[i], 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Divide the work among processes based on line count and process ID
    int width = line_count / size;
    int start = id * width;
    int end;

    // Determine the end index for the current process
    if (id == (size - 1))
        end = line_count;
    else
        end = (id + 1) * width;

    // Call the search function for the assigned range of contacts
    searchPhonebook(names, contacts, searchName, start, end);

    MPI_Finalize();

    return 0;
}
