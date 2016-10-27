Visualization Attributes {#user_guides__ocaf_visu_attribs}
========================

Standard visualization attributes implement the Application Interactive Services (see @ref user_guides__visualization
 chapter). in the context of Open CASCADE Technology Application Framework. Standard visualization attributes are
 AISViewer and Presentation and belong to the TPrsStd package. 

# Services

## Defining an interactive viewer attribute

The class *TPrsStd_AISViewer* allows you to define an interactive viewer attribute. There may be only one such attribute 
per one data framework and it is always placed to the root label. So, it could be set or found by any label
 ("access label") of the data framework. Nevertheless the default architecture can be easily extended and the user can
 manage several Viewers per one framework by himself. 

To initialize the AIS viewer as in the example below, use method *Find*. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
// "access" is any label of the data framework 
Handle(TPrsStd_AISViewer) viewer = TPrsStd_AISViewer::Find(access) 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Defining a presentation attribute

The class *TPrsStd_AISPresentation* allows you to define the visual presentation of document labels contents. 
In addition to various visual fields (color, material, transparency, *isDisplayed*, etc.), this attribute contains its
 driver GUID. This GUID defines the functionality, which will update the presentation every time when needed. 

## Creating your own driver

The abstract class TPrsStd_Driver allows you to define your own driver classes. Simply redefine the Update method in
 your new class, which will rebuild the presentation. 

If your driver is placed to the driver table with the unique driver GUID, then every time the viewer updates 
presentations with a GUID identical to your driver’s GUID, the *Update* method of your driver for these presentations 
must be called: 
@image html /03_user_guides/ocaf/images/ocaf_image016.png
@image latex /03_user_guides/ocaf/images/ocaf_image016.png

As usual, the GUID of a driver and the GUID of a displayed attribute are the same. 

## Using a container for drivers

You frequently need a container for different presentation drivers. The class *TPrsStd_DriverTable* provides this 
service. You can add a driver to the table, see if one is successfully added, and fill it with standard drivers. 

To fill a driver table with standard drivers, first initialize the AIS viewer as in the example above, and then pass the
 return value of the method *InitStandardDrivers* to the driver table returned by the method *Get*. Then attach a
 *TNaming_NamedShape* to a label and set the named shape in the presentation attribute using the method *Set*. Then 
attach the presentation attribute to the named shape attribute, and the *AIS_InteractiveObject*, which the presentation
 attribute contains, will initialize its drivers for the named shape. This can be seen in the example below. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
DriverTable::Get() -> InitStandardDrivers(); 
// next, attach your named shape to a label 
TPrsStd_AISPresentation::Set(NS}; 
// here, attach the AISPresentation to NS. 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
