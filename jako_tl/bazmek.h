#ifndef BAZMEK_H
#define BAZMEK_H

#include <vector>
#include <qstring.h>

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
    Zapojeni umisteni;

    float impedance(float f);
    int slozitost();
    float proveditelnost();

    void odstran_prebytecne();

    void nahodne();

    QString to_string();

    Bazmek(float hodnota_, Druh druh_, Zapojeni umisteni_);
};

#endif // BAZMEK_H
