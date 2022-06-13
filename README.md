# PhysFileDicom
Encodes text files with carriage return separated values into dicom private tags.

PhysFileDicom.exe
Includes configuration to build for Mac or Linux.
Encodes carriage return delimited files passed to the utility into a dicom file's private tags using the passed dicom as a template.
The motivating case for this utility was to encode physiological data acquired on a GE scanner, though nothing prevents compiling and
using this utility on other scanner makes or for other types of data. It is recommended that the dicoms that were acquired during
collection of the physiological data be used as a template, though, again, there is nothing to stop use in other situations.
Usage:
PhysFileDicom.exe -h
PhysFileDicom.exe -d /dicom/dir/ -t /text/files/to/encode...
   -t can be formatted in numerous ways
   -t /text/files/to/encode*
   -t /text/file/to/encode.txt -t /text/file/to/encode2.txt
   -t /text/file/to/encode.txt /text/file/to/encode2.txt /text/file/to/encode3.txt

PhysioRecovery.py
Python utility that re-encodes the physiological data from the dicom back into carriage return delimited text files.
Usage:
PhysioRecovery.py -f /dicom/with/encoding.dcm

Notes:
The following items are currently hardcoded, but could benefit from being migrated to inputs with defaults.
Dicom group is currently always 0x33. The utility will look for an open block and reserve it per the dicom standards.
The delimiting is currently set to carriage return. 

Dependencies:
DCMTK
Boost
