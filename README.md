![Alt text](/skycatalog.jpg?raw=true "Optional Title")

## Synopsis

Astronomical database (stars, galaxies, open clusters, etc) for Arduino.

## Features
- Catalog of 88 constellations with abbreviations and denominations in: Latin, English, French.
- Catalog of 494 stars up to magnitude 4.
- Messier catalog of 110 objects.
- NGC catalog of 3993 objects up to magnitude 14.
- IC catalog of 401 objects up to magnitude 14.
- For each object (excluding constellation): name, denomination, associated constellation, equatorial coordinates (J2000), visual magnitude.
- In addition for stars: annual drift in right ascension and declination, parallax.

## Installation
Install the library using the Library Manager of the Arduino IDE.
Unzip "db.zip" on your SD card.
You're ready to go with sample codes availables in menu "File->Examples->SkyCatalog".

## Goodies
SdFat library support can be easily enable in "SkyCatalog.hpp" for speed and memory improvment. SdFat is strongly recommanded. You just need to set define USE_SD_FAT to 1. By default, SdFat is not enable for "plug and play" compatibiity with default arduino libraries.

## Note for Mac users
Take care to hidden files generated on your SD card by OS X. They must be deleted before ejection. 
Open a terminal and write: find [YOUR SD PATH] -name '.*' -exec rm -rf {} \;

## Motivation

This library is part of a personnal project to improve my EM10 Takahashi mount thanks to Arduino.

## Contributors

Data generated with Coelix by  Jean Vallières.

## License

Copyright (c) 2017 by Sebastien MARCHAND 
Web:www.marscaper.com - Email:sebastien@marscaper.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.