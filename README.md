# Problem jedzących filozofów (Dining Philosophers problem)

## Opis

Ten projekt implementuje klasyczny problem synchronizacji w programowaniu współbieżnym: **problem filozofów przy stole**. Każdy filozof naprzemiennie myśli i je, dzieląc widelce (zasoby współdzielone) z sąsiadami. Celem jest uniknięcie zakleszczeń (deadlock) i zapewnienie poprawnej synchronizacji.

## Instrukcja uruchomienia

1. **Uruchomienie:**
   ```bash
   make run ARG=<liczba_filozofów>
   ```

3. **Zatrzymanie symulacji:**
   Naciśnij `Enter`, aby zakończyć działanie programu.

## Wątki

- `main()` – tworzy filozofów, kontroluje zakończenie symulacji.
- `Philosopher::operator()` – kod wykonywany przez każdy wątek (filozofa); zawiera pętlę `think()` → `eat()` → powrót.
- `SpinLock` – mechanizm synchronizacji, zapewniający bezpieczny dostęp do zasobów.

## Sekcje krytyczne i rozwiązania

- **Sekcja: Pobieranie widelców**
  - Rozwiązanie: Naprzemienny sposób pobierania widelców (najpierw lewy, potem prawy lub odwrotnie w zależności od ID filozofa), co zapobiega zakleszczeniom.
  
- **Sekcja: Zakończenie działania wątków**
  - Rozwiązanie: Zmienna atomowa `stop_flag`, która jest sprawdzana w każdej iteracji pętli filozofa. Pozwala to na bezpieczne zatrzymanie programu bez przerywania wątków.

## Autor
Adrian Janicki, nr indeksu 272899  
Politechnika Wrocławska
