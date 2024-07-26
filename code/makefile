CXX = g++
CFLAGS = -std=c++14 
TARGET = test
# OBJS = *.cpp


OBJS = log/*.cpp pool/*.cpp  \
       http/*.cpp  server/*.cpp timer/*.cpp\
       buffer/*.cpp main.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) -pthread -lmysqlclient

#怎么用浏览器访问呢    http://192.168.31.26:1316/index.html   