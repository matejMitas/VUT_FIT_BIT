/**
* \file
* \brief Toto je hlavní soubor projektu.
 */


/// Jednotlivé objekty uchovávájící informace o bodech.
/**
 * Ten struct slouží pro uchovávání jednotlivých objektů načtených z pole
 */
struct obj_t {
	/// Identifikátor objektu
    int id; 
    /// souřadnice X objektu
    /**Její velikost je nezáporné číslo z intervalu 0 až 1000 včetně */
    float x; 
    /// souřadnice Y objektu
    /**Její velikost je nezáporné číslo z intervalu 0 až 1000 včetně */
    float y;
};
/// Datová struktura pro uchovávání shluků.
/**
 * Ten struct slouží pro uchovávání jednotlivých objektů načtených z pole
 */
struct cluster_t {
	/// Značí obsazenost shluku objekty
	/**Hodnota 0 znamená prázdný shluk.*/
    int size;
    /// Značí maximální počet objektů v shluku
    /**Je potřeba zajistit, aby byla kapacita (tj. alokované místo v paměti) dostatečná pro počet objektů.*/
    int capacity;
    /// Ukazatel typu 'obj_t' na první objekt v poli objektů shluku
    /**Ukazuju na alokované místo z 'init_cluster()'*/
    struct obj_t *obj;
};

/** @defgroup cluster Práce se shlukem */

/** @addtogroup cluster
  * @brief Funkce pracující se shlukem.
  *
  * @{
  */

void init_cluster(struct cluster_t *c, int cap);
/**<
Pro práci se shlukem jej musíme nejdříve vytvořit. Začneme inicializací promněných
 z prototypu shluku. Nastavíme kapacitu na 1, alokujeme pamět pro danou velikost a zároveň nastavíme size na 0,
 jelikož v tuto chvíli je náš shluk prázdný.
\return void
\param c Shluk, který máme inicializovat
\param cap Kapacity s jakou máme shluk incializovat, typicky 1
\pre
-# c != NULL Datová struktura byla již vytvořena.
-# c >= 0 Kapacita je přirozené číslo.
*/

void clear_cluster(struct cluster_t *c);
/**<
Vyčistění shluku od objektů a jeho incializace na prázdný shluk
\return void
\param c Shluk, který máme vyčistit.
*/

/// Promněná, která značí doporučenou velikost shluku
extern const int CLUSTER_CHUNK;

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);
/**<
Změna kapacity shluku (tj. realokace místa určeního pro daný shluk).
\return cluster_t Velikostně upravený shluk
\param c 		  Shluk, který máme vyčistit.
\param new_cap    Nová kapacita shluku.
\pre
-# c->capacity >= 0 != NULL Shluk c1 musí být incializován
-# new_cap >= 0 Nová kapacita musí být větší než 0
*/

void append_cluster(struct cluster_t *c, struct obj_t obj);
/**<
Přidání objektu do shluku. Zde je nutno zaručit, že je kapacita shluku dostatečná
\return void
\param c Shluk, který máme vyčistit.
\param obj   Objekt, který chceme přidat do shluku.
*/

void sort_cluster(struct cluster_t *c);
/**<
Seřadí objekty v poli objektů ve shluku. Řadí vzestupně.
\return void
\param c Shluk k seřazení.
*/

void print_cluster(struct cluster_t *c);
/**<
Vytiskne na stdout jeden shluku, tedy všechny objektu v poli objektů za pomoci parametru velikosti.
\return void
\param c Shluk k vytisknutí.
*/
/** @} */



/** @defgroup clusters Práce s polem shluků */

/** @addtogroup clusters
  * @brief Funkce pracující s polem shluků.
  *
  * @{
  */

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
/**<
Sloučení objektů jednoho shluku do druhého. Po nalezení dvou nebližších shluků je potřeba
dané shluky sloučit do jednoho. Tato funkce vezme objekty ze shluku c2 a přidá je do pole objektů
shluku c1. Shluk c2 nemění.
\return void
\param c1 Shluk, do, kterého mám přidat objekty.
\param c2 Shluk, ze, kterého objekty bereme.
\pre
-# c1 != NULL Shluk c1 musí být incializován
-# c2 != NULL Shluk c2 musí být incializován
*/

int remove_cluster(struct cluster_t *carr, int narr, int idx);
/**<
Jelikož funkce 'merge_clusters()' pouze zkopíruje objektu z jednoho shluku do druhého,
je potřeba zajistit správné indexování pole shluků. Proto tato funkce uvolní místo v paměti
určené shluku, který chceme vymazat a posune všechny ostatní shluky s indexem větším, než je 
daný námi chtěný shluk.
\return void
\param carr Pole shluků obsahující námi vytvořené shluky.
\param narr Délka pole shluků.
\param idx Index inkriminovaného shluku, který chceme odstranit.
\pre
-# idx < narr Index prvku k odstranění musí být menší než délka pole
-# narr > 0 Délka pole musí být větší než 0
*/

void print_clusters(struct cluster_t *carr, int narr);
/**<
Vytiskne na stdout všechny shluky, které jsou použity v programu.
\return float největší vzdálenost
\param carr Ukazatel na první prvek pole shluků
\param narr Délka pole shluků
*/


float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);
/**<
Za použití funkce 'obj_distance()' je hledána maximální vzdálenost mezi shluky, tj.
Porovnávání vzdálenosti jednotlivých bodů. Z důvodu použití metody nejvzdálenějšího souseda
nám jde o největší vzdálenost.
\return float největší vzdálenost.
\param c1 První shluk k porovnání.
\param c2 Druhý shluk k porovnání.
\pre
-# c1 != NULL Shluk c1 musí být incializován
-# c2 != NULL Shluk c2 musí být incializován
-# c1->size > 0 Shluk c1 musí obsahovat alespoň jeden objekt
-# c2->size > 0 Shluk c2 musí obsahovat alespoň jeden objekt
*/


void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);
/**<
Pomocí funkce 'cluster_distance()' hledá dva nejbližší shluky v poli shluků.
\return void
\param carr Ukazel na první polek pole shluků.
\param narr Délka pole shluků.
\param c1 Index prvního ze dvojce nejbližších shluků.
\param c2 Index druhého ze dvojce nejbližších shluků.
\pre
-# narr > 0 Pole shluků je vetší než 0
*/

/** @} */


/** @defgroup helpers Pomocné funkce */

/** @addtogroup helpers
  * @brief Nezbytné funkce pro chod programu.
  *
  * @{
  */

float obj_distance(struct obj_t *o1, struct obj_t *o2);
/**<
Funkce počítá prostou euklidovskou vzdálenost mezi dvěma objekty (v tomto případě je bereme spíše jako body).
\return float vzdálenost.
\param o1 Pole shluků obsahující námi vytvořené shluky.
\param o2 Délka pole shluků.
\pre
-# o1 != NULL Objekt o1 musí být incializován
-# o2 != NULL Objekt o2 musí být incializován
*/

int load_clusters(char *filename, struct cluster_t **arr);
/**<
Otevře soubor pomocí parametru 'filename'. Přečte první řádek dokumentu, kde je uveden počet shluků.
Po daný počet alokuje místo v paměti. Dále načítá po data po řádcích, pro každý řádek vytvoří nový objekt, který obsahuje
načtené souřadnice. Nakonec nově vytvořené shluky uloží do pole shluků.
\return int počet načtených shluků
\param filename Název souboru pro otevření.
\param arr Ukazatel na ukazatel na první pole shluků
\pre
-# arr != NULL Ukazatel na první prvek pole ukazatelů musí být definován
\post
-# fp != NULL Soubor se podařilo úspěšně načíst.
*/

/** @} */

