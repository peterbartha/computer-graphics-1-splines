# Számítógépes grafika és képfeldolgozás
2014/15 őszi félév, BME Számítógépes grafika és képfeldolgozás tárgya

## 1. beadandó feladat
### Feladat leírása
Készítsen „**Síkon táncoló kontrollpontok**” programot. A felhasználó az egér balgomb egyszeri lenyomással/elengedéssel veszi fel a kontrollpontokat (max 10-et), amelyekhez 2cm sugarú kisebb fekete köröket rendelünk. Ha a kontrollpontok száma legalább kettő, azokra egy türkiszkék színű, kitöltött konvex burkot, piros Bézier görbét, nulla kezdő és végsebességű, a kontrollpont lehelyezésének idejét paraméterként használó zöld Catmull-Rom spline-t és kék Catmull-Clark görbét illeszt.
A háttér világosszürke. Legnagyobb prioritása a kontrollpontoknak van, majd a görbék jönnek, végül jön a konvex burok.
Space lenyomására a kontrollpontok egy-egy 5 cm-es kör tetejéről elindulva, 5 másodpercenként egy teljes fordulatot téve, elkezdenek keringeni, mégpedig a páros indexűek az óramutató járásával megegyező, a páratlan indexűek pedig azzal ellentétes irányban. A konvex burok és a görbék követik a kontrollpontokat. Mindezt a felhasználó egy 58cm x 68 cm-es kameraablakon keresztül látja. Ha a felhasználó az egér jobb gombbal rábök egy kontrollpontra, akkor a kameraablakot ehhez köti, a kontrollpont elmozdulása automatikusan a kameraablakot is arrébb viszi.

### Futtatás
#### Windows - Visual Studio 201X
Első körben le kell tölteni a GLUT-ot: [letöltés](http://user.xmission.com/~nate/glut.html)
- glut.h a C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\gl könyvtárba (a gl könyvtárat Neked kell létrehozni!)
- glut32.lib a C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\lib könyvtárba
- glut32.dll a C:\Windows\SysWOW64 könyvtárba
Amennyiben a telepítés helye nem a default, akkor a .h és .lib fájlokat az általad megadott ***\Microsoft Visual Studio 11.0\VC\ könyvtárba kell tenned!

#### Linux + Szövegszerkesztő
1. Telepíteni kell a következő csomagokat (*eg: debian alapú disztrókon: sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev build-essential*):
  - libglu1-mesa-dev
  - freeglut3-dev
  - mesa-common-dev
  - build-essential
2. Tetszőleges szövegszerkesztővel szerkesztjük a kódot (*eg: vim, nano, pico, gedit, leafpad, stb.*)
3. Fordítás: g++ -Wall grafh.cpp -lGL -lGLU -lglut -o grafx, ahol:
  - Wall: összes warning megjelenítése (pl: a nem használt változókról)
  - o graf1: kimenet neve
  - lglut -lGL -lGLU: glut és GL libek linkelése
  - grafh1.cpp: maga a forrás
4. Futtatás: dupla klikk a létrejövő binárisra. (Parancssorban: ./graf1)

### Formai követelmények
A beadó rendszer kifejezetten egy fájl beküldésére lett fejlesztve, így a feltöltött C++ forrás is így került be ebbe a repository-ba. Ezért előre is elnézést kérek, hiszen így eléggé zsúfolt lett helyenként a kód és nehezebb értelmezni. De törekedtem a beszédes változók és logikus statement-ek írására, illetve ahol csak lehetett, próbáltam külön függvényekbe kiszervezni a logikákat, így támogatva a modularitást, illetve a tesztelhetőséget.

### Jogi kérdések
Az itt fentlevő **forráskód(ok) (részében vagy egészében) másolása, újrafelhasználása, hivatkozása minden formában tilos** és elítélendő. A tárgy vezetői rendkívül komolyan veszik a plágiumot, éppen ezért mindent megtesznek az ilyen beadott munkák kiszűréséért. Nem célom ezekkel az élő példákkal a tárgyat éppen hallgatók helyett megoldani a feladatokat. Ha bárki ennek ellenére visszaél ezen forrásokkal, azért semmilyen szinten nem vállalok felelősséget.
A licensz típusa pedig; GNU General Public License v2 (GPLv2)
