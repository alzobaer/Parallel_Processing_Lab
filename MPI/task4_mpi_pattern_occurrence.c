#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<mpi.h>

#define MAX_PATTERN_LENGTH 100

int countPatternOccurrences(const char* content, size_t contentSize, const char* pattern) {
    int count = 0;
    const char* currentPos = content;

    while ((currentPos = strstr(currentPos, pattern)) != NULL) {
        if(currentPos >= content + contentSize)
            break;
        count++;
        currentPos += strlen(pattern);
    }

    return count;
}

int main() {
    char *fileContent;
    char pattern[MAX_PATTERN_LENGTH];
    int fileSize;
    int *ocrs;

    int id, size;

    MPI_Init(NULL, NULL);
    MPI_Status status;
    MPI_Comm Comm = MPI_COMM_WORLD;
    MPI_Comm_rank(Comm, &id);
    MPI_Comm_size(Comm, &size);

    if(id == 0){
        FILE *file = fopen("pattern_occurrence.txt", "rb");

        // Determine the size of the file
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory to store the entire file content
        fileContent = (char*)malloc(fileSize + 1);

        // Read the file content into memory
        fread(fileContent, 1, fileSize, file);
        fileContent[fileSize] = '\0'; // Null-terminate the content
        fclose(file);
        
        // printf("Enter the pattern to count: ");
        // scanf("%s", pattern);
        strcpy(pattern, "Nvidia\0");
        ocrs = (int *)malloc(size * sizeof(int));
        MPI_Bcast(&fileSize, 1, MPI_INT, 0, Comm);
    }

    else {
        MPI_Bcast(&fileSize, 1, MPI_INT, 0, Comm);

        fileContent = (char*)malloc(fileSize + 1);
    }

    MPI_Barrier(Comm);
    MPI_Bcast(pattern, MAX_PATTERN_LENGTH, MPI_CHAR, 0, Comm);
    MPI_Bcast(fileContent, fileSize+1, MPI_CHAR, 0, Comm);

    int start = id * fileSize / size;
    int end = (id + 1) * fileSize / size;

    // printf("pid-%d: start=%d, end=%d, size=%d\n",id,  start, end, end-start);
    int occurrences = countPatternOccurrences(fileContent + start, end - start, pattern);
    // printf("Occurrences of the pattern '%s' for process-%d: %d\n", pattern, id, occurrences);
    free(fileContent);

    MPI_Gather(&occurrences, 1, MPI_INT, ocrs, 1, MPI_INT, 0, Comm);

    if(id == 0){
        int result = 0;
        for (int i = 0; i < size; i++)
            result += ocrs[i];
            printf("Total Occurrences of the pattern '%s': %d\n", pattern, result);
    }

    MPI_Finalize();

    return 0;
}