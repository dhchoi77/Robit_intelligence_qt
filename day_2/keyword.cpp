#include "keyword.h"

QVector<Keyword> defaultKeywords()
{
    // Features(tempo, mood, energy, weight, era)
    return {
        { QStringLiteral("집중"),   QStringLiteral("작업할 때 방해되지 않는 곡"), Features(0.45, 0.55, 0.30, 0.35, 0.45) },
        { QStringLiteral("잔잔함"), QStringLiteral("느리고 편안한 곡"),           Features(0.20, 0.55, 0.20, 0.30, 0.40) },
        { QStringLiteral("웅장함"), QStringLiteral("스케일이 큰 관현악"),         Features(0.55, 0.55, 0.85, 0.95, 0.55) },
        { QStringLiteral("활기참"), QStringLiteral("빠르고 경쾌한 곡"),           Features(0.85, 0.85, 0.75, 0.55, 0.45) },
        { QStringLiteral("슬픔"),   QStringLiteral("어둡고 서정적인 곡"),         Features(0.30, 0.15, 0.35, 0.45, 0.50) },
        { QStringLiteral("몽환"),   QStringLiteral("꿈결 같고 나른한 곡"),        Features(0.30, 0.50, 0.30, 0.40, 0.75) },
        { QStringLiteral("긴장"),   QStringLiteral("어둡고 격렬한 곡"),           Features(0.75, 0.20, 0.90, 0.75, 0.55) },
        { QStringLiteral("밝음"),   QStringLiteral("맑고 화사한 곡"),             Features(0.60, 0.90, 0.55, 0.45, 0.40) },
    };
}
