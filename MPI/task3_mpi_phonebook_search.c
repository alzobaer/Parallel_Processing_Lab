#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_FILES 10
#define MAX_LINE_LENGTH 1000
#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 15

int main(int argc, char **argv) {
    char names[MAX_LINE_LENGTH][MAX_NAME_LENGTH];
    char contacts[MAX_LINE_LENGTH][MAX_PHONE_LENGTH];
    char searchName[MAX_NAME_LENGTH];

    int id, size;
    int line_count = 0;

    double cal_start_time, cal_end_time;

    MPI_Init(&argc, &argv);

    // Get the rank (id) and size of the MPI communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *phonebookFiles[MAX_FILES];

    // the 'argc' (the number of command-line arguments) will be 1, as the program name itself is counted as an argument
    if (argc < 3) {
        if (id == 0) {
            fprintf(stderr, "Usage: %s <phonebook_file1> [<phonebook_file2> ...]\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    // Store command-line arguments (phonebook filenames) in an array
    for (int i = 1; i < argc; i++) {
        phonebookFiles[i - 1] = argv[i];
    }

    cal_start_time = MPI_Wtime();

    // Read and process phonebook files on the root process (id == 0)
    if (id == 0) {
        char c;
        FILE* file;

        for (int i = 0; i < argc - 1; i++) {
            // Open the current phonebook file for reading
            file = fopen(phonebookFiles[i], "r");
            
            if (file == NULL) {
                fprintf(stderr, "Error opening file: %s\n", phonebookFiles[i]);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            // Read lines from the file and store names and contacts in arrays
            while (fscanf(file, "%49[^,],%14[^\n]\n", &names[line_count], &contacts[line_count]) != EOF) {
                line_count += 1;
            }

            fclose(file);       // Close the current file
        }
        // Prompt the user to enter the name to search for
        scanf("%s", searchName);
    }
    
    // Broadcast the line count, search name to all processes
    MPI_Bcast(&line_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(searchName, 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Broadcast each name and contact to all processes
    for (int i = 0; i < line_count; i++) {
        MPI_Bcast(names[i], 50, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(contacts[i], 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);


    // Divide the work among processes based on line count and process ID
    int width = line_count / size;
    int start = id * width;
    int end;

    // Determine the end index for the current process
    if (id == (size - 1))
        end = line_count;   // higher rank executes till last line
    else
        end = (id + 1) * width;
        
    // search for a name in a range of contacts and print matching results
    for (int i = start; i < end; i++) {
        // Case-sensitive search for the given name in the phonebook
        if (strstr(names[i], searchName) != NULL){      // searches for the substring searchName within the string names[i]
            //printf("%s\t%s\n", names[i], contacts[i]);
        }
    }


    cal_end_time = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    printf("PID %d: Total time: %lf\n", id, (cal_end_time - cal_start_time));   

    MPI_Finalize();

    return 0;
}
