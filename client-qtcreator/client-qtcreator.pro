SOURCES += \
    client.cpp \
    my_message.pb.cc \
    client-protobuf.cpp

HEADERS += \
    EasyTcpClient.hpp \
    MessageHeader.hpp \
    my_message.pb.h


LIBS += -lprotobuf

 QMAKE_CXXFLAGS += -std=c++11
