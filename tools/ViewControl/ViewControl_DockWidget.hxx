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

#ifndef ViewControl_DockWidget_H
#define ViewControl_DockWidget_H

#include <inspector/ViewControl.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif

#include <QDockWidget>
#include <QCloseEvent>
#include <QSize>
#include <QWidget>

//! \class ViewControl_TreeView
//! Extended tree view control with possibility to set predefined size.
class VIEWCONTROL_EXPORT ViewControl_DockWidget : public QDockWidget
{
  Q_OBJECT

public:
  //! Constructor
  ViewControl_DockWidget (const QString& theTitle, QWidget* theParent) : QDockWidget (theTitle, theParent) {}

  //! Destructor
  virtual ~ViewControl_DockWidget() {}

  //! Sets default size of control, that is used by the first control show
  //! \param theDefaultWidth the width value
  //! \param theDefaultHeight the height value
  //void SetPredefinedSize (const QSize& theSize) { myDefaultSize = theSize; }

  //! Returns predefined size if both values are positive, otherwise parent size hint
  //virtual QSize sizeHint() const Standard_OVERRIDE { return QSize(10, 10); }//myDefaultSize.isValid() ? myDefaultSize : QDockWidget::sizeHint(); }

  /// Redefinition of virtual method
  //virtual QSize minimumSizeHint() const
  //{
  //  return QSize (10, 10);
    //int aHeight = 4/*2*/*QFontMetrics(font()).height();
    //QSize aSize = QListWidget::minimumSizeHint();
    //return QSize(aSize.width(), aHeight);
  //}

//signals:
  //! Signal about the dock widget is closed, emits before the closeEvent is performed
  //void closed();

protected:
  //! Reimplementation to emit a signal about the panel close
  //! \param theEvent close event
  //void closeEvent (QCloseEvent* theEvent) { emit closed(); QDockWidget::closeEvent (theEvent); }

private:
  //QSize myDefaultSize; //! default size, empty value if it should not be used
};

#endif
