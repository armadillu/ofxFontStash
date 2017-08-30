# All variables and this file are optional, if they are not present the PG and the
# makefiles will try to parse the correct values from the file system.
#
# Variables that specify exclusions can use % as a wildcard to specify that anything in
# that position will match. A partial path can also be specified to, for example, exclude
# a whole folder from the parsed paths from the file system
#
# Variables can be specified using = or +=
# = will clear the contents of that variable both specified from the file or the ones parsed
# from the file system
# += will add the values to the previous ones in the file or the ones parsed from the file 
# system
# 
# The PG can be used to detect errors in this file, just create a new project with this addon 
# and the PG will write to the console the kind of error and in which line it is

meta:
	ADDON_NAME = ofxFontStash
	ADDON_DESCRIPTION = Easy (and fast) unicode string rendering addon for OpenFrameworks. FontStash is made by Andreas Krinke and Mikko Mononen
	ADDON_AUTHOR = armadillu
	ADDON_TAGS = "Font" "Typography" "Unicode" "fontstash"
	ADDON_URL = https://github.com/armadillu/ofxFontStash

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	# ADDON_DEPENDENCIES =
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	  ADDON_INCLUDES += libs/fontstash/src
	
	# any special flag that should be passed to the compiler when using this
	# addon 
	#  ADDON_CFLAGS
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	# ADDON_LDFLAGS =
	
	# linux only, any library that should be included in the project using
	# pkg-config
	# ADDON_PKG_CONFIG_LIBRARIES =
	
	# osx/iOS only, any framework that should be included in the project
	# ADDON_FRAMEWORKS =
	
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here
	# ADDON_SOURCES =
	
	# some addons need resources to be copied to the bin/data folder of the project
	# specify here any files that need to be copied, you can use wildcards like * and ?
	# ADDON_DATA = 
	
	# when parsing the file system looking for libraries exclude this for all or
	# a specific platform
	# ADDON_LIBS_EXCLUDE =
	
	ADDON_DEFINES = USE_OFX_FONTSTASH
	
	linuxarmv6l: 
	
	# I tried everything that came to mind, but I cant get the .c files in this addon to compile on the rPi.
	# As a workardound, rename /libs/fontstash/src/fontstash.c and /libs/fontstash/src/stb_truetype.c to .cpp.
	
	# TODO the "$(addon)" is hacky as I peek through the config.addons.mk to find it and might change at any time
	# the intention here is to get the makefile to compile the .c file (seems to only handle .cpp)
	#  ADDON_CFLAGS = -x c $(addon)/libs/fontstash/src/fontstash.c -x c $(addon)/libs/fontstash/src/stb_truetype.c

	# ADDON_SOURCES = $(addon)/libs/fontstash/src/fontstash.c $(addon)/libs/fontstash/src/stb_truetype.c
	