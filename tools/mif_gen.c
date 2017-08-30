#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <errno.h>

extern int errno;

typedef unsigned long long      iros_uint64;
typedef long long               iros_int64;
typedef unsigned int            iros_uint32;
typedef int                     iros_int32;
typedef unsigned short          iros_uint16;
typedef short                   iros_int16;
typedef unsigned char           iros_uint8;
typedef char                    iros_int8;
typedef unsigned char           iros_boolean;


#define IROS_MIF_MAGIC    0x80ff7593

typedef enum {
    IROS_MIF_RET_OK               = 0,
    IROS_MIF_RET_INVALID_PARSER   = 1,
    IROS_MIF_RET_INVALID_STATE    = 2,
    IROS_MIF_RET_INVALID_BUF      = 3,
    IROS_MIF_RET_INVALID_HEADER   = 4,
    IROS_MIF_RET_INVALID_TLV      = 5,
    IROS_MIF_RET_TLV_NO_PARTITION = 6,
    IROS_MIF_RET_ERROR            = 0xffffffff
} iros_mif_result_e;

typedef enum {
    MIF_TLV_TYPE_ZIMG              = 0x0,
    MIF_TLV_TYPE_JFFS2              = 0x1,
    MIF_TLV_TYPE_SCFG               = 0x2,
    MIF_TLV_TYPE_BINARY             = 0x3,
    MIF_TLV_TYPE_ELF_EXEC           = 0x4,
    MIF_TLV_TYPE_ELF                = 0x5,
    MIF_TLV_TYPE_JFFS2_OVERWRITE    = 0x6,
    MIF_TLV_TYPE_JFFS2_MERGE        = 0x7,
    MIF_TLV_TYPE_EXEC_PARAM         = 0x8,
    MIF_TLV_TYPE_SCFG_WITH_MASK     = 0x9,
    MIF_TLV_TYPE_KERNEL             = 0xa,
    MIF_TLV_TYPE_ROOTFS             = 0xb,
    MIF_TLV_TYPE_LOADER             = 0xc,
    MIF_TLV_TYPE_MAX
} iros_mif_tlv_type_e;

char *tlv_type_to_str[] = {
    "ZBLOB",
    "JFFS2",
    "SCFG",
    "BINARY",
    "ELF_EXEC",
    "ELF",
    "JFFS2_OVERWRITE",
    "JFFS2_MERGE",
    "EXEC_PARAM",
    "SCFG_WITH_MASK",
    "KERNEL",
    "ROOTFS",
    "LOADER"
};


typedef enum {
    MIF_STATE_MIF_HEADER = 0,
    MIF_STATE_MIF_HEADER_COMP,
    MIF_STATE_TLV_HEADER,
    MIF_STATE_TLV_HEADER_COMP,
    MIF_STATE_TLV_BODY,
    MIF_STATE_TLV_BODY_COMP,
    MIF_STATE_MIF_COMP,
    MIF_STATE_MAX
} iros_mif_state_e;

typedef struct {
    iros_uint32   mif_magic;
    iros_uint8    version[4];
    iros_uint32   chip_bmp;
    iros_uint32   board_bmp;
    iros_uint32   mif_crc32;
    iros_uint32   length;
    iros_uint32   length_xor;
} __attribute__((packed)) iros_mif_header_t;

typedef struct {
    iros_uint8    type;
    iros_uint8    decompress;
    iros_uint8    rsvd[2];
    iros_uint32   destination;
    iros_uint32   tlv_crc;
    iros_uint8    version[4];
    iros_uint32   width;
    iros_uint32   width_xor;
} __attribute__((packed)) iros_mif_tlv_header_t;




//following MACRO is used for multi image facility.
typedef enum {
    MIF_COM_CREAT = 0,
    MIF_COM_DUM
} MIF_COM_TYPE_T;


/* ====================================================================== */
/*  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or       */
/*  code or tables extracted from it, as desired without restriction.     */
/*                                                                        */
/*  First, the polynomial itself and its table of feedback terms.  The    */
/*  polynomial is                                                         */
/*  X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0   */
/*                                                                        */
/* ====================================================================== */

static const iros_uint32 crc32_tab[] = {
      0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
      0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
      0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
      0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
      0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
      0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
      0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
      0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
      0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
      0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
      0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
      0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
      0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
      0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
      0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
      0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
      0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
      0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
      0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
      0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
      0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
      0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
      0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
      0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
      0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
      0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
      0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
      0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
      0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
      0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
      0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
      0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
      0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
      0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
      0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
      0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
      0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
      0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
      0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
      0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
      0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
      0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
      0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
      0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
      0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
      0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
      0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
      0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
      0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
      0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
      0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
      0x2d02ef8dL
   };
/* This is the standard Gary S. Brown's 32 bit CRC algorithm, but
   accumulate the CRC into the result of a previous CRC. */
iros_uint32
cyg_crc32_accumulate(iros_uint32 crc32val, unsigned char *s, int len)
{
  int i;

  for (i = 0;  i < len;  i++) {
    crc32val = crc32_tab[(crc32val ^ s[i]) & 0xff] ^ (crc32val >> 8);
  }
  return crc32val;
}

/* This is the standard Gary S. Brown's 32 bit CRC algorithm */
iros_uint32
cyg_crc32(unsigned char *s, int len)
{
  return (cyg_crc32_accumulate(0,s,len));
}



void show_help()
{
     printf("Usage:\n "
            "mk_mif  -c mif_version mif_file [-crc32] [chip option] [board option]\n"
            "        -[B|J|S|K|R|L] [file_version] [file_name]\n"
            "        -U [type] [decompress] [destination] [file_version] [file_name]\n"
            "        -d mif_file \n" );

     printf("[board option]: include -s, -s6045, -s6046, -s6097, -o4, -o2. \n");
     printf("If don't have any of above chip or board option, it will support all chips or boards. \n");

     printf("For more info, please check Function Spec [3.1.169] \n\n");
     printf("Example 1:  ./mk_mif -c 6.3.0.0 output_filename -Z 6.3.0.1 z_filename -J 6.3.0.2 JFFS2_filename\n");
     printf("Example 2:  ./mk_mif -c 6.3.0.0 output_filename -K 6.3.0.1 kernel_filename -R 6.3.0.2 rootfs_filename\n");
     printf("Example 3:  ./mk_mif -d input_filename\n\n");
}

void mif_header_hton(iros_mif_header_t* header)
{
    header->mif_magic    = htonl(header->mif_magic);
    header->chip_bmp     = htonl(header->chip_bmp);
    header->board_bmp    = htonl(header->board_bmp);
    header->mif_crc32    = htonl(header->mif_crc32);
    header->length       = htonl(header->length);
    header->length_xor   = htonl(header->length_xor);
}

void mif_header_ntoh(iros_mif_header_t* header)
{
    header->mif_magic    = ntohl(header->mif_magic);
    header->mif_crc32    = ntohl(header->mif_crc32);
    header->length       = ntohl(header->length);
    header->length_xor   = ntohl(header->length_xor);
}

void tlv_header_hton(iros_mif_tlv_header_t* header)
{
    header->destination  = htonl(header->destination);
    header->tlv_crc      = htonl(header->tlv_crc);
    header->width        = htonl(header->width);
    header->width_xor    = htonl(header->width_xor);
}

void tlv_header_ntoh(iros_mif_tlv_header_t* header)
{
    header->destination  = ntohl(header->destination);
    header->tlv_crc      = ntohl(header->tlv_crc);
    header->width        = ntohl(header->width);
    header->width_xor    = ntohl(header->width_xor);
}

//dump the content of requested multi image file.
void dump_mif_file(char * file_name)
{
    int input_file_id;
    int input_file_size;
    char* buf;
    char* read_ptr;
    iros_uint8 tlv_idx;
    struct stat input_file_stat;
    iros_mif_header_t mif_header;
    iros_mif_tlv_header_t tlv_header;

    printf("MIF content in brief: \n");

    input_file_id = open(file_name, O_RDONLY);
    if(-1 == input_file_id)
    {
        printf("open input file error\n");
        show_help ();
        return;
    }

    fstat(input_file_id, &input_file_stat);
    input_file_size =  input_file_stat.st_size;

    buf = malloc(input_file_size);
    if (buf == NULL){
        printf("malloc() function failed\n");
        close(input_file_id);
        return;
    }

    if(read(input_file_id, buf, input_file_size) == -1){
        printf("read input file failed\n");
        free(buf);
        close(input_file_id);
        return;
    }

    // check and dump mif header
    read_ptr = buf;
    memcpy((iros_uint8 *)&mif_header, read_ptr, sizeof(mif_header));
    mif_header_ntoh(&mif_header);

    if(mif_header.mif_magic != IROS_MIF_MAGIC
        || mif_header.length != ~mif_header.length_xor)
    {
        printf("Invalid MIF file, check magic no and length\n");
        free(buf);
        close(input_file_id);
        return;
    }

    printf("MIF header:\n");
    printf("    magic 0x%08x\n", mif_header.mif_magic);
    printf("    version %d.%d.%d.%d\n", mif_header.version[0], mif_header.version[1],
                                        mif_header.version[2], mif_header.version[3]);
    printf("    crc32 0x%08x\n", mif_header.mif_crc32);
    printf("    length 0x%08x\n", mif_header.length);
    printf("    length_xor 0x%08x\n", mif_header.length_xor);

    read_ptr += sizeof(mif_header);
    tlv_idx = 0;

    while(read_ptr - buf < input_file_size)
    {
        memcpy((iros_uint8 *)&tlv_header, read_ptr, sizeof(tlv_header));
        read_ptr += sizeof(tlv_header);

        tlv_header_ntoh(&tlv_header);

        if(tlv_header.width != ~tlv_header.width_xor
            || tlv_header.tlv_crc != cyg_crc32((iros_uint8 *)read_ptr, tlv_header.width))
        {
            printf("Invalid TLV %d, CRC error\n", tlv_idx);
            free(buf);
            close(input_file_id);
            return;
        }

        printf("TLV %d\n", tlv_idx);
        printf("    type %s\n", tlv_type_to_str[tlv_header.type]);
        printf("    decompress %d\n", tlv_header.decompress);
        printf("    destination 0x%08x\n", tlv_header.destination);
        printf("    tlv_crc 0x%08x\n", tlv_header.tlv_crc);
        printf("    version %d.%d.%d.%d\n", tlv_header.version[0], tlv_header.version[1],
                                            tlv_header.version[2], tlv_header.version[3]);
        printf("    width 0x%08x\n", tlv_header.width);
        printf("    width_xor 0x%08x\n", tlv_header.width_xor);

        read_ptr += tlv_header.width;
        tlv_idx++;
    }

    free(buf);
    close(input_file_id);
    return;
}


int main(int argc, char *argv[])
{
    MIF_COM_TYPE_T command_type = MIF_COM_DUM;
    bool command_type_found, enable_mif_crc32;
    iros_int32 argv_num_for_output = 0;
    iros_int32 argv_num_for_input = 0;
    iros_int32 mif_fid, input_file_id;
    iros_int32 input_file_size;
    char *temp_buff;
    iros_uint32 version[4];
    iros_uint32 mif_version[4] = {0};
    iros_uint32 input_num[4];

    struct stat input_file_stat;
    struct stat output_file_stat;

    iros_mif_header_t mif_header;
    iros_mif_tlv_header_t tlv_header;


    command_type_found = false;
    enable_mif_crc32 = false;

    if ( argc < 3) {
        show_help();
        return 0;
    }

    int i;
    //first pass. Decide whether it is a dump or create type command, and decide the output file name.
    for (i = 1; i < argc; i++)
    {
        if (strcmp (argv[i], "-c") == 0) {
            if (command_type_found == true) {
                printf("Error: at least 2 creat or dump option is found \n");
                show_help();
                return -1;
            }
            command_type_found = true;
            command_type = MIF_COM_CREAT;
            sscanf(argv[i+1], "%u.%u.%u.%u", input_num, input_num + 1, input_num + 2, input_num + 3);
            if(input_num[0] > 255 || input_num[1] > 255 || input_num[2] > 255 || input_num[3] > 255)
            {
                printf("Invalid version format: x.x.x.x, x should be 0~255\n");
                return -1;
            }

            sscanf(argv[i+1], "%u.%u.%u.%u", mif_version, mif_version + 1, mif_version + 2, mif_version + 3);
            argv_num_for_output = i+2;
        }
        else if(strcmp (argv[i], "-d") == 0){
            if (command_type_found == true) {
                printf("Error: at least 2 creat or dump option is found \n");
                show_help();
                return -1;
            }
            command_type_found = true;
            command_type = MIF_COM_DUM;
            argv_num_for_output = i+1;
        }
    }

    // If it is dump command, process it here.
    if ( command_type == MIF_COM_DUM) {
        dump_mif_file (argv[argv_num_for_output]);
        return 0;
    }

    //Open the output file and Write the mif header first. Re-write this header later.
    mif_fid = open(argv[argv_num_for_output], O_CREAT|O_RDWR|O_TRUNC, 0664);
    if (mif_fid == -1) {
        printf("Can not open the output file\n");
	      return -1;
    }

    // write fake mif header
    memset((void *)&mif_header, 0, sizeof(iros_mif_header_t));

    if(write(mif_fid, (void *)&mif_header, sizeof (mif_header)) != sizeof (mif_header)) {
         fprintf(stderr, "write file  failed\n");
         return(-1);
    }

    for (i=1; i<argc; i++) {
        memset ((void *)&tlv_header, 0, sizeof(iros_mif_tlv_header_t));

        if (strcmp(argv[i], "-crc32") ==0 )
            enable_mif_crc32  = true;
        else if (strcmp (argv[i], "-d") == 0)
        {
            i++; //skip next argv, mif filename;
            continue;
        }
        else if (strcmp (argv[i], "-c") ==0)
        {
            i = i + 2; // skip next 2 argv, mif version and filename
            continue;
        }
        else {
            //process all other options which used to indicate which file
            //need to added to the output file, also invalid options.
            if( (strcmp (argv[i], "-Z") ==0)
                || (strcmp (argv[i], "-J") ==0)
                || (strcmp (argv[i], "-S") ==0)
                || (strcmp (argv[i], "-K") ==0)
                || (strcmp (argv[i], "-R") ==0)
                || (strcmp (argv[i], "-L") ==0) )
            {
                if (argc < i+2) {
                    printf("Invalid option\n");
                    show_help ();
                    return -1;
                }

                if (strcmp (argv[i], "-Z") ==0)
                    tlv_header.type = MIF_TLV_TYPE_ZIMG;
                else if (strcmp (argv[i], "-J") ==0)
                    tlv_header.type = MIF_TLV_TYPE_JFFS2;
                else if (strcmp (argv[i], "-S") ==0)
                    tlv_header.type = MIF_TLV_TYPE_SCFG;
                else if (strcmp (argv[i], "-K") ==0)
                    tlv_header.type = MIF_TLV_TYPE_KERNEL;
                else if (strcmp (argv[i], "-R") ==0)
                    tlv_header.type = MIF_TLV_TYPE_ROOTFS;
                else if (strcmp (argv[i], "-L") ==0)
                    tlv_header.type = MIF_TLV_TYPE_LOADER;
                else
                    printf("Invalid option\n");

                sscanf(argv[++i], "%u.%u.%u.%u", version, version + 1, version + 2, version + 3);
                argv_num_for_input = ++i;
                tlv_header.version[0] = (iros_uint8)version[0];
                tlv_header.version[1] = (iros_uint8)version[1];
                tlv_header.version[2] = (iros_uint8)version[2];
                tlv_header.version[3] = (iros_uint8)version[3];
            }
            else if (strcmp (argv[i], "-U") ==0) {
                if (argc < i+5) {
                    printf("Invalid option\n");
                    show_help ();
	                  return -1;
                }
                tlv_header.type = atoi (argv[++i]);
                tlv_header.decompress = atoi (argv[++i]);
                tlv_header.destination = atoi (argv[++i]);
                sscanf(argv[++i], "%u.%u.%u.%u", version, version + 1, version + 2, version + 3);
                argv_num_for_input = ++i;
                tlv_header.version[0] = (iros_uint8)version[0];
                tlv_header.version[1] = (iros_uint8)version[1];
                tlv_header.version[2] = (iros_uint8)version[2];
                tlv_header.version[3] = (iros_uint8)version[3];
            }
            else {
                printf("Invalide option [%s]\n", argv[i]);
                show_help ();
                return -1;
            }

            //write tlv header and input file data to output file.
            input_file_id = open(argv[argv_num_for_input], O_RDONLY);
            if( -1== input_file_id)
            {
                printf("open input file error\n");
                show_help ();
                return -1;
            }

            fstat(input_file_id, &input_file_stat);
            tlv_header.width = input_file_stat.st_size;
            input_file_size =  input_file_stat.st_size;

            //read the input file and write its content to output file
            temp_buff = malloc(input_file_size);
            if (temp_buff == NULL){
                printf("malloc() function failed\n");
                return -1;
            }

            if (read(input_file_id, temp_buff, input_file_size) == -1){
                printf("read input file failed\n");
                free (temp_buff);
                return -1;
            }

            tlv_header.tlv_crc      = cyg_crc32((unsigned char *) temp_buff, input_file_size);
            tlv_header.width_xor    = ~tlv_header.width;

            printf("TLV for %s:\n",             argv[argv_num_for_input]);
            printf("    type %s\n",             tlv_type_to_str[tlv_header.type]);
            printf("    decompress %d\n",       tlv_header.decompress);
            printf("    destination 0x%08x\n",  tlv_header.destination);
            printf("    tlv_crc 0x%08x\n",      tlv_header.tlv_crc);
            printf("    version %d.%d.%d.%d\n", tlv_header.version[0], tlv_header.version[1],
                                                tlv_header.version[2], tlv_header.version[3]);
            printf("    width 0x%08x\n",        tlv_header.width);
            printf("    width_xor 0x%08x\n",    tlv_header.width_xor);

            //change to network byte order and write into file
            tlv_header_hton(&tlv_header);

            if (write(mif_fid, (void *)&tlv_header, sizeof (iros_mif_tlv_header_t)) != sizeof (iros_mif_tlv_header_t)) {
                printf("write tlv header failure\n");
                close (input_file_id);
                close (mif_fid);
                return(-1);
            }

            if (write(mif_fid, temp_buff, input_file_size) == -1){
                printf("write output file failed\n");
                free (temp_buff);
                return -1;
            }

            free (temp_buff);
            close (input_file_id);

            mif_header.length += sizeof(iros_mif_tlv_header_t);
            mif_header.length += input_file_size;
        }  //end for() loop
    }

    //change some fields of the mif header to network byte order and re-write it to the output file.
    //before re-write the header to output file, if it is still 0, set it to 0xffffffff
    if (0 == mif_header.chip_bmp)
        mif_header.chip_bmp = 0xffffffff;

    if (0 == mif_header.board_bmp)
        mif_header.board_bmp = 0xffffffff;

    mif_header.mif_magic    = IROS_MIF_MAGIC;
    mif_header.mif_crc32    = 0;
    mif_header.version[0]   = (iros_uint8)mif_version[0];
    mif_header.version[1]   = (iros_uint8)mif_version[1];
    mif_header.version[2]   = (iros_uint8)mif_version[2];
    mif_header.version[3]   = (iros_uint8)mif_version[3];
    mif_header.length_xor   = ~mif_header.length;

    mif_header_hton(&mif_header);

    if (lseek(mif_fid, 0, SEEK_SET) != 0) {
         fprintf(stderr, "lseek1 failure\n");
         return(-1);
    }

    if (sizeof (iros_mif_header_t) != write(mif_fid, (void *)&mif_header, sizeof (iros_mif_header_t)))
    {
         fprintf(stderr, "write file  failed\n");
         return(-1);
    }

    //add the crc32 calculate here.
    if (enable_mif_crc32 == true){
         if (lseek(mif_fid, 0, SEEK_SET) != 0) {
             fprintf(stderr, "lseek2 failure\n");
             return(-1);
         }

         if (fstat(mif_fid, &output_file_stat) == -1) {
             fprintf(stderr, "get dat file stat failed\n");
             return(-1);
         }

         int total_size = output_file_stat.st_size;
         temp_buff = (char *)malloc(total_size);
         if(NULL == temp_buff) {
             fprintf(stderr, "malloca failed\n");
             return(-1);    
         }
         
         char * cur_pos;
         int cur_bytes_read;

         cur_pos = temp_buff;
         do {
            cur_bytes_read = read(mif_fid, cur_pos, total_size);
            if (cur_bytes_read == -1) {
                if (errno == EINTR) {
                    continue;
                } else {
                    fprintf(stderr, "read output failed - %s\n", strerror(errno));
                    exit(-1);
                }
            }
            cur_pos += cur_bytes_read;
         } while (((cur_pos - temp_buff) < total_size) && (cur_bytes_read != 0));

        total_size = cur_pos - (char *)temp_buff;

        unsigned int crc32 = cyg_crc32((unsigned char *)temp_buff, total_size);
        crc32 = htonl(crc32);
        mif_header.mif_crc32 = crc32;

        if (lseek(mif_fid, 16, SEEK_SET) != 16) {
            fprintf(stderr, "lseek3 failure\n");
            free (temp_buff);
            return(-1);
        }

        if (write(mif_fid, (void *)&crc32, 4) != 4)
        {
             fprintf(stderr, "write file  failed\n");
             free (temp_buff);
             return(-1);
        }

        free (temp_buff);
    }

    close (mif_fid);

    mif_header_ntoh(&mif_header);
    printf("Multi image file [%s] created successfully\n", argv[argv_num_for_output]);
    printf("MIF header:\n");
    printf("    magic 0x%08x\n", mif_header.mif_magic);
    printf("    version %d.%d.%d.%d\n", mif_header.version[0], mif_header.version[1],
                                        mif_header.version[2], mif_header.version[3]);
    printf("    crc32 0x%08x\n", mif_header.mif_crc32);
    printf("    length 0x%08x\n", mif_header.length);
    printf("    length_xor 0x%08x\n", mif_header.length_xor);

    return 0;
}

