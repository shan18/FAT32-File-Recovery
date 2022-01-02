# FAT32 File Recovery

FAT32 is still one of the most widely used file systems in SD cards and USB flash drives due to its compatibility. This project is a simple recovery tool that can help in recovering accidentally deleted files from a FAT32 disk.

## Compiling

To compile the project, run the command

```
$ make
```

Execute the `run` executable file to display the usage instructions

```
$ ./run
Usage: ./run disk <options>
  -i                     Print the file system information.
  -l                     List the root directory.
  -r filename [-s sha1]  Recover a contiguously allocated file.
```

The first argument is the filename of the disk image. After that, any one of the options mentioned above can be given.

## Usage

This sections provides describes how the tool can be used to recover files from the disk.

_Note:_ All the outputs shown in the examples below are with respect to the disk created by following the instructions in the [Creating a FAT32 Disk Image](#Creating-a-FAT32-Disk-Image) section.

### Printing the file system information

The option `-i`, displays the following information about the FAT32 file system

- Number of FATs
- Number of bytes per sector
- Number of sectors per cluster
- Number of reserved sectors

#### Example

```
$ ./run fat32.disk -i
Number of FATs = 2
Number of bytes per sector = 512
Number of sectors per cluster = 1
Number of reserved sectors = 32
```

### Listing the root directory

The option `-l`, lists all valid entries in the root directory with the following information

- Filename
- File size
- Starting cluster

#### Example

```
$ ./run fat32.disk -l
HELLO.TXT (size = 14, starting cluster = 3)
DIR/ (size = 0, starting cluster = 4)
EMPTY (size = 0, starting cluster = 0)
Total number of entries = 3
```

### Recovering a file

The option `-r filename`, recovers a regular file with the specified name given the file is present in the root directory and is contiguously allocated in the disk.

#### Example

Let's first delete a file from the disk. The workflow below performs the following steps

- Mount the disk
- List all the files in the disk
- Display the contents of file `HELLO.TXT`
- Delete the file `HELLO.TXT`
- List all the files again to show that the file has actually been deleted
- Unmount the disk

```
$ sudo mount -o umask=0 fat32.disk /mnt/disk
$ ls -p /mnt/disk
DIR/  EMPTY  HELLO.TXT
$ cat /mnt/disk/HELLO.TXT
Hello, world.
$ rm /mnt/disk/HELLO.TXT
$ ls -p /mnt/disk
DIR/  EMPTY
$ sudo umount /mnt/disk
```

Now we can recover the file

```
$ ./run fat32.disk -r HELLO.TXT
HELLO.TXT: successfully recovered
```

Let's check the recovered file

```
$ sudo mount -o umask=0 fat32.disk /mnt/disk
$ ls -p /mnt/disk
DIR/  EMPTY  HELLO.TXT
$ cat /mnt/disk/HELLO.TXT
Hello, world.
```

#### Ambiguous file names

When a file is scheduled for deletion in FAT32 filesystem, instead of removing the contents of the file, only the first character of the filename is changed to the `0xe5` character. Thus, if two filenames differ only by their first character and both are deleted then there is no way for the tool on its own to identify which file to recover because after deletion both have the same name. To solve this ambiguity, check the next section.

The example below shows the workflow where recovering an ambiguous file can create an issue.

```
$ sudo mount -o umask=0 fat32.disk /mnt/disk
$ echo "My first name is Alan." > /mnt/disk/FNAME.TXT
$ echo "My last name is Turing." > /mnt/disk/LNAME.TXT
$ sync
$ rm /mnt/disk/FNAME.TXT /mnt/disk/LNAME.TXT
$ sudo umount /mnt/disk
$ ./run fat32.disk -r FNAME.TXT
FNAME.TXT: multiple candidates found
```

### Recovering a file with SHA-1 hash

To solve the aforementioned ambiguity, the user can provide a `SHA-1` hash via command-line option `-s sha1` to help identify which deleted directory entry should be the target file.

#### Example

```
$ ./run fat32.disk -r FNAME.TXT -s 28a2c7e038106041ac187931d86412fe96ac005f
FNAME.TXT: successfully recovered with SHA-1
$ ./run fat32.disk -l
HELLO.TXT (size = 14, starting cluster = 3)
DIR/ (size = 0, starting cluster = 4)
EMPTY (size = 0, starting cluster = 0)
FNAME.TXT (size = 22, starting cluster = 5)
Total number of entries = 4
```

## Creating a FAT32 Disk Image

If you don't have a FAT32 disk to try out this tool then just follow the steps below to create a FAT32 disk image from scratch.

1. Create an empty file of a certain size. In the example below, we create a 256KB empty file named `fat32.disk`.

   ```[bash]
   $ dd if=/dev/zero of=fat32.disk bs=256k count=1
   ```

1. Format the disk with FAT32. The example below creates the FAT32 filesystem with 2 FATs, 512 bytes per sector, 1 sector per cluster and 32 reserved sectors.

   ```[bash]
   $ mkfs.fat -F 32 -f 2 -S 512 -s 1 -R 32 fat32.disk
   ```

1. Mount the file system. The example below mounts the disk at the `/mnt/disk` directory.

   ```
   $ sudo mkdir /mnt/disk
   $ sudo mount -o umask=0 fat32.disk /mnt/disk
   ```

1. Create files in the file system.

   ```
   $ echo "Hello, world." > /mnt/disk/HELLO.TXT
   $ mkdir /mnt/disk/DIR
   $ touch /mnt/disk/EMPTY
   ```

1. Unmount the file system.
   ```
   $ sudo umount /mnt/disk
   ```

## Assumptions/Limitations

This is a very simple tool so it has the following assumptions in order to work properly.

- All files to be recovered are present in the root directory.
- The files to be recovered are regular files and not directories.
- All the filenames (along with the extension) contain ASCII characters and are in uppercase with atmost 7 characters for the file name, followed by a `.` which is again followed by atmost 3 characters for the extension.
- All the files have been contiguously allocated in the disk.
- To recover files with ambiguous filenames, their `SHA1` has to be known beforehand.

## Contact/Getting Help

If you need any help or want to report a bug, raise an issue in the repo.
