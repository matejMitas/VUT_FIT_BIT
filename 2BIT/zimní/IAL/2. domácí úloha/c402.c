
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**                                         Úpravy: Radek Hranický, říjen 2016
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{ 
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**	
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,	
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	(*RootPtr) = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/

	// pokud pridavame do prazdneho stromu
	if (*RootPtr == NULL) {
		// nova polozka
		tBTNodePtr new_item = (tBTNodePtr) malloc(sizeof(struct tBTNode));

		if (new_item == NULL)
			return;

		new_item->Cont = Content;
		// pointery na dalsi polozky
		new_item->LPtr = NULL;
		new_item->RPtr = NULL;
		// prirad
		*RootPtr = new_item;

	} else {
		// pokud existuje a klic se rovna pouze zmenime
		tBTNodePtr parent_item = NULL; // rodic nami hledane pozice na vlozeni
		tBTNodePtr loop_item = *RootPtr; // kde zaciname
		int branch = -1; // ktera vetev, at to potom nemusime znova provnavat, 0 - leva, 1 - prava;

		while (loop_item != NULL) { // dokud nedojedeme do prazdneho uzlu
			if (loop_item->Cont > Content) {
				// nastavime rodice
				parent_item = loop_item;
				// nastavime hodnotu rodice
				branch = 0;
				// presuneme se do leva
				loop_item = loop_item->LPtr;
			} else if (loop_item->Cont < Content) {
				// nastavime rodice
				parent_item = loop_item;
				// nastavime hodnotu rodice
				branch = 1;
				// presuneme se do leva
				loop_item = loop_item->RPtr;
			} else {
				return;
			}
		}

		// vytvorime novou polozku
		tBTNodePtr new_item = (tBTNodePtr) malloc(sizeof(struct tBTNode));

		if (new_item == NULL)
			return;

		new_item->Cont = Content;
		// pointery na dalsi polozky
		new_item->LPtr = NULL;
		new_item->RPtr = NULL;

		// kterou vetev pouzit
		if (!branch)
			parent_item->LPtr = new_item;
		else 
			parent_item->RPtr = new_item;
	}	
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/

	// dokud mame co prochazet
	while (ptr != NULL) {
		SPushP(Stack, ptr);
		// zpracuj uzel
		BTWorkOut(ptr);
		// nastav dasli uzel
		ptr = ptr->LPtr;
	}
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	// nebudeme prochazet prazdnym stromem
	if (RootPtr != NULL) {
		// promenna pro prochazeni stacku
		tBTNodePtr stack_loop = RootPtr;
		// zasobnik na pointery
		tStackP pointers;
		// inicializace
		SInitP(&pointers);
		// naplnime stack uzly
		Leftmost_Preorder(RootPtr, &pointers);
		// dokud neprojdeme celou vetev
		while (!SEmptyP(&pointers)) {
			stack_loop = STopPopP(&pointers);
			Leftmost_Preorder(stack_loop->RPtr, &pointers);
		}
	}
}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku. 
**/
	
	// dokud mame co prochazet
	while (ptr != NULL) {
		SPushP(Stack, ptr);
		// nastav dasli uzel
		ptr = ptr->LPtr;
	}
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	// nebudeme prochazet prazdnym stromem
	if (RootPtr != NULL) {
		// promenna pro prochazeni stacku
		tBTNodePtr stack_loop = RootPtr;
		// zasobnik na pointery
		tStackP pointers;
		// inicializace
		SInitP(&pointers);
		// naplnime stack uzly
		Leftmost_Inorder(RootPtr, &pointers);
		// dokud neprojdeme celou vetev
		while (!SEmptyP(&pointers)) {
			stack_loop = STopPopP(&pointers);
			// rozdil oproti BTPreorder, zpracuju uzel az tady
			BTWorkOut(stack_loop);
			Leftmost_Inorder(stack_loop->RPtr, &pointers);
		}
	}	
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat. 
**/
	// dokud mame co prochazet
	while (ptr != NULL) {
		SPushP(StackP, ptr);
		// uz projito?
		SPushB(StackB, true);
		// nastav dasli uzel
		ptr = ptr->LPtr;
	}	
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/

	// nebudeme prochazet prazdnym stromem
	if (RootPtr != NULL) {
		// promenna pro prochazeni stacku
		tBTNodePtr stack_loop = RootPtr;
		// z jake strany prochazim
		bool leftwise;
		
		// zasobnik na pointery
		tStackP pointers;
		// inicializace
		SInitP(&pointers);
		// zasobnik na booly
		tStackB bools;
		// inicializace
		SInitB(&bools);

		// naplnime stack uzly
		Leftmost_Postorder(RootPtr, &pointers, &bools);
		// dokud neprojdeme celou vetev
		while (!SEmptyP(&pointers)) {
			// ukazatel
			stack_loop = STopPopP(&pointers);
			// misto toho, abych si mohl implementovat funkci na vraceni vrchu,
			// tak to musim popovat a pak zase vratit
			SPushP(&pointers, stack_loop);
			// smer
			leftwise = STopPopB(&bools);
			// jak jsme prochazeli? (zleva nebo zprava?)
			if (leftwise) {
				// tady jsme jiz byli
				SPushB(&bools, false);
				Leftmost_Postorder(stack_loop->RPtr, &pointers, &bools);
			} else {
				// vymaz
				STopPopP(&pointers);
				// zpracuj
				BTWorkOut(stack_loop);
			}
		}
	}	
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/

	// nebudeme prochazet prazdnym stromem
	if (*RootPtr != NULL) {
		// promenna pro prochazeni stacku
		tBTNodePtr stack_loop = *RootPtr;
		// zasobnik na pointery
		tStackP pointers;
		// inicializace
		SInitP(&pointers);

		do {
			if (stack_loop == NULL) {
				if (!SEmptyP(&pointers))
					// ukazatel
					stack_loop = STopPopP(&pointers);
			} else {
				if (stack_loop->RPtr != NULL) {
					SPushP(&pointers, stack_loop->RPtr);
				}
				// ulozeni pro pozdejsi odstraneni
				tBTNodePtr temp_item = stack_loop;
				//
				stack_loop = stack_loop->LPtr;
				//
				free(temp_item);
			}

		} while ((stack_loop != NULL) || !SEmptyP(&pointers));

		// potrebuji znovu incializovat jako NULL
		*RootPtr = NULL;
	}	
}

/* konec c402.c */
