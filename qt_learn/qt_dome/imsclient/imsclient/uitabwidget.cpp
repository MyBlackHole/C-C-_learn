#include "uitabwidget.h"

#include <QtWidgets/QTabBar>

UiTabWidget::UiTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    tabBar()->hide();
}

UiTabWidget::~UiTabWidget()
{

}