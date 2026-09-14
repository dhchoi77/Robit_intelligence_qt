#include "musiclibrary.h"
#include <algorithm>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

bool MusicLibrary::loadFromFile(const QString &path)
{
    m_tracks.clear();
    m_error.clear();

    QFile file(path);
    if (!file.exists()) {
        m_error = QStringLiteral("곡 목록 파일을 찾을 수 없습니다: %1").arg(path);
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = QStringLiteral("곡 목록 파일을 열 수 없습니다: %1").arg(path);
        return false;
    }

    const QByteArray raw = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        m_error = QStringLiteral("JSON 파싱 오류: %1").arg(parseError.errorString());
        return false;
    }
    if (!doc.isArray()) {
        m_error = QStringLiteral("JSON 최상위는 곡들의 배열이어야 합니다.");
        return false;
    }

    const QJsonArray arr = doc.array();
    for (const QJsonValue &v : arr) {
        if (!v.isObject())
            continue; // 형식이 이상한 항목은 건너뛴다
        const Track t = Track::fromJson(v.toObject());
        if (t.isValid())
            m_tracks.append(t);
    }

    if (m_tracks.isEmpty()) {
        m_error = QStringLiteral("유효한 곡이 하나도 없습니다.");
        return false;
    }
    return true;
}

Track MusicLibrary::trackById(int id) const
{
    for (const Track &t : m_tracks)
        if (t.id() == id)
            return t;
    return Track();
}

int MusicLibrary::nextId() const
{
    int maxId = 0;
    for (const Track &t : m_tracks)
        maxId = std::max(maxId, t.id());
    return maxId + 1;
}

void MusicLibrary::addTrack(const Track &t)
{
    if (t.isValid())
        m_tracks.append(t);
}

bool MusicLibrary::saveToFile(const QString &path)
{
    QJsonArray arr;
    for (const Track &t : m_tracks)
        arr.append(t.toJson());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_error = QStringLiteral("곡 목록 파일을 저장할 수 없습니다: %1").arg(path);
        return false;
    }
    const QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
