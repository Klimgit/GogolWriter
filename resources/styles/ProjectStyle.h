#pragma once

#include <QString>


inline QString TreeViewStyle = "QTreeView {"
    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0.89, stop:0 #585266, stop:1 #4A4459);"
    "   color: #ECEFF4;"
    "   padding: 5px;"
    " border: 0px;"
    "}"
    "QTreeView::item {"
    "   padding: 5px;"
    "}"
    "QTreeView::item:hover {"
    "   background: #5E81AC;"
    "}"
    "QTreeView::item:selected {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "}";

inline QString SidePanelStyle = "QWidget {"
    "   background-color: #625B71;"
    "   border: 0px;"
    "   padding: 5px 0px 5px 0px;"
    "}"

    "QToolButton {"
    "   background-color: transparent;"
    "   border-radius: 3px;"
    "   padding: 5px;"
    "   margin: 10px 0px 2px 0px;"
    "}"

    "QToolButton:hover {"
    "   background-color: #5E81AC;"
    "   border-radius: 3px;"
    "}"

    "QToolButton:pressed {"
    "   background-color: #3B4252;"
    "   border: 1px solid #5E81AC;"
    "}"

    "QToolButton:checked {"
    "   background-color: #5E81AC;"
    "   border: 1px solid #5E81AC;"
    "}";

inline QString ButtonContainerStyle = "QWidget {"
    "   background: #585266;"
    "   padding: 5px;"
    "}";

inline QString ToolButtonStyle =
    "QToolButton {"
    "   background: transparent;"
    "   border: none;"
    "   padding: 3px;"
    "}"
    "QToolButton:hover {"
    "   background-color: #5E81AC;"
    "   border-radius: 3px;"
    "}";

inline QString TabWidgetStyle =
    "QTabWidget::pane {"
    "   border: none;"
    "   background: #1D1B20;"
    "}"
    "QTabBar {"
    "   background: #1D1B20;"
    "}"

    "QTabBar::tab {"
    "   background: #4A4458;"
    "   color: #D8DEE9;"
    "   padding: 4px 6px;"
    "   border: 0px;"
    "   border-bottom-left-radius: 10px;"
    "   border-bottom-right-radius: 10px;"
    "   min-width: 100px;"
    "}"

    "QTabBar::tab:selected {"
    "   background: #625B71;"
    "   color: #ECEFF4;"
    "   font-weight: bold;"
    "}"

    "QTabBar::tab:!selected {"
    "   background: #4A4458;"
    "   color: #D8DEE9;"
    "   border-bottom: 2px solid #4A4458;"
    "}"

    "QTabBar::tab:hover {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "}"

    "QTabBar::close-button {"
    "   subcontrol-position: right;"
    "   image: url(../resources/icons/close.png);"
    "   width: 8px;"
    "   height: 8px;"
    "}"

    "QTabBar::close-button:hover {"
    "   background: #BF616A;"
    "   opacity: 0.5;"
    "}"

    "QTabBar::tab:last {"
    "   margin-right: 5px;"
    "}";

inline QString EditorContextMenuStyle = "QMenu {"
    "   background-color: #4A4458;"
    "   color: #ECEFF4;"
    "   border: 1px solid #5E81AC;"
    "}"
    "QMenu::item:selected {"
    "   background-color: #5E81AC;"
    "}"
    "QMenu::separator {"
    "   height: 1px;"
    "   background: #5E81AC;"
    "   margin: 5px 0;"
    "}";

inline QString CalendarStyle = "QCalendarWidget QWidget#qt_calendar_navigationbar {"
    "   background: #625B71;" // Цвет фона шапки
    "   color: #ECEFF4;"
    "}"

    "QCalendarWidget {"
    "   background: #1D1B20;"
    "   color: #ECEFF4;"
    "   border: 1px solid #5E81AC;"
    "   font-size: 12px;"
    "   font-weight:bold;"
    "}"
    // дни недели
    "QCalendarWidget QWidget {"
    "   alternate-background-color: #4A4458;"
    "   font-size: 12px"
    "}"
    "QCalendarWidget QToolButton {"
    "   color: #ECEFF4;"
    "   background: transparent;"
    "   border: none;"
    "   font-size: 12px;"
    "   icon-size: 16px;"
    "   padding: 5px;"
    "}"
    "QCalendarWidget QToolButton:hover {"
    "   background: #81A1C1;"
    "}"

    "QCalendarWidget QToolButton::menu-indicator {"
    "   image: none;"
    "}"
    // кнопки дедлайнов
    "QCalendarWidget QMenu {"
    "   background: #4A4458;"
    "   color: #ECEFF4;"
    "}"
    // год
    "QCalendarWidget QSpinBox {"
    "   background: #4A4458;"
    "   color: #ECEFF4;"
    "   border: 1px solid #5E81AC;"
    "   padding: 2px;"
    "   font-size: 12px;"
    "}"
    "QCalendarWidget QAbstractItemView:enabled {"
    "   color: #ECEFF4;"
    "   background: #4A4458;"
    "   selection-background-color: #4A4458;"
    "   selection-color: #ECEFF4;"
    "   font-size: 12px;"
    "}"
    "QCalendarWidget QAbstractItemView:disabled {"
    "   color: #4C566A;"
    "}";
inline QString CalendarTabStyle = "QWidget {"
    "   background: #1D1B20;"
    "   color: #ECEFF4;"
    "}";

inline QString CalendarPrevBtnStyle = "QToolButton {"
    "   background: transparent;"
    "   border: none;"
    "   padding: 5px;"
    "}"
    "QToolButton:hover {"
    "   background: #81A1C1;"
    "}";

inline QString CalendarNavigationBtnStyle =
    "QToolButton {"
    "   background: transparent;"
    "   border: none;"
    "   padding: 5px;"
    "}"
    "QToolButton:hover {"
    "   background: #81A1C1;"
    "}";

inline QString DeadlinesDialogStyle = "QDialog {"
    "   background: #4A4458;"
    "   border: 1px solid #5E81AC;"
    " border-radius: 10px;"
    "   padding: 5px;"
    "}";

inline QString DeadlineListStyle = "QListWidget {"
    "   background: #3B4252;"
    "   color: #ECEFF4;"
    "   border: none;"
    "}"
    "QListWidget::item {"
    "   padding: 5px;"
    "}"
    "QListWidget::item:hover {"
    "   background: #434C5E;"
    "}";

inline QString AddDeadlineBtnStyle = "QPushButton {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "   border: none;"
    "   padding: 5px;"
    "   border-radius: 3px;"
    "}"
    "QPushButton:hover {"
    "   background: #81A1C1;"
    "}";

inline QString RemoveSelectedDeadlineBtnStyle = "QPushButton {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "   border: none;"
    "   padding: 5px;"
    "   border-radius: 3px;"
    "}"
    "QPushButton:hover {"
    "   background: #81A1C1;"
    "}";

inline QString CloseDeadlineListBtnStyle = "QPushButton {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "   border: none;"
    "   padding: 5px;"
    "   border-radius: 3px;"
    "}"
    "QPushButton:hover {"
    "   background: #81A1C1;"
    "}";

inline QString CharacterstabStyle = "QWidget {"
    "   background: #1D1B20;" // Основной фон
    "   color: #FFFFFF;" // Цвет текста
    "border: none;"
    "}";

inline QString CharactersButtonStyle =
    "QPushButton {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "   border: none;"
    "   padding: 5px 10px;"
    "   border-radius: 3px;"
    "   min-width: 80px;"
    "}"
    "QPushButton:hover {"
    "   background: #81A1C1;"
    "}"
    "QPushButton:disabled {"
    "   background: #4C566A;"
    "}";
inline QString CharactersImageBtnStyle = "QPushButton {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "   border: none;"
    "   padding: 5px;"
    "   border-radius: 3px;"
    "}"
    "QPushButton:hover {"
    "   background: #81A1C1;"
    "}";

inline QString CardWidgetStyle = "QWidget {"
    "   background: #79747E;"
    "   border-radius: 8px;"
    "   margin: 5px;"
    "   color: #FFFFFF;"
    "}"
    "QWidget:hover {"
    "}"
    "QWidget:selected {"
    "}";

inline QString DeskBtnStyle = "QPushButton {"
    "   background: #5E81AC;"
    "   color: #ECEFF4;"
    "   border: none;"
    "   padding: 5px 10px;"
    "   border-radius: 3px;"
    "}"
    "QPushButton:hover {"
    "   background: #81A1C1;"
    "}";
inline QString DeskEditStyle = "QTextEdit {"
    "   background: #4A4458;"
    "   color: #ECEFF4;"
    "   border-radius: 5px;"
    "}";

inline QString PlotContextMenuStyle = "QMenu {"
    "   background: #625B71;"
    "   color: #ECEFF4;"
    "   border: 1px solid #5E81AC;"
    "}"
    "QMenu::item:selected {"
    "   background: #5E81AC;"
    "}";
