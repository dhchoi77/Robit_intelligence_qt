#ifndef FEATURES_H
#define FEATURES_H

#include <QJsonObject>

// 곡의 '느낌'을 5개의 숫자 축으로 표현하는 특성 벡터.
// 모든 값은 0.0 ~ 1.0 범위로 정규화된다. 추천 알고리즘의 기본 단위다.
class Features
{
public:
    double tempo  = 0.5; // 0 = 매우 느림,    1 = 매우 빠름
    double mood   = 0.5; // 0 = 어둡고 서정적,  1 = 밝고 경쾌
    double energy = 0.5; // 0 = 차분함,        1 = 강렬함
    double weight = 0.5; // 0 = 가벼움/독주,    1 = 웅장함/관현악
    double era    = 0.5; // 0 = 바로크(옛날),   1 = 근현대

    Features() = default;
    Features(double t, double m, double e, double w, double er);

    // 두 특성 벡터 사이의 유클리드 거리. 작을수록 '느낌이 비슷'하다.
    double distanceTo(const Features &other) const;

    // 현재 벡터를 target 쪽으로 alpha(0~1)만큼 끌어당긴 새 벡터를 반환.
    // 사용자가 곡을 고를 때 취향 프로파일을 갱신하는 데 쓴다.
    Features blend(const Features &target, double alpha) const;

    // JSON 객체에서 특성 벡터를 읽어온다. 누락된 값은 0.5로 채운다.
    static Features fromJson(const QJsonObject &obj);

    // 특성 벡터를 JSON 객체로 변환한다(저장용).
    QJsonObject toJson() const;

private:
    static double clamp01(double v);
};

#endif // FEATURES_H
