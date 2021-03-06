#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H

#include <stdbool.h>
#include "filesys/off_t.h"
#include "devices/block.h"
#include <list.h>
#include "threads/synch.h"

#define DIRECT_BLOCK 123
#define INDIRECT_BLOCK 1
#define DOUBLY_INDIRECT_BLOCK 1
#define INDEX_BLOCK 128

struct bitmap;

/* Inode type */
enum inode_type
{
  INODE_FILE,   /* File inode */
  INODE_DIR,    /* Directory inode */
};

/* In-memory inode. */
struct inode 
  {
    struct list_elem elem;              /* Element in inode list. */
    block_sector_t sector;              /* Sector number of disk location. */
    int open_cnt;                       /* Number of openers. */
    bool removed;                       /* True if deleted, false otherwise. */
    int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
    struct lock extension_lock;         /* Extension lock */
    enum inode_type type;               /* Inode type (INODE_FILE or INODE_DIR */
    off_t pos;                          /* If directory, current position */    
  };

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
  {
    off_t length;                       /* File size in bytes */
    enum inode_type type;               /* Inode type */
    block_sector_t direct[DIRECT_BLOCK];                
    block_sector_t indirect[INDIRECT_BLOCK];          
    block_sector_t doubly_indirect[DOUBLY_INDIRECT_BLOCK];    
    unsigned magic;                     /* Magic number. */
  };

struct index_disk
  {
    block_sector_t index[INDEX_BLOCK];
  };      

void inode_init (void);
bool inode_create (block_sector_t, off_t, enum inode_type);
struct inode *inode_open (block_sector_t);
struct inode *inode_reopen (struct inode *);
block_sector_t inode_get_inumber (const struct inode *);
void inode_close (struct inode *);
void inode_remove (struct inode *);
off_t inode_read_at (struct inode *, void *, off_t size, off_t offset);
off_t inode_write_at (struct inode *, const void *, off_t size, off_t offset);
void inode_deny_write (struct inode *);
void inode_allow_write (struct inode *);
off_t inode_length (const struct inode *);
void inode_extend (struct inode *, size_t);
#endif /* filesys/inode.h */
