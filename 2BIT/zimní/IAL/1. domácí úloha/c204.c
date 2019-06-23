
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, říjen 2017                                        */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;

/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {
	// pokud mame prazdno
	if (s == NULL)
		return;
	// precti prvni polozku
	char st = s->arr[s->top];
	// znak leve zavorky
	char left_parenthesis = '(';

	while (!stackEmpty(s)) {
		// pridej do vystupniho retezce
		if (st == left_parenthesis) {
			stackPop(s);
			break;
		} else {
			postExpr[*postLen] = st;
			*postLen += 1;
		}

		stackPop(s);
		// nova hodnota vrcholu
		st = s->arr[s->top];
	}
}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {

	if (stackEmpty(s) || s->arr[s->top] == '(') {
		// easy job, pokud je stack prazdny nebo je tam leva zavorka, proste tam ten operator hodime
		stackPush(s, c);
	} else if (!stackEmpty(s)) {
		// pokud neni, musime se podivat, jestli splnuje
		char st = s->arr[s->top];

		if ((c == 42 || c == 47) && (st == 43 || st == 45)) { // na zasobniku je operator s nizsi prioritou
			stackPush(s, c);
		} else {
			// 
			while (!stackEmpty(s)) {
			 	postExpr[*postLen] = st;
			 	*postLen += 1;
			 	stackPop(s);
			 	
			 	// zapis na vystup
			 	st = s->arr[s->top];

			 	if (stackEmpty(s) || s->arr[s->top] == '(' || ((c == 42 || c == 47) && (st == 43 || st == 45))) { // na zasobniku je operator s nizsi prioritou
					stackPush(s, c);
					break;
				}
			}
		}
	}
}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {

	/**
	 * Init veci potrebnych pro prevodni funkci
	 */

	// naalokuj pamet pro rezetec
	char *output = (char *) malloc(sizeof(char) * MAX_LEN);
	unsigned int output_index = 0;
	// kontola mallocu
	if (output == NULL)
		return output;

	// naalokuj zasobnik
	tStack *stack = (tStack *) malloc(sizeof(tStack));
	// inituj zasobnik
	stackInit(stack);

	/**
	 * Parser pro vstupni retezec
	 */

	while (*infExpr != '\0') {
		// docasna promenna pro lepsi praci s nacitanym znakem
		int c = *infExpr;

		// business logic parseru
		if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
			// 0..9, a..z, A..Z z ASCII
			// vloz na konec retezce
			output[output_index] = c;
			output_index += 1;

		} else if ((c >= 42 && c <= 47) && (c != 44 || c != 46)) { 
			// operatory + - * /
			// do operation
			doOperation(stack, c, output, &output_index);

		} else if (c == 40) {
			// leva zavorka
			// standartne uloz na stack
			stackPush(stack, c);

		} else if (c == 41) { 
			// prava zavorka
			untilLeftPar(stack, output, &output_index);

		} else if (c == 61) { 
			// rovnitko
			// dokud neco na zasobniku je, vyber hodnotu, uloz ji do retezce a popni
			while (!stackEmpty(stack)) { 
				output[output_index] = stack->arr[stack->top];
				stackPop(stack);
				output_index += 1;
			}

			// vloz na konec retezce
			output[output_index] = '=';
			output_index += 1;
		}

		// loopuj dal retezcem
		infExpr++;
	}

	// vraz tam koncovou nulu
	output[output_index] = '\0';
	// odstran zasobnik
	free(stack);

	return output;
}

/* Konec c204.c */
