/*********************************************************************
 *                                                                   *
 * KILLER MUD is copyright 1999-2008 Killer MUD Staff (alphabetical) *
 *                                                                   *
 *   ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                     *
 *                                                                   *
 *   Pietrzak Marcin     (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
 *   Sawicki Tomasz      (furgas@killer-mud.net         ) [Furgas  ] *
 *   Trebicki Marek      (maro@killer.radom.net         ) [Maro    ] *
 *                                                                   *
 *********************************************************************/
/* $Id: hints.c 7666 2009-07-03 12:04:22Z illi $ */
#if defined (macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

void hint_update( void )
{
    CHAR_DATA * ch;
    CHAR_DATA *ch_next;

    for ( ch = char_list;ch != NULL;ch = ch_next )
    {
        ch_next = ch->next;

        if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_HINT ) )
        {
            send_to_char( "{R[PORADA]: ", ch );
            switch ( number_range( 0, 52 ) )
            {
                default: send_to_char( "{YJe¶li czego¶ nie wiesz, skorzystaj na pocz±tku z pomocy, komenda {Ghelp{Y (polskie: {Gpomoc{Y). Je¶li nie znajdziesz tam odpowiedzi na swoje pytanie, mo¿esz zadaæ je innym graczom - wpisz znak zapytania i tre¶æ pytania (np. ?jak napiæ siê wody?).{x", ch ); break;
                case 1: send_to_char( "{YJe¶li nie chcesz traciæ ekwipunku, kiedy wychodzisz z gry musisz siê przej¶æ do jednej z gospod i wynaj±æ pokój. Wpisz {Grent{Y (polskie: {Gwynajmij{Y), kiedy ju¿ jeste¶ w gospodzie.{x", ch ); break;
                case 2: send_to_char( "{YKomenda {Goffer{Y (polskie: {Goferta{Y) poka¿e ci ile bêdziesz p³aci<&³/³a/³o> za przechowywanie swojego ekwipunku w gospodzie. Mo¿e okazaæ siê, ze szkoda pieniêdzy na niektóre jego czê¶ci. Sprzedaj je wtedy lub te¿ po prostu wyrzuæ.{x", ch ); break;
                case 3: send_to_char( "{YPamiêtaj, ¿e czasami, kiedy chodzisz w grupie z innymi graczami ³atwiej jest sobie poradziæ z niektórymi potworami. Praca zespo³owa czyni cuda.{x", ch ); break;
                case 4: send_to_char( "{YJe¶li nie masz wystarczaj±co du¿o pieniêdzy na przechowywanie ekwipunku w gospodzie, pomy¶l nad sprzedaniem go czy tez innymi podobnymi rozwi±zaniami. Mo¿esz przechowywaæ tylko tyle dni na ile ciê staæ, potem ca³y ekwipunek przepada.{x", ch ); break;
                case 5: send_to_char( "{YZawsze, kiedy chcesz siê zmierzyæ z ró¿nymi potworami mo¿esz wpisaæ {Gconsider imiê potwora{Y (polskie: {Goceñ{Y), (na przyk³ad {Gconsider troll{Y). Pozwoli ci to w przybli¿eniu oceniæ czy poradzisz sobie z jak±¶ besti± czy nie. Czasami po prostu nie warto ryzykowaæ walki praktycznie z góry skazanej na pora¿kê.{x", ch ); break;
                case 6: send_to_char( "{YJe¶li nie wiesz jak u¿yæ jakiej¶ komendy spróbuj wpisaæ {Ghelp komenda{Y (polskie: {Gpomoc{Y). Pomoc powinna mniej wiêcej podpowiedzieæ ci do czego dana komenda s³u¿y.{x", ch ); break;
                case 7: send_to_char( "{YSzybciej odnawiaj± siê twoje si³y ¿yciowe, kiedy jeste¶ najedzony i napity. Warto wiec zawsze mieæ ze sob± jaki¶ pojemnik z wod± i prowiant.{x", ch ); break;
                case 8: send_to_char( "{YJe¶li jeste¶ wypoczêty lepiej walczysz, niewyspany stajesz siê nieprecyzyjny. Pamiêtaj o tym. Kiedy d³uga przygoda da ci czasami szansê odpoczynku, korzystaj z niego.{x", ch ); break;
                case 9: send_to_char( "{YJe¶li widzisz, ¿e walka jest praktycznie dla ciebie przegrana spróbuj uciec walcz±cemu z tob± przeciwnikowi. Pomocna bêdzie komenda {Gflee{Y (polskie: {Guciekaj{Y).{x", ch ); break;
                case 10: send_to_char( "{YCzasami zwyczajnie nie warto ryzykowaæ straty du¿ej partii punktów do¶wiadczenia, kiedy do ukoñczenia kolejnego etapu przygody (poziomu do¶wiadczenia) pozosta³o ci ju¿ naprawdê niewiele, ale... Wszystko zale¿y od ciebie.{x", ch ); break;
                case 11: send_to_char( "{YJedna osoba nie mo¿e graæ w jednym momencie dwiema postaciami. Je¶li taki proceder zostanie wykryty spodziewaj siê kary... £±cznie z tymi najsurowszymi.{x", ch ); break;
                case 12: send_to_char( "{YPamiêtaj, ¿e imiê twojej postaci musi spe³niaæ postawione przez twórców tego muda i umieszczone na etapie tworzenia postaci kryteria. Je¶li ich nie przeczyta³e¶ - zrób to.{x", ch ); break;
                case 13: print_char( ch, "{YTroche informacji o naszym mudzie, mudach w ogóle i innych rzeczach, jakie mog± ciê zainteresowaæ mo¿esz znale¼æ na stronie naszego muda pod adresem: %s{x", WWW_ADDRESS ); break;
                case 14: send_to_char( "{YPamiêtaj, ze kiedy rozmawiasz z jednym z w³adców (Nie¶miertelnych) tego muda mo¿esz zachowywaæ siê tak jak to uwa¿asz za stosowne, ale... Pamiêtaj tez, ¿e i ów w³adca zrobi z tob± to, co uwa¿a za stosowne i we¼mie pod uwagê to jak siê zachowywa³e¶. Jak wszêdzie – chamstwo, wrzaskliwo¶æ i temu podobne niechlubne zachowania na pewno nie zostan± docenione.{x", ch ); break;
                case 15: send_to_char( "{YKiedy wpiszesz komendê {Gsave{Y (polskie: {Gzapisz{Y) dane twojej postaci zostan± zachowane na serwerze muda. Co jaki¶ czas jednak mud zachowuje te dane automatycznie.{x", ch ); break;
                case 16: send_to_char( "{YZapoznaj siê dok³adnie z komend± {Gconfig{Y (polskie: {Gopcje{Y). Pozwala ona wy¶wietliæ i ustawiæ parametry gry oraz automatyczne zachowania Twojej postaci. Dok³adniejsze informacje znajduj± siê w pomocy do tej komendy ({Ghelp config{Y lub {Gpomoc opcje{Y).{x", ch ); break;
                case 17: send_to_char( "{YKiedy chcesz zaatakowaæ jakiego¶ potwora wpisz {Gkill potwór{Y (polskie: {Gzabij{Y). Nie zawsze bêdzie dane ci wygraæ. Mierz si³y na zamiary. Ale... Jak zwykle, wszystko zale¿y od ciebie.{x", ch ); break;
                case 18: send_to_char( "{YMo¿esz zg³osiæ literówkê za pomoc± komendy {Gtypo{Y (polskie: {Gliterówka{Y).{x", ch ); break;
                case 19: send_to_char( "{YJe¶li chcesz zg³osiæ b³±d wpisz {Gbug{Y (polskie: {Gb³±d{Y) i krótki jego opis. {x", ch ); break;
                case 20: send_to_char( "{YJe¶li masz jaki¶ pomys³ wpisz {Gidea{Y i krótko wyja¶nij na czym ten pomys³ polega.{x", ch ); break;
                case 21: send_to_char( "{YKomenda {Geat{Y (polskie: {Gzjedz{Y) pozwoli ci spo¿yæ co¶ co znajduje siê w twoim inwentarzu. Niektóre rzeczy mog± byæ truj±ce, a trucizny z kolei ¶miertelne. Na ogó³ jednak sporo rzeczy sprzedawanych w sklepach i na straganach zgodnie ze swoim przeznaczeniem do zjedzenia siê nadaje… Do czasu jednak, po pewnym czasie wiêkszo¶æ siê psuje.{x", ch ); break;
                case 22: send_to_char( "{YKiedy widzisz opis jakiego¶ kierunku w nawiasie zwyk³ym, na przyk³ad (E) oznacza to, ze w tym kierunku s± drzwi. Mo¿esz spróbowaæ je otworzyæ komend± {Gopen kierunek{Y (polskie: {Gotwórz{Y). Do otwarcia niektórych drzwi jednak potrzebny bêdzie klucz.{x", ch ); break;
                case 23: send_to_char( "{YOtwarcie drzwi zamkniêtych na klucz nie jest rzecz± skomplikowan±. Wystarczy wpisaæ {Gunlock kierunek{Y, b±d¼ te¿ {Gunlock nazwa drzwi{Y (polskie: {Godklucz{Y). Przedtem musisz jednak zdobyæ odpowiedni klucz i trzymaæ go w swoim inwentarzu.{x", ch ); break;
                case 24: send_to_char( "{YKiedy ¶pisz twoje si³y ¿yciowe szybciej do ciebie wracaj±. Korzystaj z tego.{x", ch ); break;
                case 25: send_to_char( "{YPamiêtaj, ze mo¿esz byæ zaatakowany, kiedy ¶pisz lub te¿ odpoczywasz w niebezpiecznych miejscach. O ile to w ogóle mo¿liwe próbuj czasami spaæ w miejscach o mniejszym stopniu ryzyka.{x", ch ); break;
                case 26: send_to_char( "{YKiedy odpoczywasz szybciej wracaj± ci si³y ¿yciowe. Nie tak szybko wprawdzie jak podczas snu, ale zawsze. Dodatkowo podczas odpoczynku profesje czaruj±ce mog± zapamiêtywaæ zaklêcia.{x", ch ); break;
                case 27: send_to_char( "{YNie ka¿dy typ terenu jest dla ciebie równie ³atwy do pokonania. Wiêcej energii bêdzie trzeba w³o¿yæ na terenach górzystych ni¿ na prostej drodze. Jednak czasami mo¿na dosi±¶æ jakiego¶ wierzchowca, a czasami dziêki uprzejmo¶ci jakiego¶ maga nawet polewitowaæ w powietrzu.{x", ch ); break;
                case 28: send_to_char( "{YTwoja postaæ bêdzie posiada³a wiele umiejêtno¶ci. Na pocz±tku niektóre ze wzglêdu na mizerny poziom wytrenowania nie na wiele siê zdadz±. Rozwijaj je, byæ mo¿e przydadz± siê w przysz³o¶ci.{x", ch ); break;
                case 29: send_to_char( "{YKiedy chodzisz na bosaka twoje stopy s± podatniejsze na zranienie. Zawsze nawet najgorsze buciory mog± siê na co¶ przydaæ.{x", ch ); break;
                case 30: send_to_char( "{YCzê¶æ pieniêdzy mo¿esz przechowywaæ w banku. Zwyczaj taki mo¿e ciê uratowaæ przed ¶mierci± g³odow±, kiedy kto¶ ciê napadnie i ograbi na rozstajach dróg.{x", ch ); break;
                case 31: send_to_char( "{YZawsze mo¿esz skasowaæ postaæ, która ciê znudzi³a, b±d¼ tez sta³a siê dla ciebie bezu¿yteczna (opcja kasowania postaci w menu przy wchodzeniu do gry).{x", ch ); break;
                case 32: send_to_char( "{YMniej siê mêczysz przemierzaj±c ¶wiat na grzbiecie wierzchowca. Niektóre kosztuj±, ale... s± po¿yteczne.{x", ch ); break;
                case 33: send_to_char( "{YKiedy u¿ywasz ró¿nych swoich umiejêtno¶ci zdobywasz w nich punkty wytrenowania. Je¶li ich trochê uzbierasz mo¿esz siê przej¶æ do odpowiedniego nauczyciela a on nauczy ciê o danej umiejêtno¶ci czego¶ wiêcej.{x", ch ); break;
                case 34: send_to_char( "{YKiedy wychodzisz prawid³owo z muda i zachowujesz ekwipunek komend± {Grent{Y zapoznaj siê z informacj± na ile dni wystarczy ci pieniêdzy na przechowywanie i wróæ przed up³ywem tego terminu, aby ekwipunek ci nie przepad³.{x", ch ); break;
                case 35: send_to_char( "{YNiektóre potwory s± zbyt silne aby walczyæ z nimi samotnie. Zapoznaj siê z komend± {Ggroup{Y (help group) (polskie: {Ggrupa{Y) i spróbuj wybraæ siê na takiego potwora w towarzystwie dru¿yny.{x", ch ); break;
                case 36: send_to_char( "{YRó¿ne postaci posiadaj± odmienne umiejêtno¶ci. Mo¿e siê okazaæ, ¿e po³±czone umiejêtno¶ci w dru¿ynie s± w stanie zdzia³aæ cuda.{x", ch ); break;
                case 37: send_to_char( "{YKiedy zginiesz twoje cia³o bêdzie le¿a³o tam, gdzie kto¶ lub co¶ ciê zabi³o. Musisz wróciæ po cia³o, aby odzyskaæ rzeczy jakie nosi³<&e¶/a¶/e¶> przed ¶mierci±.{x", ch ); break;
                case 38: send_to_char( "{YCzasami mo¿e siê zdarzyæ, ¿e zginiesz z miejscu, z którego nie ma wyj¶cia (np. przepa¶cie). Lepiej nie wracaj tam po swój ekwipunek chyba, ¿e chcesz zgin±æ kolejny raz. Czasem trzeba pogodziæ siê ze strat± ca³ego dobytku.{x", ch ); break;
                case 39: send_to_char( "{YKiedy zginiesz i masz niewiele punktów do¶wiadczenia mo¿esz straciæ poziom do¶wiadczenia, punkty ¿yciowe i kilka innych rzeczy. Zachowuj ostro¿no¶æ.{x", ch ); break;
                case 40: send_to_char( "{YNie wykorzystuj b³êdów w grze, mo¿esz za to ponie¶æ karê. B³êdy nale¿y zg³aszaæ komenda {Gbug{Y (polskie: {Gb³±d{Y).{x", ch ); break;
                case 41: send_to_char( "{YPamiêtaj, ¿e niektóre produkty psuj± siê, kiedy d³ugo je nosisz ze sob± i nie zjadasz. Uwa¿aj na to, gdy¿ mo¿esz siê zatruæ.{x", ch ); break;
                case 42: send_to_char( "{YKiedy u¿ywasz jakiej¶ umiejêtno¶ci rozwijasz j±, aby jednak zamieniæ wszystkie twoje ogólne spostrze¿enia i przyzwyczajenia na rzeczywist± wiedzê musisz przej¶æ siê do okre¶lonego nauczyciela danej umiejêtno¶ci i pobraæ u niego nauki. S³u¿y do tego komenda {Glearn{Y (polskie: {Gucz{Y).{x", ch ); break;
                case 43: send_to_char( "{YJe¶li twoja profesja jest tak zwan± profesj± czaruj±c± mo¿esz uczyæ siê ni± ró¿nych zaklêæ. Niektórych z nich mo¿esz nauczyæ siê u nauczycieli porozrzucanych po ca³ym ¶wiecie. Niektórych trzeba naprawdê szukaæ, szukaæ, szukaæ. Magia w koñcu nie jest czym¶ pospolitym.{x", ch ); break;
                case 44: send_to_char( "{YUczenia siê danego czaru u danego nauczyciela mo¿esz spróbowaæ tylko raz na poziom do¶wiadczenia, je¶li ci siê nie uda, nastêpna próba odbywa siê dopiero na nastêpnym levelu. Mo¿esz te¿ poszukaæ innego nauczciela tego czaru.{x", ch ); break;
                case 45: send_to_char( "{YNie wszystkich czarów mo¿na siê nauczyæ u nauczycieli, niektóre znajdziesz w ksiêgach i zwojach, które czêsto bêdzie trudno znale¼æ i zdobyæ. Do nauki czarów z ksi±g s³u¿y komenda {Gstudy{Y (polskie: {Gstudiuj{Y){x", ch ); break;
                case 46: send_to_char( "{YMagowie nie najlepiej nadaj± siê na ¿ywe tarcze, dlatego czêsto bêd± potrzebowali do dru¿yny wojownika czy innego wytrzyma³ego specjalisty od nadstawiania twarzy.{x", ch ); break;
                case 47: send_to_char( "{YDobry mag powinien troszczyæ siê o osobnika, który broni go przed wszelkimi formami bezpo¶redniego starcia. Czary ochronne powinny byæ tu bardzo przydatne.{x", ch ); break;
                case 48: send_to_char( "{YGrupie wyprawiaj±cej siê w zapomniane przez ¶wiat ostêpy, zamieszka³e przez tajemnicze i bardzo gro¼ne bestie przyda siê kleryk, bêdzie leczy³ rany w krytycznych chwilach i zajmowa³ siê rannymi w czasie odpoczynku.{x", ch ); break;
                case 49: send_to_char( "{YBarbarzyñcy nie mog± co prawda nosiæ wszystkich rodzajów zbroi i nie s± tak opancerzeni jak paladyni czy wojownicy, ale ich dziki hart ducha i wrodzona wytrzyma³o¶æ rekompensuj± te braki.{x", ch ); break;
                case 50: send_to_char( "{YDobra grupa jest jak palce jednej rêki, warto czasami przemy¶leæ i skoordynowaæ przysz³e dzia³ania, aby z sukcesem dokonaæ rzeczy niebezpiecznych.{x", ch ); break;
                case 51: send_to_char( "{YWarto pamiêtaæ, ¿e do¶wiadczenie mo¿na zdobyæ nie tylko walcz±c i zabijaj±c. Rozwi±zywanie zadañ mo¿e siê nierzadko okazaæ nie tylko op³acalne, ale i bardzo interesuj±ce.{x", ch ); break;
                case 52: send_to_char( "{YJe¶li potrzebujesz pomocy mo¿esz zadaæ pytanie bardziej do¶wiadczonym graczom. Aby to zrobiæ wpisz znak zapytania a za nim tre¶æ pytania (np. ?jak napiæ siê wody?). Mo¿e siê zdarzyæ, ¿e w grze nie bêdzie nikogo kto zna³by odpowied¼ lub chcia³by odpowiedzieæ. U¿ywaj tej funkcji {Rz umiarem{Y.{x", ch ); break;
            }
            send_to_char( " {yAby wy³±czyæ porady wpisz: {Gconfig hints{Y (polskie: {Gopcje{Y).{x\n\r", ch );
        }
    }
    return ;
}
