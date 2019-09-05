//
// Created by Ivan Golubović on 05.09.19.
//

#include "profile.h"
#include <QDebug>
#include <utility>

Profile::Profile(QString what) : m_what(std::move(what))
{
    m_timer.start();
}

void Profile::restart(QString what)
{
    elapsed();
    m_what = std::move(what);
    m_timer.start();
}

Profile::~Profile()
{
    elapsed();
//	qDebug().noquote() << what << "took" << timer.elapsed() << "milliseconds";
}

void Profile::elapsed() const
{
    qDebug().noquote() << m_what << "took" << m_timer.elapsed() << "ms";
}
