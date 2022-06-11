#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ASCII_SIZE 128
#define EMPTY 0
#define MAX_ZAKODOWANY_SIZE 1000000

using namespace std;


/*


Program powinien:

1. wczytywaæ tekst z pliku

2. zapisywaæ skompresowany tekst do pliku

 dodatkowo:

3. plik wyjœciowy powinien zawieraæ jawnie podany "s³ownik" oraz binarnie zapisany skompresowany tekst

4. w rozwi¹zaniu nale¿y u¿yæ w³asnej implementacji kolejki priorytetowej (wymagane jest uzycie kopcow)

5. u¿yj takiego tekstu, aby plik skompresowany zajmowa³ mniej miejsca

6. do rozwi¹zania do³¹cz dwa pliki (przed i po kompresji)

7. projekt nale¿y zaprezentowaæ aby otrzymaæ punkty.

*/
struct Wezel {
    char znak = NULL;
    int iloscWystapien;
    struct Wezel *lewyDzieciak = NULL;
    struct Wezel *prawyDzieciak = NULL;
    int kod;
};

struct Kod {
    char znak;
    unsigned int kod;
};

void print(struct Wezel wezly[], int iloscWezlow) {
    for(int i=0; i< iloscWezlow; i++) {
        cout << wezly[i].znak << wezly[i].iloscWystapien << endl;
    }
}

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
    if(root->lewyDzieciak != NULL) {
        root->lewyDzieciak->kod = 2*kodRoota;
        przypiszKody(root->lewyDzieciak, kody, ileKodow);
    }
    if(root->prawyDzieciak != NULL) {
        root->prawyDzieciak->kod = 2*kodRoota + 1;
        przypiszKody(root->prawyDzieciak, kody, ileKodow);
    }
    if(root->znak != NULL) {
        Kod kod;
        kod.znak = root->znak;
        kod.kod = root->kod;
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

    int ileKodow = 0;
    przypiszKody(&root, kody, &ileKodow);
    cout << "ile kodow: " << ileKodow << endl;
    for(int i=0; i<ileKodow; i++) {
        cout << kody[i].znak << " : " << kody[i].kod << endl;
    }
}

// ******************************** KODOWANIE *********************************

unsigned int znajdzKod(char znak, struct Kod kody[]) {
    int i = 0;
    while(kody[i].znak != znak) {
        i++;
    }
    return kody[i].kod;
 };

void dopiszKod(unsigned int num, bool zakodowane[], int *dlugoscZakodowana){
    unsigned int uIntSize = sizeof(unsigned int);
    int i;
    bool bit;
    bool omijacZera = true;
    for(i = uIntSize*8-1; i>= 0; i--){
        bit = ((num >>i) & 1);
        if(omijacZera) {
            if(bit == true || i == 0) {
                omijacZera = false;
            }
        }

        if(!omijacZera) {
            cout << bit;
            zakodowane[*dlugoscZakodowana] = bit;
            *dlugoscZakodowana = *dlugoscZakodowana + 1;
        }
    }
    cout << endl;
}

void koduj(char znak, struct Kod kody[], bool zakodowane[], int *dlugoscZakodowana) {
    unsigned int kod = znajdzKod(znak, kody);
    cout << znak << kod << ", ";
    bool bit;
    int nrBitu = 0;
    bool pomijacZera = true;

    dopiszKod(kod, zakodowane, dlugoscZakodowana);
}

void koduj(char text[], int sizeText, struct Kod kody[], int ileKodow, bool zakodowane[], int *dlugoscZakodowana) {
    for(int i=0; i<sizeText; i++) {
        koduj(text[i], kody, zakodowane, dlugoscZakodowana);
    }
    cout << endl << endl;
    for(int i=0; i<*dlugoscZakodowana; i++) {
        cout << zakodowane[i];
    }
}

// ****************************** ODKODOWANIE *********************************

bool sprobujZnalezcKod(int potencjalnyKod, struct Kod kody[], int ileKodow, char *znak) {
    for(int i=0; i<ileKodow; i++) {
        if(kody[i].kod == potencjalnyKod) {
            *znak = kody[i].znak;
            return true;
        }
    }
    return false;
}

int odkoduj(bool zakodowane[], int dlugoscZakodowana, struct Kod kody[], int ileKodow, char odkodowane[]) {
    int potencjalnyKod = 0;
    int dlugoscOdkodowana = 0;
    char znak;

    for(int i=0; i<dlugoscZakodowana; i++) {
        potencjalnyKod *= 2;
        if(zakodowane[i]) {
            potencjalnyKod++;
        }

        if(sprobujZnalezcKod(potencjalnyKod, kody, ileKodow, &znak)) {
            odkodowane[dlugoscOdkodowana] = znak;
            dlugoscOdkodowana++;
            potencjalnyKod = 0;
        }
    }

    for(int i=0; i<dlugoscOdkodowana; i++) {
        cout << odkodowane[i];
    }
    cout << endl;
    return dlugoscOdkodowana;
}

// ****************************** OBSLUGA PLIKOW ******************************

// ********************************** MAIN ************************************

int main() {
    char text[] = "Barbara ma rabarbar";
    int iloscWystapien[ASCII_SIZE];
    for(int c=0; c<=ASCII_SIZE-1; c++) {
        iloscWystapien[c] = 0;
    }
    int sizeText = sizeof(text)/sizeof(text[0]) -1;

    policzIloscWystapien(iloscWystapien, text, sizeText);

    struct Wezel wezly[ASCII_SIZE * 2];
    struct Wezel wezlyArchiwalne[ASCII_SIZE * 2];
    struct Kod kody[ASCII_SIZE];
    int iloscWezlow = utworzWezly(wezly, iloscWystapien);

    zbudujKopiecMin(wezly, iloscWezlow);
    cout << text << endl;
    print(wezly, iloscWezlow);

    kodyHuffmana(wezly, iloscWezlow, wezlyArchiwalne, kody);

    bool zakodowane[MAX_ZAKODOWANY_SIZE];
    int dlugoscZakodowana = 0;
    koduj(text, sizeText, kody, iloscWezlow, zakodowane, &dlugoscZakodowana);

    char odkodowane[100000];
    odkoduj(zakodowane, dlugoscZakodowana, kody, iloscWezlow, odkodowane);
}
