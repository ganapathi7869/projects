//Implementation for all file system commands


// Get the size of a file in bytes
int getfilesize(char *filename);

// Checks whether the given file is a text file
int istextfile(char *filename);

// Format the disk with required BLOCKSIZE.
int format();

// Validates the disk format
int checkformatcorrectness();

// Updates header with a new file
void insertfiletoheader(char *target, unsigned int startblock, unsigned int reqblocks, unsigned int lenoffile, unsigned int *blocks);

// Gives list of available block numbers
unsigned int *getfreeblocks(unsigned int reqblocks);

// Copy a file from user's local to the file system(upload).
void copyin(char *source);

// Copy a file from file system to the user's local(download).
void copyout(char *source, char *target);

// Print the content of a text file.
void cat(char *file);

// Delete a file from the file system.
int deletefile(char *file);

// List all the files available in the file system
void listfiles();

// Prints the content of a text file.
void printdebuginfo();
