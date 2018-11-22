
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "list.h"

int errflg;

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
    tElemPtr actual = L->First;
    tElemPtr next = NULL;
	while (actual != NULL) {  // iterate over list and delete elements (clean up memory)
        next = actual->ptr;
        free(actual->instruction);
        free(actual);
        actual = next;
    }
    L->Act = NULL;  // there can be InitList() instead, but I rather do it this way
    L->First = NULL;
}

tElemPtr AllocElem(size_t instr_len) {
    tElemPtr tElemToAddPtr = (tElemPtr) malloc(sizeof(struct tElem));
    if (tElemToAddPtr == NULL) return NULL;

    char *instruction = (char *) malloc(sizeof(char) * (instr_len + 1));
    if (instruction == NULL) return NULL;

    tElemToAddPtr->instruction = instruction;
    return tElemToAddPtr;
}

char *AppendToList(tList *L, size_t instr_len) {
    tElemPtr tElemToAddPtr = (tElemPtr) malloc(sizeof(struct tElem));
    if (tElemToAddPtr == NULL) return NULL;
    tElemToAddPtr->ptr = NULL;

    char *instruction = (char *) malloc(sizeof(char) * (instr_len + 1));
    if (instruction == NULL) return NULL;

    tElemToAddPtr->instruction = instruction;

    if (L->First == NULL) {
        L->First = tElemToAddPtr;
    } else {
        tElemPtr tmp = L->First;
        while (tmp->ptr != NULL) {
            tmp = tmp->ptr;
        }
        tmp->ptr = tElemToAddPtr;
    }
    return tElemToAddPtr->instruction;
}

int InsertFirst (tList *L, char *val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/
    tElemPtr tElemToAddPtr = (tElemPtr) malloc(sizeof(struct tElem));
    if (tElemToAddPtr == NULL) return ERR_INTERNAL;

    tElemToAddPtr->ptr = L->First;
    char *instruction = (char *) malloc(sizeof(char) * (strlen(val) + 1));
    if (instruction == NULL) return ERR_INTERNAL;
    tElemToAddPtr->instruction = instruction;
    L->First = tElemToAddPtr;
    return 0;
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/
    L->Act = L->First;
}

int CopyFirst (tList *L, char *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
	if (L->First == NULL) return ERR_INTERNAL;
    val = L->First->instruction;
    return 0;
}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
	if (L->First != NULL) {
        if (L->First == L->Act) {
            L->Act = NULL;
        }
        tElemPtr secondElement = L->First->ptr;
        free(L->First->instruction);
        free(L->First);
        L->First = secondElement;
    }
}

void PostDelete (tList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/
	if (Active(L) && L->Act->ptr != NULL) {  // ensure active elem. is not last
        tElemPtr toDelete = L->Act->ptr;
        L->Act->ptr = toDelete->ptr;
        free(toDelete->instruction);
        free(toDelete);
    }
}

int PostInsert (tList *L, char *val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/
	if (Active(L)) {
        tElemPtr toInsert = (tElemPtr) malloc(sizeof(struct tElem));
        if (toInsert == NULL) return ERR_INTERNAL;
        toInsert->ptr = L->Act->ptr;
        char *instruction = (char *) malloc(sizeof(char) * (strlen(val) + 1));
        if (instruction == NULL) return ERR_INTERNAL;
        toInsert->instruction = instruction;
        L->Act->ptr = toInsert;  // insert it after active elem.
    }
    return 0;
}

int Copy (tList *L, char *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
    if (Active(L)) {
        val = L->Act->instruction;
        return 0;
    } else {
        return ERR_INTERNAL;
    }
}

void Actualize (tList *L, char *val) {
/*
** Přepíše instruction aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/
    if (Active(L)) {
        L->Act->instruction = val;
    }
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
    if (Active(L)) {
        L->Act = L->Act->ptr;
    }
}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return.
**/
    return (L->Act == NULL) ? 0 : 1;
}

void printList (tList *L) {
    tElemPtr temp = L->First;
    while (temp != NULL) {
        fprintf(stdout, "%s", temp->instruction);
        temp = temp->ptr;
    }
}