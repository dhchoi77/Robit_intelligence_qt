#ifndef RECOMMENDER_H
#define RECOMMENDER_H
#include <random>
#include <QVector>
#include <QSet>
#include "track.h"
#include "musicfeatures.h"
class MusicLibrary;

// 프로그램의 '두뇌'.
// 키워드로 취향 프로파일을 초기화하고, 단계마다 사용자의 선택으로
// 프로파일을 갱신한 뒤, 최종적으로 가장 취향에 가까운 곡들을 추천한다.
// (콘텐츠 기반 추천 + 온라인 프로파일 갱신 방식)
class Recommender
{
public:
    explicit Recommender(const MusicLibrary *library);

    // 키워드 선택 → 취향 프로파일 초기화 및 상태 리셋
    void start(const Features &keywordSeed);

    // 이번 단계에서 사용자에게 보여줄 곡 count개를 고른다.
    // 프로파일에 가까우면서도 서로는 다양하게 골라, 선택이 의미 있게 한다.
    QVector<Track> nextChoices(int count = 3);

    // 사용자가 고른 곡으로 취향 프로파일을 갱신한다.
    void choose(const Track &chosen);

    // 최종 추천 플레이리스트: 최종 프로파일에 가장 가까운 곡 count개.
    QVector<Track> finalPlaylist(int count = 6) const;

    // 현재 취향을 사람이 읽을 수 있는 문장으로 설명한다(발표/창의성용).
    QString explainProfile() const;

    Features profile() const { return m_profile; }

private:
    // 곡들을 프로파일과의 거리(가까운 순)로 정렬해 반환.
    QVector<Track> rankByCloseness(const QVector<Track> &pool) const;

    const MusicLibrary *m_library;
    Features m_profile;
    QSet<int> m_shownIds;  // 이미 보여준 곡(중복 방지)
    double m_alpha = 0.45; // 선택을 얼마나 강하게 반영할지 (0~1)
    std::mt19937 m_rng{std::random_device{}()};  // ← 추가: 시드곡 무작위 선택용
};

#endif // RECOMMENDER_H
