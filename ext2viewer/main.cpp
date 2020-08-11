#include <stdio.h>
#include <stdint.h>
#include <ctime>

#define EXT2_SIGNATURE 0xef53

struct PartitionTable {
    uint8_t boostrap[440];
    uint32_t disk_signature;
    uint16_t copy_protected;

    struct Partition {
        uint8_t boot_flag;
        uint8_t chs_first_head;
        uint8_t chs_first_cyl_sect;
        uint8_t chs_first_cyl;
        uint8_t type;
        uint8_t chs_last_head;
        uint8_t chs_last_cyl_sect;
        uint8_t chs_last_cyl;
        uint32_t lba;
        uint32_t num_sectors;
    };
    Partition partition[4];
    uint16_t boot_signature;
} __attribute__((packed));

struct Superblock {
    uint32_t num_inodes;
    uint32_t num_blocks;
    uint32_t num_reserved_blocks;
    uint32_t num_unallocated_blocks;
    uint32_t num_unallocated_inodes;
    uint32_t block_number_superblock;
    uint32_t block_size;
    uint32_t fragment_size;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mount_time;
    uint32_t last_written_time;
    uint16_t consis1;
    uint16_t consis2;
    uint16_t sig;
    uint16_t file_system_state;
    uint16_t error_handling;
    uint16_t version_minor;
    uint32_t consis_last_time;
    uint32_t consis_interval_time;
    uint32_t os_id;
    uint32_t version_major;
    uint16_t user_id;
    uint16_t group_id;
    
    uint32_t first_non_reserved_inode;
    uint16_t size_of_each_inode;
    uint16_t block_group_superblock;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t read_only_features;
    char file_system_id[16];
    char volume_name[16];
    char path_mounted[64];
    uint32_t compression_algorithms;
    uint8_t file_preallocate_block;
    uint8_t directory_preallocate_block;
    uint16_t unused0;
    char journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t head_orphan_inode;
    uint8_t unused1[787];
} __attribute__((packed));

struct BlockDescriptor {
        uint32_t block_usage_bitmap_block_address;
        uint32_t inode_usage_bitmap_block_address;
        uint32_t inode_table_block_address;
        uint16_t num_unallocated_blocks;
        uint16_t num_unallocated_inodes;
        uint8_t unused[14];
} __attribute__((packed));

struct Inode {
    uint16_t type_permissions;
    uint16_t user_id;
    uint32_t lower_size;
    uint32_t last_access_time;
    uint32_t creation_time;
    uint32_t last_modification_time;
    uint32_t deletion_time;
    uint16_t group_id;
    uint16_t hard_links;
    uint32_t disk_sectors;
    uint32_t flags;
    uint32_t os_specific1;
    uint32_t block_pointer[12];
    uint32_t singly_block_pointer;
    uint32_t double_block_pointer;
    uint32_t triply_block_pointer;
    uint32_t generation_number;
    uint32_t extended_block_attrib;
    uint32_t upper_size;
    uint32_t fragment_block_address;
    uint8_t os_specific2[12];
} __attribute__((packed));

const char* partition_type[] = {
    "","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f",
    "10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f",
    "20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f",
    "30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f",
    "40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f",
    "50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f",
    "60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f",
    "70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f",
    "80","81","82","linux","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f",
    "90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f",
    "a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af",
    "b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf",
    "c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf",
    "d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df",
    "e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef",
    "f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"
};

struct EXT2 {
    Superblock* superblock;
    BlockDescriptor* block_descriptor_table;
    Inode** inode;
    uint32_t blocksize;
    uint32_t num_blockgroups;
    uint32_t partition_byte_offset;
};

void print_partition_table(PartitionTable p) {
    printf("PARTITION TABLE FOR %u\n", p.disk_signature);
    printf("#############################################\n");
    printf("Name        | Bootable |   Start   |   Length  | Type\n");
    for (int i = 0; i < 4; ++i) {
        printf("Partition %d | %c        |%8u   | %8u  | %s\n",
                    i,
                    p.partition[i].boot_flag==0x80?'*':' ',
                    p.partition[i].lba,
                    p.partition[i].num_sectors,
                    partition_type[p.partition[i].type]);
    }
}

void print_superblock(EXT2* e) {
    printf("EXT2 version %u.%u SUPERBLOCK FOR \"%s\"\n",
            e->superblock->version_major, e->superblock->version_minor, e->superblock->volume_name);
    printf("(SUPERBLOCK LOCATED AT BLOCK %u)\n", e->superblock->block_number_superblock);
    printf("#############################################\n");
    printf("Block size: %llu\n", 1024 << e->superblock->block_size);
    printf("Fragment size: %llu\n", 1024 << e->superblock->fragment_size);
    printf("Blocks used: %u / %u\n",
            e->superblock->num_blocks - e->superblock->num_unallocated_blocks, e->superblock->num_blocks);
    printf("Inodes used: %u / %u\n",
            e->superblock->num_inodes - e->superblock->num_unallocated_inodes, e->superblock->num_inodes);
    printf("Reserved blocks: %u\n", e->superblock->num_reserved_blocks);
    printf("Blocks per block group: %u\n", e->superblock->blocks_per_group);
    printf("Fragments per block group: %u\n", e->superblock->fragments_per_group);
    printf("Inodes per block group: %u\n", e->superblock->inodes_per_group);
    printf("Number of block groups: %u\n", e->superblock->num_blocks / e->superblock->blocks_per_group);
    std::time_t t = e->superblock->last_mount_time;
    printf("Last mounted at: %s", std::asctime(std::localtime(&t)));
    t = e->superblock->last_written_time;
    printf("Last written at: %s", std::asctime(std::localtime(&t)));
    t = e->superblock->consis_last_time;
    printf("Last consistency check: %s", std::asctime(std::localtime(&t)));

    {
        uint16_t x = e->superblock->consis_interval_time;
        uint16_t seconds = x % 60;
        x /= 60;
        uint16_t minutes = x % 60;
        x /= 60;
        uint16_t hours = x;

        printf("Interval between (forced) consistency checks: %u:%u:%u\n", hours, minutes, seconds);
    }

    printf("Number of times mounted since last consistency check: %u\n", e->superblock->consis1);
    printf("Number of mounts allowed before a consistency check must be done: %u\n", e->superblock->consis2);
    printf("File system state: %s (%u)\n",
            ((char*[]){"clean","has errors"})[e->superblock->file_system_state - 1], e->superblock->file_system_state);
    printf("OS id: %u\nUser id: %u\nGroup id: %u\n", e->superblock->os_id, e->superblock->user_id, e->superblock->group_id);
    printf("Error handling: %s\n",
        ((char*[]){"Ignore the error", "Remount the system as read-only", "Kernel panic"})[e->superblock->error_handling - 1]);
    
}

void print_extended_superblock(EXT2* e) {
    if (e->superblock->version_major >= 1) {
        printf("\nEXTENDED SUPERBLOCK\n");
        printf("(SUPERBLOCK BLOCK GROUP %u)\n", e->superblock->block_number_superblock);
        printf("#############################################\n");
        printf("Filesystem id: %.2hhx%.2hhx%.2hhx%.2hhx-%.2hhx%.2hhx-%.2hhx%.2hhx-%.2hhx%.2hhx-%.2hhx%.2hhx%.2hhx%.2hhx%.2hhx%.2hhx\n",
            e->superblock->file_system_id[0], e->superblock->file_system_id[1], e->superblock->file_system_id[2], e->superblock->file_system_id[3],
            e->superblock->file_system_id[4], e->superblock->file_system_id[5], e->superblock->file_system_id[6], e->superblock->file_system_id[7],
            e->superblock->file_system_id[8], e->superblock->file_system_id[9], e->superblock->file_system_id[10], e->superblock->file_system_id[11],
            e->superblock->file_system_id[12], e->superblock->file_system_id[13], e->superblock->file_system_id[14], e->superblock->file_system_id[15]);
        printf("Volume name: \"%s\"\n", e->superblock->volume_name);
        printf("Last mounted to: \"%s\"\n", e->superblock->path_mounted);
        printf("First non-reserved inode: %u\n", e->superblock->first_non_reserved_inode);
        printf("Inode size: %u\n", e->superblock->size_of_each_inode);
        if (e->superblock->optional_features & 0x1) {
            printf("Blocks preallocated for files: %u\n", e->superblock->file_preallocate_block);
            printf("Blocks preallocated for directories: %u\n", e->superblock->directory_preallocate_block);
        }
        if (e->superblock->required_features & 0x8 || e->superblock->optional_features & 0x4) {
            printf("Journal id: %.2hhx%.2hhx%.2hhx%.2hhx-%.2hhx%.2hhx-%.2hhx%.2hhx-%.2hhx%.2hhx-%.2hhx%.2hhx%.2hhx%.2hhx%.2hhx%.2hhx\n",
                e->superblock->journal_id[0], e->superblock->journal_id[1], e->superblock->journal_id[2], e->superblock->journal_id[3],
                e->superblock->journal_id[4], e->superblock->journal_id[5], e->superblock->journal_id[6], e->superblock->journal_id[7],
                e->superblock->journal_id[8], e->superblock->journal_id[9], e->superblock->journal_id[10], e->superblock->journal_id[11],
                e->superblock->journal_id[12], e->superblock->journal_id[13], e->superblock->journal_id[14], e->superblock->journal_id[15]);
            printf("Journal inode: %u\n", e->superblock->journal_inode);
            printf("Journal device: %u\n", e->superblock->journal_device);
        }
        printf("Head of orphan inode list: %u\n", e->superblock->head_orphan_inode);
        printf("Required features (for read or write) present:\n");
        for (int i = 0; i < 4; ++i) {
            if (e->superblock->required_features & (1 << i)) {
                printf(" - %s\n",((char*[]){
                    "compression is used",
                    "directory entries contain a type field",
                    "file system needs to replay it's journal",
                    "file system uses a journal device"})[i]);
            }
        }
        printf("Optional features present:\n");
        for (int i = 0; i < 6; ++i) {
            if (e->superblock->optional_features & (1 << i)) {
                printf(" - %s\n",((char*[]){
                    "preallocate blocks (to reduce fragmentation)",
                    "AFS server inodes exist",
                    "file system has a journal (EXT3)",
                    "inodes have extended attributes",
                    "file system can be resized",
                    "directories use hash index",})[i]);
            }
        }
        printf("Features requiring support in order to write to the system:\n");
        for (int i = 0; i < 3; ++i) {
            if (e->superblock->read_only_features & (1 << i)) {
                printf(" - %s\n",((char*[]){
                    "sparse superblocks and group descriptor tables",
                    "file system uses a 64-bit file size",
                    "directory contents are stored in a binary tree"})[i]);
            }
        }
        if (e->superblock->required_features & 0x1) {
            printf("Compression algorithms used: %u\n", e->superblock->compression_algorithms);
        }
    }
}

void print_block_descriptor_table(EXT2* e) {
    printf("\nBLOCK DESCRIPTOR TABLE\n");
    printf("#############################################\n");
    for (int i = 0; i < e->num_blockgroups; ++i) {
        printf("Entry #%u\n", i);
        printf("Block usage bitmap block address: %u\n", e->block_descriptor_table[i].block_usage_bitmap_block_address);
        printf("Inode usage bitmap block address: %u\n", e->block_descriptor_table[i].inode_usage_bitmap_block_address);
        printf("Inode table block address: %u\n", e->block_descriptor_table[i].inode_table_block_address);
        printf("Number of unallocated blocks: %u\n", e->block_descriptor_table[i].num_unallocated_blocks);
        printf("Number of unallocated inodes: %u\n", e->block_descriptor_table[i].num_unallocated_inodes);
    }
}

void print_inode(Inode& inode, EXT2* e) {
    printf("User ID: %u\n", inode.user_id);
    printf("Group ID: %u\n", inode.group_id);
    uint64_t data_size = inode.lower_size + (inode.upper_size << 32);
    double data_size_human = data_size;
    uint8_t data_size_human_index = 0;
    while(data_size_human >= 1024) {
        data_size_human /= 1024;
        ++data_size_human_index;
    }
    printf("Data size: %llu (%.3f %s)\n", data_size, data_size_human, ((char*[]){
                                                                "bytes",
                                                                "KiB",
                                                                "MiB",
                                                                "GiB",
                                                                "TiB"})[data_size_human_index]);
    std::time_t t = inode.last_access_time;
    printf("Last access time: %s", std::asctime(std::localtime(&t)));
    t = inode.last_modification_time;
    printf("Last modification time: %s", std::asctime(std::localtime(&t)));
    t = inode.deletion_time;
    printf("Deletion time: %s", std::asctime(std::localtime(&t)));
    printf("Number of hard links to this inode: %u\n", inode.hard_links);
    printf("Disk sectors in use by data of this node: %u\n", inode.disk_sectors);
    printf("Generation number: %u\n", inode.generation_number);
    if (e->superblock->version_major >= 1) {
        // extended attribute block
        // whatever
    }
    printf("Block address of fragment: %u\n", inode.fragment_block_address);
	printf("Permissions: ");
    for (int i = 9; i--;) {
		char c = '-';
        if (inode.type_permissions & (1 << i)) {
			c = ((char[]){
				'x',
				'w',
				'r',
				'x',
				'w',
				'r',
				'x',
				'w',
				'r'})[i];
        }

		printf("%c", c);
    }
	printf("\nType: %s (0x%x)\n", ((char*[]){
								"",
								"FIFO",
								"Character Device",
								"",
								"Directory",
								"",
								"Block Device",
								"",
								"Regular file",
								"",
								"Symbolic Link",
								"",
								"Unix Socket"})[inode.type_permissions>>12],
			inode.type_permissions >> 12);
	printf("Flags:\n");
	for (int i = 0; i < 8; ++i) {
		if (inode.flags & (1 << i)) {
			printf(" - %s\n", ((char*[]){
								   "Secure Deletion",
								   "Keep a copy of data when deleted",
								   "File compression",
								   "Syncronous Updates (Data is written immediately to disk)",
								   "Read-only",
								   "Append-only",
								   "File is not included in 'dump' command",
								   "Last accessed time should not be updated"
							   })[i]);
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (inode.flags & (1 << (i+20))) {
			printf(" - %s\n", ((char*[]){
								   "Hash indexed directory",
								   "AFS directory",
								   "Journal file data"
							   })[i]);
		}
	}
}

void print_inode_table(EXT2* e, int group) {
	printf("\nINODE TABLE #%u\n", group);
    printf("#############################################\n");
	for (int i = 0; i < e->superblock->inodes_per_group - e->block_descriptor_table[group].num_unallocated_inodes; ++i) {
		if (e->inode[group][i].type_permissions != 0) {
            printf("Inode #%u\n", 1 + i + (group * e->superblock->inodes_per_group));
            print_inode(e->inode[group][i], e);
            printf("\n");
        }
	}
}

void print_all_inodes(EXT2* e) {
	for (int i = 0; i < e->num_blockgroups; ++i) {
		print_inode_table(e, i);
	}
}

void load_block_descriptor_table(FILE* file, long int length, EXT2* e) {
    fseek(file, (e->superblock->block_number_superblock + 1) * e->blocksize + e->partition_byte_offset, SEEK_SET);
    e->block_descriptor_table = new BlockDescriptor[e->num_blockgroups];
    for (int i = 0; i < e->num_blockgroups; ++i) {
        fread(&e->block_descriptor_table[i], 1, 32, file);
    }
}

void load_inode_table(FILE* file, long int length, EXT2* e) {
    e->inode = new Inode*[e->num_blockgroups];
    for (int i = 0; i < e->num_blockgroups; ++i) {
		e->inode[i] = new Inode[e->superblock->inodes_per_group - e->block_descriptor_table[i].num_unallocated_inodes];
        fseek(file,
            e->partition_byte_offset +
            e->block_descriptor_table[0].inode_table_block_address *
            e->blocksize,
            SEEK_SET);
        // Add in check or summin for v1+ cause otherwise this will
        // fail on versions prior
        fread(
			e->inode[i],
            1,
			(e->superblock->inodes_per_group - e->block_descriptor_table[i].num_unallocated_blocks) * 128,
			file);
    }
}

void delete_ext2(EXT2* e) {
	delete e->superblock;
	delete [] e->block_descriptor_table;
	for (int i = 0; i < e->num_blockgroups; ++i) {
		delete [] e->inode[i];
	}
	delete [] e->inode;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: ext2viewer [FILE]\n");
        return -1;
    }

    FILE* file = fopen(argv[1], "rb");
    fseek(file, 0, SEEK_END);
    long int length = ftell(file);
    if (length < 2048) {
        printf("file is not a valid ext2 system\n");
        return -1;
    }

    fseek(file, 0, SEEK_SET);

    PartitionTable partition_table;

    fread(&partition_table, 1, 512, file);


    print_partition_table(partition_table);

    printf("\n\n");

	EXT2* ext = new EXT2;
	ext->superblock = new Superblock;
    for (int i = 0; i < 4; ++i) {
        ext->partition_byte_offset = partition_table.partition[i].lba * 512;
        fseek(file, ext->partition_byte_offset + 1024, SEEK_SET);
		fread(ext->superblock, 1, 1024, file);
		if (ext->superblock->sig == EXT2_SIGNATURE) {
			ext->blocksize = 1024 << ext->superblock->block_size;
			ext->num_blockgroups = ext->superblock->num_blocks / ext->superblock->blocks_per_group;
            print_superblock(ext);
            print_extended_superblock(ext);
            load_block_descriptor_table(file, length, ext);
            print_block_descriptor_table(ext);
            load_inode_table(file, length, ext);
			print_all_inodes(ext);
        }
    }
}
