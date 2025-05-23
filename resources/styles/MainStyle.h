#pragma once
#include <QString>

inline QString MinimizeButtonStyle = "QPushButton {"
    "   background-color: transparent;"
    "   color: white;"
    "   font-size: 24px;"
    "font-weight: bold;"
    "   border: none;"
    "letter-spacing: -5px;"
    "}"
    "QPushButton:hover {"
    "   background-color: #4C566A;"
    "}";

inline QString CloseButtonStyle = "QPushButton {"
    "   background-color: transparent;"
    "   color: white;"
    "   font-size: 24px;"
    "font-weight: bold;"
    "   border: none;"
    "}"
    "QPushButton:hover {"
    "   background-color: #BF616A;"
    "}";

inline QString ProjectListStyle = "QListWidget {"
    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0.89, stop:0 #585266, stop:1 #4A4459);"
    "   border: 0px;"
    "   color: #FFFFFF;"
    "   font-size: 16px;"
    "}"
    "QListWidget::item {"
    "   text-align: center;"
    "   padding: 5px;"
    "}"
    "QListWidget::item:hover {"
    "   background-color: #4C566A;"
    "}"
    "QListWidget::item:selected {"
    "   background-color: #5E81AC;"
    "   color: #ECEFF4;"
    "}";

inline QString CreateButtonStyle = "text-align: left;"
    "font-size:20px;"
    "color: white;"
    "font-weight: bold;"
    "text-decoration: underline;"
    "border: 0px;";

inline QString LanguageComboStyle = "QComboBox {"
    "   background-color: #4A4458;"
    "   color: #ECEFF4;"
    "   font-size: 14px;"
    "   padding: 5px;"
    "   border: 0px;"
    "   border-radius: 10px;"
    "}"
    "QComboBox::drop-down {"
    "   subcontrol-origin: padding;"
    "   subcontrol-position: top right;"
    "   width: 15px;"
    "   border-left-width: 1px;"
    "   border-left-color: #4A4458;"
    "   border-left-style: solid;"
    "   padding-right: 10px;"
    "}"
    "QComboBox::down-arrow {"
    "   image: url(../resources/icons/down-arrow.png);"
    "}"
    "QComboBox QAbstractItemView {"
    "   background-color: #4A4458;"
    "   color: #ECEFF4;"
    "}";
