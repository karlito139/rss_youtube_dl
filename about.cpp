#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::About)
{
  ui->setupUi(this);

  ui->version->setText(CURRENT_VERSION);
}

About::~About()
{
  delete ui;
}
