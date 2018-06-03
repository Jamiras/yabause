
#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h> 
#else
#include <windows.h>
#endif

#include "stv.h"
#include "cs0.h"
#include "junzip.h"
#include "yabause.h"
#include "zlib/zlib.h"

#define LOGSTV

#define NB_STV_GAMES 7

static GameLink availableGames[NB_STV_GAMES];
static int loadGames(char* path);
int copyFile(JZFile *zip, void* data);
	
static const Game GameList[NB_STV_GAMES]={
  {"STV Bios",
    0x0,
    {
        BIOS_BLOB, "epr-20091.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr-19730.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr-17951a.ic8",  0x000000, 0x080000,
	BIOS_BLOB, "epr-17740a.ic8",  0x000000, 0x080000,
	BIOS_BLOB, "epr-17740.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr-17954a.ic8",  0x000000, 0x080000,
	BIOS_BLOB, "epr-17952a.ic8",  0x000000, 0x080000,
	BIOS_BLOB, "epr-19854.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr-17953a.ic8",  0x000000, 0x080000,
	BIOS_BLOB, "epr-17742a.ic8",  0x000000, 0x080000,
	BIOS_BLOB, "stv110.bin",      0x000000, 0x080000,
	BIOS_BLOB, "stv1061.bin",     0x000000, 0x080000,
	BIOS_BLOB, "epr20091.ic8",    0x000000, 0x080000,
	BIOS_BLOB, "epr19730.ic8",    0x000000, 0x080000,
	BIOS_BLOB, "epr17951a.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr17740a.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr17740.ic8",    0x000000, 0x080000,
	BIOS_BLOB, "epr17954a.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr17952a.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr19854.ic8",    0x000000, 0x080000,
	BIOS_BLOB, "epr17953a.ic8",   0x000000, 0x080000,
	BIOS_BLOB, "epr17742a.ic8",   0x000000, 0x080000,
        GAME_END, "", 0, 0
    },
  },
  {"Astra SuperStars (J 980514 V1.002)",
    0x052e2901,
    {
	HEADER_BLOB, "epr20825.13",    0x0000001, 0x0100000,
        GAME_BYTE_BLOB, "epr20825.13",    0x0200000, 0x0100000,
        GAME_BYTE_BLOB, "epr20825.13",    0x0300000, 0x0100000,
	GAME_WORD_BLOB, "mpr20827.2",     0x0400000, 0x0400000,
	GAME_WORD_BLOB, "mpr20828.3",     0x0800000, 0x0400000,
	GAME_WORD_BLOB, "mpr20829.4",     0x0c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr20830.5",     0x1000000, 0x0400000,
	GAME_WORD_BLOB, "mpr20831.6",     0x1400000, 0x0400000,
	GAME_WORD_BLOB, "mpr20826.1",     0x1800000, 0x0400000,
	GAME_WORD_BLOB, "mpr20832.8",     0x1c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr20833.9",     0x2000000, 0x0400000,
        GAME_END, "", 0, 0
    },
  },
  {"Cotton 2 (JUET 970902 V1.000)",
    0x0,
    {
	GAME_WORD_BLOB, "mpr20122.7",    0x0200000, 0x0200000,
	GAME_WORD_BLOB, "mpr20117.2",    0x0400000, 0x0400000,
	GAME_WORD_BLOB, "mpr20118.3",    0x0800000, 0x0400000,
	GAME_WORD_BLOB, "mpr20119.4",    0x0c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr20120.5",    0x1000000, 0x0400000,
	GAME_WORD_BLOB, "mpr20121.6",    0x1400000, 0x0400000,
	GAME_WORD_BLOB, "mpr20116.1",    0x1800000, 0x0400000,
	GAME_WORD_BLOB, "mpr20123.8",    0x1c00000, 0x0400000,
        GAME_END, "", 0, 0
    },
  },
  {"Radiant Silvergun (JUET 980523 V1.000)",
    0x05272d01,
    {
	GAME_WORD_BLOB, "mpr20958.7",   0x0200000, 0x0200000,
	GAME_WORD_BLOB, "mpr20959.2",   0x0400000, 0x0400000,
	GAME_WORD_BLOB, "mpr20960.3",   0x0800000, 0x0400000,
	GAME_WORD_BLOB, "mpr20961.4",   0x0c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr20962.5",   0x1000000, 0x0400000,
        GAME_END, "", 0, 0
    },
  },
  {"Cotton Boomerang (JUET 980709 V1.000)",
    0x0,
    {
	GAME_WORD_BLOB, "mpr21075.7",    0x0200000, 0x0200000,
	GAME_WORD_BLOB, "mpr21070.2",    0x0400000, 0x0400000,
	GAME_WORD_BLOB, "mpr21071.3",    0x0800000, 0x0400000,
	GAME_WORD_BLOB, "mpr21072.4",    0x0c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr21073.5",    0x1000000, 0x0400000,
	GAME_WORD_BLOB, "mpr21074.6",    0x1400000, 0x0400000,
	GAME_WORD_BLOB, "mpr21069.1",    0x1800000, 0x0400000,
        GAME_END, "", 0, 0
    },
  },
  {"Touryuu Densetsu Elan-Doree / Elan Doree - Legend of Dragoon (JUET 980922 V1.006)",
    0x05226d41,
    {
	GAME_WORD_BLOB, "mpr21307.7",    0x0200000, 0x0200000,
	GAME_WORD_BLOB, "mpr21301.2",    0x0400000, 0x0400000,
	GAME_WORD_BLOB, "mpr21302.3",    0x0800000, 0x0400000,
	GAME_WORD_BLOB, "mpr21303.4",    0x0c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr21304.5",    0x1000000, 0x0400000,
	GAME_WORD_BLOB, "mpr21305.6",    0x1400000, 0x0400000,
	GAME_WORD_BLOB, "mpr21306.1",    0x1800000, 0x0400000,
	GAME_WORD_BLOB, "mpr21308.8",    0x1c00000, 0x0400000,
        GAME_END, "", 0, 0
    },
  },
  {"Baku Baku Animal (J 950407 V1.000)",
    0,
    {
	HEADER_BLOB,    "fpr17969.13",   0x0000001, 0x0100000,
	GAME_BYTE_BLOB, "fpr17969.13",   0x0200000, 0x0100000,
	GAME_BYTE_BLOB, "fpr17969.13",   0x0300000, 0x0100000,
	GAME_WORD_BLOB, "mpr17970.2",    0x0400000, 0x0400000,
	GAME_WORD_BLOB, "mpr17971.3",    0x0800000, 0x0400000,
	GAME_WORD_BLOB, "mpr17972.4",    0x0c00000, 0x0400000,
	GAME_WORD_BLOB, "mpr17973.5",    0x1000000, 0x0400000,
        GAME_END, "", 0, 0
    },
  },
};


static u8 hasBios = 0;
static u8 fileFound[NB_STV_GAMES][MAX_GAME_FILES];

int processFile(JZFile *zip,void *input) {
    JZFileHeader header;
    char filename[1024];
    unsigned char *data;
    int i,j;

   if (input != NULL) return copyFile(zip, input);

    if(jzReadLocalFileHeader(zip, &header, filename, sizeof(filename))) {
        LOGSTV("Couldn't read local file header!");
        return -1;
    }

    //LOGSTV("%s, %d / %d bytes at offset %08X\n", filename, header.compressedSize, header.uncompressedSize, header.offset);
    for (i=0; i<NB_STV_GAMES; i++) {
      j=0;
      while(GameList[i].blobs[j].type != GAME_END) {
        if (strncmp(GameList[i].blobs[j].filename, filename, 1024) == 0) {
          //Compatible file found
          fileFound[i][j] = 1;
        }
        j++;
      }
    }

    return 0;
}
static int biosloaded = 0;
int copyFile(JZFile *zip, void* id) {
    JZFileHeader header;
    char filename[1024];
    u8* data;
    int i,j, dataAvailable;
    int gameId = *((int*)id);
    LOGSTV("copyFile %s\n", filename);
    if(jzReadLocalFileHeader(zip, &header, filename, sizeof(filename))) {
        LOGSTV("Couldn't read local file header!");
        return -1;
    }

    if((data = (u8*)malloc(header.uncompressedSize)) == NULL) {
        LOGSTV("Couldn't allocate memory!");
        return -1;
    }

    i=0;
    dataAvailable = 0;
    while(availableGames[gameId].entry->blobs[i].type != GAME_END) {
      if (strncmp(availableGames[gameId].entry->blobs[i].filename, filename, 1024) == 0) {
        if (dataAvailable == 0) {
          dataAvailable = (jzReadData(zip, &header, data) == Z_OK);
        }
        if(dataAvailable != 0) {
          fileFound[gameId][i] = 1;
          switch (availableGames[gameId].entry->blobs[i].type) {
            case BIOS_BLOB:
              if (biosloaded == 0) {
                for (j=0; j<availableGames[gameId].entry->blobs[i].length;j++) {
                  T1WriteByte(BiosRom, availableGames[gameId].entry->blobs[i].offset+j, data[j]);
                }
                biosloaded = 1;
              }
              break;
            case GAME_WORD_BLOB:
              for (j=0; j<availableGames[gameId].entry->blobs[i].length;j++) {
                T2WriteByte(CartridgeArea->rom, availableGames[gameId].entry->blobs[i].offset+j, data[j]);
              }
              break;
            case HEADER_BLOB:
              for (j=0; j<availableGames[gameId].entry->blobs[i].length;j++) {
                T1WriteByte(CartridgeArea->rom, availableGames[gameId].entry->blobs[i].offset+2*j, data[j]);
              }
              break;
            case GAME_BYTE_BLOB:
              for (j=0; j<availableGames[gameId].entry->blobs[i].length;j++) {
                T1WriteByte(CartridgeArea->rom, availableGames[gameId].entry->blobs[i].offset+j, data[j]);
              }
              break;
          }
        }
      }
      i++;
    }

    free(data);

    return 0;
}

int recordCallback(JZFile *zip, int idx, JZFileHeader *header, char *filename, void *user_data) {
    long offset;

    offset = zip->tell(zip); // store current position

    if(zip->seek(zip, header->offset, SEEK_SET)) {
        LOGSTV("Cannot seek in zip file!");
        return 0; // abort
    }

    processFile(zip, user_data); // alters file offset

    zip->seek(zip, offset, SEEK_SET); // return to position

    return 1; // continue
}

static int updateGameList(const char* file, int *nbGames){
  FILE *fp;
  JZEndRecord endRecord;
  JZFile *zip;
  int i, j;
  u8 isASTVGame, isBiosFound, isBlobFound;

  memset(fileFound, 0x0, NB_STV_GAMES*MAX_GAME_FILES);

  if(!(fp = fopen(file, "rb"))) {
        LOGSTV("Couldn't open \"%s\"!", file);
        return 0;
  }

  zip = jzfile_from_stdio_file(fp);

  if(jzReadEndRecord(zip, &endRecord)) {
    LOGSTV("Couldn't read ZIP file end record.");
    goto endClose;
  }

  if(jzReadCentralDirectory(zip, &endRecord, recordCallback, NULL)) {
    LOGSTV("Couldn't read ZIP file central record.");
    goto endClose;
  }

  for (i=0; i<NB_STV_GAMES; i++) {
    isASTVGame = 1;
    isBiosFound = 0;
    isBlobFound = 1;
    j=0;
    while(GameList[i].blobs[j].type != GAME_END) {
      if (GameList[i].blobs[j].type == BIOS_BLOB) {
        isBiosFound |= fileFound[i][j];
      } else {
        isBlobFound &= fileFound[i][j];
      }
      j++;
    }
    if (i==0 && isBiosFound) hasBios = 1;
    else isASTVGame = isBlobFound & (isBiosFound | hasBios);
    if (isASTVGame == 1) {
      //Add the filename as a Game
      int found = 0;
      for (j=0; j<NB_STV_GAMES; j++)
        if (availableGames[j].entry == &GameList[i]) {
          found = 1;
          break;
        }
      if (found == 0) {
        availableGames[*nbGames].entry = &GameList[i];
        strncpy(availableGames[*nbGames].path, file, 1024);
        (*nbGames)++;
        LOGSTV("Game %s has been detected\n", GameList[i].name);
      }
    }
  }

endClose:
    zip->close(zip);
}

static int loadBios(){
  FILE *fp;
  JZEndRecord endRecord;
  JZFile *zip;
  int i = 0;
  u8 isBiosFound = 0;
  int biosId = 0;

  memset(fileFound, 0x0, NB_STV_GAMES*MAX_GAME_FILES);

  if(!(fp = fopen(availableGames[0].path, "rb"))) {
        LOGSTV("Couldn't open \"%s\"!", availableGames[0].path);
        return 0;
  }

  zip = jzfile_from_stdio_file(fp);

  if(jzReadEndRecord(zip, &endRecord)) {
    LOGSTV("Couldn't read ZIP file end record.");
  } else {
    if(jzReadCentralDirectory(zip, &endRecord, recordCallback, &biosId)) {
      LOGSTV("Couldn't read ZIP file central record.");
    }
  }
  zip->close(zip);

  while(availableGames[0].entry->blobs[i].type != GAME_END) {
    if (availableGames[0].entry->blobs[i].type == BIOS_BLOB) {
      isBiosFound |= fileFound[0][i];
    } 
    i++;
  }
  return isBiosFound;
}

int loadGame(int gameId){
  FILE *fp;
  JZEndRecord endRecord;
  JZFile *zip;
  int i = 0;
  u8 isBiosFound = 0;
  u8 isBlobFound = 1;
  u8 hasBios = 0;

  hasBios = loadBios();

  LOGSTV("Loading game[%d] %s from %s\n", gameId, availableGames[gameId].entry->name, availableGames[gameId].path);

  memset(fileFound, 0x0, NB_STV_GAMES*MAX_GAME_FILES);

  if(!(fp = fopen(availableGames[gameId].path, "rb"))) {
        LOGSTV("Couldn't open \"%s\"!", availableGames[gameId].path);
        return 0;
  }

  zip = jzfile_from_stdio_file(fp);

  if(jzReadEndRecord(zip, &endRecord)) {
    LOGSTV("Couldn't read ZIP file end record.");
  } else {
    if(jzReadCentralDirectory(zip, &endRecord, recordCallback, &gameId)) {
      LOGSTV("Couldn't read ZIP file central record.");
    }
  }
  zip->close(zip);

  while(availableGames[gameId].entry->blobs[i].type != GAME_END) {
    if (availableGames[gameId].entry->blobs[i].type == BIOS_BLOB) {
      isBiosFound |= fileFound[gameId][i];
    } else {
      isBlobFound &= fileFound[gameId][i];
    }
    i++;
  }

  if (isBlobFound & (isBiosFound|hasBios)) {
    LOGSTV("%s has been sucessfully loaded\n", availableGames[gameId].entry->name);
    cyptoSetKey(availableGames[gameId].entry->key);
    return 0;
  }
  return -1;
}

#ifndef WIN32
int STVGetRomList(const char* path, int force){
//List number of files in directory
  DIR *d;
  FILE *fp;
  int i, nbGames = 0;
  char savefile[1024];
  memset(availableGames, 0x0, sizeof(GameLink)*NB_STV_GAMES);
  snprintf(savefile, 1024, "%s/gamelist.save", path);
  if (force == 0) {
    nbGames = loadGames(savefile);
    if (nbGames != 0) return nbGames;
  }
  struct dirent *dir;
  d = opendir(path);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type == DT_REG)
      {
        char *dot = strrchr(dir->d_name, '.');
        if (dot && !strcmp(dot, ".zip")) {
          unsigned int len = strlen(path)+strlen("/")+strlen(dir->d_name)+1;
          unsigned char *file = malloc(len);
          snprintf(file, len, "%s/%s",path, dir->d_name);
          updateGameList(file, &nbGames);
          free(file);
        }
      }
    }
    closedir(d);
    fp = fopen(savefile, "w");
    for (i=0; i<nbGames; i++) {
      fprintf(fp, "%s,%s\n", availableGames[i].entry->name, availableGames[i].path);
    }
    fclose(fp);
  }
  return nbGames;
}
#else
int STVGetRomList(const char* path, int force){
//List number of files in directory
  FILE *fp;
  int i, nbGames = 0;
  char savefile[1024];
  char pathfile[1024];
  memset(availableGames, 0x0, sizeof(GameLink)*NB_STV_GAMES);
  snprintf(savefile, 1024, "%s/gamelist.save", path);
  snprintf(pathfile, 1024, "%s/*.zip", path);
  if (force == 0) {
    nbGames = loadGames(savefile);
    if (nbGames != 0) return nbGames;
  }
  HANDLE hFind;
  WIN32_FIND_DATA FindFileData;
  if((hFind = FindFirstFile(pathfile, &FindFileData)) != INVALID_HANDLE_VALUE){
    do{
      unsigned int len = strlen(path)+strlen("/")+strlen(FindFileData.cFileName)+1;
      unsigned char *file = malloc(len);
      snprintf(file, len, "%s/%s",path, FindFileData.cFileName);
      updateGameList(file, &nbGames);
      free(file);
    }while(FindNextFile(hFind, &FindFileData));
    FindClose(hFind);
    fp = fopen(savefile, "w");
    for (i=0; i<nbGames; i++) {
      fprintf(fp, "%s,%s\n", availableGames[i].entry->name, availableGames[i].path);
    }
    fclose(fp);
  }
  return nbGames;
}
#endif

char* getSTVGameName(int id) {
  return availableGames[id].entry->name;
}

static int loadGames(char* path) {
  int i, nbGames = 0;
  char* field;
  char gameName[1024];
  char gamePath[1024];
  FILE *fp;
  LOGSTV("Look in %s\n", path);
  fp = fopen(path, "r");
  if (fp == NULL) return 0;
  for(;;) {
    i=0;
    field = &gameName[0];
    while((field[i] = fgetc(fp)) != EOF) {
      if (field[i] == '\n') {
        field[i] = '\0';
        break;
      }
      else {
        if (field[i] == ',') {
          field[i] = '\0';
          field = &gamePath[0];
          i = 0;
        } else {
          i++;
        }
      }
    }
    if (field[i] == EOF) break;
    LOGSTV("Scan new game %s %s!!!\n", gameName, gamePath);
    for (i=0; i<NB_STV_GAMES; i++) {
      if (strncmp(gameName,GameList[i].name,1024)==0) {
        availableGames[nbGames].entry = &GameList[i];
        strncpy(availableGames[nbGames].path, gamePath, 1024);
        LOGSTV("Rebuild %s from %s\n", gameName, gamePath);
        nbGames++;
        break;
      }
    }
  }
  fclose(fp);
  return nbGames;
}

int STVInit(int id, char* path){
  if (CartridgeArea->carttype != CART_ROMSTV) return 0;
  int nbGames = STVGetRomList(path, 0);
  if (nbGames == 0) return -1;
  if (nbGames <= id) return -1;
  if (loadGame(id) == 0) {
    yabsys.isSTV = 1;
    return 0;
  }
  return -1;
}
