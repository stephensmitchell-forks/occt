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

#include <inspector/MessageView_Window.hxx>
#include <inspector/MessageView_VisibilityState.hxx>

#include <inspector/MessageModel_Actions.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_TreeModel.hxx>

#include <inspector/TreeModel_ContextMenu.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <OSD_Environment.hxx>

#include <OSD_Directory.hxx>
#include <OSD_Protection.hxx>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_AlertWithShape.hxx>

#include <XmlDrivers_MessageReportStorage.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_ToolBar.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>
#include <inspector/View_Viewer.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <QApplication>
#include <QAction>
#include <QComboBox>
#include <QDockWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <QVBoxLayout>

const int DEFAULT_TEXT_VIEW_WIDTH = 400;// 800;
const int DEFAULT_TEXT_VIEW_HEIGHT = 700;
const int DEFAULT_TEXT_VIEW_POSITION_X = 430;
const int DEFAULT_TEXT_VIEW_POSITION_Y = 30;
const int DEFAULT_TEXT_VIEW_DELTA = 100;

const int DEFAULT_SHAPE_VIEW_WIDTH = 400;// 900;
const int DEFAULT_SHAPE_VIEW_HEIGHT = 450;
const int DEFAULT_SHAPE_VIEW_POSITION_X = 60;
const int DEFAULT_SHAPE_VIEW_POSITION_Y = 60;

const int DEFAULT_DETACH_POSITION_X = 5;
const int DEFAULT_DETACH_POSITION_Y = 450;

const int MESSAGEVIEW_DEFAULT_TREE_VIEW_WIDTH = 950; //600
const int MESSAGEVIEW_DEFAULT_TREE_VIEW_HEIGHT = 500;

const int MESSAGEVIEW_DEFAULT_VIEW_WIDTH = 200;// 400;
const int MESSAGEVIEW_DEFAULT_VIEW_HEIGHT = 300;// 1000;

//! \class MessageView_TreeView
//! Extended tree view control with possibility to set predefined size.
class MessageView_TreeView : public QTreeView
{
public:
  //! Constructor
  MessageView_TreeView (QWidget* theParent) : QTreeView (theParent), myDefaultWidth (-1), myDefaultHeight (-1) {}

  //! Destructor
  virtual ~MessageView_TreeView() {}

  //! Sets default size of control, that is used by the first control show
  //! \param theDefaultWidth the width value
  //! \param theDefaultHeight the height value
  void SetPredefinedSize (int theDefaultWidth, int theDefaultHeight);

  //! Returns predefined size if both values are positive, otherwise parent size hint
  virtual QSize sizeHint() const Standard_OVERRIDE;

private:

  int myDefaultWidth; //!< default width, -1 if it should not be used
  int myDefaultHeight; //!< default height, -1 if it should not be used
};

// =======================================================================
// function : SetPredefinedSize
// purpose :
// =======================================================================
void MessageView_TreeView::SetPredefinedSize (int theDefaultWidth, int theDefaultHeight)
{
  myDefaultWidth = theDefaultWidth;
  myDefaultHeight = theDefaultHeight;
}

// =======================================================================
// function : sizeHint
// purpose :
// =======================================================================
QSize MessageView_TreeView::sizeHint() const
{
  if (myDefaultWidth > 0 && myDefaultHeight > 0)
    return QSize (myDefaultWidth, myDefaultHeight);
  return QTreeView::sizeHint();
}

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
MessageView_Window::MessageView_Window (QWidget* theParent)
: QObject (theParent)
{
  myMainWindow = new QMainWindow (theParent);

  myTreeView = new MessageView_TreeView (myMainWindow);
  ((MessageView_TreeView*)myTreeView)->SetPredefinedSize (MESSAGEVIEW_DEFAULT_TREE_VIEW_WIDTH,
                                                          MESSAGEVIEW_DEFAULT_TREE_VIEW_HEIGHT);
  MessageModel_TreeModel* aModel = new MessageModel_TreeModel (myTreeView);
  for (int i = 6; i <= 8; i++) // hide shape parameters columns
  {
    TreeModel_HeaderSection anItem = aModel->GetHeaderItem (i);
    anItem.SetIsHidden (true);
    aModel->SetHeaderItem (i, anItem);
  }

  myTreeView->setModel (aModel);
  MessageView_VisibilityState* aVisibilityState = new MessageView_VisibilityState (aModel);
  aModel->SetVisibilityState (aVisibilityState);
  TreeModel_Tools::UseVisibilityColumn (myTreeView);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aModel);
  myTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  myTreeView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  myTreeViewActions = new MessageModel_Actions (myMainWindow, aModel, aSelectionModel);
  myTreeView->setContextMenuPolicy (Qt::CustomContextMenu);
  connect (myTreeView, SIGNAL (customContextMenuRequested (const QPoint&)),
          this, SLOT (onTreeViewContextMenuRequested (const QPoint&)));
  new TreeModel_ContextMenu (myTreeView);

  QModelIndex aParentIndex = myTreeView->model()->index (0, 0);
  myTreeView->setExpanded (aParentIndex, true);

  myMainWindow->setCentralWidget (myTreeView);

  // view
  myViewWindow = new View_Window (myMainWindow, false);
  aVisibilityState->SetDisplayer (myViewWindow->GetDisplayer());
  aVisibilityState->SetPresentationType (View_PresentationType_Main);
  myViewWindow->GetView()->SetPredefinedSize (MESSAGEVIEW_DEFAULT_VIEW_WIDTH, MESSAGEVIEW_DEFAULT_VIEW_HEIGHT);

  myViewDockWidget = new QDockWidget (tr ("View"), myMainWindow);
  myViewDockWidget->setObjectName (myViewDockWidget->windowTitle());
  myViewDockWidget->setWidget (myViewWindow);
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myViewDockWidget);

  myMainWindow->resize (DEFAULT_SHAPE_VIEW_WIDTH, DEFAULT_SHAPE_VIEW_HEIGHT);
  myMainWindow->move (DEFAULT_SHAPE_VIEW_POSITION_X, DEFAULT_SHAPE_VIEW_POSITION_Y);
}

// =======================================================================
// function : SetParent
// purpose :
// =======================================================================
void MessageView_Window::SetParent (void* theParent)
{
  QWidget* aParent = (QWidget*)theParent;
  if (aParent)
  {
    QLayout* aLayout = aParent->layout();
    if (aLayout)
      aLayout->addWidget (GetMainWindow());
  }
  else
  {
    GetMainWindow()->setParent (0);
    GetMainWindow()->setVisible (true);
  }
}

// =======================================================================
// function : FillActionsMenu
// purpose :
// =======================================================================
void MessageView_Window::FillActionsMenu (void* theMenu)
{
  QMenu* aMenu = (QMenu*)theMenu;
  QList<QDockWidget*> aDockwidgets = myMainWindow->findChildren<QDockWidget*>();
  for (QList<QDockWidget*>::iterator it = aDockwidgets.begin(); it != aDockwidgets.end(); ++it)
  {
    QDockWidget* aDockWidget = *it;
    if (aDockWidget->parentWidget() == myMainWindow)
      aMenu->addAction (aDockWidget->toggleViewAction());
  }
}

// =======================================================================
// function : GetPreferences
// purpose :
// =======================================================================
void MessageView_Window::GetPreferences (TInspectorAPI_PreferencesDataMap& theItem)
{
  theItem.Clear();
  theItem.Bind ("geometry",  TreeModel_Tools::ToString (myMainWindow->saveState()).toStdString().c_str());

  QMap<QString, QString> anItems;
  TreeModel_Tools::SaveState (myTreeView, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
}

// =======================================================================
// function : SetPreferences
// purpose :
// =======================================================================
void MessageView_Window::SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem)
{
  if (theItem.IsEmpty())
  {
    TreeModel_Tools::SetDefaultHeaderSections (myTreeView);
    return;
  }

  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (theItem); anItemIt.More(); anItemIt.Next())
  {
    if (anItemIt.Key().IsEqual ("geometry"))
      myMainWindow->restoreState (TreeModel_Tools::ToByteArray (anItemIt.Value().ToCString()));
    else if (TreeModel_Tools::RestoreState (myTreeView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
  }
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void MessageView_Window::UpdateContent()
{
  bool isUpdated = false;
  TCollection_AsciiString aName = "TKMessageView";
  if (myParameters->FindParameters (aName))
  {
    NCollection_List<Handle(Standard_Transient)> aParameters = myParameters->Parameters (aName);
    // Init will remove from parameters those, that are processed only one time (TShape)
    Init (aParameters);
    myParameters->SetParameters (aName, aParameters);
    isUpdated = true;
  }
  if (myParameters->FindFileNames (aName))
  {
    for (NCollection_List<TCollection_AsciiString>::Iterator aFilesIt (myParameters->FileNames (aName));
         aFilesIt.More(); aFilesIt.Next())
      openFile (aFilesIt.Value());

    NCollection_List<TCollection_AsciiString> aNames;
    myParameters->SetFileNames (aName, aNames);
    isUpdated = true;
  }
  updateTreeModel();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageView_Window::Init (NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  Handle(AIS_InteractiveContext) aContext;
  NCollection_List<Handle(Standard_Transient)> aParameters;
  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (theParameters);
       aParamsIt.More(); aParamsIt.Next())
  {
    Handle(Standard_Transient) anObject = aParamsIt.Value();
    Handle(Message_Report) aMessageReport = Handle(Message_Report)::DownCast (anObject);
    if (!aMessageReport.IsNull())
    {
      aMessageReport->SetCallBack (myParameters->GetCallBack());
      addReport (aMessageReport);
    }
    else
    {
      aParameters.Append (anObject);
      if (aContext.IsNull())
        aContext = Handle(AIS_InteractiveContext)::DownCast (anObject);
    }
  }
  QAbstractItemModel* aModel = myTreeView->model();
  if (!aModel)
    return;
  MessageModel_TreeModel* aTreeModel = dynamic_cast<MessageModel_TreeModel*> (aModel);
  if (!aTreeModel)
    return;
  aTreeModel->EmitLayoutChanged();

  if (!aContext.IsNull())
    myViewWindow->SetContext (View_ContextType_External, aContext);

  theParameters = aParameters;
}

// =======================================================================
// function : openFile
// purpose :
// =======================================================================
void MessageView_Window::openFile(const TCollection_AsciiString& theFileName)
{
  Handle(Message_Report) aReport = XmlDrivers_MessageReportStorage::ImportReport(theFileName);
  if (aReport.IsNull())
    return;

  TCollection_AsciiString aReportName = OSD_Path(theFileName).Name();
  addReport (aReport, aReportName);
}

// =======================================================================
// function : updateTreeModel
// purpose :
// =======================================================================
void MessageView_Window::updateTreeModel()
{
  MessageModel_TreeModel* aViewModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  aViewModel->UpdateTreeModel();
}

// =======================================================================
// function : addReport
// purpose :
// =======================================================================
void MessageView_Window::addReport (const Handle(Message_Report)& theReport,
                                    const TCollection_AsciiString& theReportDescription)
{
  MessageModel_TreeModel* aModel = dynamic_cast<MessageModel_TreeModel*> (myTreeView->model());
  aModel->AddReport (theReport, theReportDescription);
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void MessageView_Window::onTreeViewContextMenuRequested (const QPoint& thePosition)
{
  QMenu* aMenu = new QMenu (GetMainWindow());

  MessageModel_ItemRootPtr aRootItem;
  MessageModel_ItemReportPtr aReportItem;
  QModelIndexList aSelectedIndices = myTreeView->selectionModel()->selectedIndexes();

  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    aRootItem = itemDynamicCast<MessageModel_ItemRoot> (anItemBase);
    if (aRootItem)
      break;
    aReportItem = itemDynamicCast<MessageModel_ItemReport> (anItemBase);
    if (aReportItem)
      break;
  }
  if (aRootItem)
    aMenu->addAction (TreeModel_Tools::CreateAction (tr ("Import Report"), SLOT (onImportReport()), myMainWindow, this));
  else if (aReportItem)
    aMenu->addAction (TreeModel_Tools::CreateAction (tr ("Export Report"), SLOT (onExportReport()), myMainWindow, this));

  aMenu->addSeparator();
  myTreeViewActions->AddMenuActions (aSelectedIndices, aMenu);

  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec (aPoint);
}

// =======================================================================
// function : onExportReport
// purpose :
// =======================================================================
void MessageView_Window::onExportReport()
{
  Handle(Message_Report) aReport;
  MessageModel_ItemReportPtr aReportItem;
  QModelIndexList aSelectedIndices = myTreeView->selectionModel()->selectedIndexes();
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    aReportItem = itemDynamicCast<MessageModel_ItemReport> (anItemBase);
    if (aReportItem)
      break;
  }
  if (!aReportItem)
    return;

  QString aFilter (tr ("Document file (*.xml *)"));
  QString aSelectedFilter;
  QString aFileName = QFileDialog::getSaveFileName (0, tr ("Export report to file"), QString(), aFilter, &aSelectedFilter);

  XmlDrivers_MessageReportStorage::ExportReport (aReportItem->GetReport(),
                                                 TCollection_AsciiString (aFileName.toStdString().c_str()));
}

// =======================================================================
// function : onImportReport
// purpose :
// =======================================================================
void MessageView_Window::onImportReport()
{
  QString aFilter (tr ("Document file (*.cbf *)"));
  QString aSelectedFilter;

  QString aFileName = QFileDialog::getOpenFileName (0, tr("Import report"), QString(), aSelectedFilter);
  openFile (TCollection_AsciiString (aFileName.toStdString().c_str()));
}
