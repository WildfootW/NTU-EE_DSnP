cmdCommon.o: cmdCommon.cpp ../../include/util.h cmdCommon.h cmdParser.h \
 cmdCharDef.h
cmdCharDef.o: cmdCharDef.cpp cmdParser.h cmdCharDef.h
cmdParser.o: cmdParser.cpp ../../include/util.h cmdParser.h cmdCharDef.h
