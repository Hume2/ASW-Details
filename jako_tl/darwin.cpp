#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>

#include "darwin.h"

Darwin::Darwin() :
  populace(),
  generace(0),
  max_vyber(100)
{
}

void Darwin::pridej_tvory(int pocet) {
  for (int i = pocet; i; i--) {
    Bazmekarium tvor;
    tvor.nahodne();
    populace.push_back(tvor);
  }
}

void Darwin::vypis_populaci() {
  std::cout << "Výpis " << generace << ". generace:" << std::endl;
  for (std::vector<Bazmekarium>::iterator tvor = populace.begin(); tvor != populace.end() ; ++tvor) {
    std::cout << tvor->to_string().toUtf8().constData() << std::endl;
  }
}
