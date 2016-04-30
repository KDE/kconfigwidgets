#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys

sys.path.append(sys.argv[1])

from PyQt5 import QtCore
from PyQt5 import QtWidgets

from PyKF5 import KConfigWidgets

def main():
    app = QtWidgets.QApplication(sys.argv)

    rfa = KConfigWidgets.KRecentFilesAction(None)

    rfa.addUrl(QtCore.QUrl("http://kde.org"))
    rfa.addUrl(QtCore.QUrl("http://gnu.org"))

    assert(len(rfa.urls()) == 2)
    assert(QtCore.QUrl("http://kde.org") in rfa.urls())
    assert(QtCore.QUrl("http://gnu.org") in rfa.urls())

if __name__ == '__main__':
    sys.exit(main())
