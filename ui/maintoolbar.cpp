#include "maintoolbar.h"
#include <QPushButton>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
#include <QToolButton>
#include <QIcon>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFont>
#include <QSizePolicy>

MainToolbar::MainToolbar(QWidget *parent) : QWidget{parent}
{
    QToolButton *button_new = new QToolButton();
    // QIcon icon_new = QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew);
    QIcon icon_new = QIcon::fromTheme("bqm-add");
    button_new->setIcon(icon_new);
    button_new->setText("Add");
    button_new->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QToolButton *button_hamburger = new QToolButton();
    QIcon icon_ham = QIcon::fromTheme("application-menu");
    button_hamburger->setIcon(icon_ham);

    QLabel *title = new QLabel();
    title->setText(" Mappings");
    QFont title_font = QFont();
    title_font.setPixelSize(18);
    title->setFont(title_font);

    QToolBar *toolbar = new QToolBar(this);
    toolbar->addWidget(title);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    toolbar->addWidget(spacer);
    toolbar->addWidget(button_new);
    toolbar->addWidget(button_hamburger);
}
