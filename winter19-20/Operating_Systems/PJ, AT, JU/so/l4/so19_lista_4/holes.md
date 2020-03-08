Holes
A hole in a file is an area represented by zeros. It is created when data is written at an offset beyond the current file size, or the file size is "truncated" to something larger than the current file size. The space between the old file size and the offset (or new file size) is filled by zeros. Most filesystems are smart enough to mark the holes in the inode, and not store them physically on disk (these are also known as sparse files). The filesystem marks blocks in the inode to denote that they are part of a hole. When a user requests data from an offset in a hole, the filesystem creates a page filled with zeroes and passes it to user space.

The handling of holes becomes a little tricky when the holes are not aligned to the filesystem block boundary. In that case, parts of blocks must be zeroed to represent the holes. For example, a 12k file on a filesystem with 4k block size with a hole at offset 2500 of size 8192, would require the last 1596 (4096-2500) bytes of the first block to be set to zero and the first 2500 bytes of the third block to be set to zeroes. The second block is bypassed in the inode's list of data blocks and does not occupy any space on disk.


