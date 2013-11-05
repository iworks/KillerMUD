#pragma once


//poziomy debugowania (czyli ilosci informacji vs spam do loga)
#define DEBUG_LEVEL_ERROR				0
#define DEBUG_LEVEL_SUSPECTED		1
#define DEBUG_LEVEL_INFO				2
#define DEBUG_LEVEL_ALL					3

//flagi projektow
#define DEBUG_PROJECT_UNKNOWN		0
#define DEBUG_PROJECT_BIGFLAG		1
#define DEBUG_PROJECT_OLCLAW		2
#define DEBUG_PROJECT_LAW				3
#define DEBUG_PROJECT_SPELL_ITEMS 4
#define DEBUG_PROJECT_DOOM			5
#define DEBUG_PROJECT_STEEL_SKIN	6

//Nil: more verbosive true and false for checking whetcher particular features are on or off
#define DISPLAY_ON TRUE
#define DISPLAY_OFF FALSE

//rellik: do debugowania, zmienna okre¶laj±ca czy aktywny tryb debugowania
int debugged_project;
bool debuguj;

//rellik: do debugowania, poziomy debugowania 0 tylko b³êdy (zawsze w³±czone) kolejne zapisuj± coraz wiêcej informacji
//rellik: zmiana z poziomu debugowania na tabelê projektów [20080516]
struct projects
{
	unsigned int number; //number to jest tez numer bitu w BIGFLAG, w ten sposób mo¿na pó¥niej sprawdziæ czy s± aktywne 2 debugi projektow
	char *name; //name of project
	bool display_is_on;
	const char *display_to; //player name to display in-game debug info
};

typedef struct projects PROJECTS;

extern const PROJECTS projects_table[];

char *nazwa_projektu( int nr );

/* Projekty:

Pomocne przy wyszukiwaniu zmian nale¿±cych do danego projektu.

* rellik: friend_who
Lista znajomych pokazywanych na who.

* rellik: do_bash
Przerobiona w do¶æ znacznym stopniu

* rellik: wtykanie pochodni
Wtykanie, zawieszanie itp. lamp, pochodni...
Niestety niekomentowane zmiany.

* rellik: do debugowania
Funkcje i zmiany maj±ce na celu logowanie wiêkszej ilo¶ci informacji do analizy czemu mud siê wysypa³ np.
Dodana komenda debug x, gdzie x to poziom dok³adno¶ci zapisu zdarzeñ do loga.
Dany jest przyk³ad u¿ycia tego z funkcjami w których mo¿ena dodaæ argument caller i takimi w których nie mo¿na (u¿ywane globalne gcaller).

* rellik: carve
Dodanie carvingu (nie wszystkie zmiany skomentowane)

* rellik: kary za zgon
Zmiany w karach za zgon.

* rellik: prawo w miastach
Wprowadzenie stref w których obowi±zuje prawo.

Czê¶æ OLC:

law
	U¿ycie: law
		Wypisuje zajete ID stref.
	U¿ycie: law {set|clear} <vnum_min> <vnum_max> <id_law>.\n\r", ch );
		Ustawia lub czysci przynaleznosc zakresu <vnum_min> do <vnum_max> do strefy <id_law>
	U¿ycie: law id <id_law>
		Wyswietla dane strefy o id <id_law>
	U¿ycie: law name <nazwa_strefy>
		Wyswietla dane strefy o nazwie <name>

LZEdit
	Sk³adnia: lzedit create [vnum] lub lzedit [vnum] lub lzedit save\n\r", ch );
	lzedit create [vnum] - tworzy now± strefê i ustawia edytor w tryb edycji strefy
	lzedit [vnum] - ustawia edytor w tryb edycji strefy
	lzedit save - zapisuje listê stref (u¿ywaæ po zakoñczeniu modyfikacji)

W trybie edycji roomu:
	setlaw - ustawianie przynale¿nosci rooma do strefy objêtej prawem
	clearlaw - resetowanie przynale¿nosci rooma do strefy objêtej prawem

W trybie edycji stref (lzedit):

	show						//pokazuje ustawienia strefy
	remove					//usuwa strefe
	newid						//ustawia nowe id dla aktualnie edytowanej strefy
	copy						//kopiuje dane ze strefy o id podanym jako argument
	frace						//ustawianie flagi ras
	fprof						//ustawianie flagi profesji
	falign					//ustawianie flagi align
	level						//ustawianie levela do ktorego chroni prawo
	guard						//ustawianie vnum stra¿ników ( 1, 2, 3)
	nguard					//ustawianie ilu strazników ( 1, 2, 3)
	time						//ustawianie czasu pamiêtania zbrodni
	penalty					//ustawianie rodzaju przestepstw z odpowiadaj±cymi karami
	hunters					//ustawianie czy dozwoleni ³owcy g³ów
	prison					//ustawianie vnum lokacji która ma byæ miejscem ka¥ni

* Raszer: config showposition

- plik act_info, funkcje show_config i do_config, szukac po showpos
- plik comm.c, funckja bust_a_prompt, szukac po showpos, tutaj jest funkcja wyswietlajaca prompt podczas walki i grupowy
- plik bit.c, linia 210, flagi PLR_SHOWPOS i PLR_SHOWPOSS, umowzliwiajace wlaczenie/wylaczenie showposition i wybor rodzaju (pelny lub skrocony)
- plik merc.h linia 1571 dodanie extern, deklaracji flagi?
- tables.c, szukac po showpos

* Raszer: zmiana mv dla nieumarlych

- plik act_info, szukac po mv_undead
- plik act_move, szukac po mv_undead, edytowanie dosyc rozlegle, powtykane miedzy linijki 660 do 860
- plik comm.c, kilka miejsc, szukac tak jak wyzej RELLIK: SZUKAÆ TAK JAK WY¯EJ? MV_UNDEAD NIE ZNALEZIONO W TYM PLIKU...
- update.c, najwa¿niejsza zmiana w 4785 lini, szukac po mv_undead RELLIK: mv_undead nie znaleziono w tym pliku

* Raszer: zmiana efektu po spadnieciu energize dla nieumarlych
- zamiast zmeczenia i sennosci - weaken i slow
- plik handler.c - szukac po ener_undead RELLIK: ener_undead nie znaleziono w tym pliku

* Drake: dodanie czarów nature ally I do IV
- podzielenie summon animal druida na cztery czary
- sprawdzenie istniej±cych mobów
- dodanie piêciu nowych mobów: monstrualny paj±k (nature ally III), pomniejszy element powietrza/wody/ziemi/ognia (nature ally IV)
- spells_dru, const, merc, magic.h

* Drake: Dodanie czarów cure/reinvigore animal/plant
- Zmiana zasady dzia³ania tych czarów (wcze¶niej bra³y pod uwage max_hp moba), teraz s± jak zwyk³e cure'y
- przeniesienie heal plant/animal na 6 kr±g
- cure/reinvigore s³abszymi wersjami heal'a.
- spells_dru, const, magic.h

* Drake: dodanie czarów bark guardian i wildthorn (sumonuj± planty dla druida) - lekkie podci±gniêcie liveoaka
- ¶miesznie niski poziom (od 2 do 20 losowo - jak na czar 5 krêgu!), od którego zale¿a³y statystyki, które
	mia³ do¶æ dobrze zbalansowane. Podci±gno³em do 12-18, bark i wildthorn podobni, tyle tylko, ¿e inne przedzia³y
	poziomowe. (Bark na III kr±g, wildthorn na VII)
- spells_dru, const, magic.h, merc.

* Drake: Zmiana w niektórych czarach magów, druidów i kleryków.
- Spells_cle: dispel evil i dispel good juz nie zabijaja przy duzej roznicy poziomow. Teraz przy roznicy 8 i wiecej
	poziomow - za kazdy poziom powyzej 8 kleryk dostaje dodatkowo +1d10 do obrazen czarku.
- Spells_dru: poprawki we wczesniej wprowadzonych czarach.
- Spells_mag: czary create_lesser_ilusion i create_greater_ilusion - iluzje maja ustawiany typ ataku 'iluzoryczne
uderzenie', o rodzaju obrazen mental. Teraz nie powinny nic robic rzeczom bezmyslnym, jak nieumarli i golemy.
- Const i merc - wprowadzenie nowego typu obrazen, 'iluzoryczne uderzenie' i zwiekszenie ilosci attack_type
 (nadaremno, iluzoryczne uderzenie jest 44, na 46 max).

* Tener: FS#4379, czyli shapeshifting
- Zmieniona struktura gracza w merc.h, co za tym idzie zmieniona procedura inicjalizacji tej struktury w save.c
- W save.c dodana obs³uga zapisu gracza przemienionego: jest on na u³amek sekundy przywracany do swojej oryginalnej postaci,
  zapisywany, i przemieniany ponownie.
- W trakcie copyover zapisywany jest numer przemiany gracza, po copy jest on przywrócony do tej w³a¶nie przemiany
- Immo widz± przemienionego gracza, je¿eli stoj± tu¿ obok.
- Ponadto w wielu miejscach w kodzie postaæ pod wp³ywem shapeshift'a udaje moba
- Dla pe³niejszego wykazu: grep ss_data *.[hc]

* rellik: bigflagi, system obslugi flag [20080517]
Struktura i funkcje do obslugi flag na bitach. Du¿o prostsze od istniej±cego mechanizmu EXT_, nie operuje tak na pamiêci.
Dzia³a na systemach 32 i 64 bitowych, na tych drugich ma 2 razy wiêksz± pojemno¶æ ale "o tym wie".
Flagi teraz zapisujemy jako numer bitu a nie jego warto¶æ wiêc #define MOJA_FLAGA 3 odnosi siê do 3 bitu, #define INNA_FLAGA 51 do 51 bitu.
Struktura w merc.h, funkcje w bit.c.

* rellik: FS#4288 , dodanie do affectów pola które odmierza czas trwania affectu nawet gdy postaæ nie jest w grze
Je¶li bêdzie potrzebne pozosaje zakodowanie rt_duration w olc. Na razie przewidujê jedynie ustawianie z kodu.

* rellik: kana³ newbie [20080711]
Komenda newbie która dzia³a jak immtalk czyli info do wszystkich, po³±czone z istnieniem kana³u NEWBIE
oraz blocknewbie dla immo ¿eby komu¶ blokowac ten kana³. Kana³ otwarty tylko dla <=LEVEL_NEWBIE
Dodanie w config komendy okre¶laj±cej czy chcemy widzieæ ten kana³.
Za to do ludzi o tym levelu mo¿na mówiæ na tell.

* rellik: komponenty [20080820]
Komponenty niezbêdne do rzucania czarów.
Aby obiekt by³ komponentem musi mieæ zdefiniowane w definicji lub instancji pole is_spell_item na TRUE.
W definicji i/lub instancji jest równie¿ okre¶lona ilo¶æ ³adunków oraz czas ¿ycia komponentu.
W tabeli spell_items_table s± okre¶lone dodatkowe parametry komponentu oraz jego powi±zanie z czarem.
Komponent mo¿na ustawiaæ standardowymi funkcjami OLC, dodatkowo SITABLE pozwala ogl±daæ tabelê powi±zañ komponentów z czarami
a SILIST pokazuje obiekty które s± komponentami.
W parametrach postaci gracza zosta³a dodana bigflaga która definiuje znajomo¶æ poszczególnych komponentów.
Wa¿ne miejsca:
spell_items_table - ustawianie powi±zania komponentów z czarami, aby obiekt by³ komponentem powi±zanym z czarem pole spell_item_name musi byæ w obj->name
Dodane komendy
set SIKnow - ustawianie znajomo¶ci komponentów przez postaæ
set object - ustawianie parametrów komponentowych obiektu
SITable - pokazywanie tabeli powi±zañ komponentów z czarami
SIList - pokazywanie listy obiektów ustawionych jako komponenty
spell <nazwa_czaru> - pokazuje komponenty jakie znamy do czaru <nazwa_czaru>
Dzia³anie:
Aby u¿yæ komponentu musimy mieæ o nim wiedzê z ksiêgi. Najpierw musimy nauczyæ siê czaru a pó¼niej uczymy siê komponentów dla tego czaru.
Nastêpnie do rzucania niektórych czarów s± u¿ywane komponenty, maj± ró¿ne dzia³anie, mo¿e byæ te¿ zdefiniowana grupa komponentów która dzia³a
jedynie gdy s± obecne wszystkie sk³adniki grupy. Je¶li mamy wiêcej ni¿ jeden przydatny komponent to wybierany jest losowy.
Komponenty maj± okre¶lon± ilo¶æ ³adunków (lub nieskoñczon±) i okre¶lony czas ¿ycia. Po odpaleniu ostatniego ³adunku komponent mo¿e znikn±æ.

Proces dodawania komponentu:

1. Okre¶lamy sn czaru do jakiego chcemy dodaæ komponent
2a. Je¶li wycinany to dodajemy w const.c do tabeli 'body_parts_table' i w skills.c w 'carve_do_spec_fun' nadajemy czê¶ci w³a¶ciwo¶ci komponentowe
2b. Je¶li jest na mudzie to w oedit nadajemy komend± komponent w³a¶ciwo¶ci komponentowe
3. W const.c w tabeli 'spell_items_table' dodajemy komponent
4. W definicji danego czaru wywo³ujemy funkcjê 'spell_item_check' i po zwróceniu TRUE (ma niezbêdny komponent) lub FALSE (nie ma) okre¶lamy stosown± akcjê.

* rellik: mining [20080914]
Normalny skill, potrzebny kilof (ró¿ne klasy) i wydobywa siê kruszec.

v dodaæ narzêdzia, v0 - jako¶æ narzêdzia 1-9, v1 - poziom skilla od którego mo¿na u¿ywaæ, v2 - typ narzêdzia ( z tool_table )
v dodaæ sam skill
v dodaæ tabelê materia³ów i makro
v dodaæ w roomach tablelê prawdopodobieñstwa wypadniêcia grudki danego materia³u, dodaæ w olc ustawianie tego i zapisywanie w plikach
v Mo¿na wprowadziæ "pojemno¶æ" rooma, czyli ile bry³ek/warto¶ci mo¿na wykuæ zanim ¿y³a siê wyczerpie (do resetu pewnie)
v zró¿nicowaæ komunikaty podczas wydobywania
v dodaæ uczenie siê skilla
v dodaæ pole capcity_now do room i przy resecie przepisywaæ z capcity bo cholera pamiêta aktualn± warto¶æ
- zrobiæ zamiast wait_print to wait_function i tam wypisywac i kontynuowaæ (jako parametr ile jeszcze wywo³añ) je¶li warunki spe³nione (np. postaæ nie walczy) i waita nak³adaæ tylko do nastêpnego sprawdzenia
- helpy dorobiæ
v dodaæ mêczenie siê przy wydobywaniu (spadek mv)
v jakby narzêdzia sie tak psu³y jak bronie
v w try trzeba by te¿ dorobiæ narzêdzia i sprawdzaæ poziom skilla, informuje je¶li za niski i mówi od jakiego poziomu mo¿na
v sprawdzaæ sector_type ¿eby w wodzie albo lesie nie kuli

Builderzy:
- dodaæ kilofy
- poustawiaæ lokacje
- dodaæ skill do nauczycieli
- poustawiaæ bry³ki i vnumy przepisaæ do merc
- poustawiaæ ¿eby naprawiali te¿ narzêdzia

* Nil: steel_skin [20100601]
Szukaæ w kodzie po: steel_skin
Wszystkie zmiany skomentowane.
W merc.h jest define STEEL_SKIN_ON, mo¿na to zakomentowaæ i wy³±czy ten spell.
Przerobiony steel skin. Na razie dzia³a analogicznie jak stone skin. Poziom jest ustawiony na 25.
Obni¿a AC o 30. Przy uderzeniu tarcz± jest b. du¿a szansa na wywrotkê (bez waita).
Mo¿na rzucaæ na innych

*/

//TODO: zrobiæ wy¶wietlanie sitable jako¶ po ludzku
/*TODO: (rellik#) potestowaæ komponenty ostatnie
 * czy ¿o³êd¼ gdzie¶ siê ³aduje
 */
