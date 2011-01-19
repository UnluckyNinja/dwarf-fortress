include/hidden
============

This folder contains declarations that the library has to use for internal structures, but which definitions are compiled in dwarf fortress closed binary.
We cannot change anything to these.
Definitions of such declarations are in the src/hidden folder, which isn't build in the library, but which may be build for the closed binary.