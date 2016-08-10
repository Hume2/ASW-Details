#ifndef BAZMEKARIUM_H
#define BAZMEKARIUM_H

#include <vector>

#include "bazmek.h"

class Bazmekarium
{
  public:
    Bazmekarium();

    std::vector<Bazmek> bazmeky;
    static std::vector<float> kmitocty;
    static int max_slozitost;

    float spocitej_swr(float Zlr, float Zli, float f);
    float spocitej_uzitecnost(float Zlr, float Zli);
    int spocitej_slozitost();
    float spocitej_proveditelnost();
    float ohodnot(float Zlr, float Zli);

    void odstran_zbytecne();
};

#endif // BAZMEKARIUM_H
