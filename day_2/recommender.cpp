#include "recommender.h"
#include "musiclibrary.h"
#include <algorithm>
#include <limits>

Recommender::Recommender(const MusicLibrary *library)
    : m_library(library)
{
}

void Recommender::start(const Features &keywordSeed)
{
    m_profile = keywordSeed; // 키워드가 정한 출발점에서 시작
    m_shownIds.clear();
}

QVector<Track> Recommender::rankByCloseness(const QVector<Track> &pool) const
{
    QVector<Track> sorted = pool;
    std::sort(sorted.begin(), sorted.end(),
              [this](const Track &a, const Track &b) {
                  return a.features().distanceTo(m_profile)
                       < b.features().distanceTo(m_profile);
              });
    return sorted;
}

QVector<Track> Recommender::nextChoices(int count)
{
    // 1) 아직 안 보여준 곡만 후보로 모은다.
    QVector<Track> pool;
    for (const Track &t : m_library->tracks())
        if (!m_shownIds.contains(t.id()))
            pool.append(t);

    if (pool.isEmpty())
        return {};

    // 2) 프로파일에 가까운 순으로 정렬한 뒤, 후보군을 '넉넉히' 잡는다.
    //    K를 count보다 크게(기본 9개) 잡아, 매번 똑같은 곡만 나오지 않게 한다.
    QVector<Track> ranked = rankByCloseness(pool);
    const int k = std::min<int>(ranked.size(), std::max(count * 3, 9)); // count=3 → 상위 9곡
    QVector<Track> candidates = ranked.mid(0, k);

    // 3) 시드곡(첫 곡)을 후보군에서 '무작위로' 하나 고른다.
    //    → 항상 가장 가까운 곡만 나오던 지루함을 없앤다. (여전히 취향에 가까운 9곡 안에서만 뽑음)
    QVector<Track> picked;
    if (!candidates.isEmpty()) {
        std::uniform_int_distribution<int> dist(0, candidates.size() - 1);
        const int seedIdx = dist(m_rng);
        picked.append(candidates[seedIdx]);
        candidates.removeAt(seedIdx);
    }

    // 4) 나머지 곡은 기존과 동일하게 max-min 다양성으로 채운다.
    //    이미 고른 곡들과 '가장 여유롭게 떨어진' 곡을 하나씩 추가한다.
    while (picked.size() < count && !candidates.isEmpty()) {
        int bestIdx = 0;
        double bestMinDist = -1.0;
        for (int i = 0; i < candidates.size(); ++i) {
            double minDist = std::numeric_limits<double>::max();
            for (const Track &p : picked)
                minDist = std::min(minDist,
                                   candidates[i].features().distanceTo(p.features()));
            if (minDist > bestMinDist) {
                bestMinDist = minDist;
                bestIdx = i;
            }
        }
        picked.append(candidates[bestIdx]);
        candidates.removeAt(bestIdx);
    }

    // 5) 보여준 곡으로 기록해 다음 단계에서 중복되지 않게 한다.
    for (const Track &t : picked)
        m_shownIds.insert(t.id());

    return picked;
}

void Recommender::choose(const Track &chosen)
{
    if (!chosen.isValid())
        return;
    // 취향 프로파일을 고른 곡 쪽으로 alpha 만큼 끌어당긴다.
    m_profile = m_profile.blend(chosen.features(), m_alpha);
}

QVector<Track> Recommender::finalPlaylist(int count) const
{
    QVector<Track> ranked = rankByCloseness(m_library->tracks());
    return ranked.mid(0, std::min<int>(count, ranked.size()));
}

QString Recommender::explainProfile() const
{
    auto pick = [](double v, const QString &lo, const QString &mid, const QString &hi) {
        if (v < 0.4) return lo;
        if (v > 0.6) return hi;
        return mid;
    };
    const QString tempoW  = pick(m_profile.tempo,  QStringLiteral("느린"),           QStringLiteral("보통 빠르기의"),   QStringLiteral("빠른"));
    const QString moodW   = pick(m_profile.mood,   QStringLiteral("어둡고 서정적인"), QStringLiteral("중립적인 분위기의"), QStringLiteral("밝고 경쾌한"));
    const QString energyW = pick(m_profile.energy, QStringLiteral("차분한"),          QStringLiteral("적당한 에너지의"),  QStringLiteral("강렬한"));
    const QString weightW = pick(m_profile.weight, QStringLiteral("가볍고 아담한"),    QStringLiteral("균형 잡힌"),       QStringLiteral("웅장한 관현악의"));

    return QStringLiteral("당신은 %1 · %2 · %3 · %4 곡에 끌리는 것 같아요.")
            .arg(tempoW).arg(moodW).arg(energyW).arg(weightW);
}
