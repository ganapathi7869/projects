
// Disk I/O
extern char MOUNTEDDISK[];
extern unsigned int BLOCKSIZE; // default: 16KB

// Get the content of a block
char *getblock(unsigned int block);

// Update the content of a block
void writetoblock(char *buf, unsigned int block);
