Kompilacja wersji Release:
scons

Kompilacja wersji Debug:
scons -Q debug=1

Kompilacja moze wymagac edytowania pliku SConstruct, ze wzgledu na wykorzystanie
bibliotek boost.
Tworzona jest aplikacja o nazwie id3 w glownym katalogu projektu.
Uruchomienie utworzonej aplikacji bez podania parametrow powoduje wydrukowanie
sposobu uzycia.
Projekt udalo sie skompilowac przy pomocy narzedzia SCons oraz utworzonego pliku
SConstruct na platformie Windows 7 oraz Mac OS X Snow Leopard.
