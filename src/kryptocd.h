/***************************************************************************
                          kryptocd.h  -  description
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

#ifndef KRYPTOCD_H
#define KRYPTOCD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapp.h>
#include <qwidget.h>

/** KryptoCD is the base class of the porject */
class KryptoCD : public QWidget
{
  Q_OBJECT 
  public:
    /** construtor */
    KryptoCD(QWidget* parent=0, const char *name=0);
    /** destructor */
    ~KryptoCD();
};

#endif
