#ifndef BAZMEK_H
#define BAZMEK_H

#include <vector>

class Bazmek
{
  public:
    Bazmek();

    std::vector<Bazmek> b;
    float hodnota;

    enum Druh{CIVKA, KONDENZATOR};
    Druh druh;

    enum Zapojeni{SERIOVE, PARALELNI};
    Zapojeni zapojeni;

    float impedance(float f);
    int slozitost();
};

#endif // BAZMEK_H
