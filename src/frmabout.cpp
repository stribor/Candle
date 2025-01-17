// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include <QDesktopServices>
#include <QStandardPaths>
#include <QFile>
#include "frmabout.h"
#include "ui_frmabout.h"

frmAbout::frmAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmAbout)
{
    ui->setupUi(this);

    ui->lblAbout->setText(ui->lblAbout->text().arg(qApp->applicationVersion()));

#ifdef Q_OS_MAC
    // on mac os applicationDirPath points to exe location inside app bundle (appname.app/Contents/MacOS) and we need Resources path
    auto appPath = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).last();;
#else
    auto appPath = qApp->applicationDirPath();
#endif
    QFile file(appPath + "/LICENSE");

    if (file.open(QIODevice::ReadOnly)) {
        ui->txtLicense->setPlainText(file.readAll());
    }
}

frmAbout::~frmAbout()
{
    delete ui;
}

void frmAbout::on_cmdOk_clicked()
{
    this->hide();
}

void frmAbout::on_lblAbout_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(link);
}
