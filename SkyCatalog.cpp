/*
 * SkyCatalog.cpp
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
#include "SkyCatalog.hpp"

char *newPathByAppendingPaths(char *path, char *path1, char *path2, char *path3)
{
  int bufferSize = strlen(path)+1;
  
  if( path1 )
  {
    bufferSize += strlen(path1)+1;
  }
  
  if( path2 )
  {
    bufferSize += strlen(path2)+1;
  }
  
  if( path3 )
  {
    bufferSize += strlen(path3)+1;
  }
  
  // Create a new empty string
  char *fullPath = (char*)calloc(bufferSize,sizeof(char));
  
  // Copy first path
  strcpy(fullPath, path);
  
  if( path1 )
  {
    // Add / separator
    strncpy(&fullPath[strlen(fullPath)],"/",1);
    
    // Copy path
    strncpy(&fullPath[strlen(fullPath)],path1,strlen(path1));
  }
  
  if( path2 )
  {
    // Add / separator
    strncpy(&fullPath[strlen(fullPath)],"/",1);
    
    // Copy path
    strncpy(&fullPath[strlen(fullPath)],path2,strlen(path2));
  }
  
  if( path3 )
  {
    // Add / separator
    strncpy(&fullPath[strlen(fullPath)],"/",1);
    
    // Copy path
    strncpy(&fullPath[strlen(fullPath)],path3,strlen(path3));
  }
  
  return fullPath;
}

void readLine(File *file, char *string, int stringLength)
{
  //get bytes from file
  int strIndex = 0;
  char inputChar = file->read();
  while (inputChar != '\n' && inputChar != EOF && strIndex<stringLength)
  {
    string[strIndex] = inputChar;
    
    strIndex++;
    inputChar = file->read();
  }
  
  // Clip string if needed
  if( strIndex == stringLength )
  {
    strIndex--;
  }
  
  // Set null terminated string char for security
  string[strIndex] = '\0';
}

#if USE_SD_FAT
int countFile(SdFat *sd, char *path, char *cntName)
#else
int countFile(SDClass *sd, char *path, char *cntName)
#endif
{
  int count = -1;
  
  // Simple test to check if directory contains "star.cnt"
  char *cntPath = newPathByAppendingPaths(path,cntName,NULL,NULL);
  
  // Open file
  File cntFile = sd->open(cntPath);
  
  // Free string path
  free(cntPath);
  
  // Does file exist?
  if(cntFile)
  {
    char string[6];
    readLine(&cntFile, string, 6);
    
    // Convert to int value
    count = atoi(string);
    
    // Close file
    cntFile.close();
  }
  
  return count;
}

SkyCatalog::SkyCatalog()
{
#if USE_SD_FAT
  _sd = new SdFat();
#else
  _sd = &SD;
#endif
}

SkyCatalog::~SkyCatalog()
{
  delete _sd;
  
  free(_dbDirectory);
  
  closeObjectList();
  
  closeConstellationList();
}

bool SkyCatalog::initDatabase(int sd_pin,char *directory)
{
  // Init SD
#if USE_SD_FAT
  if (!_sd->begin(sd_pin, SPI_FULL_SPEED))
#else
    if (!_sd->begin(sd_pin))
#endif
    {
      return false;
    }
  
  // Simple test to check if directory contains "star.cnt"
  if( countFile(_sd,directory,"star.cnt") == -1 )
  {
    return false;
  }
  
  // Copy db path
  _dbDirectory = (char*)malloc(strlen(directory)+1*sizeof(char));
  strcpy(_dbDirectory, directory);
  
  return true;
}

SkyDbDescrStruct SkyCatalog::databaseDescription()
{
  SkyDbDescrStruct dbDescr;
  dbDescr.starCount    = countFile(_sd,_dbDirectory,"star.cnt");
  dbDescr.messierCount = countFile(_sd,_dbDirectory,"messier.cnt");
  dbDescr.ngcCount     = countFile(_sd,_dbDirectory,"ngc.cnt");
  dbDescr.icCount      = countFile(_sd,_dbDirectory,"ic.cnt");
  return dbDescr;
}

SkyObjectStruct SkyCatalog::object(char *consAbrv, long objIndex, DatabaseObjectType objType)
{
  // Create path
  char fileName[11];
  sprintf(fileName, "%06d.txt\0",objIndex);
  
  // Get type directory according to objType
  char *type = NULL;
  switch (objType)
  {
    case DatabaseObjectStar:
    {
      type = "star";
      break;
    }
      
    case DatabaseObjectMessier:
    {
      type = "messier";
      break;
    }
      
    case DatabaseObjectNGC:
    {
      type = "ngc";
      break;
    }
      
    case DatabaseObjectIC:
    {
      type = "ic";
      break;
    }
  }
  
  // Create path to object directory
  char *path;
  if( consAbrv )
  {
    // Filter by constellation
    
    // Add "const" directory to Abbreviation path
    consAbrv = newPathByAppendingPaths("const", consAbrv, NULL, NULL);
    
    path = newPathByAppendingPaths(_dbDirectory,consAbrv,type,fileName);
    
    // Remove allocated path
    free(consAbrv);
  }
  else
  {
    // All objects
    path = newPathByAppendingPaths(_dbDirectory,type,fileName,NULL);
  }
  
  // Open file
  File objectFile = _sd->open(path);
  
  // Delete path
  free(path);
  
  // Return object from file
  SkyObjectStruct object = readObject(&objectFile,objIndex,objType);
  
  objectFile.close();
  
  return object;
}

bool SkyCatalog::openObjectList(char *consAbrv, DatabaseObjectType objectType)
{
  if( !_dbDirectory )
  {
    return false;
  }
  
  _currentObjetListType = objectType;
  
  // Get type directory according to objType
  char *type = NULL;
  switch (objectType)
  {
    case DatabaseObjectStar:
    {
      type = "star";
      break;
    }
      
    case DatabaseObjectMessier:
    {
      type = "messier";
      break;
    }
      
    case DatabaseObjectNGC:
    {
      type = "ngc";
      break;
    }
      
    case DatabaseObjectIC:
    {
      type = "ic";
      break;
    }
  }
  
  // Create path to object directory
  char *path;
  if( consAbrv )
  {
    // Filter by constellation
    path = newPathByAppendingPaths(_dbDirectory, "const", consAbrv, type);
  }
  else
  {
    // All objects
    path = newPathByAppendingPaths(_dbDirectory, type, NULL, NULL);
  }
  
  
  // No file
  _fileObjectIndex = 0;
  
  // Close old directory if open
  if( _currentObjectDir ) _currentObjectDir.close();
  
  // Open directory
  _currentObjectDir = _sd->open(path);
  
  // Remove allocated path
  free(path);
  
  return _currentObjectDir;
}

SkyObjectStruct SkyCatalog::readObject(File *objectFile, long objIndex, DatabaseObjectType objType)
{
  SkyObjectStruct skyObject;
  
  // Clean object
  memset(&skyObject,0,sizeof(SkyObjectStruct));
  
  if( *objectFile != false )
  {
    char tmpString[12];
    int  tmpStringSize = sizeof(tmpString);
    
    // Valid index
    skyObject.index = objIndex;
    
    // Name
    readLine(objectFile, skyObject.name, sizeof(skyObject.name));
    
    // Designation
    readLine(objectFile, skyObject.designation, sizeof(skyObject.designation));
    
    // Const Abbreviation
    readLine(objectFile, skyObject.constAbrv, sizeof(skyObject.constAbrv));
    
    // R.A
    readLine(objectFile, tmpString, tmpStringSize);
    skyObject.ra = atof(tmpString);
    
    // Dec
    readLine(objectFile, tmpString, tmpStringSize);
    skyObject.dec = atof(tmpString);
    
    // Visual magnitude
    readLine(objectFile, tmpString, tmpStringSize);
    skyObject.magnitude = atof(tmpString);
    
    if( objType == DatabaseObjectStar )
    {
      // drift RA per year
      readLine(objectFile, tmpString, tmpStringSize);
      skyObject.dRa = atof(tmpString);
      
      // drift Dec per year
      readLine(objectFile, tmpString, tmpStringSize);
      skyObject.dDec = atof(tmpString);
      
      // Parallax
      readLine(objectFile, tmpString, tmpStringSize);
      skyObject.parallax = atof(tmpString);
    }
    else
    {
      skyObject.dRa      = NAN;
      skyObject.dDec     = NAN;
      skyObject.parallax = NAN;
    }
  }
  
  return skyObject;
}

bool SkyCatalog::currentObjectInList(SkyObjectStruct *object)
{
  bool res = false;
  
  if( _currentObjectFile )
  {
    // Get filename
#if USE_SD_FAT
    char name[14];
    _currentObjectFile.getName(name,14);
#else
    char *name = _currentObjectFile.name();
#endif
    
    // Extension should be txt
    int extIdx = strlen(name)-4;
    if( strstr(strlwr(name+extIdx), "txt"))
    {
      // Remove extension
      name[extIdx] = '\0';
      
      // Read object
      *object = readObject(&_currentObjectFile, atoi(name), _currentObjetListType);
      
      res = true;
    }
  }
  else
  {
    // Clean object
    memset(object,0,sizeof(SkyObjectStruct));
  }
  
  return res;
}

bool SkyCatalog::gotoNextObjectInList()
{
  bool res = false;
  
  if( _currentObjectDir )
  {
    if( _currentObjectFile )
    {
      _currentObjectFile.close();
    }
    
    // Go to next file if exists
    File nextFile = _currentObjectDir.openNextFile();
    
    // Is there a valid file?
    if( nextFile )
    {
      _currentObjectFile = nextFile;
      _fileObjectIndex++;
      
      res = true;
    }
  }
  
  return res;
}

bool SkyCatalog::gotoPreviousObjectInList()
{
  bool res = false;
  
  if( (_fileObjectIndex-1) && _currentObjectDir )
  {
    // If file already open. Close it.
    if( _currentObjectFile )
    {
      _currentObjectFile.close();
    }
    
    // Go to beginning
    unsigned int index = 1;
    _currentObjectDir.rewindDirectory();
    _currentObjectFile = _currentObjectDir.openNextFile();
    
    // Scroll down
    while (index != _fileObjectIndex-1 )
    {
      // Close current
      _currentObjectFile.close();
      
      // Open next
      _currentObjectFile = _currentObjectDir.openNextFile();
      index++;
    }
    
    // Is there a valid file?
    if( _currentObjectFile )
    {
      _fileObjectIndex = index;
      res = true;
    }
    else
    {
      // No valid file
      _fileObjectIndex = 0;
    }
  }
  
  return res;
}

void SkyCatalog::closeObjectList()
{
  _fileObjectIndex = 0;
  
  _currentObjetListType = SkyObjectNone;
  
  if( _currentObjectDir ) _currentObjectDir.close();
  
  if( _currentObjectFile ) _currentObjectFile.close();
}

#pragma mark -
#pragma mark Constellation

bool SkyCatalog::openConstellationList()
{
  if( !_dbDirectory )
  {
    return false;
  }
  
  char *path = newPathByAppendingPaths(_dbDirectory, "const", NULL, NULL);
  
  // No file
  _fileConstIndex = 0;
  
  // Close old directory if open
  if( _currentConstDir ) _currentConstDir.close();
  
  // Open directory
  _currentConstDir = _sd->open(path);
  
  // Remove allocated path
  free(path);
  
  return _currentConstDir;
}

bool SkyCatalog::gotoNextConstellationInList()
{
  bool res = false;
  
  if( _currentConstDir )
  {
    if( _currentConstFile )
    {
      _currentConstFile.close();
    }
    
    // Go to next file if exists
    File nextFile = _currentConstDir.openNextFile();
    
    // Is there a valid file?
    if( nextFile )
    {
      _currentConstFile = nextFile;
      _fileConstIndex++;
      
      res = true;
    }
  }
  
  return res;
}

bool SkyCatalog::gotoPreviousConstellationInList()
{
  bool res = false;
  
  if( (_fileConstIndex-1) && _currentConstDir )
  {
    // If file already open. Close it.
    if( _currentConstFile )
    {
      _currentConstFile.close();
    }
    
    // Go to beginning
    unsigned int index = 1;
    _currentConstDir.rewindDirectory();
    _currentConstFile = _currentConstDir.openNextFile();
    
    // Scroll down
    while (index != _fileConstIndex-1 )
    {
      // Close current
      _currentConstFile.close();
      
      // Open next
      _currentConstFile = _currentConstDir.openNextFile();
      index++;
    }
    
    // Is there a valid file?
    if( _currentConstFile )
    {
      _fileConstIndex = index;
      res = true;
    }
    else
    {
      // No valid file
      _fileConstIndex = 0;
    }
  }
  
  return res;
}

SkyConstStruct SkyCatalog::constellation(char *consAbrv)
{
  SkyConstStruct object;
  
  // Get translation path
  char *tslPath;
  switch (_language)
  {
    case SkyTranslateLatin:
      tslPath = newPathByAppendingPaths(_dbDirectory, "const", consAbrv, "lat.tsl");
      break;
      
    case SkyTranslateFrench:
      tslPath = newPathByAppendingPaths(_dbDirectory, "const", consAbrv, "fra.tsl");
      break;
      
    default:
      tslPath = newPathByAppendingPaths(_dbDirectory, "const", consAbrv, "eng.tsl");
      break;
  }
  
  File tslFile = _sd->open(tslPath,FILE_READ);
  if( tslFile )
  {
    // Read translation
    readLine(&tslFile, object.fullName, sizeof(object.fullName));
    tslFile.close();
  }
  else
  {
    // Use abbreviation instead
    strcpy(object.fullName, consAbrv);
  }
  
  strcpy(object.abrv,     consAbrv);
  
  free(tslPath);
  
  return object;
}

bool SkyCatalog::currentConstellationInList(SkyConstStruct *object)
{
  bool res = false;
  
  if( _currentConstFile )
  {
    // Get filename
#if USE_SD_FAT
    char name[14];
    _currentConstFile.getName(name,14);
    
    strcpy(object->abrv, name);
#else
    strcpy(object->abrv, _currentConstFile.name());
#endif
    
    // Get translation path
    char *tslPath;
    switch (_language)
    {
      case SkyTranslateLatin:
        tslPath = newPathByAppendingPaths(_dbDirectory, "const", object->abrv, "lat.tsl");
        break;
        
      case SkyTranslateFrench:
        tslPath = newPathByAppendingPaths(_dbDirectory, "const", object->abrv, "fra.tsl");
        break;
        
      default:
        tslPath = newPathByAppendingPaths(_dbDirectory, "const", object->abrv, "eng.tsl");
        break;
    }
    
    File tslFile = _sd->open(tslPath,FILE_READ);
    if( tslFile )
    {
      // Read translation
      readLine(&tslFile, object->fullName, sizeof(object->fullName));
      
      tslFile.close();
    }
    else
    {
      // Use abbreviation instead
      strcpy(object->fullName, object->abrv);
    }
    
    free(tslPath);
  }
  else
  {
    // Clean object
    memset(object,0,sizeof(SkyConstStruct));
  }
  
  return res;
}

void SkyCatalog::closeConstellationList()
{
  _fileConstIndex = 0;
  
  if( _currentConstDir ) _currentConstDir.close();
  
  if( _currentConstFile ) _currentConstFile.close();
}
