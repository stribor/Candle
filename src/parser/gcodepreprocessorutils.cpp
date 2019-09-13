// This file is a part of "Candle" application.
// This file was originally ported from "GcodePreprocessorUtils.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include <QDebug>
#include <QVector3D>
#include <QRegularExpression>
#include "gcodepreprocessorutils.h"
#include "limits"
#include "../tables/gcodetablemodel.h"

/**
* Searches the command string for an 'f' and replaces the speed value
* between the 'f' and the next space with a percentage of that speed.
* In that way all speed values become a ratio of the provided speed
* and don't get overridden with just a fixed speed.
*/
QByteArray GcodePreprocessorUtils::overrideSpeed(QString command, double speed, double *original)
{
    static QRegularExpression re("[Ff]([0-9.]+)");

    auto match = re.match(command);
    if (match.hasMatch()) {
        Q_ASSERT(command.count(re) == 1); // otherwise all matches would get overridden with first match
        auto const orig_val = match.captured(1).toDouble();
        command.replace(re, QString("F%1").arg(orig_val / 100.0 * speed));

        if (original) *original = orig_val;
    }


    return command.toUtf8();
}

/**
* Removes any comments within parentheses or beginning with a semi-colon.
*/
QByteArray GcodePreprocessorUtils::removeComment(QByteArray command)
{
    int pos;

    // Remove any comments within first ( and last )
    pos = command.indexOf('(');
    if (pos >= 0) {
        int pos2 = command.lastIndexOf(')');
        command.remove(pos, pos2-pos+1);
    }

    // Remove any comment beginning with ';'
    pos = command.indexOf(';');
    if (pos >= 0 )
        command.truncate(pos);

    return command.trimmed();
}

/**
* Searches for a comment in the input string and returns the first match.
*/
QString GcodePreprocessorUtils::parseComment(QString command)
{
    // REGEX: Find any comment, includes the comment characters:
    // "(?<=\()[^\(\)]*|(?<=\;)[^;]*"
    // "(?<=\\()[^\\(\\)]*|(?<=\\;)[^;]*"

    static QRegularExpression re(R"((\([^\(\)]*\)|;[^;].*))");

    auto match = re.match(command);
    if (match.isValid()) {
        return match.captured(1);
    }
    return "";
}

QByteArray GcodePreprocessorUtils::truncateDecimals(int length, QString command)
{
    static QRegularExpression re(R"((\d*\.\d*))");
    int pos = 0;

    QRegularExpressionMatch match = re.match(command, pos);
    while ((pos = match.capturedStart(0)) != -1) {
        QString newNum = QString::number(match.captured(1).toDouble(), 'f', length);
        command = command.left(pos) + newNum + command.mid(match.capturedEnd(0));
        pos += newNum.size() + 1;
        match = re.match(command, pos);
    }

    return command.toUtf8();
}

QByteArray GcodePreprocessorUtils::removeAllWhitespace(QByteArray command)
{
#if 1
    // guess should be faster than regex
    return command.simplified().replace(' ', QByteArray());
#else
    static QRegularExpression rx("\\s");

    return command.remove(rx);
#endif
}
//struct GM {QLatin1String v; GCodes m;};
static QHash<QString, GCodes> gm = {{
        {QLatin1String("G0"), G00},
        {QLatin1String("G1"), G01},
        {QLatin1String("G38.2"), G38_2},
        {QLatin1String("G2"), G02},
        {QLatin1String("G3"), G03},
        {QLatin1String("G17"), G17},
        {QLatin1String("G18"), G18},
        {QLatin1String("G19"), G19},
        {QLatin1String("G20"), G20},
        {QLatin1String("G21"), G21},
        {QLatin1String("G90"), G90},
        {QLatin1String("G90.1"), G90_1},
        {QLatin1String("G91"), G91},
        {QLatin1String("G91.1"), G91_1},
        {QLatin1String("g0"), G00},
        {QLatin1String("g1"), G01},
        {QLatin1String("g38.2"), G38_2},
        {QLatin1String("g2"), G02},
        {QLatin1String("g3"), G03},
        {QLatin1String("g17"), G17},
        {QLatin1String("g18"), G18},
        {QLatin1String("g19"), G19},
        {QLatin1String("g20"), G20},
        {QLatin1String("g21"), G21},
        {QLatin1String("g90"), G90},
        {QLatin1String("g90.1"), G90_1},
        {QLatin1String("g91"), G91},
        {QLatin1String("g91.0"), G91_1}
}};

GcodePreprocessorUtils::gcodesContainer GcodePreprocessorUtils::parseCodesEnum(QByteArrayList const &args, QChar /*code*/)
{
    gcodesContainer l;

    for (auto &arg : args) {
#if 1
        GCodes v = unknown;
        auto c = arg.data();
        if (c[0] != 'G') // code must be G code
            continue;

        ++c;

        switch (arg.size()) {
        case 2: // 0, 1, 2, 3
            switch (c[0]) {
            case '0':v = G00;
                break;
            case '1':v = G01;
                break;
            case '2':v = G02;
                break;
            case '3':v = G03;
                break;
            case '7':v = G07;
                break;
            case '8':v = G08;
                break;
            }
            break;
        case 3:
            switch (c[0]) {
            case '0':
                switch (c[1]) {
                case '0':v = G00;
                    break;
                case '1':v = G01;
                    break;
                case '2':v = G02;
                    break;
                case '3':v = G03;
                    break;
                case '7':v = G07;
                    break;
                case '8':v = G08;
                    break;
                }
                break;
            case '1':
                switch (c[1]) {
                case '7': v = G17;
                    break;
                case '8': v = G18;
                    break;
                case '9': v = G19;
                    break;
                }
                break;
            case '2':
                switch (c[1]) {
                case '0': v = G20;
                    break;
                case '1': v = G21;
                    break;
                }
                break;
            case '9':
                switch (c[1]) {
                case '0': v = G90;
                    break;
                case '1': v = G91;
                    break;
                }
                break;
            }
            break;
        case 4: // G5.1, G5.2
            if (c[1] == '.') {
                switch (c[0]) {
                case '5':
                    switch (c[2]) {
                    case '1': v = G05_1;
                        break;
                    case '2': v = G05_2;
                        break;
                    }
                    break;
                }
            }
            break;
        case 5:
            if (c[2] != '.') // code must be xx.y
                break;
            switch (c[0]) {
            case '0':
                if (c[1] == '5') {
                    switch (c[3]) {
                    case '1':v = G05_1;
                        break;
                    case '2':v = G05_2;
                        break;
                    }
                }
                break;
            case '3':
                if (c[1] == '8') {
                    switch (c[3]) {
                    case '2':v = G38_2;
                        break;
                    case '3':v = G38_3;
                        break;
                    case '4':v = G38_4;
                        break;
                    case '5':v = G38_5;
                        break;
                    }
                }
                break;
            case '9': // 9xxx
                switch (c[1]) {
                case '0': if (c[3] == '1') v = G90_1;
                    break;
                case '1': if (c[3] == '1') v = G91_1;
                    break;
                }
                break;
            }
        }
//        auto v = G.value(arg, unknown);
#else
        v = gm.value(arg, unknown);
#endif
        if (v != unknown)
            l.push_back(v);
    }
    return l;
}

QList<float> GcodePreprocessorUtils::parseCodes(const QStringList &args, QChar code)
{
    QList<float> l;

    auto small_c = code.toLower();

    for (auto const &s : args) {
        if (s.size() > 0 && (s[0] == code || s[0] == small_c)) l.append(s.midRef(1).toDouble());
    }

    return l;
}

QList<int> GcodePreprocessorUtils::parseGCodes(QString const &command)
{
    static QRegularExpression re("[Gg]0*(\\d+)");

    QList<int> codes;
    int pos = 0;

    QRegularExpressionMatch match = re.match(command, pos);
    while ( match.hasMatch()) {
        codes.append(match.captured(1).toInt());
        pos = match.capturedEnd();
        match = re.match(command, pos);
    }

    return codes;
}

QList<int> GcodePreprocessorUtils::parseMCodes(QString const &command)
{
    static QRegularExpression re("[Mm]0*(\\d+)");

    QList<int> codes;
    int pos = 0;

    QRegularExpressionMatch match = re.match(command, pos);
    while (match.hasMatch()) {
        codes.append(match.captured(1).toInt());
        pos = match.capturedEnd();
        match = re.match(command, pos);
    }

    return codes;
}

/**
* Update a point given the arguments of a command.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(QByteArray const &command, const QVector3D &initial, bool absoluteMode)
{
    auto l = splitCommand(command);
    return updatePointWithCommand(l, initial, absoluteMode);
}

/**
* Update a point given the arguments of a command, using a pre-parsed list.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(
        QByteArrayList const &commandArgs, const QVector3D &initial,
        bool absoluteMode)
{
    double x = qQNaN();
    double y = qQNaN();
    double z = qQNaN();
    for (auto const & command: commandArgs) {
        if (!command.isEmpty()) {
            switch (command[0]) {
            case 'X': case 'x':
                x = atof(command.data() +1);// command.mid(1).toDouble();
                break;
            case 'Y': case 'y':
                y = atof(command.data() +1);// command.mid(1).toDouble();
                break;
            case 'Z':case 'z':
                z = atof(command.data() +1);// command.mid(1).toDouble();
                break;
            }
        }
    }

    return updatePointWithCommand(initial, x, y, z, absoluteMode);
}

/**
* Update a point given the new coordinates.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode)
{
    QVector3D newPoint(initial.x(), initial.y(), initial.z());

    if (absoluteMode) {
        if (!qIsNaN(x)) newPoint.setX(x);
        if (!qIsNaN(y)) newPoint.setY(y);
        if (!qIsNaN(z)) newPoint.setZ(z);
    } else {
        if (!qIsNaN(x)) newPoint.setX(newPoint.x() + x);
        if (!qIsNaN(y)) newPoint.setY(newPoint.y() + y);
        if (!qIsNaN(z)) newPoint.setZ(newPoint.z() + z);
    }

    return newPoint;
}

QVector3D GcodePreprocessorUtils::updateCenterWithCommand(QByteArrayList const &commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise)
{
    double i = qQNaN();
    double j = qQNaN();
    double k = qQNaN();
    double r = qQNaN();

    for (auto &t : commandArgs) {
        if (t.size() > 0) {
            auto c = toUpper(t[0]);
            switch (c) {
            case 'I':
                i = t.mid(1).toDouble();
                break;
            case 'J':
                j = t.mid(1).toDouble();
                break;
            case 'K':
                k = t.mid(1).toDouble();
                break;
            case 'R':
                r = t.mid(1).toDouble();
                break;
            }
        }
    }

    if (qIsNaN(i) && qIsNaN(j) && qIsNaN(k)) {
        return convertRToCenter(initial, nextPoint, r, absoluteIJKMode, clockwise);
    }

    return updatePointWithCommand(initial, i, j, k, absoluteIJKMode);
}

QString GcodePreprocessorUtils::generateG1FromPoints(QVector3D const &start, QVector3D const &end, bool absoluteMode, int precision)
{
    QString sb("G1");

    if (absoluteMode) {
        if (!qIsNaN(end.x())) sb.append("X" + QString::number(end.x(), 'f', precision));
        if (!qIsNaN(end.y())) sb.append("Y" + QString::number(end.y(), 'f', precision));
        if (!qIsNaN(end.z())) sb.append("Z" + QString::number(end.z(), 'f', precision));
    } else {
        if (!qIsNaN(end.x())) sb.append("X" + QString::number(end.x() - start.x(), 'f', precision));
        if (!qIsNaN(end.y())) sb.append("Y" + QString::number(end.y() - start.y(), 'f', precision));
        if (!qIsNaN(end.z())) sb.append("Z" + QString::number(end.z() - start.z(), 'f', precision));
    }

    return sb;
}

///**
//* Splits a gcode command by each word/argument, doesn't care about spaces.
//* This command is about the same speed as the string.split(" ") command,
//* but might be a little faster using precompiled regex.
//*/
QByteArrayList GcodePreprocessorUtils::splitCommand(QByteArray const &command) {
    QByteArrayList l;
    bool readNumeric = false;

    QByteArray sb;
    for (const auto &c:  command) {
        if (readNumeric && !isDigit(c) && c != '.') {
            readNumeric = false;
            l.append(sb);
            sb.clear();
            if (isLetter(c)) sb.append(c);
        } else if (isDigit(c) || c == '.' || c == '-') {
            sb.append(c);
            readNumeric = true;
        } else if (isLetter(c)) sb.append(c);
    }
    if (sb.size() > 0) l.append(sb);
    return l;
}

// TODO: Replace everything that uses this with a loop that loops through
// the string and creates a hash with all the values.
double GcodePreprocessorUtils::parseCoord(QByteArrayList const &argList, char c)
{
    auto small_c = tolower(c);
    for (auto const &t : argList) {
        if (t.size() > 0 && (t[0] == c || t[0] == small_c)) return atof(t.data() + 1);
    }
    return qQNaN();
}

//static public List<String> convertArcsToLines(Point3d start, Point3d end) {
//    List<String> l = new ArrayList<String>();

//    return l;
//}

QVector3D GcodePreprocessorUtils::convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise) {
    double R = radius;
    QVector3D center;

    double x = end.x() - start.x();
    double y = end.y() - start.y();

    double h_x2_div_d = 4 * R * R - x * x - y * y;
    if (h_x2_div_d < 0) { qDebug() << "Error computing arc radius."; }
    h_x2_div_d = (-sqrt(h_x2_div_d)) / hypot(x, y);

    if (!clockwise) h_x2_div_d = -h_x2_div_d;

    // Special message from gcoder to software for which radius
    // should be used.
    if (R < 0) {
        h_x2_div_d = -h_x2_div_d;
        // TODO: Places that use this need to run ABS on radius.
        radius = -radius;
    }

    double offsetX = 0.5 * (x - (y * h_x2_div_d));
    double offsetY = 0.5 * (y + (x * h_x2_div_d));

    if (!absoluteIJK) {
        center.setX(start.x() + offsetX);
        center.setY(start.y() + offsetY);
    } else {
        center.setX(offsetX);
        center.setY(offsetY);
    }

    return center;
}

/**
* Return the angle in radians when going from start to end.
*/
double GcodePreprocessorUtils::getAngle(QVector3D start, QVector3D end) {
    double deltaX = end.x() - start.x();
    double deltaY = end.y() - start.y();

    double angle = 0.0;

    if (deltaX != 0) { // prevent div by 0
        // it helps to know what quadrant you are in
        if (deltaX > 0 && deltaY >= 0) { // 0 - 90
            angle = atan(deltaY / deltaX);
        } else if (deltaX < 0 && deltaY >= 0) { // 90 to 180
            angle = M_PI - fabs(atan(deltaY / deltaX));
        } else if (deltaX < 0 && deltaY < 0) { // 180 - 270
            angle = M_PI + fabs(atan(deltaY / deltaX));
        } else if (deltaX > 0 && deltaY < 0) { // 270 - 360
            angle = M_PI * 2 - fabs(atan(deltaY / deltaX));
        }
    }
    else {
        // 90 deg
        if (deltaY > 0) {
            angle = M_PI / 2.0;
        }
        // 270 deg
        else {
            angle = M_PI * 3.0 / 2.0;
        }
    }

    return angle;
}

double GcodePreprocessorUtils::calculateSweep(double startAngle, double endAngle, bool isCw)
{
    double sweep;

    // Full circle
    if (startAngle == endAngle) {
        sweep = (M_PI * 2);
        // Arcs
    } else {
        // Account for full circles and end angles of 0/360
        if (endAngle == 0) {
            endAngle = M_PI * 2;
        }
        // Calculate distance along arc.
        if (!isCw && endAngle < startAngle) {
            sweep = ((M_PI * 2 - startAngle) + endAngle);
        } else if (isCw && endAngle > startAngle) {
            sweep = ((M_PI * 2 - endAngle) + startAngle);
        } else {
            sweep = fabs(endAngle - startAngle);
        }
    }

    return sweep;
}

/**
* Generates the points along an arc including the start and end points.
*/
GcodePreprocessorUtils::vectoContainer
GcodePreprocessorUtils::generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D start, QVector3D end, QVector3D center, bool clockwise, double R, double minArcLength, double arcPrecision, bool arcDegreeMode)
{
    double radius = R;

    // Rotate vectors according to plane
    QMatrix4x4 m;
    m.setToIdentity();
    switch (plane) {
    case PointSegment::XY:
        break;
    case PointSegment::ZX:
        m.rotate(90, 1.0, 0.0, 0.0);
        break;
    case PointSegment::YZ:
        m.rotate(-90, 0.0, 1.0, 0.0);
        break;
    }
    start = m * start;
    end = m * end;
    center = m * center;

    // Check center
    if (qIsNaN(center.length())) return {};

    // Calculate radius if necessary.
    if (radius == 0) {
        radius = sqrt(pow((double)(start.x() - center.x()), 2.0) + pow((double)(end.y() - center.y()), 2.0));
    }

    double startAngle = getAngle(center, start);
    double endAngle = getAngle(center, end);
    double sweep = calculateSweep(startAngle, endAngle, clockwise);

    // Convert units.
    double arcLength = sweep * radius;

    // If this arc doesn't meet the minimum threshold, don't expand.
//    if (minArcLength > 0 && arcLength < minArcLength) {
//        QList<QVector3D> empty;
//        return empty;
//    }

    int numPoints;

    if (arcDegreeMode && arcPrecision > 0) {
        numPoints = qMax(1.0, sweep / (M_PI * arcPrecision / 180));
    } else {
        if (arcPrecision <= 0 && minArcLength > 0) {
            arcPrecision = minArcLength;
        }
        numPoints = (int)ceil(arcLength/arcPrecision);
    }

    return generatePointsAlongArcBDring(plane, start, end, center, clockwise, radius, startAngle, sweep, numPoints);
}

/**
* Generates the points along an arc including the start and end points.
*/
GcodePreprocessorUtils::vectoContainer
GcodePreprocessorUtils::generatePointsAlongArcBDring(PointSegment::planes plane, QVector3D p1, QVector3D p2,
                                                                      QVector3D center, bool isCw,
                                                                      double radius, double startAngle,
                                                                      double sweep, int numPoints)
{
    // Prepare rotation matrix to restore plane
    QMatrix4x4 m;
    m.setToIdentity();
    switch (plane) {
    case PointSegment::XY:
        break;
    case PointSegment::ZX:
        m.rotate(-90, 1.0, 0.0, 0.0);
        break;
    case PointSegment::YZ:
        m.rotate(90, 0.0, 1.0, 0.0);
        break;
    }

    QVector3D lineEnd(p2.x(), p2.y(), p1.z());
    vectoContainer segments;
    double angle;

    // Calculate radius if necessary.
    if (radius == 0) {
        radius = sqrt(pow((double)(p1.x() - center.x()), 2.0) + pow((double)(p1.y() - center.y()), 2.0));
    }

    double zIncrement = (p2.z() - p1.z()) / numPoints;
    for (int i = 1; i < numPoints; i++)
    {
        if (isCw) {
            angle = (startAngle - i * sweep / numPoints);
        } else {
            angle = (startAngle + i * sweep / numPoints);
        }

        if (angle >= M_PI * 2) {
            angle = angle - M_PI * 2;
        }

        lineEnd.setX(cos(angle) * radius + center.x());
        lineEnd.setY(sin(angle) * radius + center.y());
        lineEnd.setZ(lineEnd.z() + zIncrement);

        segments.push_back(m * lineEnd);
    }

    segments.push_back(m * p2);

    return segments;
}

bool GcodePreprocessorUtils::isDigit(char c)
{
    return c > 47 && c < 58;
}

bool GcodePreprocessorUtils::isLetter(char c)
{
    return (c > 64 && c < 91) || (c > 96 && c < 123);
}

char GcodePreprocessorUtils::toUpper(char c)
{
    return (c > 96 && c < 123) ? c - 32 : c;
}
