// This file is a part of "Candle" application.
// This file was originally ported from "GcodePreprocessorUtils.java" class
// of "Universal GcodeSender" application written by Will Winder
// (https://github.com/winder/Universal-G-Code-Sender)

// Copyright 2015-2016 Hayrullin Denis Ravilevich

#include "gcodepreprocessorutils.h"
#include "utils/util.h"

#include <QDebug>
#include <QVector3D>
#include <algorithm>
#include <cctype>
#include <fmt/core.h>
#include <limits>
#include <string_view>

/**
* Searches the command string for an 'f' and replaces the speed value
* between the 'f' and the next space with a percentage of that speed.
* In that way all speed values become a ratio of the provided speed
* and don't get overridden with just a fixed speed.
*/
Command GcodePreprocessorUtils::overrideSpeed(CommandView input, double speedPercentage, double *original)
{
    Command command;
    command.reserve(input.size());
    auto it = input.begin();

    while (it != input.end()) {
        auto next_f = std::find_if(it, input.end(), [](char c) { return c == 'F' || c == 'f'; });
        command.append(it, next_f);

        if (next_f != input.end()) {
            auto start = next_f + 1;
            auto end = std::find_if(start, input.end(), [](char c) { return !std::isdigit(c) && c != '.'; });
            CommandView number_str(start, end);

            double orig_val = AtoF(number_str);
            if (original) {
                *original = orig_val;
            }
            fmt::format_to(std::back_inserter(command), "F{:.2f}", orig_val / 100.0 * speedPercentage);

            it = end;
        } else {
            it = next_f;
        }
    }

    return command;
}

/**
 * Removes any comments within parentheses or beginning with a semi-colon.
 * also removes any whitespace
*/
Command GcodePreprocessorUtils::removeComment(CommandView command)
{
    Command result;

    // Find the semicolon and get the substring until that point
    CommandView lineUntilSemicolon = command.substr(0, command.find(';'));

    // Remove comments in parentheses and copy the remaining characters
    bool inComment = false;
    std::copy_if(lineUntilSemicolon.begin(), lineUntilSemicolon.end(), std::back_inserter(result),
                 [&inComment](char c) {
                     if (c == '(') {
                         inComment = true;
                     } else if (c == ')') {
                         inComment = false;
                         return false;
                     }
                     return !inComment && !std::isspace(c);
                 });

    return result;
}

/**
* Searches for a comment in the input string and returns the first match.
*/
Command GcodePreprocessorUtils::parseComment(CommandView command)
{
    std::string firstComment;

    // Find the start of the comment
    const auto *it = std::find_if(command.begin(), command.end(), [](char c) {
        return c == '(' || c == ';';
    });

    if (it != command.end()) {
        if (*it == '(') {
            Util::copy_until(it + 1, command.end(), std::back_inserter(firstComment), [](char c) { return c == ')'; });
        } else if (*it == ';') {
            // Copy characters until the end of the line, excluding the semicolon
            std::copy(it + 1, command.end(), std::back_inserter(firstComment));
        }
    }

    return firstComment;
}

/// \brief returns reformatted string with all decimal numbers truncated to specified number of digits (or less if trailing zeros)
/// \param decimals number of digits after decimal point
/// \param input
/// \return
Command GcodePreprocessorUtils::truncateDecimals(int decimals, CommandView input)
{
    Command result;
    auto pos = input.begin();

    auto is_number_start = [](char c) { return std::isdigit(c) || c == '-'; };

    while (pos != input.end()) {
        if (is_number_start(*pos)) {
            // Found a number
            auto end = std::find_if_not(pos, input.end(), [](char c) { return std::isdigit(c) || c == '.' || c == '-'; });
            CommandView number_str(pos, std::distance(pos, end));

            if (number_str.find('.') != std::string_view::npos) {
                // Decimal number found
                double number = AtoF(number_str);
#if 0
                size_t initial_size = result.size();
#endif
                result.append(fmt::format("{:.{}f}", number, decimals));

#if 0
                // Remove trailing zeros
                while (result.size() > initial_size && result.back() == '0') {
                    result.pop_back();
                }
                // Remove trailing decimal point
                if (result.size() > initial_size && result.back() == '.') {
                    result.pop_back();
                }
#endif
            } else {
                // Integer found
                result.append(number_str);
            }
            pos = end;
        } else {
            result.push_back(*pos);
            ++pos;
        }
    }

    return result;
}

Command GcodePreprocessorUtils::removeAllWhitespace(CommandView command)
{
    Command result;
    std::remove_copy_if(command.begin(), command.end(), std::back_inserter(result), std::isspace);

    return result;
}

GCodes GcodePreprocessorUtils::parseGCodeEnum(CommandView arg)
{
    GCodes v = unknown;

    auto c = arg.data();
    if (c[0] != 'G' && c[0] != 'g')// code must be G code
        return v;

    ++c;


    switch (arg.size()) {
    case 2:// 0, 1, 2, 3
        switch (c[0]) {
        case '0':
            v = G00;
            break;
        case '1':
            v = G01;
            break;
        case '2':
            v = G02;
            break;
        case '3':
            v = G03;
            break;
        case '7':
            v = G07;
            break;
        case '8':
            v = G08;
            break;
        }
        break;
    case 3:
        switch (c[0]) {
        case '0':
            switch (c[1]) {
            case '0':
                v = G00;
                break;
            case '1':
                v = G01;
                break;
            case '2':
                v = G02;
                break;
            case '3':
                v = G03;
                break;
            case '7':
                v = G07;
                break;
            case '8':
                v = G08;
                break;
            }
            break;
        case '1':
            switch (c[1]) {
            case '7':
                v = G17;
                break;
            case '8':
                v = G18;
                break;
            case '9':
                v = G19;
                break;
            }
            break;
        case '2':
            switch (c[1]) {
            case '0':
                v = G20;
                break;
            case '1':
                v = G21;
                break;
            }
            break;
        case '9':
            switch (c[1]) {
            case '0':
                v = G90;
                break;
            case '1':
                v = G91;
                break;
            }
            break;
        }
        break;
    case 4:// G5.1, G5.2
        if (c[1] == '.') {
            switch (c[0]) {
            case '5':
                switch (c[2]) {
                case '1':
                    v = G05_1;
                    break;
                case '2':
                    v = G05_2;
                    break;
                }
                break;
            }
        }
        break;
    case 5:
        if (c[2] != '.')// code must be xx.y
            break;
        switch (c[0]) {
        case '0':
            if (c[1] == '5') {
                switch (c[3]) {
                case '1':
                    v = G05_1;
                    break;
                case '2':
                    v = G05_2;
                    break;
                }
            }
            break;
        case '3':
            if (c[1] == '8') {
                switch (c[3]) {
                case '2':
                    v = G38_2;
                    break;
                case '3':
                    v = G38_3;
                    break;
                case '4':
                    v = G38_4;
                    break;
                case '5':
                    v = G38_5;
                    break;
                }
            }
            break;
        case '9':// 9xxx
            switch (c[1]) {
            case '0':
                if (c[3] == '1') v = G90_1;
                break;
            case '1':
                if (c[3] == '1') v = G91_1;
                break;
            }
            break;
        }
    }
    //    if (v == unknown) {
    //        qDebug() << "Unknown code" << arg;
    //    }
    return v;
}

/**
* Update a point given the arguments of a command.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(CommandView command, const QVector3D &initial, bool absoluteMode)
{
    auto l = splitCommand(command);
    return updatePointWithCommand(l, initial, absoluteMode);
}

/**
* Update a point given the arguments of a command, using a pre-parsed list.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(
        CommandList const &commandArgs, const QVector3D &initial,
        bool absoluteMode)
{
    QVector3D vec(initial);
    for (auto const &command : commandArgs) {
        if (command.size() > 1) {
            switch (command[0]) {
            case 'X':
            case 'x':
                if (absoluteMode)
                    vec.setX(AtoF(std::string_view(command.data() + 1, command.size() - 1)));// command.mid(1).toDouble();
                else
                    vec.setX(initial.x() + AtoF(std::string_view(command.data() + 1, command.size() - 1)));// command.mid(1).toDouble();
                break;
            case 'Y':
            case 'y':
                if (absoluteMode)
                    vec.setY(AtoF(std::string_view(command.data() + 1, command.size() - 1)));// command.mid(1).toDouble();
                else
                    vec.setY(initial.y() + AtoF(std::string_view(command.data() + 1, command.size() - 1)));// command.mid(1).toDouble();
                break;
            case 'Z':
            case 'z':
                if (absoluteMode)
                    vec.setZ(AtoF(std::string_view(command.data() + 1, command.size() - 1)));// command.mid(1).toDouble();
                else
                    vec.setZ(initial.z() + AtoF(std::string_view(command.data() + 1, command.size() - 1)));// command.mid(1).toDouble();
                break;
            }
        }
    }
    return vec;
}

/**
* Update a point given the new coordinates.
*/
QVector3D GcodePreprocessorUtils::updatePointWithCommand(const QVector3D &initial, double x, double y, double z, bool absoluteMode)
{
    QVector3D newPoint(initial);

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

QVector3D GcodePreprocessorUtils::updateCenterWithCommand(CommandList const &commandArgs, QVector3D initial, QVector3D nextPoint, bool absoluteIJKMode, bool clockwise)
{
    double i = qQNaN();
    double j = qQNaN();
    double k = qQNaN();
    double r = qQNaN();

    for (const auto &t : commandArgs) {
        if (t.size() > 0) {
            switch (t[0]) {
            case 'I':
            case 'i':
                i = AtoF(t.data() + 1);
                break;
            case 'J':
            case 'j':
                j = AtoF(t.data() + 1);
                break;
            case 'K':
            case 'k':
                k = AtoF(t.data() + 1);
                break;
            case 'R':
            case 'r':
                r = AtoF(t.data() + 1);
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

/**
* Splits a gcode command by each word/argument, doesn't care about spaces.
* This command is about the same speed as the string.split(" ") command,
* but might be a little faster using precompiled regex.
*/
// modified split command to be able to process lines with comments and white space in so no trimming/comment removal needed
// from https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=823374
// 3.3.4 Comments and MessagesPrintable characters and white space inside parentheses is a comment.
// A left parenthesis always starts a comment. The comment ends at the first right parenthesis found thereafter.
// Once a left parenthesis is placed on a line, a matching right parenthesis must appear before the end of the line.
// Comments may not be nested; it is an error if a left parenthesis is found after the start of a comment and before the end of the comment.
// Here is an example of a line containing a comment:“G80 M5 (stop motion)”.
// Comments do not cause a machining center to do anything.

CommandList GcodePreprocessorUtils::splitCommand(CommandView gcode)
{
    CommandList commands;
    Command current_command;

    for (auto it = gcode.begin(); it != gcode.end();) {
        char c = *it;

        if (c == ';') { // End processing for inline comments
            break;
        } else if (c == '(') { // Skip to the closing parenthesis for parenthetical comments
            it = std::find(it, gcode.end(), ')');
            if (it != gcode.end()) {
                ++it;
            }
        } else if (c == '[' || c == ']') { // Ignore '[' and ']'
            ++it;
        } else if (!std::isspace(c)) {
            bool is_new_command = c == 'G' || c == 'M' || c == 'T' || c == 'S' || c == 'F' || c == 'X' || c == 'Y' || c == 'Z' || c == 'I' || c == 'J' || c == '$';

            if (is_new_command && !current_command.empty()) {
                commands.push_back(current_command);
                current_command.clear();
            }

            current_command += c;
            ++it;

            if (c == '$') [[unlikely]] {
                while (it != gcode.end() && !std::isspace(*it)) {
                    if (*it == ';') {
                        break;
                    } else if (*it == '(') {
                        it = std::find(it, gcode.end(), ')');
                        if (it != gcode.end()) {
                            ++it;
                        }
                    } else {
                        current_command += *it;
                        ++it;
                    }
                }
            }
        } else {
            ++it;
        }
    }

    if (!current_command.empty()) {
        commands.push_back(current_command);
    }

    return commands;
}

bool GcodePreprocessorUtils::parseCoord(CommandView arg, char c, double &outVal)
{
    auto small_c = toLower(c);
    if (arg.size() > 0 && (arg[0] == c || arg[0] == small_c)) {
        outVal = AtoF(arg.data() + 1);
        return true;
    }
//    outVal = qQNaN();
    return false;
}
// TODO: Replace everything that uses this with a loop that loops through
// the string and creates a hash with all the values.
double GcodePreprocessorUtils::parseCoord(CommandList const &argList, char c)
{
    auto small_c = toLower(c);
    for (auto const &t : argList) {
        if (t.size() > 0 && (t[0] == c || t[0] == small_c)) return AtoF(t.data() + 1);
    }
    return qQNaN();
}

//static public List<String> convertArcsToLines(Point3d start, Point3d end) {
//    List<String> l = new ArrayList<String>();

//    return l;
//}

QVector3D GcodePreprocessorUtils::convertRToCenter(QVector3D start, QVector3D end, double radius, bool absoluteIJK, bool clockwise)
{
    double const R = radius;
    QVector3D center;

    double const x = end.x() - start.x();
    double const y = end.y() - start.y();

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

    double const offsetX = 0.5 * (x - (y * h_x2_div_d));
    double const offsetY = 0.5 * (y + (x * h_x2_div_d));

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
double GcodePreprocessorUtils::getAngle(QVector3D start, QVector3D end)
{
    double const deltaX = end.x() - start.x();
    double const deltaY = end.y() - start.y();

    double angle = 0.0;

    if (deltaX != 0) {// prevent div by 0
        // it helps to know what quadrant you are in
        if (deltaX > 0 && deltaY >= 0) {// 0 - 90
            angle = atan(deltaY / deltaX);
        } else if (deltaX < 0 && deltaY >= 0) {// 90 to 180
            angle = M_PI - fabs(atan(deltaY / deltaX));
        } else if (deltaX < 0 && deltaY < 0) {// 180 - 270
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
    start = m.map(start);
    end = m.map(end);
    center = m.map(center);

    // Check center
    if (qIsNaN(center.length())) return {};

    // Calculate radius if necessary.
    if (radius == 0) {
        radius = sqrt(pow((double) (start.x() - center.x()), 2.0) + pow((double) (end.y() - center.y()), 2.0));
    }

    double const startAngle = getAngle(center, start);
    double const endAngle = getAngle(center, end);
    double const sweep = calculateSweep(startAngle, endAngle, clockwise);

    // Convert units.
    double const arcLength = sweep * radius;

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

    double const zIncrement = (p2.z() - p1.z()) / numPoints;
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

        segments.push_back(m.map(lineEnd));
    }

    segments.push_back(m.map(p2));

    return segments;
}

bool GcodePreprocessorUtils::has_M2_M30(const CommandList &commandList)
{
    // Set M2 & M30 commands sent flag
    return std::any_of(commandList.begin(), commandList.end(),
                       [](auto const &command) {
                           if (command.size() > 1 && command[0] == 'M') {
                               if (command == "M02" || command == "M2" || command == "M30") return true;
                           }
                           return false;
                       });

}

double GcodePreprocessorUtils::AtoF(std::string_view str)
{
    // skip white space at start
    str.remove_prefix(std::min(str.find_first_not_of(" \t"), str.size()));

    auto num = str.begin();
    if (num == str.end())
        return 0;

    int integerPart = 0;
    int fractionPart = 0;
    int divisorForFraction = 1;
    int sign = 1;
    bool inFraction = false;

    /*Take care of +/- sign*/
    if (*num == '-') {
        ++num;
        sign = -1;
    } else if (*num == '+') {
        ++num;
    }
    while (num != str.end() && *num != '\0') {
        if (*num >= '0' && *num <= '9') {
            if (inFraction) {
                /*See how are we converting a character to integer*/
                fractionPart = fractionPart * 10 + (*num - '0');
                divisorForFraction *= 10;
            } else {
                integerPart = integerPart * 10 + (*num - '0');
            }
        } else if (*num == '.') {
            if (inFraction)
                return double(sign) * (double(integerPart) + double(fractionPart) / double(divisorForFraction));
            inFraction = true;
        } else {
            return double(sign) * (double(integerPart) + double(fractionPart) / double(divisorForFraction));
        }
        ++num;
    }
    return sign * (double(integerPart) + double(fractionPart) / double(divisorForFraction));
}
