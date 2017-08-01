#ifndef DARWIN2_H
#define DARWIN2_H

#include <stdio.h>
#include <vector>
#include <string>

//#include <complex>

class Darwin2
{
  public:
    class Bazmek{
      public:
        std::string houby;
        float swr, hodnota;
        Bazmek() :
          houby(),
          swr(-1),
          hodnota(0)
        { }

        std::string lidsky();
        void ohodnot();
        void transformuj(float& Zr, float& Zi, float f);
        float dej_cast(unsigned int& i, bool paralel, float f);
        void uzavri();
    };

    class Zadani {
      public:
        Zadani(float Zlr_, float Zli_, float f_) :
          Zlr(Zlr_),
          Zli(Zli_),
          f(f_)
        { }
        float Zlr, Zli, f;
    };

    static float kondik(float c, float f);
    static float civka(float l, float f);
    static float paralelne(float a, float b);
    static float seriove(float a, float b);

  public:
    Darwin2();

    std::vector<Bazmek> populace;

    static std::vector<Zadani> zadani;

    int generace;
    int max_vyber;
    int mutacni_index;

    static bool zpusob_vyberu;

    void pridej_tvory(int pocet);
    void mutace();
    void krizeni();
    void rozmnozuj();
    void serad();
    void ohodnot();

    void vypis_populaci();

    void vyres(int zac);
    void nastav_druhe_kolo();

    static bool je_lepsi(const Bazmek& A, const Bazmek& B);

    static float spocitej_swr(float Zlr, float Zli, float ref);
};

#endif // DARWIN2_H
