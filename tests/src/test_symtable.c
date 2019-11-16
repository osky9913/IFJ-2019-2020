#include "../../src/symtable.h"



/* ******************** s016-advanced-test.c *********************** */
/*  předmět: Algoritmy (IAL) - FIT VUT Brno                          */
/*  úkol: c202 - Tabulka s rozptýlenými položkami                    */
/*  vytvořil: Václav Topinka                                         */
/*  datum: září 2005                                                 */
/*  modifikace: 2013 Karel Masarik                                   */
/*  kódování: iso-8859-2                                             */
/* ***************************************************************** */

/* Testy pro domácí úkol na téma zásobník znaků (c202.c).
Obsahuje procedury a funkce, které testují studentské verze jejich voláním.
Toto je rozšířená verze, obsahující komplexní testování.
Slouží pro lektory ke kontrole příslušné domácí úlohy. */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>


symbol_t* UNDEFPTR;
hash_table * ptrht;
int solved;

/* tiskne tData ze struktury */
void htPrintData (symbol_attributes * attributes) {
	if ( attributes ){
		printf ("%.2f\n", attributes->var_att.double_value);
    }
    else
		printf ("NULL\n");
}

/* tiskne tItem ze struktury */
void htPrintItem ( symbol_t* item ) {
	if ( item )
		printf ("%s - %.2f\n", item->id, item->attributes.var_att.double_value );
	else
		printf ("NULL\n");
}

/* tiskne celou tabulku */
void htPrintTable( hash_table * ptrht ) {
	int maxlen = 0;
	int sumcnt = 0;
	int sumcnt2 = 0;
	int n = 0;
	
	printf ("------------HASH TABLE--------------\n");
	for ( int i=0; i<SYM_TABLE_SIZE; i++ ) {
		printf ("%i:",i);
		int cnt = 0;
		symbol_t* ptr = (*ptrht)[i];
		while ( ptr != NULL ) {
			printf (" (%s,%.2f)",ptr->id,ptr->attributes.var_att.double_value);
			if ( ptr != UNDEFPTR )
				cnt++;
			ptr = ptr->next;
		}
        
		printf ("\n");
	
		if (cnt > maxlen)
			maxlen = cnt;
		n += cnt;
		sumcnt += cnt;
		sumcnt2 += cnt*cnt;
	}
	
	printf ("------------------------------------\n");
	printf ("Items count %i   The longest list %i\n",sumcnt,maxlen);
	if ( n < 2 )
		n = 2;
	printf ("Smerodatna odchylka: %f\n",sqrt((sumcnt2-((float)(sumcnt*sumcnt)/n))/(n-1)));
	printf ("------------------------------------\n");
}

/* vola funkci htInit a v pripade ze neni resena tiskne chybu */
void use_init ( hash_table* ptrht ) {
	solved = 1;
	symtable_init( ptrht );
	if ( ! solved )
		printf("[W] Funkce HTInit nebyla implementovana...\n");
}

/* vola funkci htSearch a tiskne vysledek,
 v pripade ze neni resena tiskne chybu */
void use_search( hash_table* ptrht, char * key ) {
	solved = 1;
	symbol_t* ptritem = symtable_search(ptrht,key);
	if ( ! solved )
		printf("[W] Funkce HTSearch nebyla implementovana...\n");
	else 
		htPrintItem ( ptritem );
}

/* vola funkci htInsert a v pripade ze neni resena tiskne chybu */
void use_insert ( hash_table* ptrht, char * key, double  data ) {

    symbol_type_t temp = STYPE_VAR;

    symbol_attributes temp2;
    temp2.var_att.double_value = data;
    symtable_insert(ptrht,key,temp,temp2);
	
}

/* vola funkci htRead a tiskne vysledek,
 v pripade ze neni resena tiskne chybu */
void use_read( hash_table* ptrht, char * key ) {
	
    
	symbol_t * item = symtable_search( ptrht, key );
    if( item ){
		htPrintData ( &item->attributes );
    }
}

/* vola funkci htDelete a v pripade ze neni resena tiskne chybu */
void use_delete ( hash_table* ptrht, char * key ) {
	solved = 1;
	symtable_delete( ptrht, key );
	if ( ! solved )
		printf("[W] Funkce HTDelete nebyla implementovana...\n");
}

/* vola funkci htClearAll a v pripade ze neni resena tiskne chybu */
void use_clear_all ( hash_table* ptrht  ) {
	symtable_clear_all( ptrht );
}

/* Hlavni funkce pro testovani */
int main() {
	UNDEFPTR = (symbol_t*) malloc ( sizeof(symbol_t) );
	UNDEFPTR->id = "*UNDEF*";
	UNDEFPTR->attributes.var_att.int_value = -1;
	UNDEFPTR->next = NULL;
	ptrht = (hash_table*) malloc ( sizeof(hash_table) );
	for ( int i=0; i<SYM_TABLE_SIZE; (*ptrht)[i++] = UNDEFPTR );

/********************* SCRIPT START ******************************************/
	printf ("Hash Table - testing script\n");
	printf ("---------------------------\n");
	printf ("\nLet's set SYM_TABLE_SIZE to 19 ( must be prvocislo )\n");
    	//htPrintTable(ptrht);


	printf ("\n[TEST01] Table initialization\n");
	use_init ( ptrht );
	htPrintTable(ptrht);
    
	printf ("\n[TEST02] Search for non-existing item\n");
	use_search( ptrht, "rohlik" );

    
	printf ("\n[TEST03] Let's try htInsert()\n");
	use_insert ( ptrht, "rohlik", 1.50 );
	htPrintTable(ptrht);
    
	printf ("\n[TEST04] Search for existing item\n");
	use_search (ptrht,"rohlik");
    
	printf ("\n[TEST05] Let's insert next items\n");
	use_insert ( ptrht, "chleba", 20.00);
	use_insert ( ptrht, "pepsi", 30.00);
	use_insert ( ptrht, "kecup", 28.90);
	use_insert ( ptrht, "spagety", 14.60);
	use_insert ( ptrht, "debrecinka", 19.60);
	use_insert ( ptrht, "ryze", 26.10);
	use_insert ( ptrht, "vajicko", 2.50);
	use_insert ( ptrht, "olej", 32.50);
	use_insert ( ptrht, "", 14.60);
	use_insert ( ptrht, "jogurt", 8.50);
	use_insert ( ptrht, "fish", 99.90);
	use_insert ( ptrht, "korgon", 18.00 );
	use_insert ( ptrht, "zlaty bazant", 20.00);
	use_insert ( ptrht, "gambrinus", 18.50);
	use_insert ( ptrht, "starobrno", 16.50);
	use_insert ( ptrht, "plzen", 25.00);
	use_insert ( ptrht, "velvet", 26.00);
	use_insert ( ptrht, "kelt", 30.00);
	use_insert ( ptrht, "kofola", 15.00);
	use_insert ( ptrht, "graham", 4.50);
	use_insert ( ptrht, "fernet", 95.00);
	use_insert ( ptrht, "sul", 10.00);
	use_insert ( ptrht, "cukr", 25.50);
	htPrintTable(ptrht);

	printf ("\n[TEST06] htSearch for \"kecup\"\n");
	use_search ( ptrht, "kecup" );

	printf ("\n[TEST07] Let's change kecup price to 12.50\n");
	use_insert ( ptrht, "kecup", 12.50);
	use_search ( ptrht, "kecup" );

	printf ("\n[TEST08] Is \"kiwi\" in table?\n");
	use_search ( ptrht, "kiwi" );
	htPrintTable(ptrht);

	printf ("\n[TEST09] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST10] Change fish price to 105.50\n" );
	use_insert ( ptrht, "fish", 105.50 );
	use_search (ptrht, "fish" );

	printf ("\n[TEST11] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST12] How much is kiwi?\n");
	use_read ( ptrht, "kiwi" );

	printf ("\n[TEST13] Let's delete non-existing \"kiwi\"\n");
	use_delete ( ptrht, "kiwi" );
	htPrintTable ( ptrht );

	printf ("\n[TEST14] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST15] Lets delete fish twice\n");
	use_delete( ptrht, "fish" );
	use_delete( ptrht, "fish" );
	htPrintTable ( ptrht );

	printf ("\n[TEST16] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST17] Lets delete next items\n");
	use_delete ( ptrht, "zlaty bazant");
	use_delete ( ptrht, "gambrinus");
	use_delete ( ptrht, "starobrno");
	use_delete ( ptrht, "plzen");
	use_delete ( ptrht, "velvet");
	use_delete ( ptrht, "kelt");
	use_delete ( ptrht, "");
	htPrintTable ( ptrht );

	printf ("\n[TEST18] Let's delete whole table\n");
	use_clear_all ( ptrht );
	htPrintTable ( ptrht );

	printf ("\n[TEST19] And try delete in empty table\n");
	use_delete ( ptrht, "zlaty bazant");
	use_delete ( ptrht, "gambrinus");
	use_delete ( ptrht, "starobrno");
	htPrintTable ( ptrht );

	printf ("\n[TEST20] htSearch for \"kecup\" in empty table\n");
	use_search ( ptrht, "kecup" );
	htPrintTable ( ptrht );
    
	free (UNDEFPTR);
	free (ptrht);
	return 0;
}

