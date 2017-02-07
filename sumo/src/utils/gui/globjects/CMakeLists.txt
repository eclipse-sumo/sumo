message("entering globjects")

set(utguiglobjects_files
	GLIncludes.h
	GUIGLObjectPopupMenu.cpp GUIGLObjectPopupMenu.h
	GUIGlObject.cpp GUIGlObject.h
	GUIGlObjectStorage.cpp GUIGlObjectStorage.h
	GUIGlObjectTypes.h
	GUIGlObject_AbstractAdd.cpp GUIGlObject_AbstractAdd.h
	GUIShapeContainer.cpp GUIShapeContainer.h
	GUIPointOfInterest.cpp GUIPointOfInterest.h
	GUIPolygon.cpp GUIPolygon.h)

add_library(utguiglobjects STATIC ${utguiglobjects_files})
