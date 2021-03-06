/*
Mod Organizer shared UI functionality

Copyright (C) 2012 Sebastian Herbord. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "uibase/utility.h"
#include "uibase/report.h"

#include <QApplication>
#include <QBuffer>
#include <QDesktopWidget>
#include <QDir>
#include <QTextCodec>
#include <QtDebug>
#include <QtWinExtras/QtWin>
#include <common/sane_windows.h>

#include <ShlObj.h>
#include <memory>
#include <shellapi.h>
#define FO_RECYCLE 0x1003

namespace MOBase {

MyException::MyException(const QString& text) : std::exception(), m_Message(text.toLocal8Bit()) {}

QString windowsErrorString(DWORD errorCode) {
    QByteArray result;
    QTextStream stream(&result);

    LPWSTR buffer = nullptr;
    // TODO: the message is not english?
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, nullptr) == 0) {
        stream << " (errorcode " << errorCode << ")";
    } else {
        // remove line break
        LPWSTR lastChar = buffer + wcslen(buffer) - 2;
        *lastChar = L'\0';
        stream << ToQString(buffer) << " (errorcode " << errorCode << ")";
        LocalFree(buffer); // allocated by FormatMessage
    }
    stream.flush();
    return QString(result);
}

bool removeDir(const QString& dirName) {
    QDir dir(dirName);

    if (dir.exists()) {
        Q_FOREACH (QFileInfo info,
                   dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files,
                                     QDir::DirsFirst)) {
            if (info.isDir()) {
                if (!removeDir(info.absoluteFilePath())) {
                    return false;
                }
            } else {
                ::SetFileAttributesW(ToWString(info.absoluteFilePath()).c_str(), FILE_ATTRIBUTE_NORMAL);
                QFile file(info.absoluteFilePath());
                if (!file.remove()) {
                    reportError(QObject::tr("removal of \"%1\" failed: %2")
                                    .arg(info.absoluteFilePath())
                                    .arg(file.errorString()));
                    return false;
                }
            }
        }

        if (!dir.rmdir(dirName)) {
            reportError(QObject::tr("removal of \"%1\" failed").arg(dir.absolutePath()));
            return false;
        }
    } else {
        reportError(QObject::tr("\"%1\" doesn't exist (remove)").arg(dirName));
        return false;
    }

    return true;
}

bool copyDir(const QString& sourceName, const QString& destinationName, bool merge) {
    QDir sourceDir(sourceName);
    if (!sourceDir.exists()) {
        return false;
    }
    QDir destDir(destinationName);
    if (!destDir.exists()) {
        destDir.mkdir(destinationName);
    } else if (!merge) {
        return false;
    }

    QStringList files = sourceDir.entryList(QDir::Files);
    foreach (QString fileName, files) {
        QString srcName = sourceName + "/" + fileName;
        QString destName = destinationName + "/" + fileName;
        QFile::copy(srcName, destName);
    }

    files.clear();
    // we leave out symlinks because that could cause an endless recursion
    QStringList subDirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (QString subDir, subDirs) {
        QString srcName = sourceName + "/" + subDir;
        QString destName = destinationName + "/" + subDir;
        copyDir(srcName, destName, merge);
    }
    return true;
}

static DWORD TranslateError(int error) {
    switch (error) {
    case 0x71:
        return ERROR_INVALID_PARAMETER; // same file
    case 0x72:
        return ERROR_INVALID_PARAMETER; // many source, one destination. shouldn't happen due to how parameters are
                                        // transformed
    case 0x73:
        return ERROR_NOT_SAME_DEVICE;
    case 0x74:
        return ERROR_INVALID_PARAMETER;
    case 0x75:
        return ERROR_CANCELLED;
    case 0x76:
        return ERROR_BAD_PATHNAME;
    case 0x78:
        return ERROR_ACCESS_DENIED;
    case 0x79:
        return ERROR_BUFFER_OVERFLOW; // path exceeds max_path
    case 0x7A:
        return ERROR_INVALID_PARAMETER;
    case 0x7C:
        return ERROR_BAD_PATHNAME;
    case 0x7D:
        return ERROR_INVALID_PARAMETER;
    case 0x7E:
        return ERROR_ALREADY_EXISTS;
    case 0x80:
        return ERROR_ALREADY_EXISTS;
    case 0x81:
        return ERROR_BUFFER_OVERFLOW;
    case 0x82:
        return ERROR_WRITE_PROTECT;
    case 0x83:
        return ERROR_WRITE_PROTECT;
    case 0x84:
        return ERROR_WRITE_PROTECT;
    case 0x85:
        return ERROR_DISK_FULL;
    case 0x86:
        return ERROR_WRITE_PROTECT;
    case 0x87:
        return ERROR_WRITE_PROTECT;
    case 0x88:
        return ERROR_WRITE_PROTECT;
    case 0xB7:
        return ERROR_BUFFER_OVERFLOW;
    case 0x402:
        return ERROR_PATH_NOT_FOUND;
    case 0x10000:
        return ERROR_GEN_FAILURE;
    default:
        return static_cast<DWORD>(error);
    }
}

static bool shellOp(const QStringList& sourceNames, const QStringList& destinationNames, QWidget* dialog,
                    UINT operation, bool yesToAll) {
    std::vector<wchar_t> fromBuffer;
    std::vector<wchar_t> toBuffer;

    foreach (const QString& from, sourceNames) {
        // SHFileOperation has to be used with absolute maths, err paths ("It cannot be overstated" they say)
        std::wstring tempFrom = ToWString(QDir::toNativeSeparators(QFileInfo(from).absoluteFilePath()));
        fromBuffer.insert(fromBuffer.end(), tempFrom.begin(), tempFrom.end());
        fromBuffer.push_back(L'\0');
    }

    bool recycle = operation == FO_RECYCLE;

    if (recycle) {
        operation = FO_DELETE;
    }

    if ((destinationNames.count() == sourceNames.count()) || (destinationNames.count() == 1)) {
        foreach (const QString& to, destinationNames) {
            std::wstring tempTo = ToWString(QDir::toNativeSeparators(QFileInfo(to).absoluteFilePath()));
            toBuffer.insert(toBuffer.end(), tempTo.begin(), tempTo.end());
            toBuffer.push_back(L'\0');
        }
    } else if ((operation == FO_DELETE) && (destinationNames.count() == 0)) {
        // pTo is not used but as I understand the documentation it should still be double-null terminated
        toBuffer.push_back(L'\0');
    } else {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    // both buffers have to be double-null terminated
    fromBuffer.push_back(L'\0');
    toBuffer.push_back(L'\0');

    SHFILEOPSTRUCTW op;
    memset(&op, 0, sizeof(SHFILEOPSTRUCTW));
    if (dialog != nullptr) {
        op.hwnd = (HWND)dialog->winId();
    } else {
        op.hwnd = nullptr;
    }
    op.wFunc = operation;
    op.pFrom = &fromBuffer[0];
    op.pTo = &toBuffer[0];

    if ((operation == FO_DELETE) || yesToAll) {
        op.fFlags = FOF_NOCONFIRMATION;
        if (recycle) {
            op.fFlags |= FOF_ALLOWUNDO;
        }
    } else {
        op.fFlags = FOF_NOCOPYSECURITYATTRIBS | // always use security of target directory
                    FOF_SILENT |                // don't show a progress bar
                    FOF_NOCONFIRMMKDIR;         // silently create directories

        if (destinationNames.count() == sourceNames.count()) {
            op.fFlags |= FOF_MULTIDESTFILES;
        }
    }

    int res = ::SHFileOperationW(&op);
    if (res == 0) {
        return true;
    } else {
        ::SetLastError(TranslateError(res));
        return false;
    }
}

bool shellCopy(const QStringList& sourceNames, const QStringList& destinationNames, QWidget* dialog) {
    return shellOp(sourceNames, destinationNames, dialog, FO_COPY, false);
}

bool shellCopy(const QString& sourceNames, const QString& destinationNames, bool yesToAll, QWidget* dialog) {
    return shellOp(QStringList() << sourceNames, QStringList() << destinationNames, dialog, FO_COPY, yesToAll);
}

bool shellMove(const QStringList& sourceNames, const QStringList& destinationNames, QWidget* dialog) {
    return shellOp(sourceNames, destinationNames, dialog, FO_MOVE, false);
}

bool shellMove(const QString& sourceNames, const QString& destinationNames, bool yesToAll, QWidget* dialog) {
    return shellOp(QStringList() << sourceNames, QStringList() << destinationNames, dialog, FO_MOVE, yesToAll);
}

bool shellRename(const QString& oldName, const QString& newName, bool yesToAll, QWidget* dialog) {
    return shellOp(QStringList(oldName), QStringList(newName), dialog, FO_RENAME, yesToAll);
}

bool shellDelete(const QStringList& fileNames, bool recycle, QWidget* dialog) {
    return shellOp(fileNames, QStringList(), dialog, recycle ? FO_RECYCLE : FO_DELETE, false);
}

bool moveFileRecursive(const QString& source, const QString& baseDir, const QString& destination) {
    QStringList pathComponents = destination.split("/");
    QString path = baseDir;
    for (QStringList::Iterator iter = pathComponents.begin(); iter != pathComponents.end() - 1; ++iter) {
        path.append("/").append(*iter);
        if (!QDir(path).exists() && !QDir().mkdir(path)) {
            reportError(QObject::tr("failed to create directory \"%1\"").arg(path));
            return false;
        }
    }

    QString destinationAbsolute = baseDir.mid(0).append("/").append(destination);
    if (!QFile::rename(source, destinationAbsolute)) {
        // move failed, try copy & delete
        if (!QFile::copy(source, destinationAbsolute)) {
            reportError(QObject::tr("failed to copy \"%1\" to \"%2\"").arg(source).arg(destinationAbsolute));
            return false;
        } else {
            QFile::remove(source);
        }
    }
    return true;
}

bool copyFileRecursive(const QString& source, const QString& baseDir, const QString& destination) {
    QStringList pathComponents = destination.split("/");
    QString path = baseDir;
    for (QStringList::Iterator iter = pathComponents.begin(); iter != pathComponents.end() - 1; ++iter) {
        path.append("/").append(*iter);
        if (!QDir(path).exists() && !QDir().mkdir(path)) {
            reportError(QObject::tr("failed to create directory \"%1\"").arg(path));
            return false;
        }
    }

    QString destinationAbsolute = baseDir.mid(0).append("/").append(destination);
    if (!QFile::copy(source, destinationAbsolute)) {
        reportError(QObject::tr("failed to copy \"%1\" to \"%2\"").arg(source).arg(destinationAbsolute));
        return false;
    }
    return true;
}

std::wstring ToWString(const QString& source) { return source.toStdWString(); }

std::string ToString(const QString& source, bool utf8) {
    QByteArray array8bit;
    if (utf8) {
        array8bit = source.toUtf8();
    } else {
        array8bit = source.toLocal8Bit();
    }
    return std::string(array8bit.constData());
}

QString ToQString(const std::string& source) { return QString::fromUtf8(source.c_str()); }

QString ToQString(const std::wstring& source) { return QString::fromWCharArray(source.c_str()); }

QString ToString(const SYSTEMTIME& time) {
    char dateBuffer[100];
    char timeBuffer[100];
    int size = 100;
    GetDateFormatA(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &time, nullptr, dateBuffer, size);
    GetTimeFormatA(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &time, nullptr, timeBuffer, size);
    return QString::fromLocal8Bit(dateBuffer) + " " + QString::fromLocal8Bit(timeBuffer);
}

bool fixDirectoryName(QString& name) {
    QString temp = name.simplified();
    while (temp.endsWith('.'))
        temp.chop(1);

    temp.replace(QRegExp("[<>:\"/\\|?*]"), "");
    static QString invalidNames[] = {"CON",  "PRN",  "AUX",  "NUL",  "COM1", "COM2", "COM3", "COM4",
                                     "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3",
                                     "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
    for (unsigned int i = 0; i < sizeof(invalidNames) / sizeof(QString); ++i) {
        if (temp == invalidNames[i]) {
            temp = "";
            break;
        }
    }

    temp = temp.simplified();

    if (temp.length() > 1) {
        name = temp;
        return true;
    } else {
        return false;
    }
}

QString getDesktopDirectory() {
    LPWSTR path;
    HRESULT res = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path);
    if (res != S_OK) {
        qWarning() << "Couldn't get desktop - error " << res;
        throw std::runtime_error("Couldn't get path to desktop");
    }
    QString dir = QString::fromWCharArray(path);
    CoTaskMemFree(path);
    return dir;
}

QString getStartMenuDirectory() {
    LPWSTR path;
    HRESULT res = SHGetKnownFolderPath(FOLDERID_StartMenu, 0, nullptr, &path);
    if (res != S_OK) {
        qWarning() << "Couldn't get desktop - error " << res;
        throw std::runtime_error("Couldn't get path to desktop");
    }
    QString dir = QString::fromWCharArray(path);
    CoTaskMemFree(path);
    return dir;
}

bool shellDeleteQuiet(const QString& fileName, QWidget* dialog) {
    if (!QFile::remove(fileName)) {
        return shellDelete(QStringList(fileName), false, dialog);
    }
    return true;
}

QString readFileText(const QString& fileName, QString* encoding) {
    // the functions from QTextCodec we use are supposed to be reentrant so it's
    // safe to use statics for that
    static QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");

    QFile textFile(fileName);
    if (!textFile.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QByteArray buffer = textFile.readAll();
    QTextCodec* codec = QTextCodec::codecForUtfText(buffer, utf8Codec);
    QString text = codec->toUnicode(buffer);

    // check reverse conversion. If this was unicode text there can't be data loss
    // this assumes QString doesn't normalize the data in any way so this is a bit unsafe
    if (codec->fromUnicode(text) != buffer) {
        qDebug("conversion failed assuming local encoding");
        codec = QTextCodec::codecForLocale();
        text = codec->toUnicode(buffer);
    }

    if (encoding != nullptr) {
        *encoding = codec->name();
    }

    return text;
}

void removeOldFiles(const QString& path, const QString& pattern, int numToKeep, QDir::SortFlags sorting) {
    QFileInfoList files = QDir(path).entryInfoList(QStringList(pattern), QDir::Files, sorting);

    if (files.count() > numToKeep) {
        QStringList deleteFiles;
        for (int i = 0; i < files.count() - numToKeep; ++i) {
            deleteFiles.append(files.at(i).absoluteFilePath());
        }

        if (!shellDelete(deleteFiles)) {
            qWarning("failed to remove log files: %s", qPrintable(windowsErrorString(::GetLastError())));
        }
    }
}

QIcon iconForExecutable(const QString& filePath) {
    HICON winIcon;
    UINT res = ::ExtractIconExW(ToWString(filePath).c_str(), 0, &winIcon, nullptr, 1);
    if (res == 1) {
        QIcon result = QIcon(QtWin::fromHICON(winIcon));
        ::DestroyIcon(winIcon);
        return result;
    } else {
        return QIcon(":/MO/gui/executable");
    }
}

} // namespace MOBase
