#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>

#include "darwin.h"

Darwin::Darwin() :
  populace(),
  generace(0),
  max_vyber(100),
  mutacni_index(20)
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
    std::cout << tvor->to_string().toUtf8().constData() << tvor->hodnota << " " << tvor->swr << " " << tvor->neprizpusobeno << " " << tvor->neudelatelno << std::endl;
  }
}

void Darwin::mutace() {
  for (std::vector<Bazmekarium>::iterator tvor = populace.begin(); tvor != populace.end() ; ++tvor) {
    //std::cout << "    " << tvor->to_string().toUtf8().constData() << std::endl;
    if (rand() % 100 < 5) {
      tvor->mutace(mutacni_index);
    }
  }
}

void Darwin::serad() {
  std::sort(populace.begin(), populace.end(), je_lepsi);
  populace.resize(std::min(max_vyber, int(populace.size())));
}

void Darwin::ohodnot() {
  for (std::vector<Bazmekarium>::iterator tvor = populace.begin(); tvor != populace.end() ; ++tvor) {
    tvor->ohodnot();
  }
}

void Darwin::krizeni() {
  for (int i = max_vyber / 2; i; i--) {
    int a = rand() % populace.size();
    int b = rand() % populace.size();
    if (a == b) continue;

    populace[a].krizeni(populace[b]);
  }
}

void Darwin::rozmnozuj() {
  for (int i = populace.size(); i; i--) {
    int a = rand() % populace.size();
    populace.push_back(populace[a]);
  }
}

bool Darwin::je_lepsi(const Bazmekarium &A, const Bazmekarium &B) {
  /*if (A.neudelatelno != B.neudelatelno) {
    return A.neudelatelno < B.neudelatelno;
  } else*/// if (A.neprizpusobeno != B.neprizpusobeno) {
  //  return A.neprizpusobeno < B.neprizpusobeno;
  //} else {
    return A.hodnota < B.hodnota;
  //}
}
