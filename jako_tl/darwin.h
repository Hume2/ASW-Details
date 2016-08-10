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

    void pridej_tvory(int pocet);

    void vypis_populaci();
};

#endif // DARWIN_H
