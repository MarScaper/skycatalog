/*
 * SkyCatalog.hpp
 *
 * Copyright (c) 2017 by Sebastien MARCHAND (Web:www.marscaper.com, Email:sebastien@marscaper.com)
 */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SkyCatalog_h
#define SkyCatalog_h

#include <Arduino.h>

/*! Enable/Disable SdFat library. Use of SdFat is strongly recommanded 
 *  for performances and memory improvment. */
#define USE_SD_FAT 0

#if USE_SD_FAT
/*! \class SdFat
 *  SdFat is an external library available at:<br>
 *  https://github.com/greiman/SdFat
 *  <br><br>
 *  The Arduino SdFat library provides read/write access to FAT16/FAT32 file systems on SD/SDHC flash cards.
 *
 *  @ingroup ExternalLib
 */
#include <SdFat.h>
#else
#include <SD.h>
#endif

/*! Languages. */
enum SkyTranslate
{
  SkyTranslateEnglish,
  SkyTranslateLatin,
  SkyTranslateFrench
};

/*! Database object types. */
enum DatabaseObjectType
{
  SkyObjectNone,
  DatabaseObjectStar,
  DatabaseObjectMessier,
  DatabaseObjectNGC,
  DatabaseObjectIC,
};

/*! This structure defines the content of the database. */
struct SkyDbDescrStruct
{
  int starCount;
  int messierCount;
  int ngcCount;
  int icCount;
};

/*! This structure describe object (Star,Messier,NGC,IC). 
 *  Annual drift (dRA, dDec) and parallax are only available for stars. */
struct SkyObjectStruct
{
  /*! Index (SAO number, Messier number, NGC number or IC number. */
  long  index;
  
  /* Designation (ex: NGC 7000) */
  char  designation[32];
  
  /*! Usual name if exists. */
  char  name[32];

  /*! Constellation abbreviation.*/
  char  constAbrv[5];
  
  /* Right Ascension coordinate. */
  float ra;
  
  /* Declination coordinate. */
  float dec;
  
  /* Visual magnitude. */
  float magnitude;
  
  /* Right Ascension drift per year. -stars only- */
  float dRa;
  
  /* Declination drift per year. -stars only- */
  float dDec;
  
  /* Parallax. -stars only- */
  float parallax;
};

/*! Describe constellation. */
struct SkyConstStruct
{
  /*! Abbreviation. */
  char abrv[5];
  
  /*! Translation. */
  char fullName[32];
};

/*!
 * This class is used to manage the deep sky object database stored on the SD card.
 *
 *  @ingroup Model
 */
class SkyCatalog
{
public:
  
  SkyCatalog();
  
  ~SkyCatalog();
  
  /*! Set language for translation. Default is English. */
  void setLanguage(SkyTranslate language) { _language=language; };
  
  bool initDatabase(int sd_pin,char *bdDirectory);
  
  SkyDbDescrStruct databaseDescription();
  
  //
  // Manage constellations
  //
  
  /*! Open constellation list. */
  bool openConstellationList();
  
  /*! Go to next constellation. */
  bool gotoNextConstellationInList();
  
   /*! Go to previous constellation. */
  bool gotoPreviousConstellationInList();
  
  /*! Read current constellation in list. */
  bool currentConstellationInList(SkyConstStruct *object);
  
  /*! Read constellation according to abbreviation. */
  SkyConstStruct constellation(char *consAbrv);
  
  /*! Close object list. */
  void closeConstellationList();
  
  
  //
  // Manage objects (stars, galaxies, etc)
  //
  
  /*! Open object list. */
  bool openObjectList(char *consAbrv, DatabaseObjectType objectType);
  
  /*! Go to next object. */
  bool gotoNextObjectInList();
  
  /*! Go to previous object. */
  bool gotoPreviousObjectInList();
  
  /*! Read current object in list. */
  bool currentObjectInList(SkyObjectStruct *object);
  
  /*! Read object according to identifier (SAO number for stars, Messier/NGC/Ic
   *  identifier for deep sky objects) and type. */
  SkyObjectStruct object(char *consAbrv, long identifier, DatabaseObjectType type);
  
  /*! Close object list. */
  void closeObjectList();
  
private:
  
  File _currentConstDir;
  File _currentConstFile;
  unsigned int  _fileConstIndex = 0;
  
  File _currentObjectDir;
  File _currentObjectFile;
  unsigned int  _fileObjectIndex = 0;
  DatabaseObjectType _currentObjetListType = SkyObjectNone;
  
#if USE_SD_FAT
  SdFat *_sd = NULL;
#else
  SDClass *_sd = NULL;
#endif
  
  char *_dbDirectory = NULL;
  
  SkyTranslate _language = SkyTranslateEnglish;
  
  SkyObjectStruct readObject(File *objectFile, long objIndex, DatabaseObjectType objType);
};

#endif
