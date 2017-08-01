#ifndef DARWIN_H
#define DARWIN_H

#include <stdio.h>
#include <vector>

#include "bazmekarium.h"

class Darwin
{
  public:
    Darwin();

    std::vector<Bazmekarium> populace;

    int generace;
    int max_vyber;
    int mutacni_index;

    void pridej_tvory(int pocet);
    void mutace();
    void krizeni();
    void rozmnozuj();
    void serad();
    void ohodnot();

    void vypis_populaci();

    static bool je_lepsi(const Bazmekarium& A, const Bazmekarium& B);
};

#endif // DARWIN_H
