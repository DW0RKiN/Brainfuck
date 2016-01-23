# Brainfuck
text2brain_alfa.c
=================
Converts a string to a brainfuck code that prints that string.  

Program prevadejici zadany textovy retezec na program v Brainfucku co vypisuje zadany retezec. :)

Je to jedna varianta ze 4 co me napadly. Neni to klasicka varianta co je na netu, ktera si vytvori pole PREVOD[256][256], ale o neco efektivnejsi reseni pro delsi text.

Pomoci pouhe jedne hlavni smycky dokaze ziskat hodnotu pro RETEZEC[i] = Konstanta + nasobek * A[i] + B[i]

Pouziva jeden trik pro ziskani te Konstanty tim ze se posun ukazatele na pocitadlo smycky provadi pomoci vnitrni smycky [ Konstanta/nasobek <]> ktera se zarazi o nulovou bunku pred pocitadlem. Protoze ale nechceme zvedat i pocitadlo smycky tak je ten tvar <[> Konstanta/nasobek <<]>

Nevyhoda je, ze se musi hlidat aby hodnoty v pameti nebyly pred navratem ukazatele na pocitadlo nulove a to ze pro kazdy znak zabere jednu pametovou bunku.

    <±...<±...< ±A1 < ±A0 < +B [> ±C0 > ±C1 >±...>±...<[> +D <<]-]>[.>]

    string[0] = B*D + B*C0 + A0
    string[1] = B*D + B*C1 + A1

Vystup vypada nejak takto:

    dworkin@dworkin-Aspire-E1-572G:~/Programovani/Cecko/Brainfuck/Alfa$ ./a.out
    Ukonceni v linuxu ctrl+d, pokud se nema prevadet ENTER tak prvne stisknete ESC.
    Napis neco...
    Hello World!
    nasobek: 11 posun 0 delka 107
    -<<+<--<++++<+<-<-<+<--<--<++<++++++<+++++++++++[>--->>+>+>+>------>->+>+>+>>+++>+<<<[>+++++++++<<]>-]>[.>]
    Napis neco...
    Hello World!^[
    nasobek: 11 posun 0 delka 101
    +<--<++++<+<-<-<+<--<--<++<++++++<+++++++++++[>--->>+>+>+>------>->+>+>+>>+++<<[>+++++++++<<]>-]>[.>]
    Napis neco...


text2brain_gama.c
=================
Converts a string to a brainfuck code that prints that string.  

Program prevadejici zadany textovy retezec na program v Brainfucku co vypisuje zadany retezec. :)

Treti varianta prevodu. Neni to klasicka varianta co je na netu, ktera si vytvori pole PREVOD[256][256], ale o neco efektivnejsi reseni pro delsi text.

Pouziva 2 smycky. Prvne zpetnou hlavni smycku, ktera efektivne vytvori zakladni konstantu. Pomoci jedineho plus napr. 110, ktera lezi uprostred nejcastejsich malych pismen.

Hlavni_smycka[-3..3] = 182, 36, 146, 0, 110, 220, 74 

Navrat v hlavni smycce na adresu pocitadla se provadi pres [>]<. Tzn. nemuzeme pouzit Hlavni_smycka[0] = 0.

Nasleduje druha zpetna opravna smycka a korekce, ktere konstantu opravi na pozadovanou hodnotu. 

Tisk se provede pres [>.] 

Nevyhoda je, ze algoritmus pro kazdy znak zabere jednu pametovou bunku.

        ++[<±...<A1<A0[>]<-------]B[<±...<C1<C0[>]<-]<D0<D1<±...<[.>]
        
        string[0] = FceA(A0) + B*C0 + D0
        string[0] = FceA(A1) + B*C1 + D1
        
        FceA[-3..3] = 182, 36, 146, 0, 110, 220, 74
        
Vystup vypada nejak takto:

        dworkin@dworkin-Aspire-E1-572G:~/Programovani/Cecko/Brainfuck/Gama$ ./a.out Ukonceni v linuxu ctrl+d, pokud se nema prevadet ENTER tak prvne stisknete ESC.
        Napis neco...
        Hello World!
        nasobek: 4 delka 116
        ++[<--<+<+<+<+<+++<--<+<+<+<+<+++[>]<-------]>++++[<++<-<--<<+<<+++<-<<<<--<[>]<-]<++<+<--<--<<+<+<<+<--<--<-<--[.>]
        Napis neco...
        Hello World!^[
        nasobek: 4 delka 109
        ++[<--<+<+<+<+<+++<--<+<+<+<+<+++[>]<-------]++++[<-<--<<+<<+++<-<<<<--<[>]<-]<+<--<--<<+<+<<+<--<--<-<--[.>]
        Napis neco...
