# Opis
Celem projektu było stworzenie automatycznego bota do komunikatora Discord, ułatwiającego pewne aspekty moderacji serwera. Głównym jego zadaniem jest wyszukiwanie użytkowników, którzy nie posiadają wymaganych przez zasady danego serwera ról. Funkcjonalności bota są aktywowane przez wywoływanie na jednym z kanałów danego serwera komend, na które w odpowiedni sposób reaguje bot.
Obecne funkcjonalności bota:
- wyszukiwanie użytkowników bez zdefiniowanych dla danego serwera ról
- wyświetlanie listy komend z instrukcją wywołań
- wyświetlenie informacji o ilości czasu, jaka upłynęła od kiedy ostatnio użytkownik dołączył do serwera
- zapisywanie pliku z logami dotyczącymi działania bota (do diagnostyki ewentualnych błędów)

Do uruchomienia bota potrzebny jest, poza umieszczonymi tu plikami, plik .env z tokenem bota, który ze względów bezpieczeństwa nie jest udostępniony.

# Pliki
- keep_alive.py - plik tworzący serwer internetowy bota, w celu utrzymywania  go aktywnym bez przerwy (strona UptimeRobot umożliwia regularne pingowanie tak utworzonego serwera, przez co program nie jest usypiany w internetowym IDE Replit, w którym wykonywany jest kod) - nie jest to plik mojego autorstwa, źródło: https://www.freecodecamp.org/news/create-a-discord-bot-with-python/
- Wraith_bot.py - główny plik uruchamiający bota i zapewniający jego działanie, zawiera definicje wszystkich komend