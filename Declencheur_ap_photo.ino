/* Sample using LiquidCrystal library */
#include <LiquidCrystal.h>

/* define some values used by the panel and buttons */
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

/* déclaration de la broche du relais */
#define PIN_RELAIS 52

/* Machine a etat */
typedef enum
{
  AFFICHE_ETAT,
  AFFICHE_MENU,
  AFFICHE_ON_OFF,
  /* Duree prise*/
  AFFICHE_DUREE_PRISE,
  /* Temps entre 2 photo*/
  AFFICHE_DUREE_ENR,
  /* Nombre de prises*/
  AFFICHE_NB_PRISES,
  /* Duree du declencheur */
  AFFICHE_DUREE_DECL,
} STATE_AFFICHE;

typedef enum
{
  IDLE,
  PREND_PHOTO,
  ATTEND_DECLENCHEUR,
  ATTEND_DUREE_PRISE,
  ATTEND_DUREE_ENREGISTREMENT
} STATE_PHOTO;


/* Déclaration machine à état */
STATE_AFFICHE t_state_machine;

/* Machine à état pour les photos */
STATE_PHOTO t_state_photo;

/* Numero de la prise courrante */
int numPrise;
/* Nombre total de prise  */
int nbPrises;
/* duree prise */
int dureePrise;
/* Duree de l'enregistrement de la photo */
int dureeEnregistrement;
/* duree du declencheur */
int dureeDeclencheur;

/* Flag pour indiquer si le sequentiel est lance*/
int onOff;

/* Déclaration de l'écran */
/* select the pins used on the LCD panel */
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/* valeur des bouton */
int button;

/* chaine pour l'affichage */
char char2Print[5];

/* valeur de la ligne a afficher dans le menu */
int ligneMenu;

/* read the buttons */
int read_LCD_buttons()
{
  int adc_key_in  = 0;
  int retour = btnNONE;
  /* read the value from the sensor */
  adc_key_in = analogRead(0);
  /* The buttons when read are centered at these valies: 0, 144, 329, 504, 741
    we add approx 50 to those values and check to see if we are close */
  /* We make this the 1st option for speed reasons since it will be the most likely result */
  if (adc_key_in > 800)
  {
    retour = btnNONE;
  }
  else if (adc_key_in < 50)
  {
    retour = btnRIGHT;
  }
  else if (adc_key_in < 150)
  {
    retour = btnUP;
  }
  else if (adc_key_in < 300)
  {
    retour = btnDOWN;
  }
  else if (adc_key_in < 450)
  {
    retour = btnLEFT;
  }
  else /*if (adc_key_in < 700)*/
  {
    retour = btnSELECT;
  }
  return retour;
}

void affichage (void)
{
  int reste;
  lcd.clear();
  /* Lecture des boutons */
  button = read_LCD_buttons();
  lcd.setCursor(0, 0);
  switch (t_state_machine)
  {
    case AFFICHE_ETAT:
      lcd.print("ON:");
      lcd.print(onOff);
      lcd.print(" Photo:");
      lcd.print(numPrise);
      lcd.setCursor(0, 1);
      lcd.print("Reste :");
      reste = nbPrises - numPrise ;
      lcd.print(reste);
      if (btnSELECT == button)
      {
        t_state_machine = AFFICHE_MENU;
      }
      break;
    case AFFICHE_MENU:
      switch (ligneMenu)
      {
        case 0:
          lcd.print (">Sel nb photo");
          lcd.setCursor(0, 1);
          lcd.print(" Duree photo");
          /* si boutton bas ou haut on change la ligne du menu */
          /* si boutton droite on rentre dans le menu */
          /* si boutton gauche on sort dans le menu */
          if (btnDOWN == button) {
            ligneMenu = 1;
          } else if (btnRIGHT == button)
          {
            t_state_machine = AFFICHE_NB_PRISES;
          } else if (btnLEFT == button)
          {
            t_state_machine = AFFICHE_ETAT;
          } else if (btnUP == button)
          {
            ligneMenu = 4;
          } else
          {
            /* nothing to do */
          }
          break;

        case 1:
          lcd.print (">Duree photo");
          lcd.setCursor(0, 1);
          lcd.print(" Duree enregistrement");
          /* si boutton bas ou haut on change la ligne du menu */
          /* si boutton droite on rentre dans le menu */
          /* si boutton gauche on sort dans le menu */
          if (btnDOWN == button) {
            ligneMenu = 2;
          } else if (btnRIGHT == button)
          {
            t_state_machine = AFFICHE_DUREE_PRISE;
          } else if (btnLEFT == button)
          {
            t_state_machine = AFFICHE_ETAT;
          } else if (btnUP == button)
          {
            ligneMenu = 0;
          } else
          {
            /* nothing to do */
          }
          break;
        case 2:
          lcd.print (">Duree enregistrement");
          lcd.setCursor(0, 1);
          lcd.print(" Duree declencheur");
          /* si boutton bas ou haut on change la ligne du menu */
          /* si boutton droite on rentre dans le menu */
          /* si boutton gauche on sort dans le menu */
          if (btnDOWN == button) {
            ligneMenu = 3;
          } else if (btnRIGHT == button)
          {
            t_state_machine = AFFICHE_DUREE_ENR;
          } else if (btnLEFT == button)
          {
            t_state_machine = AFFICHE_ETAT;
          } else if (btnUP == button)
          {
            ligneMenu = 1;
          } else
          {
            /* nothing to do */
          }
          break;
        case 3:
          lcd.print (">Duree declencheur");
          lcd.setCursor(0, 1);
          lcd.print(" ON/OFF");
          /* si boutton bas ou haut on change la ligne du menu */
          /* si boutton droite on rentre dans le menu */
          /* si boutton gauche on sort dans le menu */
          if (btnDOWN == button) {
            ligneMenu = 4;
          } else if (btnRIGHT == button)
          {
            t_state_machine = AFFICHE_DUREE_DECL;
          } else if (btnLEFT == button)
          {
            t_state_machine = AFFICHE_ETAT;
          } else if (btnUP == button)
          {
            ligneMenu = 2;
          } else
          {
            /* nothing to do */
          }
          break;
        case 4:
          lcd.print(">ON / OFF");
          lcd.setCursor(0, 1);
          lcd.print(" Sel nb photo");
          if (btnDOWN == button) {
            ligneMenu = 0;
          } else if (btnUP == button) {
            ligneMenu = 3;
          } else if (btnRIGHT == button)
          {
            t_state_machine = AFFICHE_ON_OFF;
          } else if (btnLEFT == button)
          {
            t_state_machine = AFFICHE_ETAT;
          } else
          {
            /* nothing to do */
          }
          break;
        default:
          ligneMenu = 0;
          break;
      }
      break;
    case AFFICHE_ON_OFF:
      lcd.print("ON / OFF");
      lcd.setCursor(0, 1);
      lcd.print(onOff);
      if ((btnDOWN == button) || (btnUP == button))
      {
        onOff = !onOff;
      } else if (btnLEFT == button)
      {
        if (1 == onOff)
        {
          t_state_machine = AFFICHE_ETAT;
          t_state_photo = PREND_PHOTO;
          numPrise = 0;
        } else
        {
          t_state_machine = AFFICHE_ETAT;
        }
      }
      break;
    case AFFICHE_DUREE_PRISE:
      lcd.print("Duree Prise (s)");
      lcd.setCursor(0, 1);
      lcd.print (dureePrise);
      if (btnLEFT ==  button)
      {
        t_state_machine = AFFICHE_MENU;
      } else if ((btnDOWN == button) && (dureePrise > 0))
      {
        dureePrise = dureePrise - 1;
      } else if ((btnUP == button) && (dureePrise < 120))
      {
        dureePrise = dureePrise + 1;
      }
      break;
    case AFFICHE_DUREE_ENR:
      lcd.print("Duree Enre (s)");
      lcd.setCursor(0, 1);
      lcd.print (dureeEnregistrement);
      if (btnLEFT ==  button)
      {
        t_state_machine = AFFICHE_MENU;
      } else if ((btnDOWN == button) && (dureeEnregistrement > 0))
      {
        dureeEnregistrement = dureeEnregistrement - 1;
      } else if ((btnUP == button) && (dureeEnregistrement < 120))
      {
        dureeEnregistrement = dureeEnregistrement + 1;
      }
      break;
    case AFFICHE_NB_PRISES:
      lcd.print("NB Prises");
      lcd.setCursor(0, 1);
      lcd.print (nbPrises);
      if (btnLEFT ==  button)
      {
        t_state_machine = AFFICHE_MENU;
      } else if ((btnDOWN == button) && (nbPrises > 0))
      {
        nbPrises = nbPrises - 1;
      } else if ((btnUP == button) && (nbPrises < 120))
      {
        nbPrises = nbPrises + 1;
      }
      break;
    case AFFICHE_DUREE_DECL:
      lcd.print("Duree decl (s)");
      lcd.setCursor(0, 1);
      lcd.print (dureeDeclencheur);
      if (btnLEFT ==  button)
      {
        t_state_machine = AFFICHE_MENU;
      } else if ((btnDOWN == button) && (dureeDeclencheur > 0))
      {
        dureeDeclencheur = dureeDeclencheur - 1;
      } else if ((btnUP == button) && (dureeDeclencheur < 20))
      {
        dureeDeclencheur = dureeDeclencheur + 1;
      }
      break;
    default:
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Demarrage");
  delay(1000);
  //etatRelais = LOW;
  pinMode(PIN_RELAIS, OUTPUT);

  /* mise a zero de tout les compteurs */
  numPrise = 0;
  nbPrises = 0;
  dureePrise = 30; /* valeur par defaut 30s */
  dureeEnregistrement = 1; /* valeur par defaut 1s */
  dureeDeclencheur = 0;
  onOff = 0;
  ligneMenu = 0;

  /* mise a zero de la chaine de caracteres. je le fais caracteres par caracteres pour aller plus vite */
  char2Print[0] = 0;
  char2Print[1] = 0;
  char2Print[2] = 0;
  char2Print[3] = 0;
  char2Print[4] = 0;

  /* initialisation des variables */
  t_state_machine = AFFICHE_ETAT;
  t_state_photo = IDLE;
}

void loop() {
  int delaisEnMs;
  /* durée cycle */
  //date_debut = millis();

  if ( 1 == onOff)
  {
    /* traitement série de photo en cours */
    switch (t_state_photo)
    {
      /* photo */
      case PREND_PHOTO:
        digitalWrite(PIN_RELAIS, HIGH);
        delay(100);
        digitalWrite(PIN_RELAIS, LOW);
        t_state_photo = ATTEND_DECLENCHEUR;
        break;
      /* attente du déclencheur */
      case ATTEND_DECLENCHEUR:
        delaisEnMs = dureeDeclencheur * 1000;
        delay(delaisEnMs);
        t_state_photo = ATTEND_DUREE_PRISE;
        break;
      /* attente de la duree de la photo */
      case ATTEND_DUREE_PRISE:
        delaisEnMs = dureePrise * 1000;
        delay(delaisEnMs);
        t_state_photo = ATTEND_DUREE_ENREGISTREMENT;
        break;
      /* attente enregistrement */
      case ATTEND_DUREE_ENREGISTREMENT:
        delaisEnMs = dureeEnregistrement * 1000;
        delay(delaisEnMs);
        numPrise ++;
        if (numPrise > nbPrises) {
          /* on a fini */
          onOff = 0;
          t_state_photo = IDLE;

        }
        else
        {
          t_state_photo = PREND_PHOTO;
        }
        break;
      default:
        t_state_photo = IDLE;
        delay(100);
        break;
    }

  }
  else
  {
    /* nothing to do */
    delay(150);
  }
  affichage();

}
