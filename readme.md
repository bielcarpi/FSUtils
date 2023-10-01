# FSUtils
Utility that helps with FAT16 and EXT2 partitions. It allows you to easily:

- [x] Print information about an EXT2 or FAT16 partition
- [x] Read a file and cat its contents
- [x] Show a tree of the files in a partition

## Usage
```bash
# Clone the repository
$ git clone https://github.com/bielcarpi/FSUtils.git

# Compile it for your system
$ make

# Run it, with the -h flag to see the help
$ ./fsutils --help

# Print information about a partition
$ ./fsutils --info <partition>

# Read a file from the partition and cat its contents
$ ./fsutils --cat <partition> <file>

# Show a tree of the files in a partition
$ ./fsutils --tree <partition>
```

## Authors
Guillem Godoy (guillem.godoy@students.salle.url.edu)
Biel Carpi(biel.carpi@students.salle.url.edu)
