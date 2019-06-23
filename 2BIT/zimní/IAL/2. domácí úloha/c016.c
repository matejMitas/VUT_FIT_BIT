
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	// musime mit platny pointer
	if (ptrht != NULL)
		// napln NULL pointery
		for (int i = 0; i < HTSIZE; i++)
			(*ptrht)[i] = NULL;
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	// musime mit platny pointer
	if (ptrht != NULL) {
		// najdu polozku/polozky, kterym odpovida index
		tHTItem * found_item = (*ptrht)[hashCode(key)];
		// loopujeme dokud nenajdeme hodnotu
		while (found_item != NULL) {
			// pokud jsme nasli
			if (strcmp(found_item->key, key) == 0)
				return found_item;
 			// posuvej se v seznamu
			found_item = found_item->ptrnext;
		}
		// pokud nic nenajdes
		return NULL;
	}
	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	// musime mit platny pointer
	if (ptrht != NULL) {
		// jestli klic ma nejake polozky
		// najdu polozku/polozky, kterym odpovida index
		tHTItem * found_item = htSearch(ptrht, key);
		// polozka neexistuje
		if (found_item == NULL) {
			// nova polozka
			tHTItem * new_item = (tHTItem *) malloc(sizeof(tHTItem));
			// obligatni kontrola na malloc
			if (new_item == NULL)
				return;
			// novy klic
			new_item->key = (char *) malloc(sizeof(char) + strlen(key));
			// obligatni kontrola na malloc
			if (new_item->key == NULL) {
				// polozka je nutno vycistit, protoze uz jeji alokace probehla
				free(new_item);
				return;
			}
			// muzeme kopirovat
			strcpy(new_item->key, key);
			// vlozim data
			new_item->data = data;
			// vezmu index
			int index = hashCode(key);
			// vlozim na zacatek, vezmu predchazejici polozku
			new_item->ptrnext = (*ptrht)[index];
			// ulozime do tabulky
			(*ptrht)[index] = new_item;

		} else { // polozka uz existuje
			// proste zkopirujeme cislo
			found_item->data = data;
		}
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	// musime mit platny pointer
	if (ptrht != NULL) {
		// najdu polozku/polozky, kterym odpovida index
		tHTItem * found_item = htSearch(ptrht, key);
		// polozka neexistuje
		if (found_item == NULL)
			return NULL;
		else
			return &(found_item)->data;
	}
	return NULL;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	// musime mit platny pointer
	if (ptrht != NULL) {
		// najdi klic
		int index = hashCode(key);
		// pomocna polozka
		tHTItem * prev_item = NULL;
		// najdu polozku/polozky, kterym odpovida index
		tHTItem * curr_item = (*ptrht)[index];
		// klic existuje
		if (curr_item != NULL) {
			// musime loopovat
			if (strcmp(curr_item->key, key) != 0) {
				// musime loopovat
				while (curr_item != NULL) {
					// pokud jsme nasli
					if (strcmp(curr_item->key, key) == 0) {
						// premosti polozky
						prev_item->ptrnext = curr_item->ptrnext;
						// odstran polozky
						free(curr_item->key);
						free(curr_item);
						//
						return;
					}
					// posuvame se
					prev_item = curr_item;
					curr_item = curr_item->ptrnext;
				}
			} 
			// je to prvni polozka, takze staci jenom premostit a vymazat
			else {
				(*ptrht)[index] = curr_item->ptrnext;
				// vymaz
				free(curr_item->key);
				free(curr_item);
				// konec
				return;
			}
		} else {
			return;
		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	// musime mit platny pointer
	if (ptrht != NULL) {
		// projedu polozky 
		for (int i = 0; i < HTSIZE; i++) {
			tHTItem * curr_item = (*ptrht)[i];
			// pokud neni polozka prazdna
			if (curr_item != NULL) {
				// dokud se nedostaneme na konec
				while (curr_item != NULL) {
					// docasna promenna
					tHTItem * temp_item = curr_item;
					// uvolni
					free(temp_item->key);
					free(temp_item);
					// 
					curr_item = curr_item->ptrnext;
				}
				// nastav konec
				(*ptrht)[i] = NULL;
			}
		}
	}
}
