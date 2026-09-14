#include "track.h"

Track Track::fromJson(const QJsonObject &obj)
{
    Track t;
    t.m_id             = obj.value("id").toInt(-1);
    t.m_title          = obj.value("title").toString(QStringLiteral("(제목 없음)"));
    t.m_composer       = obj.value("composer").toString(QStringLiteral("(작곡가 미상)"));
    t.m_filePath       = obj.value("file").toString();
    t.m_previewStartMs = static_cast<qint64>(obj.value("previewStart").toDouble(0));
    t.m_features       = Features::fromJson(obj.value("features").toObject());
    return t;
}

QJsonObject Track::toJson() const
{
    QJsonObject o;
    o["id"]           = m_id;
    o["title"]        = m_title;
    o["composer"]     = m_composer;
    o["file"]         = m_filePath;
    o["previewStart"] = static_cast<double>(m_previewStartMs);
    o["features"]     = m_features.toJson();
    return o;
}
