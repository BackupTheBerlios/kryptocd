/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Son Jan 21 12:29:20 CET 2001
    copyright            : (C) 2001 by Malte Knoerr
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

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "kryptocd.h"

static const char *description =
	I18N_NOOP("KryptoCD");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

  KAboutData aboutData( "kryptocd", I18N_NOOP("KryptoCD"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2001, Malte Knoerr");
  aboutData.addAuthor("Malte Knoerr",0, "malte@knoerr.escape.de");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication a;
  KryptoCD *kryptocd = new KryptoCD();
  a.setMainWidget(kryptocd);
  kryptocd->show();  

  return a.exec();
}
