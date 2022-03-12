
// Disk I/O
extern char MOUNTEDDISK[];
extern unsigned int BLOCKSIZE; // default: 16KB

// Get the content of a block
char *disk_read(unsigned int block);

// Update the content of a block
void disk_write(char *buf, unsigned int block);
