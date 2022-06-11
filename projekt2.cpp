#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ASCII_SIZE 128
#define EMPTY 0
#define MAX_ZAKODOWANY_SIZE 1000000
#define MAX_REXT_SIZE 10000

using namespace std;


/*


Program powinien:

1. wczytywa� tekst z pliku

2. zapisywa� skompresowany tekst do pliku

 dodatkowo:

3. plik wyj�ciowy powinien zawiera� jawnie podany "s�ownik" oraz binarnie zapisany skompresowany tekst

4. w rozwi�zaniu nale�y u�y� w�asnej implementacji kolejki priorytetowej (wymagane jest uzycie kopcow)

5. u�yj takiego tekstu, aby plik skompresowany zajmowa� mniej miejsca

6. do rozwi�zania do��cz dwa pliki (przed i po kompresji)

7. projekt nale�y zaprezentowa� aby otrzyma� punkty.

*/
struct Wezel {
    char znak = NULL;
    int iloscWystapien;
    struct Wezel *lewyDzieciak = NULL;
    struct Wezel *prawyDzieciak = NULL;
    unsigned int kod;
    unsigned int maska;
};

struct Kod {
    char znak;
    unsigned int kod;
    unsigned int maska;
};

// ***************************** INIT *****************************************

void policzIloscWystapien(int iloscWystapien[], char text[], int sizeText) {
    for(int i=0; i<sizeText; i++) {
        iloscWystapien[text[i]] = iloscWystapien[text[i]]+1;
    }
}

int utworzWezly(struct Wezel wezly[],  int iloscWystapien[]) {
    int iloscWezlow = 0;
    for(int i=0; i< ASCII_SIZE; i++) {
        if(iloscWystapien[i] > 0) {
            struct Wezel znak;
            znak.znak = i;
            znak.iloscWystapien = iloscWystapien[i];
            wezly[iloscWezlow] = znak;
            iloscWezlow++;
        }
    }
    return iloscWezlow;
}

// ******************************* KOPCE **********************************
void zamien(struct Wezel wezly[], int i, int j) {
    Wezel temp = wezly[i];
    wezly[i] = wezly[j];
    wezly[j] = temp;
}

int lewyDzieciak(int rodzic) {
    return 2*rodzic + 1;
}

int prawyDzieciak(int rodzic) {
    return 2*rodzic + 2;
}

// *********** KOPIEC MIN *****************

bool jestMniejszyOd(struct Wezel a, struct Wezel b) {
    if(a.iloscWystapien < b.iloscWystapien) {
        return true;
    }
    return false;
}

void przywrocWlasnoscKopcaMin(struct Wezel wezly[], int iloscWezlow, int nrElementu) {
    int l = lewyDzieciak(nrElementu);
    int r = prawyDzieciak(nrElementu);
    int lowest;
    if(l < iloscWezlow && jestMniejszyOd(wezly[l], wezly[nrElementu])) {
        lowest = l;
    } else {
        lowest = nrElementu;
    }
    if(r < iloscWezlow && jestMniejszyOd(wezly[r], wezly[lowest])) {
        lowest = r;
    }
    if(lowest != nrElementu) {
        zamien(wezly, lowest, nrElementu);
        przywrocWlasnoscKopcaMin(wezly, iloscWezlow, lowest);
    }
}

void zbudujKopiecMin(struct Wezel wezly[], int iloscWezlow) {
    int rozmiarKopca = iloscWezlow;
    for(int i=iloscWezlow/2; i >= 0; i--) {
        przywrocWlasnoscKopcaMin(wezly, rozmiarKopca, i);
    }
}

// ****************************** KOLEJKA *************************************

struct Wezel zabierzMin(struct Wezel wezly[], int iloscWezlow) {
    zamien(wezly, 0, iloscWezlow-1);
    przywrocWlasnoscKopcaMin(wezly, iloscWezlow-1, 0);
    return wezly[iloscWezlow-1];
};

void dodajWezel(struct Wezel wezly[], int iloscWezlow, struct Wezel wezel) {
    wezly[iloscWezlow] = wezel;
    przywrocWlasnoscKopcaMin(wezly, iloscWezlow+1, iloscWezlow);
};

// **************************** KODY HUFFMANA *********************************

void przypiszKody(struct Wezel *root, struct Kod kody[], int *ileKodow) {
    unsigned int kodRoota = root->kod;
    unsigned int maskaRoota = root->maska;
    if(root->lewyDzieciak != NULL) {
        root->lewyDzieciak->kod = kodRoota*2;
        root->lewyDzieciak->maska = maskaRoota*2+1;
        przypiszKody(root->lewyDzieciak, kody, ileKodow);
    }
    if(root->prawyDzieciak != NULL) {
        root->prawyDzieciak->kod = kodRoota*2 + 1;
        root->prawyDzieciak->maska = maskaRoota*2+1;
        przypiszKody(root->prawyDzieciak, kody, ileKodow);
    }
    if(root->znak != NULL) {
        Kod kod;
        kod.znak = root->znak;
        kod.kod = root->kod;
        kod.maska = root->maska;
        kody[*ileKodow] = kod;
        *ileKodow = *ileKodow + 1;
    }
}

void kodyHuffmana(struct Wezel wezly[], int iloscWezlow, struct Wezel wezlyArchiwalne[], struct Kod kody[]) {
    int iloscWezlowArchiwalnych = 0;
    zbudujKopiecMin(wezly, iloscWezlow);
    int adresX, adresY;

    while(iloscWezlow > 1) {
        struct Wezel x = zabierzMin(wezly, iloscWezlow);
        iloscWezlow--;
        wezlyArchiwalne[iloscWezlowArchiwalnych] = x;
        adresX = iloscWezlowArchiwalnych;
        iloscWezlowArchiwalnych++;

        struct Wezel y = zabierzMin(wezly, iloscWezlow);
        iloscWezlow--;
        wezlyArchiwalne[iloscWezlowArchiwalnych] = y;
        adresY = iloscWezlowArchiwalnych;
        iloscWezlowArchiwalnych++;

        struct Wezel z;
        z.iloscWystapien = x.iloscWystapien + y.iloscWystapien;
        z.lewyDzieciak = wezlyArchiwalne + adresX;
        z.prawyDzieciak = wezlyArchiwalne + adresY;
        dodajWezel(wezly, iloscWezlow, z);
        iloscWezlow++;
    }

    struct Wezel root = wezly[0];
    root.kod = 0;
    root.maska = 0;

    int ileKodow = 0;
    przypiszKody(&root, kody, &ileKodow);
}

// ******************************** KODOWANIE *********************************

Kod* znajdzKod(char znak, struct Kod kody[]) {
    int i = 0;
    while(kody[i].znak != znak) {
        i++;
    }
    return kody + i;
 };

void dopiszKod(Kod *kod, bool zakodowane[], unsigned int *dlugoscZakodowana){
    unsigned int num = kod->kod;
    unsigned int maska = kod->maska;
    unsigned int uIntSize = sizeof(unsigned int);
    int i;
    bool bit, bitMaski;
    for(i = uIntSize*8-1; i>= 0; i--){
        bit = ((num >>i) & 1);
        bitMaski = ((maska>>i) & 1);
        if(bitMaski) {
            zakodowane[*dlugoscZakodowana] = bit;
            *dlugoscZakodowana = *dlugoscZakodowana + 1;
        }
    }
}

void koduj(char znak, struct Kod kody[], bool zakodowane[], unsigned int *dlugoscZakodowana) {
    Kod *kod = znajdzKod(znak, kody);
    dopiszKod(kod, zakodowane, dlugoscZakodowana);
}

void koduj(char text[], int sizeText, struct Kod kody[], int ileKodow, bool zakodowane[], unsigned int *dlugoscZakodowana) {
    for(int i=0; i<sizeText; i++) {
        koduj(text[i], kody, zakodowane, dlugoscZakodowana);
    }
    cout << endl << "zakodowane" << endl;
    for(int i=0; i<*dlugoscZakodowana; i++) {
        cout << zakodowane[i];
    }
     cout << endl << endl;
}

// ****************************** ODKODOWANIE *********************************

bool sprobujZnalezcKod(unsigned int potencjalnyKod, unsigned int potencjalnaMaska, struct Kod kody[], int ileKodow, char *znak) {
    for(int i=0; i<ileKodow; i++) {
        if(kody[i].kod == potencjalnyKod && kody[i].maska == potencjalnaMaska) {
            *znak = kody[i].znak;
            return true;
        }
    }
    return false;
}

int odkoduj(bool zakodowane[], int dlugoscZakodowana, struct Kod kody[], int ileKodow, char odkodowane[]) {
    unsigned int potencjalnyKod = 0;
    unsigned int potencjalnaMaska = 0;
    int dlugoscOdkodowana = 0;
    char znak;

    for(int i=0; i<dlugoscZakodowana; i++) {
        potencjalnyKod *= 2;
        potencjalnaMaska = potencjalnaMaska*2 + 1;
        if(zakodowane[i]) {
            potencjalnyKod++;
        }

        if(sprobujZnalezcKod(potencjalnyKod, potencjalnaMaska, kody, ileKodow, &znak)) {
            odkodowane[dlugoscOdkodowana] = znak;
            dlugoscOdkodowana++;
            potencjalnyKod = 0;
            potencjalnaMaska = 0;
        }
    }

    cout << "odkodowane" << endl;
    for(int i=0; i<dlugoscOdkodowana; i++) {
        cout << odkodowane[i];
    }
    cout << endl;
    return dlugoscOdkodowana;
}

// ****************************** OBSLUGA PLIKOW ******************************

int odczytajZPliku(char text[]) {
    char znak;
    FILE *myfile;
    myfile = fopen("original.txt", "r");
    int i = 0;
    if (myfile != NULL){
        do {
            znak = fgetc(myfile);
            if(znak==EOF) {
                return i;
            }
            if(!feof(myfile)) {
                text[i] = znak;
            i++;
            }
        } while(!feof(myfile));

        fclose(myfile);
    } else {
        cout << "Nie mozna odczytac pliku dane.txt";
    }
    return i;
}

void zapiszDoPlikuBinarnego(struct Kod kody[], int iloscKodow, bool zakodowane[], unsigned int dlugoscZakodowana) {
    FILE *myfile;
    myfile = fopen("skompresowane", "wb+");

    fwrite(&iloscKodow, sizeof(iloscKodow), 1, myfile);
    fwrite(kody, sizeof(struct Kod) * iloscKodow, 1, myfile);
    fwrite(zakodowane, sizeof(bool) * dlugoscZakodowana, 1, myfile);
    fclose(myfile);
}

void odczytajZPlikuBinarnego() {
    FILE *myfile;
    struct Kod kody[ASCII_SIZE];
    bool zakodowane[MAX_ZAKODOWANY_SIZE];
    bool bit;
    int ileKodow, zakodowaneSize;
    myfile = fopen("skompresowane", "rb");

    fseek(myfile, 0, SEEK_END);
    long fileSize = ftell(myfile);
    fseek(myfile, 0, SEEK_SET);

    if (myfile != NULL){
        cout << "odczyt pliku binarnego" << endl;
        int i = 0;
        fread(&ileKodow, sizeof(int), 1, myfile);
        cout << "ilosc kodow: " << ileKodow << endl;
        fread(&kody, sizeof(struct Kod) * ileKodow, 1, myfile);
        long pozycja = ftell(myfile);
        long ileBitowZostalo = fileSize - pozycja;
        zakodowaneSize = ileBitowZostalo;
        cout << "Rozmiar zakodowanego tekstu: " << zakodowaneSize << endl;
        for (int i=0; i<ileKodow; i++) {
            cout << kody[i].znak << " : " << kody[i].kod << "/" << kody[i].maska << endl;
        }
        fread(&zakodowane, ileBitowZostalo, 1, myfile);
        /*
        do {
            fread(&bit, sizeof(bool), 1, myfile);
            if(!feof(myfile)) {
                zakodowane[i] = bit;
                i++;
                cout << bit;
            }
        } while(!feof(myfile));
        */

        fclose(myfile);
    } else {
        cout << "Nie mozna odczytac pliku dane.txt";
        return;
    }

    fclose(myfile);

    char odkodowane[MAX_REXT_SIZE];
    odkoduj(zakodowane, zakodowaneSize, kody, ileKodow, odkodowane);
}

// ********************************** MAIN ************************************

void wczytajTextZakodujZapisz() {
    char text[MAX_REXT_SIZE];
    int sizeText = odczytajZPliku(text);

    int iloscWystapien[ASCII_SIZE];
    for(int c=0; c<=ASCII_SIZE-1; c++) {
        iloscWystapien[c] = 0;
    }

    policzIloscWystapien(iloscWystapien, text, sizeText);

    struct Wezel wezly[ASCII_SIZE * 2];
    struct Wezel wezlyArchiwalne[ASCII_SIZE * 2];
    struct Kod kody[ASCII_SIZE];
    int iloscWezlow = utworzWezly(wezly, iloscWystapien);

    zbudujKopiecMin(wezly, iloscWezlow);
    cout << text << endl;

    kodyHuffmana(wezly, iloscWezlow, wezlyArchiwalne, kody);

    bool zakodowane[MAX_ZAKODOWANY_SIZE];
    unsigned int dlugoscZakodowana = 0;
    koduj(text, sizeText, kody, iloscWezlow, zakodowane, &dlugoscZakodowana);
    zapiszDoPlikuBinarnego(kody, iloscWezlow, zakodowane, dlugoscZakodowana);
}

int main() {
    wczytajTextZakodujZapisz();
    odczytajZPlikuBinarnego();
}
