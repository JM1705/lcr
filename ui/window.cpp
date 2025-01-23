#include "window.h"
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

Window::Window(QWidget *parent) : QWidget{parent}
{
    // Create a container window
    this->setFixedSize(720, 800);

    // QToolButton *button_new = new QToolButton();
    // // QIcon icon_new = QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew);
    // QIcon icon_new = QIcon::fromTheme("bqm-add");
    // button_new->setIcon(icon_new);
    // button_new->setText("Add");
    // button_new->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // QToolButton *button_hamburger = new QToolButton();
    // QIcon icon_ham = QIcon::fromTheme("application-menu");
    // button_hamburger->setIcon(icon_ham);

    // QLabel *title = new QLabel();
    // title->setText(" Mappings");
    // QFont title_font = QFont();
    // title_font.setPixelSize(18);
    // title->setFont(title_font);

    // QToolBar *toolbar = new QToolBar();
    // toolbar->addWidget(title);
    // QWidget* spacer = new QWidget();
    // spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    // toolbar->addWidget(spacer);
    // toolbar->addWidget(button_new);
    // toolbar->addWidget(button_hamburger);


    QTextEdit *edirot = new QTextEdit();

    MainToolbar *maintoolbar = new MainToolbar();


    QVBoxLayout *root_layout = new QVBoxLayout(this);
    root_layout->setContentsMargins(0,0,0,0);
    root_layout->setSpacing(0);
    root_layout->addWidget(maintoolbar);
    // root_layout->addWidget(edirot);
}
