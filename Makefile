CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
SRCDIR = backend
CGIDIR = cgi-bin

# Source files
SOURCES = $(SRCDIR)/attendance_system.cpp
LOGIN_SRC = $(SRCDIR)/login.cpp
MARK_SRC = $(SRCDIR)/mark_attendance.cpp
GET_SRC = $(SRCDIR)/get_attendance.cpp

# Target executables
LOGIN_TARGET = $(CGIDIR)/login.cgi
MARK_TARGET = $(CGIDIR)/mark_attendance.cgi
GET_TARGET = $(CGIDIR)/get_attendance.cgi

all: $(LOGIN_TARGET) $(MARK_TARGET) $(GET_TARGET)

$(LOGIN_TARGET): $(LOGIN_SRC) $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(MARK_TARGET): $(MARK_SRC) $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GET_TARGET): $(GET_SRC) $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(CGIDIR)/*.cgi

install: all
	chmod +x $(CGIDIR)/*.cgi

.PHONY: all clean install