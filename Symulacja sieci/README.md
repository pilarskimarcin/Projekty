# Opis
Projekt będący symulacją linii produkcyjnej w fabryce, mającej przedstawić zasadę działania sieci. Struktura sieci składa się z 3 rodzajów węzłów:
- rampy rozładunkowe (źródła sieci) - dostarczają z zadaną częstotliwością półprodukty
- robotnicy - przetwarzają półprodukty
- magazyny (ujścia sieci) - przechowują przetworzone produkty

Węzły te są połączone na kilka sposobów:
- rampa rozładunkowa -> robotnik
- robotnik -> robotnik
- robotnik -> magazyn

Symulacja obejmuje pełne działanie sieci przez określony czas "rund", w czasie których następuje dostarczenie półproduktów na rampy, przekazanie ich do odbiorców, przetworzenie przez robotników oraz raport o stanie symulacji. Uwzględnione są także indywidualne parametry ramp i robotników (częstotliwość dostarczania, czas obróbki).

Testowanie jest możliwe dzięki użyciu framework'a Googletest, którego pliki należy pobrać z repozytorium https://github.com/google/googletest oraz umieścić w folderze googletest-master (na tym samym poziomie co wymienione poniżej foldery).

# Pliki
- diagram-UML - folder zawierający diagramy UML klas użytych w projekcie. Każdy diagram jest przedstawiony w formie kodu UML, diagramu w formacie .png oraz linku do strony, na której były tworzone
- include - folder z plikami nagłówkowymi:
	- config.hpp - konfiguracja projektu, konieczna, by umożliwić testowanie kolejnych etapów projektu
	- factory.hpp - deklaracje klas potrzebnych do zdefiniowania fabryki
	- helpers.hpp - deklaracje potrzebne do utworzenia losowego generatora prawdopodobieństw
	- nodes.hpp - deklaracje klas wszystkich rodzajów węzłów
	- package.hpp - deklaracja klasy półproduktu
	- reports.hpp - deklaracje klas i funkcji używanych w generowaniu raportów
	- simulation.hpp - deklaracja funkcji odpowiedzialnej za symulację
	- storage_types.hpp - deklaracje pomocniczych struktur przechowujących półprodukty
	- types.hpp - definicja pomocniczych typów
- mocks - pliki potrzebne do wykonania testów za pomocą framework'u
- src - folder z plikami źródłowymi, implementującymi funkcje i klasy zadeklarowane w plikach nagłówkowych
- test - folder z plikami, zawierającymi testy jednostkowe funkcjonalności projektu
- CMakeLists.txt - plik konfiguracyjny, potrzebny do poprawnego zbudowania projektu ze wszystkich plików