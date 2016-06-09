/*************************************************************************************************************************
Nom ......... : Arduino_Alarme_MAE.ino
Role ........ : Projet d'alarme d'habitation comportant trois entrées TOR sous forme de capteurs : Infra-rouge (IR),
                contact de porte type Reed (ILS) et interrupeteur d'armement/désarmement (MA). Deux actionneurs TOR :
                Une  sirène (SIR) et un gyrophare (GYR).
                Le fonctionnement est le suivant : Une fois l'alarme armée MA à "1", le passage à "1" de IR ou ILS provoque
                le passage à "1" de SIR et GYR restant actif jusqu'au retour à "0" de MA.
                Le système fonctionne grâce à une machine à états avec actions de type actions de mise à zéro explicites (AMZE)
Auteur ...... : J.Serrand
Mail ........ : joris.serrand@rascol.net
Version ..... : V0 du 09/06/16
Licence ..... : Copyright (C) 2016  Joris SERRAND

                This program is free software: you can redistribute it and/or modify
                it under the terms of the GNU General Public License as published by
                the Free Software Foundation, either version 3 of the License, or
                (at your option) any later version.

                This program is distributed in the hope that it will be useful,
                but WITHOUT ANY WARRANTY; without even the implied warranty of
                MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                GNU General Public License for more details.

                You should have received a copy of the GNU General Public License
                along with this program.  If not, see <http://www.gnu.org/licenses/>

Compilation . : Avec l'IDE Arduino
**************************************************************************************************************************/

#include <Arduino.h>

//Déclaration des constantes pour les nums de pins des capteurs et actionneurs
const int pin_IR=2;
const int pin_ILS=3;
const int pin_MA=4;
const int pin_SIR=5;
const int pin_GYR=6;

//Déclaration des variables pour les états des capteurs et actionneurs
boolean IR=false;
boolean ILS=false;
boolean MA=false;
boolean Set_SIR=false;
boolean Reset_SIR=false;
boolean Set_GYR=false;
boolean Reset_GYR=false;

//Déclaration de la variable d'évolution
unsigned char etat_courant;

//Fonction de lecture et mémorisation des entrées
void lecture_memorisation_entrees(void)
{
  IR=digitalRead(pin_IR);
  ILS=digitalRead(pin_ILS);
  MA=digitalRead(pin_MA);
}

//Fonction de description de l'évolution de la MAE
unsigned char evolution(void)
{
  unsigned char etat_s;

  switch(etat_courant)
  {
    case 0: if(MA)
              etat_s=1;
            else
              etat_s=0;
            break;

    case 1: if(ILS+IR)
              etat_s=2;
            else
            {
              if(!MA)
                etat_s=0;
              else
                etat_s=1;
            }
            break;

    case 2: if(!MA)
              etat_s=0;
            else
              etat_s=2;
            break;

    default: etat_s=0;
  }
  return(etat_s);
}


//Fonction de codage des actions sur état
void actions_sur_etat(void)
{
  //Actions Set SIR et Set GYR
  switch(etat_courant)
  {
    case 2: Set_GYR=true;
            Set_SIR=true;
            break;

    default: Set_GYR=false;
             Set_SIR=false;
             break;

  }

  //Actions Reset SIR et Reset GYR
  switch(etat_courant)
  {
    case 0: Reset_GYR=true;
            Reset_SIR=true;
            break;

    default: Reset_GYR=false;
             Reset_SIR=false;
             break;

  }

  //Actions sur les pins SIR et GYR
  if(Set_GYR)
    digitalWrite(pin_GYR,HIGH);
  else
    {
      if(Reset_GYR)
        digitalWrite(pin_GYR,LOW);
    }

  if(Set_SIR)
    digitalWrite(pin_SIR,HIGH);
  else
    {
      if(Reset_SIR)
        digitalWrite(pin_SIR,LOW);
    }

}


//Fonction d'évolution de la MAE
void traitement_MAE(void)
{
  unsigned char etat_suivant;
  lecture_memorisation_entrees();
  etat_suivant=evolution();
  etat_courant=etat_suivant;
  actions_sur_etat();
}


//Programme principal
void loop(void)
{
  etat_courant=0;

  while(1)
  {
    traitement_MAE();
  }
}
