#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<mpi.h>

#define MAX_NAME_LENGTH 50
#define MAX_PHONE_LENGTH 15

typedef struct {
    char name[MAX_NAME_LENGTH];
    char phone[MAX_PHONE_LENGTH];
} Contact;

void searchPhonebook(char names[][50], char numbers[][50], char* searchName, int start, int end) {

    for (int i = start; i < end; i++){
        if(strstr(names[i], searchName) != NULL)
            printf("%s\t%s\n", names[i], numbers[i]);
    }
}

int main() {
    char names[1000][50];
    char contacts[1000][50];
    char searchName[50];

    int id, size;
    int line_count = 0;
    const char* phonebookFile = "phonebook.txt";

    MPI_Init(NULL, NULL);
    MPI_Comm Comm = MPI_COMM_WORLD;
    MPI_Status status;

    MPI_Comm_rank(Comm, &id);
    MPI_Comm_size(Comm, &size);

    if(id == 0){
        char c;
        FILE* file = fopen(phonebookFile, "r");

        line_count = 0;
        while(fscanf(file, "%49[^,],%111[^\n]\n", &names[line_count], &contacts[line_count])!= EOF){
            // printf("%s\t%s\n", names[line_count], contacts[line_count]);
            line_count += 1;
        }
        fclose(file);
        scanf("%s", searchName);
    }

    MPI_Bcast(&line_count, 1, MPI_INT, 0, Comm);
    MPI_Bcast(searchName, 50, MPI_INT, 0, Comm);
    MPI_Barrier(Comm);
    // printf("PID-%d: LineCount: %d\n",id, line_count);

    for (int i = 0; i < line_count; i++){
        MPI_Bcast(names[i], 50, MPI_CHAR, 0, Comm);
        MPI_Bcast(contacts[i], 50, MPI_CHAR, 0, Comm);
    }

    int width = line_count / size;
    int start = id * width;
    int end;

    if(id == (size - 1))
        end = line_count;
    else
        end = (id + 1) * width;

    searchPhonebook(names, contacts, searchName, start, end);

    // cout << id << " " << start << " " << end << " " << line_count << endl;

    MPI_Finalize();

    return 0;
}
