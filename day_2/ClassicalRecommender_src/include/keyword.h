#ifndef KEYWORD_H
#define KEYWORD_H

#include <QString>
#include <QVector>
#include "musicfeatures.h"

// 시작 화면에서 고르는 8개의 키워드.
// 각 키워드는 취향 공간에서의 '출발점(seed)'을 뜻한다.
struct Keyword
{
    QString name;
    QString description;
    Features seed;
};

// 8개의 기본 키워드 목록을 돌려준다.
QVector<Keyword> defaultKeywords();

#endif // KEYWORD_H
