/***************************************************************************
                          data.h  -  description
                             -------------------
    begin                : Fre Dez 22 2000
    copyright            : (C) 2000 by Malte Knoerr
    email                : malte@knoerr.escape.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATA_H
#define DATA_H


/**
  * This class contains the information, which files the user
  * wants how to backup.
  * @author Malte Knoerr
  */

#include <qstring.h>

class Data {
public:
  /**
    * Standard constructor.
    */
	Data();

  /**
   * Standard destructor.
   */
	~Data();

  /**
    * Sets the files, which the user wants to encrypt and to backup.
    */
  void setFiles(String files);

  /**
    * Sets the file, as which the encrypted backup has to be saved.
    */
  void setDestinationFile(String File);

  /**
    * Sets the GnuPG ID, for which the backup has to be encrypted.
    * This is usually the ID of the user himself.
    */
  void setDestinationID(String file);

  /**
    * Sets, wether the user wants compression of his backup.
    */
  void setCompression(bool decision);

  /**
    * Compress the tar file.
    */
  void compress();

  /**
    * Encrypt the archive with GnuPG.
    */
  void encrypt();
	
private:
  /** 
    * The files, which the user wants to encrypt and to backup. 
    */
  String files;
  
  /** 
    * The file, as which the encrypted backup has to be saved. 
    */
  String destinationFile;
  
  /** 
    * The GnuPG ID, for which the backup has to be encrypted. 
    * This is usually the ID of the user himself.
    */
  String destinationID;

  /**
    * Wants the user compression of his backup?
    */
  bool compression;  

  // Methods:

  /**
    * Verify, if the provided GnuPG ID is valid.
    */
  bool verifyID(String id);

};

#endif
