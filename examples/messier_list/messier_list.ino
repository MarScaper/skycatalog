/*
 * messier_list.ino
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
#include <SkyCatalog.hpp>

SkyCatalog *catalog = NULL;

void printDatabaseDescription()
{
  SkyDbDescrStruct dbStruct = catalog->databaseDescription();
  
  Serial.print("Star count: ");
  Serial.println(dbStruct.starCount);
  
  Serial.print("Messier count: ");
  Serial.println(dbStruct.messierCount);
  
  Serial.print("NGC count: ");
  Serial.println(dbStruct.ngcCount);
  
  Serial.print("IC count: ");
  Serial.println(dbStruct.icCount);
}

void printSkyObject(SkyObjectStruct skyObject)
{
  if( !skyObject.index )
  {
    return;
  }
  
  Serial.println("    _______________");
  
  Serial.print("    Name: ");
  Serial.println(skyObject.name);
  
  Serial.print("    Design: ");
  Serial.println(skyObject.designation);
  
  Serial.print("    Const: ");
  Serial.println(skyObject.constAbrv);
  
  Serial.print("    RA: ");
  Serial.println(skyObject.ra,6);
  
  Serial.print("    Dec: ");
  Serial.println(skyObject.dec,6);
  
  Serial.print("    Mag: ");
  Serial.println(skyObject.magnitude);
  
  if( !isnan(skyObject.dRa) )
  {
    Serial.print("    dRA: ");
    Serial.println(skyObject.dRa,6);
  }
  
  if( !isnan(skyObject.dDec) )
  {
    Serial.print("    dDec: ");
    Serial.println(skyObject.dDec,6);
  }
  
  if( !isnan(skyObject.parallax) )
  {
    Serial.print("    Parallax: ");
    Serial.println(skyObject.parallax,6);
  }
}

void setup()
{
  Serial.begin(9600);
  
  double t = millis();
  
  catalog = new SkyCatalog();
  
  if( catalog->initDatabase(10,"db") )
  {
    Serial.println("Catalog ready");
    
    Serial.println("______________________________");
    
    printDatabaseDescription();
    
    Serial.println("______________________________");
    
    catalog->openConstellationList();
    
    /*! Constellation desciption for tranlation. */
    SkyConstStruct constDescr;

    // Scan constellations
    while( catalog->gotoNextConstellationInList() )
    {
      catalog->currentConstellationInList(&constDescr);
      
      Serial.println(constDescr.fullName);
      
      // Scan Messier objects in current constellation
      if( catalog->openObjectList(constDescr.abrv,DatabaseObjectMessier) )
      {
        while( catalog->gotoNextObjectInList() )
        {
          SkyObjectStruct object;
          
          catalog->currentObjectInList(&object);
          
          printSkyObject(object);
        }
        
        catalog->closeObjectList();
      }
    }
    
    catalog->closeConstellationList();
  }
  
  Serial.println("\nCatalog ended");
  
  t = (millis()-t)/1000;
  
  Serial.print("Done: ");
  Serial.print(t,3);
  Serial.println("s");
  
}

void loop() 
{
}
