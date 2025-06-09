# Wielowątkowy serwer czatu

## Opis

Ten projekt implementuje prosty wielowątkowy serwer chatu. Serwer obsługuje wielu klientów jednocześnie, przydzielając każdemu klientowi osobny wątek do obsługi komunikacji. Celem jest efektywnej wymiany wiadomości pomiędzy klientami oraz synchronizacja dostępu do wspólnych zasobów.

## Instrukcja uruchomienia
1. **Budowanie:**
   W wierszu poleceń (np. *Developer Command Prompt for Visual Studio*) przejdź do katalogu z projektem i wpisz:

   ```cmd
   nmake
   ```

2. **Uruchomienie:**
   W tym samym katalogu uruchom najpierw serwer:
   ```cmd
   server.exe
   ```
   
   Następnie w nowym oknie terminala uruchom jednego lub więcej klientów:
   ```cmd
   client.exe
   ```

## Wątki

- `main()` – uruchamia serwer, nasłuchuje połączeń, tworzy wątki obsługujące klientów oraz kontroluje zakończenie pracy serwera.
- `ClientHandler::operator()` – funkcja wykonywana przez każdy wątek klienta; obsługuje odbiór i wysyłkę wiadomości dla przypisanego klienta.

## Sekcje krytyczne i rozwiązania

- **Sekcja: Dostęp do listy podłączonych klientów**
  - Rozwiązanie: Synchronizacja za pomocą mutexa (std::mutex), aby zapobiec równoczesnej modyfikacji listy klientów przez różne wątki.
  
- **Sekcja: Wysyłanie wiadomości do klientów**
  - Rozwiązanie: Wysyłanie wiadomości zabezpieczone jest przez odpowiednie blokady, aby uniknąć kolizji i uszkodzenia danych.

## Autor
Adrian Janicki, nr indeksu 272899  
Politechnika Wrocławska
