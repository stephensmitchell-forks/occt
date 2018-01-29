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

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_TableModel.hxx>

#include <inspector/TreeModel_Tools.hxx>

#include <QAction>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableView>
#include <QWidget>

#include <limits>

const int DEFAULT_COLUMN_WIDTH = 60;
//const int REAL_SIGNS = 16;

//class MessageView_TableViewHeader : public QHeaderView
//{
//public:
//  MessageView_TableViewHeader (QWidget* theParent, ViewControl_Table* theTable)
//    : QHeaderView (Qt::Horizontal, theParent), myTable (theTable)
//  {
//    //setClickable(true);
//    setSectionsClickable (true);
//    setHighlightSections (true);
//  }
//  ~MessageView_TableViewHeader() {}
//
//protected:
//  void contextMenuEvent (QContextMenuEvent* theEvent) { myTable->ProcessContextMenuRequsted(theEvent); }
//
//private:
//  ViewControl_Table* myTable;
//};


// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ViewControl_Table::ViewControl_Table (QWidget* theParent)
: QObject (theParent)
{
  myMainWidget = new QWidget (theParent);
  QGridLayout* aLayout = new QGridLayout (myMainWidget);
  aLayout->setContentsMargins (0, 0, 0, 0);

  //QWidget* aProperties = new QWidget(myMainWidget);
  //aLayout->addWidget(aProperties);
  //QHBoxLayout* aPropertiesLayout = new QHBoxLayout(aProperties);
  //aPropertiesLayout->setContentsMargins(0, 0, 0, 0);

  //mySize = new QLabel ("", myMainWidget);
  //mySeparateSize = new QSpinBox (myMainWidget);
  //mySeparateSize->setRange (0, 10000);
  //myVisualizedSize = new QLabel ("", myMainWidget);
  //aPropertiesLayout->addWidget (mySize);
  //aPropertiesLayout->addWidget (new QLabel ("Divide:", myMainWidget));
  //aPropertiesLayout->addWidget (mySeparateSize);
  //aPropertiesLayout->addWidget (myVisualizedSize);

  //myTolerance = new QLabel("Tolerance:", myMainWidget);
  //myToleranceSize = new QSpinBox(myMainWidget);
  //myToleranceSize->setRange(0, 100);
  //aPropertiesLayout->addWidget(myTolerance);
  //aPropertiesLayout->addWidget(myToleranceSize);

  myTableView = new QTableView (myMainWidget);
  //myTableView->setHorizontalHeader(new MessageView_TableViewHeader(myTableView, this));
  //myTableView->horizontalHeader()->setDefaultSectionSize (DEFAULT_COLUMN_WIDTH);

  QHeaderView* aVHeader = myTableView->verticalHeader();
  aVHeader->setVisible (false);
  int aDefCellSize = aVHeader->minimumSectionSize() + TreeModel_Tools::HeaderSectionMargin();
  aVHeader->setDefaultSectionSize (aDefCellSize);

  aLayout->addWidget (myTableView, 1, 0, 1, 4);

  //myModelInformation = new QPlainTextEdit (myMainWidget);
  //myModelInformation->setVisible (false);
  //aLayout->addWidget (myModelInformation, 2, 0, 1, 4);

  //myMinValue = new QPushButton ("Min", myMainWidget);
  //myMaxValue = new QPushButton("Max", myMainWidget);
  //myMinSelectedValue = new QPushButton("Min of selected", myMainWidget);
  //myMaxSelectedValue = new QPushButton("Max of selected", myMainWidget);

  //aLayout->addWidget(myMinValue, 3, 0);
  //aLayout->addWidget(myMinSelectedValue, 3, 1);
  //aLayout->addWidget(myMaxValue, 4, 0);
  //aLayout->addWidget(myMaxSelectedValue, 4, 1);

  //connect (mySeparateSize, SIGNAL (valueChanged (int)),
  //         this, SLOT (onSeparateSizeChanged (int)));
  //connect (myToleranceSize, SIGNAL (valueChanged (int)),
  //         this, SLOT (onToleranceSizeChanged (int)));
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ViewControl_Table::Init (QAbstractTableModel* theModel)
{
  //TCollection_AsciiString anInfo;
  //double aMinValue = DBL_MIN, aMaxValue = DBL_MAX;
  //if (dynamic_cast<MessageView_TableModelRealArray*>(theModel))
  //{
  //  MessageView_TableModelRealArray* aModel = dynamic_cast<MessageView_TableModelRealArray*>(theModel);
  //  if (mySeparateSize->value() != 0)
  //  {
  //    std::vector<double> aModelValues = aModel->GetValues(); // copy values
  //    aModel->SetColumnCount(mySeparateSize->value());
  //    aModel->Init(aModelValues);
  //  }
  //  aModel->GetRangeValues(aMinValue, aMaxValue);
  //  //anInfo = aModel->AdditionalInformation();
  //}
  //else if (dynamic_cast<MessageView_TableModelRealArrayCompare*>(theModel))
  //{
  //  MessageView_TableModelRealArrayCompare* aModel = dynamic_cast<MessageView_TableModelRealArrayCompare*>(theModel);
  //  if (mySeparateSize->value() != 0)
  //  {
  //    std::vector<double> aModelValues = aModel->GetValues(); // copy values
  //    std::vector<double> aSecondModelValues = aModel->GetSecondValues(); // copy values
  //    aModel->SetColumnCount (mySeparateSize->value());
  //    aModel->Init (aModelValues, aSecondModelValues);
  //  }
  //  aModel->GetRangeValues(aMinValue, aMaxValue);
  //  anInfo = aModel->AdditionalInformation();
  //}
  myTableView->setModel (theModel);
  myTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QItemSelectionModel* aSelectionModel = new QItemSelectionModel(theModel);
  myTableView->setSelectionModel (aSelectionModel);

  connect(aSelectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onTableSelectionChanged(const QItemSelection&, const QItemSelection&)));

  //mySize->setText (QString("[ %1, %2 ]").arg (theModel->rowCount()).arg (theModel->columnCount()));

  //myVisualizedSize->setText(mySize->text());
  //myModelInformation->setVisible(!anInfo.IsEmpty());
  //if (!anInfo.IsEmpty())
  //  myModelInformation->setPlainText (anInfo.ToCString());

  //myMinValue->setText (QString ("Min:") + QString::number (aMinValue, 'g', REAL_SIGNS));
  //myMaxValue->setText (QString ("Max:") + QString::number (aMaxValue, 'g', REAL_SIGNS));
  //myMinSelectedValue->setText (QString("Min of selected:") + QString::number(0, 'g', REAL_SIGNS));
  //myMaxSelectedValue->setText (QString("Max of selected:") + QString::number(0, 'g', REAL_SIGNS));
}

//// =======================================================================
//// function : ProcessContextMenuRequsted
//// purpose :
//// =======================================================================
//void ViewControl_Table::ProcessContextMenuRequsted (QContextMenuEvent* theEvent)
//{
//  emit headerContextMenuRequsted (theEvent);
//}
//
//// =======================================================================
//// function : onSeparateSizeChanged
//// purpose :
//// =======================================================================
//void ViewControl_Table::onSeparateSizeChanged (int theValue)
//{
//  if (dynamic_cast<MessageView_TableModelRealArray*>(myTableView->model()))
//  {
//    MessageView_TableModelRealArray* aModel = dynamic_cast<MessageView_TableModelRealArray*>(myTableView->model());
//    std::vector<double> aModelValues = aModel->GetValues();
//    aModel->SetColumnCount (theValue);
//    aModel->Init (aModelValues);
//
//    myVisualizedSize->setText (QString ("[ %1, %2 ]").arg (aModel->rowCount()).arg (aModel->columnCount()));
//  }
//  else {
//    if (dynamic_cast<MessageView_TableModelRealArrayCompare*>(myTableView->model()))
//    {
//      MessageView_TableModelRealArrayCompare* aModel = dynamic_cast<MessageView_TableModelRealArrayCompare*>(myTableView->model());
//      std::vector<double> aModelValues = aModel->GetValues();
//      std::vector<double> aModelSecondValues = aModel->GetSecondValues();
//      aModel->SetColumnCount (theValue);
//      aModel->Init (aModelValues, aModelSecondValues);
//
//      myVisualizedSize->setText (QString ("Table size: [ %1, %2 ]").arg (aModel->rowCount()).arg (aModel->columnCount()));
//    }
//  }
//}
//
//// =======================================================================
//// function : onToleranceSizeChanged
//// purpose :
//// =======================================================================
//void ViewControl_Table::onToleranceSizeChanged (int theValue)
//{
//  if (dynamic_cast<MessageView_TableModelRealArray*>(myTableView->model()))
//  {
//    MessageView_TableModelRealArray* aModel = dynamic_cast<MessageView_TableModelRealArray*>(myTableView->model());
//    std::vector<double> aModelValues = aModel->GetValues();
//    aModel->SetTolerance (theValue);
//    aModel->Init(aModelValues);
//  }
//  else {
//    if (dynamic_cast<MessageView_TableModelRealArrayCompare*>(myTableView->model()))
//    {
//      MessageView_TableModelRealArrayCompare* aModel = dynamic_cast<MessageView_TableModelRealArrayCompare*>(myTableView->model());
//      std::vector<double> aModelValues = aModel->GetValues();
//      std::vector<double> aModelSecondValues = aModel->GetSecondValues();
//      aModel->SetTolerance (theValue);
//      aModel->Init(aModelValues, aModelSecondValues);
//    }
//  }
//}
//
//#include <AIS_Shape.hxx>
//#include <BRep_Builder.hxx>
//#include <BRepBuilderAPI_MakeEdge.hxx>
//#include <gp_Pnt.hxx>
//
//#include <Precision.hxx>
//#include <TopoDS_Edge.hxx>
//#include <TopoDS_Shape.hxx>
//#include <TopoDS_Compound.hxx>
//#include <Quantity_NameOfColor.hxx>
//Quantity_Color getNextColor(int& theIndex)
//{
//  //if (theIndex >= 3)
//  //  theIndex = 0;
//
//  if (theIndex == 0)
//  {
//    theIndex++;
//    return Quantity_Color(Quantity_NameOfColor::Quantity_NOC_RED);
//  }
//  /*if (theIndex == 1)
//  {
//    theIndex++;
//    return Quantity_Color(Quantity_NameOfColor::Quantity_NOC_WHITE);
//  }
//  if (theIndex == 2)
//  {
//    theIndex++;
//    return Quantity_Color(Quantity_NameOfColor::Quantity_NOC_GREEN);
//  }*/
//
//  int aNextIndex = (Quantity_NameOfColor)Quantity_NOC_MATRABLUE + theIndex;
//  if (aNextIndex >= Quantity_NOC_WHITE)
//  {
//    theIndex = 0;
//    aNextIndex = (Quantity_NameOfColor)Quantity_NOC_MATRABLUE + theIndex;
//  }
//  Quantity_NameOfColor aNameOfColor = (Quantity_NameOfColor)aNextIndex;
//  theIndex = theIndex + 7;
//  return Quantity_Color (aNameOfColor);
//}
//
//// =======================================================================
//// function : onSeparateSizeChanged
//// purpose :
//// =======================================================================
//void ViewControl_Table::onTableSelectionChanged(const QItemSelection&, const QItemSelection&)
//{
//  QModelIndexList aSelected = myTableView->selectionModel()->selectedIndexes();
//  if (aSelected.isEmpty())
//  {
//    myMinSelectedValue->setText(QString("Min of selected:") + QString::number(0, 'g', REAL_SIGNS));
//    myMaxSelectedValue->setText(QString("Max of selected:") + QString::number(0, 'g', REAL_SIGNS));
//    return;
//  }
//
//  // display selected path
//  std::vector<double> aSelectedValues;
//  QAbstractItemModel* aModel = myTableView->model();
//  if (dynamic_cast<MessageView_TableModelRealArray*>(aModel))
//  {
//    MessageView_TableModelRealArray* anArrayModel = dynamic_cast<MessageView_TableModelRealArray*>(aModel);
//    anArrayModel->GetValues(aSelected, aSelectedValues);
//  }
//
//  if (!aSelectedValues.empty())
//  {
//    BRep_Builder aBuilder;
//    TopoDS_Compound aCompound;
//    aBuilder.MakeCompound(aCompound);
//
//    gp_Pnt aPreviousPoint(0, 0, 0), aCurrentPoint(0, 0, 0);
//    int aCurrentIndex = 0;
//    bool aMakeBreak = false;
//    for (std::vector<double>::const_iterator anIt = aSelectedValues.begin(); anIt != aSelectedValues.end(); anIt++, aCurrentIndex++)
//    {
//      if (*anIt == std::numeric_limits<double>::quiet_NaN() ||
//          *anIt == std::numeric_limits<double>::infinity()) {
//        aMakeBreak = true;
//        continue;
//      }
//
//      aCurrentPoint.SetX (aCurrentIndex);
//      aCurrentPoint.SetY (*anIt);
//      if (!aMakeBreak)
//      {
//        if (aPreviousPoint.Distance(aCurrentPoint) < Precision::Confusion())
//        {
//          continue;
//        }
//        TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aPreviousPoint, aCurrentPoint);
//        aBuilder.Add(aCompound, anEdge);
//      }
//      else
//        aMakeBreak = false;
//      aPreviousPoint = aCurrentPoint;
//    }
//    Handle(AIS_Shape) aPresentation = new AIS_Shape(aCompound);
//    if (myDisplayer->GetDisplayed().Extent() == 0)
//      myColorIndex = 0;
//
//    aPresentation->SetColor(getNextColor(myColorIndex));
//    myDisplayer->DisplayPresentation(aPresentation);
//    //TODO myDisplayer->SetViewProjection(0, 0, 1);
//  }
//
//  // update min/max values
//  double aMinValue = DBL_MAX, aMaxValue = DBL_MIN;
//  //std::vector<double> aSelectedValues;
//  if (dynamic_cast<MessageView_TableModelRealArray*>(aModel))
//  {
//    MessageView_TableModelRealArray* anArrayModel = dynamic_cast<MessageView_TableModelRealArray*>(aModel);
//    anArrayModel->GetRangeValues(aMinValue, aMaxValue, aSelected);
//    //anArrayModel->GetValues(aSelected, aSelectedValues);
//  }
//  else if (dynamic_cast<MessageView_TableModelRealArrayCompare*>(aModel))
//  {
//    MessageView_TableModelRealArrayCompare* anArrayModel = dynamic_cast<MessageView_TableModelRealArrayCompare*>(aModel);
//    anArrayModel->GetRangeValues(aMinValue, aMaxValue, aSelected);
//    //anArrayModel->GetValues(aSelected, aSelectedValues);
//  }
//  myMinSelectedValue->setText(QString("Min of selected:") + QString::number (aMinValue, 'g', REAL_SIGNS));
//  myMaxSelectedValue->setText(QString("Max of selected:") + QString::number (aMaxValue, 'g', REAL_SIGNS));
//}
//
//// =======================================================================
//// function : createAction
//// purpose :
//// =======================================================================
//QAction* ViewControl_Table::createAction(const QString& theText, const char* theSlot)
//{
//  QAction* anAction = new QAction (theText, this);
//  connect (anAction, SIGNAL (triggered (bool)), this, theSlot);
//  return anAction;
//}
