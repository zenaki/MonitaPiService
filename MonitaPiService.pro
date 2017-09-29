QT += core network sql
QT -= gui

CONFIG += c++11

TARGET = MonitaPiService
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    worker.cpp

HEADERS += \
    worker.h
