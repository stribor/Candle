//
// Created by Ivan Golubović on 05.09.19.
//

#ifndef CANDLE_PROFILE_H
#define CANDLE_PROFILE_H

#include <QString>
#include <QElapsedTimer>
#include <QtGlobal>

#ifdef QT_NO_DEBUG_OUTPUT
#define PROFILE_FUNCTION
#define PROFILE_SCOPE_START(_x)
#define PROFILE_SCOPE_RESTART(_x)
#define PROFILE_SUB_SCOPE_BEGIN(_x)
#define PROFILE_SUB_SCOPE_END
#else
#define PROFILE_FUNCTION Profile _prof_(Q_FUNC_INFO);
#define PROFILE_SCOPE_START(_x) Profile _prof_split(_x);
#define PROFILE_SCOPE_RESTART(_x) _prof_split.restart(_x);
#endif

/// \brief Measures time since object was created until it's destruction
class Profile {
public:
    Profile(QString what = {});

    /// \brief print current timer state and restart counting
    /// \param what
    void restart(QString what = {});
    ~Profile();

private:
    QString m_what;
    QElapsedTimer m_timer;
    void elapsed() const;
};


#endif //CANDLE_PROFILE_H
