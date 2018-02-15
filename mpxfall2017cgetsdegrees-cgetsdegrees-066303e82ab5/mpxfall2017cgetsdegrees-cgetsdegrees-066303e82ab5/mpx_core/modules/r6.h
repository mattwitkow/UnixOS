#ifndef FAT12_H
#define FAT12_H

#define FAT_PHYS_SIZE           9
#define FAT_BOOT_SECTOR         0
#define FAT_SECTOR_SIZE         512

#define EOF_FAT12       0xFF8

// Stuctures                                                            
// Boot sector                                                          
typedef struct bootsect
{
        unsigned char Jump[3];
        unsigned char Name[8];
        unsigned int BytesPerSector;
        unsigned char SectorsPerCluster;
        unsigned int ReservedSectors;
        unsigned char Fats;
        unsigned int RootDirectoryEntries;
        unsigned int NumberSectors;
        unsigned char MediumDescriptor;
        unsigned int SectorsPerFat;
        unsigned int SectorsPerTrack;
        unsigned int Heads;
        unsigned int HiddenSectors;
        unsigned char code[482];
} __attribute__ ((packed)) bootsect;



#endif
