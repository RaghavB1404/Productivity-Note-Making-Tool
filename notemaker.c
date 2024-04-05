//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*LIBRARY IMPORTS*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*STRUCTURES FOR EACH FILES FOLDERS AND TASKS*/
// Structure to represent a file node
struct FileNode{
    char filename[256];
    struct FileNode *next;
    struct FileNode *prev;
};

// Structure to represent a folder node
struct FolderNode{
    char foldername[256];
    struct FileNode *fileList;
    struct FolderNode *parentFolder;
    struct FolderNode *nextFolder;
    struct FolderNode *prevFolder;
};
// Structure to represent a task node
struct TaskNode{
    char taskname[256];
    int priority;
    struct TaskNode *next;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*CREATING FILE, FOLDER AND TASK NODES*/
// Function to create a new note node
struct FileNode *createFileNode(const char *filename){
    struct FileNode *newNode = (struct FileNode *)malloc(sizeof(struct FileNode));
    if (newNode == NULL){
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    strncpy(newNode->filename, filename, sizeof(newNode->filename));
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}
//Function to create a new folder node
struct FolderNode *createFolderNode(const char *foldername, struct FolderNode *parentFolder){
    struct FolderNode *newNode = (struct FolderNode *)malloc(sizeof(struct FolderNode));
    if (newNode == NULL){
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    strncpy(newNode->foldername, foldername, sizeof(newNode->foldername));
    newNode->fileList = NULL;
    newNode->nextFolder = NULL;
    newNode->prevFolder = NULL;
    newNode->parentFolder = parentFolder;
    return newNode;
}
//Function to create a new task node
struct TaskNode *createTaskNode(const char *taskname, int priority){
    struct TaskNode *newNode = (struct TaskNode *)malloc(sizeof(struct TaskNode));
    if (newNode == NULL){
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    strncpy(newNode->taskname, taskname, sizeof(newNode->taskname));
    newNode->priority = priority;
    newNode->next = NULL;
    return newNode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*MENU FUNCTIONALITIES*/
// 1) Function to add a note to the doubly linked list
void addFileToList(struct FileNode **head, const char *filename){
    struct FileNode *newNode = createFileNode(filename);

    if (*head == NULL){
        *head = newNode;
    }
    else{
        struct FileNode *current = *head;
        while (current->next != NULL){
            current = current->next;
        }
        current->next = newNode;
        newNode->prev = current;
    }
}

// 2) Function to open a note in Notepad
void openFileInNotepad(const char *filename){
    char command[256];
    snprintf(command, sizeof(command), "notepad %s", filename);
    system(command);
}

// 3) Function to print the list of files
void printFileList(struct FileNode *head){
    struct FileNode *current = head;
    int index = 1;
    while (current != NULL){
        printf("%d. %s\n", index, current->filename);
        current = current->next;
        index++;
    }
}

// 4) Function to delete a file from the doubly linked list and the file system
void deleteFile(struct FileNode **head, const char *filename){
    struct FileNode *current = *head;

    while (current != NULL){
        if (strcmp(current->filename, filename) == 0){
            if (current->prev != NULL){
                current->prev->next = current->next;
            }
            else{
                *head = current->next;
            }

            if (current->next != NULL){
                current->next->prev = current->prev;
            }

            remove(filename); // Delete the file from the file system
            free(current);
            return;
        }
        current = current->next;
    }
}

// 5) Function to add a folder to the doubly linked list of folders
void addFolderToTree(struct FolderNode **root, const char *foldername, struct FolderNode *parentFolder){
    struct FolderNode *newNode = createFolderNode(foldername, parentFolder);
    if (*root == NULL){
        *root = newNode;
    }
    else{
        struct FolderNode *current = *root;
        while (current->nextFolder != NULL){
            current = current->nextFolder;
        }
        current->nextFolder = newNode;
        newNode->prevFolder = current;
    }
}

// 6) Function to navigate to a child folder
struct FolderNode *navigateToChildFolder(struct FolderNode *currentFolder, const char *foldername){
    struct FolderNode *newFolder = currentFolder->nextFolder;
    while (newFolder != NULL){
        if (strcmp(newFolder->foldername, foldername) == 0){
            return newFolder;
        }
        newFolder = newFolder->nextFolder;
    }
    return NULL; // Folder not found
}

// 7) Function to navigate to the parent folder
struct FolderNode *navigateToParentFolder(struct FolderNode *currentFolder){
    return currentFolder->parentFolder;
}

// 8) Function to delete a folder and its contents, including files
void deleteFolder(struct FolderNode **root, const char *foldername){
    struct FolderNode *current = *root;
    while (current != NULL){
        if (strcmp(current->foldername, foldername) == 0){
            if (current->prevFolder != NULL){
                current->prevFolder->nextFolder = current->nextFolder;
            }
            else{
                *root = current->nextFolder;
            }

            if (current->nextFolder != NULL){
                current->nextFolder->prevFolder = current->prevFolder;
            }

            // Delete the files in the folder
            struct FileNode *fileList = current->fileList;
            while (fileList != NULL){
                remove(fileList->filename); // Delete the file from the file system
                struct FileNode *tempFile = fileList;
                fileList = fileList->next;
                free(tempFile);
            }

            free(current);
            return;
        }
        current = current->nextFolder;
    }
}

// 9) Add task to task list
void addTaskToQueue(struct TaskNode **head, const char *taskname, int priority){
    // Check if a task with the given priority already exists
    struct TaskNode *current = *head;
    while (current != NULL){
        if (current->priority == priority){
            printf("Priority %d already has a task: %s\n", priority, current->taskname);
            return;
        }
        current = current->next;
    }

    // If no task with the given priority exists, add the new task
    struct TaskNode *newNode = createTaskNode(taskname, priority);

    if (*head == NULL || priority < (*head)->priority){
        newNode->next = *head;
        *head = newNode;
    }
    else{
        current = *head;
        while (current->next != NULL && current->next->priority <= priority){
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// 10) Remove prioritized task upon completion
void executeHighestPriorityTask(struct TaskNode **head){
    if (*head != NULL){
        struct TaskNode *taskToExecute = *head;
        printf("Executing task with priority %d: %s\n", taskToExecute->priority, taskToExecute->taskname);
        *head = taskToExecute->next;
        free(taskToExecute);
    }
    else{
        printf("No tasks in the priority queue.\n");
    }
}

// 11) View the task list
void printTaskQueue(struct TaskNode *head){
    struct TaskNode *current = head;
    int index = 1;
    while (current != NULL){
        printf("%d. Priority %d: %s\n", index, current->priority, current->taskname);
        current = current->next;
        index++;
    }
}

// 12) Delete a task from the task list
void deleteTask(struct TaskNode **head, int taskIndex){
    struct TaskNode *current = *head;
    struct TaskNode *prev = NULL;
    int index = 1;

    while (current != NULL && index < taskIndex){
        prev = current;
        current = current->next;
        index++;
    }

    if (current != NULL){
        if (prev != NULL){
            prev->next = current->next;
        }
        else{
            *head = current->next;
        }

        free(current);
        printf("\n");
        printf(" Task deleted.\n");
    }
    else{
        printf("\n");
        printf(" Invalid task index.\n");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*FREE THE FILE, FOLDER AND TASKS LIST*/
// Function to free the memory allocated for the doubly linked list of files
void freeFileList(struct FileNode *head){
    while (head != NULL){
        struct FileNode *temp = head;
        head = head->next;
        free(temp);
    }
}

// Function to free the memory allocated for the doubly linked list of folders
void freeFolderTree(struct FolderNode *root){
    while (root != NULL){
        struct FolderNode *currentFolder = root;
        root = root->nextFolder;
        freeFileList(currentFolder->fileList);
        freeFolderTree(currentFolder->nextFolder);
        free(currentFolder);
    }
}

//Function to free the queue alloted to task manager
void freeTaskQueue(struct TaskNode *head){
    while (head != NULL){
        struct TaskNode *temp = head;
        head = head->next;
        free(temp);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*MAIN FUNCTION*/
int main(){
    struct FolderNode *rootFolder = createFolderNode("Root", NULL);
    struct FolderNode *currentFolder = rootFolder;
    struct TaskNode *priorityQueue = NULL;

    int choice;
    char filename[256];
    char foldername[256];
    char taskname[256];
    int priority;
    while (1){
        printf("\n");
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("__________________________________________________________________\n");
        printf("| NOTE MAKER APPLICATION                                         |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| Current Topic Folder: %s                                     |\n", currentFolder->foldername);
        printf("|----------------------------------------------------------------|\n");
        printf("| NOTES MANAGER                                                  |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| 1. Add a Note to the current Topic Folder                      |\n");
        printf("| 2. Open a Note                                                 |\n");
        printf("| 3. View all the Notes in the current Topic Folder              |\n");
        printf("| 4. Delete a Note from the current Topic Folder                 |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| TOPIC FOLDER MANAGER                                           |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| 5. Create a new Topic Folder in the current Topic Folder       |\n");
        printf("| 6. Navigate to a child Topic Folder                            |\n");
        printf("| 7. Navigate to the parent Topic Folder                         |\n");
        printf("| 8. Delete the Topic Folder of your choice                      |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| TASK MANAGER                                                   |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("|  9. Add a Task                                                 |\n");
        printf("| 10. Execute Highest Priority Task                              |\n");
        printf("| 11. View Task List                                             |\n");
        printf("| 12. Delete a Task                                              |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| 13. Exit                                                       |\n");
        printf("__________________________________________________________________\n");
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf(" Choose an option: ");
        scanf("%d", &choice);

        switch (choice){
        case 1:
            printf("\n");
            printf(" Enter the Note Title (e.g., mynote): ");
            scanf("%s", filename);
            addFileToList(&(currentFolder->fileList), filename);
            break;
        case 2:
            if (currentFolder->fileList != NULL){
                printFileList(currentFolder->fileList);
                printf("\n");
                printf(" Enter the index of the Note to open: ");
                int fileIndex;
                scanf("%d", &fileIndex);
                struct FileNode *current = currentFolder->fileList;
                int currentIndex = 1;
                while (current != NULL && currentIndex < fileIndex){
                    current = current->next;
                    currentIndex++;
                }

                if (current != NULL){
                    openFileInNotepad(current->filename);
                }
                else{
                    printf("\n");
                    printf(" Invalid index.\n");
                }
            }
            else{
                printf("\n");
                printf(" The Topic Folder is empty.\n");
            }
            break;
        case 3:
            if (currentFolder->fileList != NULL){
                printf("\n");
                printf(" List of Notes in %s:\n", currentFolder->foldername);
                printFileList(currentFolder->fileList);
            }
            else{
                printf("\n");
                printf(" The Topic Folder is empty.\n");
            }
            break;
        case 4:
            if (currentFolder->fileList != NULL){
                printFileList(currentFolder->fileList);
                printf("\n");
                printf(" Enter the index of the Note to delete: ");
                int fileIndex;
                scanf("%d", &fileIndex);

                struct FileNode *current = currentFolder->fileList;
                int currentIndex = 1;
                while (current != NULL && currentIndex < fileIndex){
                    current = current->next;
                    currentIndex++;
                }

                if (current != NULL){
                    deleteFile(&(currentFolder->fileList), current->filename);
                    printf("\n");
                    printf(" Note deleted.\n");
                }
                else{
                    printf("\n");
                    printf(" Invalid index.\n");
                }
            }
            else{
                printf("\n");
                printf(" The Topic Folder is empty.\n");
            }
            break;
        case 5:
            printf("\n");
            printf(" Enter the name of the new Topic Folder: ");
            scanf("%s", foldername);
            addFolderToTree(&(currentFolder->nextFolder), foldername, currentFolder);
            break;
        case 6:
            printf("\n");
            printf(" Enter the name of the Topic Folder to navigate to: ");
            scanf("%s", foldername);
            struct FolderNode *childFolder = navigateToChildFolder(currentFolder, foldername);
            if (childFolder != NULL){
                currentFolder = childFolder;
            }
            else{
                printf("\n");
                printf(" Topic Folder not found.\n");
            }
            break;
        case 7:;
            struct FolderNode *parentFolder = navigateToParentFolder(currentFolder);
            if (parentFolder != NULL){
                currentFolder = parentFolder;
            }
            else{
                printf("\n");
                printf(" You are already in the Default Workspace.\n");
            }
            break;
        case 8:
            printf("\n");
            printf(" Enter the name of the Topic Folder to delete: ");
            scanf("%s", foldername);

            if (currentFolder->nextFolder != NULL){
                struct FolderNode *folderToDelete = currentFolder->nextFolder;
                while (folderToDelete != NULL){
                    if (strcmp(folderToDelete->foldername, foldername) == 0){
                        deleteFolder(&(currentFolder->nextFolder), foldername);
                        printf(" Topic Folder deleted.\n");
                        break;
                    }
                    folderToDelete = folderToDelete->nextFolder;
                }
                if (folderToDelete == NULL){
                    printf(" Topic Folder not found.\n");
                }
            }
            else{
                printf(" No Topic Folders to delete.\n");
            }
            break;
        case 9:
            printf("\n");
            printf(" Enter the Task Name and Description: ");
            scanf(" %255[^\n]", taskname);
            int w = 9;
            printf(" Enter the Task Priority (an integer): ");
            scanf("%d", &priority);
            addTaskToQueue(&priorityQueue, taskname, priority);
            break;
        case 10:
            executeHighestPriorityTask(&priorityQueue);
            break;
        case 11:
            if (priorityQueue != NULL){
                printf("\n");
                printf(" Task List:\n");
                printTaskQueue(priorityQueue);
            }
            else{
                printf("\n");
                printf(" The Task List is empty.\n");
            }
            break;
        case 12:
            if (priorityQueue != NULL){
                printf("\n");
                printf(" Task List:\n");
                printTaskQueue(priorityQueue);
                printf(" Enter the index of the Task to delete: ");
                int taskIndex;
                scanf("%d", &taskIndex);
                deleteTask(&priorityQueue, taskIndex);
            }
            else{
                printf("\n");
                printf(" The Task List is empty.\n");
            }
            break;

        case 13:
            // Free memory used by the tree and exit
            freeTaskQueue(priorityQueue);
            freeFolderTree(rootFolder);
            exit(0);
        default:
            printf("\n");
            printf(" Invalid Menu Choice.\n");
        }
    }return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////