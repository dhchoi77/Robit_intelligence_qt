#include "features.h"
#include <algorithm>
#include <cmath>

Features::Features(double t, double m, double e, double w, double er)
    : tempo(t), mood(m), energy(e), weight(w), era(er)
{
}

double Features::clamp01(double v)
{
    return std::max(0.0, std::min(1.0, v));
}

double Features::distanceTo(const Features &o) const
{
    const double dt = tempo  - o.tempo;
    const double dm = mood   - o.mood;
    const double de = energy - o.energy;
    const double dw = weight - o.weight;
    const double dr = era    - o.era;
    return std::sqrt(dt * dt + dm * dm + de * de + dw * dw + dr * dr);
}

Features Features::blend(const Features &target, double alpha) const
{
    const double a = clamp01(alpha);
    return Features(
        (1.0 - a) * tempo  + a * target.tempo,
        (1.0 - a) * mood   + a * target.mood,
        (1.0 - a) * energy + a * target.energy,
        (1.0 - a) * weight + a * target.weight,
        (1.0 - a) * era    + a * target.era);
}

Features Features::fromJson(const QJsonObject &obj)
{
    Features f;
    f.tempo  = clamp01(obj.value("tempo").toDouble(0.5));
    f.mood   = clamp01(obj.value("mood").toDouble(0.5));
    f.energy = clamp01(obj.value("energy").toDouble(0.5));
    f.weight = clamp01(obj.value("weight").toDouble(0.5));
    f.era    = clamp01(obj.value("era").toDouble(0.5));
    return f;
}

QJsonObject Features::toJson() const
{
    QJsonObject o;
    o["tempo"]  = tempo;
    o["mood"]   = mood;
    o["energy"] = energy;
    o["weight"] = weight;
    o["era"]    = era;
    return o;
}
