/* ============================================================ */ // Start of prologue comments block
/* Program: CEO Email Priority Queue                            */ // Title of the program
/* Description: Uses a MaxHeap to prioritize emails by sender   */ // Brief description of program purpose
/*              category and newest date.                       */ // Continuation of description
/* Inputs: Commands (EMAIL, NEXT, READ, COUNT) from file/stdin  */ // Defines expected inputs
/* Outputs: Terminal text displaying email counts and details   */ // Defines expected outputs
/* ============================================================ */ // End of prologue comments block

#include <stdio.h>  // Include standard I/O library for printf, fgets, and file operations
#include <stdlib.h> // Include standard library for memory allocation (malloc, realloc, free)
#include <string.h> // Include string library for string comparison and memory copying

#define INITIAL_CAPACITY 16 // Define the starting capacity for the dynamic heap array
#define MAX_LINE_LEN 1024   // Define the maximum character length for a single input line
#define MAX_SENDER_LEN 128  // Define the maximum character length for an email sender name
#define MAX_SUBJECT_LEN 512 // Define the maximum character length for an email subject line
#define MAX_DATE_LEN 16     // Define the maximum character length for an email date string

typedef struct { // Begin definition of the Email data structure
    char sender[MAX_SENDER_LEN];   // Array to store the sender's category string
    char subject[MAX_SUBJECT_LEN]; // Array to store the email's subject string
    char date[MAX_DATE_LEN];       // Array to store the date string (MM-DD-YYYY format)
    int categoryRank;              // Integer representing sender priority (higher is better)
    int dateValue;                 // Integer representing chronological date (YYYYMMDD)
} Email; // End definition of the Email structure

typedef struct { // Begin definition of the list-based MaxHeap structure
    Email *data;   // Pointer to the dynamically allocated array of Emails
    int size;      // Integer tracking the current number of emails in the heap
    int capacity;  // Integer tracking the maximum number of slots currently allocated
} MaxHeap; // End definition of the MaxHeap structure

static int categoryRank(const char *category) { // Function to map sender strings to priority integers
    if (strcmp(category, "Boss") == 0) return 5;             // Boss gets highest priority (5)
    if (strcmp(category, "Subordinate") == 0) return 4;      // Subordinate gets priority 4
    if (strcmp(category, "Peer") == 0) return 3;             // Peer gets priority 3
    if (strcmp(category, "ImportantPerson") == 0) return 2;  // ImportantPerson gets priority 2
    if (strcmp(category, "Other Person") == 0) return 1;     // Other Person gets lowest valid priority (1)
    return 0; // Return 0 as a fallback for any unrecognized sender categories
} // End of categoryRank function

static int dateToInt(const char *date) { // Function to convert MM-DD-YYYY string to sortable integer
    int mm = 0, dd = 0, yyyy = 0; // Initialize month, day, and year integer variables to zero
    if (sscanf(date, "%d-%d-%d", &mm, &dd, &yyyy) != 3) { // Parse string into the three integer variables
        return 0; // Return 0 if the date string is malformed or missing fields
    } // End of if statement checking for valid date parsing
    return yyyy * 10000 + mm * 100 + dd; // Mathematically format as YYYYMMDD for direct chronological comparison
} // End of dateToInt function

static void heapInit(MaxHeap *h) { // Function to initialize a new MaxHeap instance
    h->capacity = INITIAL_CAPACITY; // Set the heap's initial capacity to the defined constant
    h->size = 0; // Initialize the current size of the heap to zero elements
    h->data = (Email *)malloc(sizeof(Email) * h->capacity); // Allocate memory for the email array
    if (!h->data) { // Check if the memory allocation failed
        fprintf(stderr, "Fatal: out of memory initializing heap.\n"); // Print error message to standard error
        exit(1); // Exit the program with an error code
    } // End of if statement checking allocation
} // End of heapInit function

static void heapFree(MaxHeap *h) { // Function to cleanly deallocate the heap's memory
    free(h->data); // Free the dynamically allocated array of emails
    h->data = NULL; // Set the pointer to NULL to prevent dangling pointer issues
    h->size = 0; // Reset the heap size to zero
    h->capacity = 0; // Reset the heap capacity to zero
} // End of heapFree function

static void heapGrowIfNeeded(MaxHeap *h) { // Function to dynamically expand the heap array if it gets full
    if (h->size >= h->capacity) { // Check if the current size has reached the maximum capacity
        int newCapacity = h->capacity * 2; // Calculate a new capacity by doubling the current capacity
        Email *newData = (Email *)realloc(h->data, sizeof(Email) * newCapacity); // Reallocate array with new size
        if (!newData) { // Check if the memory reallocation failed
            fprintf(stderr, "Fatal: out of memory growing heap.\n"); // Print error message to standard error
            exit(1); // Exit the program with an error code
        } // End of if statement checking reallocation
        h->data = newData; // Update the heap's data pointer to the newly allocated array
        h->capacity = newCapacity; // Update the heap's capacity integer to reflect the new size
    } // End of if statement checking capacity
} // End of heapGrowIfNeeded function

static int hasHigherPriority(const Email *a, const Email *b) { // Function to compare two emails
    if (a->categoryRank != b->categoryRank) { // Check if the sender category ranks are different
        return a->categoryRank > b->categoryRank; // Return true if email 'a' has a higher sender rank
    } // End of if statement checking category ranks
    return a->dateValue > b->dateValue; // Tie-breaker: return true if email 'a' has a newer date
} // End of hasHigherPriority function

static void swapEmails(Email *a, Email *b) { // Function to swap two emails in memory
    Email tmp = *a; // Store the first email in a temporary variable
    *a = *b; // Overwrite the first email with the second email
    *b = tmp; // Overwrite the second email with the temporary variable
} // End of swapEmails function

static void siftUp(MaxHeap *h, int index) { // Function to bubble an element up the heap to its correct position
    while (index > 0) { // Continue looping as long as we have not reached the root index
        int parent = (index - 1) / 2; // Calculate the mathematical index of the current node's parent
        if (hasHigherPriority(&h->data[index], &h->data[parent])) { // Check if child has higher priority than parent
            swapEmails(&h->data[index], &h->data[parent]); // Swap the child and parent nodes
            index = parent; // Update the current index to the parent's old index to continue bubbling
        } else { // If the child does not have a higher priority than the parent
            break; // Stop bubbling up because the max-heap property is satisfied
        } // End of if/else checking priority
    } // End of while loop
} // End of siftUp function

static void siftDown(MaxHeap *h, int index) { // Function to trickle an element down the heap to its correct position
    while (1) { // Begin an infinite loop that will be broken manually
        int left = 2 * index + 1; // Calculate the mathematical index of the left child
        int right = 2 * index + 2; // Calculate the mathematical index of the right child
        int largest = index; // Assume the current index holds the largest priority element initially

        if (left < h->size && hasHigherPriority(&h->data[left], &h->data[largest])) { // Check if left child exists and is larger
            largest = left; // Update largest index to point to the left child
        } // End of if statement checking left child
        if (right < h->size && hasHigherPriority(&h->data[right], &h->data[largest])) { // Check if right child exists and is larger
            largest = right; // Update largest index to point to the right child
        } // End of if statement checking right child
        if (largest == index) { // Check if the largest element is still the original parent node
            break; // Stop trickling down because the max-heap property is satisfied
        } // End of if statement checking if swap is needed
        swapEmails(&h->data[index], &h->data[largest]); // Swap the parent with the largest child
        index = largest; // Update the current index to the child's old index to continue trickling
    } // End of while loop
} // End of siftDown function

static void heapInsert(MaxHeap *h, Email e) { // Function to insert a new email into the MaxHeap
    heapGrowIfNeeded(h); // Ensure there is enough capacity in the array before inserting
    h->data[h->size] = e; // Place the new email at the very end of the heap array
    siftUp(h, h->size); // Bubble the newly added email up to its proper priority position
    h->size++; // Increment the total size of the heap by one
} // End of heapInsert function

static int heapPeek(MaxHeap *h, Email *out) { // Function to view the highest priority email without removing it
    if (h->size == 0) return 0; // Return false/0 if the heap is entirely empty
    *out = h->data[0]; // Copy the root element (highest priority) into the provided output pointer
    return 1; // Return true/1 indicating success
} // End of heapPeek function

static int heapExtractMax(MaxHeap *h) { // Function to remove the highest priority email from the heap
    if (h->size == 0) return 0; // Return false/0 silently if the heap is already empty
    h->data[0] = h->data[h->size - 1]; // Move the very last element in the array to the root position
    h->size--; // Decrement the total size of the heap by one
    if (h->size > 0) { // Check if there are still elements left in the heap
        siftDown(h, 0); // Trickle the new root element down to its proper priority position
    } // End of if statement checking size
    return 1; // Return true/1 indicating successful removal
} // End of heapExtractMax function

static void trimLine(char *s) { // Function to remove trailing whitespace and newlines from a string
    size_t len = strlen(s); // Get the initial length of the string
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) { // Loop while trailing characters are line breaks
        s[--len] = '\0'; // Replace line break with null terminator and decrement length
    } // End of while loop trimming line breaks
} // End of trimLine function

static int parseEmailFields(char *fields, Email *out) { // Function to extract comma-separated values
    char *firstComma = strchr(fields, ','); // Find the memory address of the first comma
    if (!firstComma) return 0; // Return false if no first comma is found
    char *secondComma = strchr(firstComma + 1, ','); // Find the memory address of the second comma
    if (!secondComma) return 0; // Return false if no second comma is found

    *firstComma = '\0'; // Replace first comma with null terminator to isolate the sender string
    *secondComma = '\0'; // Replace second comma with null terminator to isolate the subject string

    char *senderStr = fields; // Pointer to the start of the isolated sender string
    char *subjectStr = firstComma + 1; // Pointer to the start of the isolated subject string
    char *dateStr = secondComma + 1; // Pointer to the start of the isolated date string

    while (*subjectStr == ' ') subjectStr++; // Advance pointer to skip any leading spaces in the subject
    while (*dateStr == ' ') dateStr++; // Advance pointer to skip any leading spaces in the date

    strncpy(out->sender, senderStr, MAX_SENDER_LEN - 1); // Copy sender string safely into the struct
    out->sender[MAX_SENDER_LEN - 1] = '\0'; // Ensure the sender string is explicitly null-terminated

    strncpy(out->subject, subjectStr, MAX_SUBJECT_LEN - 1); // Copy subject string safely into the struct
    out->subject[MAX_SUBJECT_LEN - 1] = '\0'; // Ensure the subject string is explicitly null-terminated

    strncpy(out->date, dateStr, MAX_DATE_LEN - 1); // Copy date string safely into the struct
    out->date[MAX_DATE_LEN - 1] = '\0'; // Ensure the date string is explicitly null-terminated

    out->categoryRank = categoryRank(out->sender); // Calculate and store the integer priority for the sender
    out->dateValue = dateToInt(out->date); // Calculate and store the chronological integer for the date
    return 1; // Return true indicating successful parsing
} // End of parseEmailFields function

static void handleEmailCommand(MaxHeap *h, char *line) { // Function to process incoming EMAIL commands
    char *fields = line + 5; // Advance pointer past the "EMAIL" command text
    while (*fields == ' ') fields++; // Skip any spaces between the command and the sender name

    Email e; // Declare a new local Email structure variable
    memset(&e, 0, sizeof(e)); // Zero out the memory of the struct to prevent garbage data
    if (!parseEmailFields(fields, &e)) { // Attempt to parse the string; check if it fails
        return; // Silently skip malformed email commands to prevent crashes
    } // End of if statement checking parsing success
    heapInsert(h, e); // Insert the successfully parsed email into the MaxHeap
} // End of handleEmailCommand function

static void handleNextCommand(MaxHeap *h) { // Function to execute the NEXT command
    Email top; // Declare an Email struct to temporarily hold the top result
    if (!heapPeek(h, &top)) { // Attempt to peek at the root of the heap; check if empty
        return; // Silently return without printing anything to handle the empty queue edge case
    } // End of if statement checking heap state
    printf("Next email:\nSender: %s\nSubject: %s\nDate: %s\n", top.sender, top.subject, top.date); // Print formatted multiline data
} // End of handleNextCommand function

static void handleReadCommand(MaxHeap *h) { // Function to execute the READ command
    heapExtractMax(h); // Call the extraction function which safely ignores empty heaps natively
} // End of handleReadCommand function

static void handleCountCommand(MaxHeap *h) { // Function to execute the COUNT command
    printf("There are %d emails to read.\n", h->size); // Print the current integer size of the heap
} // End of handleCountCommand function

static void processStream(FILE *in, MaxHeap *h) { // Function to read lines from a file or terminal
    char line[MAX_LINE_LEN]; // Declare a character buffer to hold the incoming text line

    while (fgets(line, sizeof(line), in)) { // Loop continuously, reading one line at a time until EOF
        trimLine(line); // Remove any trailing newlines from the extracted text

        if (line[0] == '\0') continue; // Skip processing if the line is completely blank

        if (strncmp(line, "EMAIL ", 6) == 0 || strcmp(line, "EMAIL") == 0) { // Check if command starts with EMAIL
            handleEmailCommand(h, line); // Pass the line to the email handler function
        } else if (strcmp(line, "NEXT") == 0) { // Check if the command matches NEXT exactly
            handleNextCommand(h); // Call the NEXT handler function
        } else if (strcmp(line, "READ") == 0) { // Check if the command matches READ exactly
            handleReadCommand(h); // Call the READ handler function
        } else if (strcmp(line, "COUNT") == 0) { // Check if the command matches COUNT exactly
            handleCountCommand(h); // Call the COUNT handler function
        } // End of command parsing if/else blocks
    } // End of while loop reading stream
} // End of processStream function

int main(int argc, char *argv[]) { // Main function entry point for the C program
    MaxHeap heap; // Declare a local MaxHeap variable on the stack
    heapInit(&heap); // Call initialization function to allocate the internal dynamic array

    if (argc >= 2) { // Check if the user provided a filename argument via command line
        FILE *fp = fopen(argv[1], "r"); // Attempt to open the provided file in read-only mode
        if (!fp) { // Check if the file pointer is null (file not found/locked)
            fprintf(stderr, "Error: could not open file '%s'\n", argv[1]); // Print error to standard error
            heapFree(&heap); // Free heap memory before terminating to prevent leaks
            return 1; // Return error code 1 to the operating system
        } // End of if statement checking file validity
        processStream(fp, &heap); // Process the file stream through the command loop
        fclose(fp); // Close the file stream safely
    } else { // If no file argument was provided via command line
        processStream(stdin, &heap); // Process manual keyboard input from standard input stream
    } // End of if/else checking arguments

    heapFree(&heap); // Free the dynamically allocated heap array before program finishes
    return 0; // Return success code 0 to the operating system
} // End of main function