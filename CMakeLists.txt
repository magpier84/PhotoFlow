cmake_minimum_required(VERSION 2.8.9)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

project(photoflow CXX C)

# options controlling the build process
option(BUNDLED_EXIV2 "Use bundled Exiv2 library" OFF)
option(OCIO_ENABLED "Enable the integration of OpenColorIO" OFF)
IF(MINGW)
  option(BUNDLED_LENSFUN "Use bundled LensFun library" OFF)
  option(USE_NLS "Build Native Language Support (using gettext)" ON)
ELSEIF(APPLE)
  option(BUNDLED_LENSFUN "Use bundled LensFun library" OFF)
  option(USE_NLS "Build Native Language Support (using gettext)" ON)
ELSE(MINGW)
  option(BUNDLED_GEXIV2 "Use bundled Gexiv2 library" OFF)
  option(BUNDLED_LENSFUN "Use bundled LensFun library" OFF)
  option(USE_NLS "Build Native Language Support (using gettext)" ON)
ENDIF(MINGW)
option(BABL_FLIPS_DISABLED "BABL flips disabled" OFF)
option(BUNDLED_LENSFUN_DB "Use bundled LensFun database" OFF)

IF(BUNDLED_LENSFUN)
  SET(BUNDLED_LENSFUN_DB ON CACHE BOOL "Use bundled LensFun database" FORCE)
ENDIF(BUNDLED_LENSFUN)

option(AUTOMATED_VERSION_CHECK "Update version info" ON)


if(NOT INSTALL_PREFIX)
	set(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
	MESSAGE( STATUS "Setting INSTALL_PREFIX from CMAKE_INSTALL_PREFIX" )
endif(NOT INSTALL_PREFIX)
MESSAGE( STATUS "INSTALL_PREFIX:         " ${INSTALL_PREFIX} )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_POSITION_INDEPENDENT_CODE True)

#set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 0)
#set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES 0)
#set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 0)
#set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES 0)


find_package(PkgConfig)

find_package (Pugixml REQUIRED)
MESSAGE( STATUS "Pugixml_INCLUDE_DIRS:         " ${Pugixml_INCLUDE_DIRS} )
MESSAGE( STATUS "Pugixml_LIBRARIES:            " ${Pugixml_LIBRARIES} )

find_package (EXPAT)
MESSAGE( STATUS "EXPAT_INCLUDE_DIRS:         " ${EXPAT_INCLUDE_DIRS} )
MESSAGE( STATUS "EXPAT_LIBRARIES:            " ${EXPAT_LIBRARIES} )

#IF(NOT MINGW)
  find_package (Iconv REQUIRED)
  MESSAGE( STATUS "ICONV_INCLUDE_DIRS:         " ${ICONV_INCLUDE_DIRS} )
  MESSAGE( STATUS "ICONV_LIBRARIES:            " ${ICONV_LIBRARIES} )
#ENDIF(NOT MINGW)

MESSAGE(STATUS "Check for LCMS2 availability...")
#FIND_PACKAGE(LCMS2)
pkg_check_modules(LCMS2 REQUIRED lcms2) 
MESSAGE( STATUS "LCMS2_INCLUDE_DIRS:         " ${LCMS2_INCLUDE_DIRS} )
MESSAGE( STATUS "LCMS2_LIBRARIES:            " ${LCMS2_LIBRARIES} )
MESSAGE( STATUS "LCMS2_LIBRARY_DIRS:         " ${LCMS2_LIBRARY_DIRS} )

find_package (TIFF REQUIRED)
MESSAGE( STATUS "TIFF_INCLUDE_DIRS:         " ${TIFF_INCLUDE_DIRS} )
MESSAGE( STATUS "TIFF_LIBRARIES:            " ${TIFF_LIBRARIES} )
MESSAGE( STATUS "TIFF_LIBRARY_DIRS:         " ${TIFF_LIBRARY_DIRS} )

find_package (PNG REQUIRED)

find_package (JPEG REQUIRED)
MESSAGE( STATUS "JPEG_INCLUDE_DIRS:         " ${JPEG_INCLUDE_DIRS} )
MESSAGE( STATUS "JPEG_LIBRARIES:            " ${JPEG_LIBRARIES} )
MESSAGE( STATUS "JPEG_LIBRARY_DIRS:         " ${JPEG_LIBRARY_DIRS} )

find_package (ZLIB REQUIRED)

#pkg_check_modules(FFTW3 REQUIRED fftw3>=3.0)
pkg_check_modules(FFTW3 REQUIRED fftw3)
pkg_check_modules(FFTW3F REQUIRED fftw3f)

pkg_check_modules(XML2 REQUIRED libxml-2.0)

pkg_check_modules(IPTCDATA REQUIRED libiptcdata)
pkg_check_modules(EXIF REQUIRED libexif)

if(NOT BUNDLED_EXIV2)
  pkg_check_modules(EXIV2 exiv2>=0.26)
  #find_package (Exiv2)
  MESSAGE( STATUS "EXIV2_FOUND:         " ${EXIV2_FOUND} )
  if(NOT EXIV2_FOUND)
    set( BUNDLED_EXIV2 ON CACHE BOOL "Use bundled Exiv2 library" FORCE )
  endif(NOT EXIV2_FOUND)
endif(NOT BUNDLED_EXIV2)

#if(NOT BUNDLED_GEXIV2)
#  pkg_check_modules(GEXIV2 REQUIRED gexiv2)
#  MESSAGE( STATUS "GEXIV2_INCLUDE_DIRS:         " ${GEXIV2_INCLUDE_DIRS} )
#  MESSAGE( STATUS "GEXIV2_LIBRARY_DIRS:         " ${GEXIV2_LIBRARY_DIRS} )
#endif(NOT BUNDLED_GEXIV2)

if(NOT BUNDLED_LENSFUN)
  pkg_check_modules(LENSFUN REQUIRED lensfun)
  MESSAGE( STATUS "LENSFUN_INCLUDE_DIRS:         " ${LENSFUN_INCLUDE_DIRS} )
  MESSAGE( STATUS "LENSFUN_LIBRARY_DIRS:         " ${LENSFUN_LIBRARY_DIRS} )
  MESSAGE( STATUS "LENSFUN_LIBRARIES:         " ${LENSFUN_LIBRARIES})
endif(NOT BUNDLED_LENSFUN)


#IF(NOT MINGW)
pkg_check_modules(OPENEXR OpenEXR)
pkg_check_modules(ORC orc-0.4)
#ENDIF(NOT MINGW)

#FIND_PACKAGE(LibRaw 0.13 REQUIRED)
#pkg_check_modules(LIBRAW REQUIRED libraw)

#find_package (LibXML++ REQUIRED)
#pkg_check_modules(LIBXML REQUIRED libxml++-2.6) 

pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)

pkg_check_modules(PIXMAN pixman-1 REQUIRED) 
pkg_check_modules(PANGO pango) 
pkg_check_modules(PANGOFT2 pangoft2) 
pkg_check_modules(GLIB glib-2.0) 
pkg_check_modules(GMODULE gmodule-2.0) 
pkg_check_modules(GTHREAD gthread-2.0) 
pkg_check_modules(GOBJECT gobject-2.0) 
pkg_check_modules(GLIBMM glibmm-2.4) 
#pkg_check_modules(GTKMM3 gtkmm-3.0<=3.4.0) 
MESSAGE( STATUS "GLIB_INCLUDE_DIRS:         " ${GLIB_INCLUDE_DIRS} )
MESSAGE( STATUS "GLIB_LIBRARY_DIRS:         " ${GLIB_LIBRARY_DIRS} )

pkg_check_modules(VIPS REQUIRED vips>=8.5.1)
#pkg_check_modules(VIPSCC REQUIRED vipsCC)
MESSAGE( STATUS "VIPS_INCLUDE_DIRS:         " ${VIPS_INCLUDE_DIRS} )
MESSAGE( STATUS "VIPS_LIBRARY_DIRS:         " ${VIPS_LIBRARY_DIRS} )

if (OCIO_ENABLED)
find_package (OpenColorIO)
MESSAGE( STATUS "OpenColorIO_LIBRARY_DIRS: " ${OpenColorIO_LIBRARY_DIRS} )
MESSAGE( STATUS "OpenColorIO_LIBRARIES: " ${OpenColorIO_LIBRARIES} )
MESSAGE( STATUS "OpenColorIO_LIBRARY: " ${OpenColorIO_LIBRARY} )
MESSAGE( STATUS "OCIO_FOUND: " ${OCIO_FOUND} )
MESSAGE( STATUS "OCIO_LIBRARY_DIRS: " ${OCIO_LIBRARY_DIRS} )
MESSAGE( STATUS "OCIO_LIBRARIES: " ${OCIO_LIBRARIES} )
MESSAGE( STATUS "OCIO_LIBRARY: " ${OCIO_LIBRARY} )
endif(OCIO_ENABLED)

if (APPLE)
    #find_package (MacIntegration REQUIRED)
    pkg_check_modules(MacIntegration REQUIRED gtk-mac-integration-gtk2)
endif (APPLE)


if(USE_GTKMM3)
  pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0) 
else(USE_GTKMM3)
  pkg_check_modules(GTKMM2 REQUIRED gtkmm-2.4) 
endif(USE_GTKMM3)

MESSAGE( STATUS "GTKMM2_FOUND:         " ${GTKMM2_FOUND} )
MESSAGE( STATUS "GTKMM3_FOUND:         " ${GTKMM3_FOUND} )

  
IF(NOT MINGW)
#need to put here do setting LOCALE_DIR variable
if(USE_NLS)
find_package(Gettext)
if(Gettext_FOUND)
	if(NOT LOCALE_DIR)
    SET(LOCALE_DIR "${CMAKE_INSTALL_PREFIX}/share/locale")
	endif(NOT LOCALE_DIR)

	if(NOT LOCALE_DIR)
		message(QUIET "Set LOCALE_DIR to path to get Gettext working")
	else()
		find_package(Msgfmt)
		if(Msgfmt_FOUND)
			message(STATUS "Found msgfmt to convert language file. Translation enabled")
			add_subdirectory(po)
		else()
			message(STATUS "Cannot find msgfmt to convert language file. Translation won't be enabled")
		endif()
	endif(NOT LOCALE_DIR)
endif(Gettext_FOUND)
endif(USE_NLS)
ENDIF(NOT MINGW)
MESSAGE( STATUS "Msgfmt_FOUND:         " ${Msgfmt_FOUND} )


set(THEME hicolor)
set(THEME_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/icons/${THEME})

add_subdirectory(src) 
add_subdirectory(data)
