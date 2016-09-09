INCLUDEPATH  += ../../browser

MOC_DIR       = .moc
OBJECTS_DIR   = .obj

*-g++* {
    QMAKE_CXXFLAGS += -std=c++11
}
