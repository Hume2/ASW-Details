#ifndef BAZMEKARIUM_H
#define BAZMEKARIUM_H

#include <list>
#include <vector>
#include <qstring.h>

#include "bazmek.h"

class Bazmekarium
{
  public:
    Bazmekarium();

    class Zadani {
      public:
        Zadani(float Zlr_, float Zli_, float f_) :
          Zlr(Zlr_),
          Zli(Zli_),
          f(f_)
        { }
        float Zlr, Zli, f;
    };

    std::list<Bazmek> bazmeky;
    static std::vector<Zadani> zadani;
    static int max_slozitost;
    double hodnota;
    int neprizpusobeno;
    int neudelatelno;
    float swr;
    float vzdalenost;

    void spocitej_impedanci(float& Zlr, float& Zli, float f);
    float spocitej_swr(float Zlr, float Zli, float f);
    double spocitej_uzitecnost();
    int spocitej_slozitost();
    float spocitej_proveditelnost();
    double ohodnot();

    void nahodne();
    void mutace(float m);
    void krizeni(Bazmekarium& B);

    QString to_string();

    void odstran_zbytecne();
};

#endif // BAZMEKARIUM_H
