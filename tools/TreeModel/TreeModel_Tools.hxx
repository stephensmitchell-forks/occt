// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#ifndef TreeModel_Tools_H
#define TreeModel_Tools_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <QMap>
#include <QString>
#include <QByteArray>

class QAction;
class QObject;
class QTreeView;

//! \class TreeModel_Tools
//! \brief The tool that gives auxiliary methods for qt elements manipulation
class TreeModel_Tools
{
public:

  //! Converts a Qt string to byte array, string has mask: @ByteArray[...]
  //! \param theValue a converted string
  //! \return the extended filled array
  Standard_EXPORT static QString ToString (const QByteArray& theValue);

  //! Converts a Qt byte array to Qt string. It has mask: @ByteArray[...]
  //! \param theValue a converted string
  //! \return the extended filled array
  Standard_EXPORT static QByteArray ToByteArray (const QString& theValue);

  //! Creates an action with the given text connected to the slot
  //! \param theText an action text value
  //! \param theSlot a listener of triggered signal of the new action
  //! \param theParent a parent object
  //! \param theContext listener of the action toggle
  //! \return a new action
  Standard_EXPORT static QAction* CreateAction (const QString& theText, const char* theSlot,
                                                QObject* theParent, QObject* theContext);

  //! Save state of three view in a container in form: key, value. It saves:
  //! - visibiblity of columns,
  //! - columns width
  //! \param theTreeView a view instance
  //! \param theItems [out] properties
  //! \param thePrefix peference item prefix
  Standard_EXPORT static void SaveState (QTreeView* theTreeView, QMap<QString, QString>& theItems,
                                         const QString& thePrefix = QString());
  //! Restore state of three view by a container
  //! \param theTreeView a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix peference item prefix
  //! \return boolean value whether the property is applyed to the tree view
  Standard_EXPORT static bool RestoreState (QTreeView* theTreeView, const QString& theKey, const QString& theValue,
                                            const QString& thePrefix = QString());

  //! Fills tree view by default sections parameters obtained in view's tree model
  Standard_EXPORT static void SetDefaultHeaderSections (QTreeView* theTreeView);

  //! Sets using visibility column in the tree view:
  //! - sets the state in the TreeModel
  //! - set section width, not resizable
  //! \param theTreeView a view instance
  //! \param theActive boolean value if the column should be connected/visible and other
  Standard_EXPORT static void UseVisibilityColumn (QTreeView* theTreeView, const bool theActive = true);

  //! Returns the text width
  //! \param theText source text
  //! \param theParent parent widget with its own style
  //! \return calculated width value
  Standard_EXPORT static int GetTextWidth (const QString& theText, QObject* theParent);
};

#endif
