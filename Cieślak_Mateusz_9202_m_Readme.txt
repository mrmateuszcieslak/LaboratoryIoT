Laboratorium IoT w środowisku chmurowym
Autor: Mateusz Cieślak nr indeksu 9202

Opis:
Archiwum zawiera materiały do projektu stacji pogodowej IoT opartej na ESP32 i czujnikach (DHT22, BMP280, PMS7003).  
Dane są wysyłane w formacie JSON do chmury AWS i mogą być analizowane w Excelu, Power BI lub DynamoDB, S3.

Zawartość:
- Dane z czujników – próbka 20 plików JSON z pomiarami (temperatura, wilgotność, ciśnienie, pyły PM).  
- Połączenie z AWS – pliki konfiguracyjne i przykłady.  
- Kod ESP32 – program mikrokontrolera do zbierania danych i wysyłki JSON.  
- Terraform – skrypty do utworzenia zasobów AWS (IoT Core, DynamoDB, S3).  
- Diagramy – schemat ideowy, logiczny i architektoniczne oraz Analizy SWOT (pliki PNG).  
- Dokument „Cieślak_Mateusz_9202_m_Wizualizacja danych pochodzących z stacji pogodowej.pbix” – wizualizacja w Power BI.

Jak używać:
1. Otwórz pliki JSON (np. w Excelu: Dane → Z pliku JSON) – zobaczysz przykładowe pomiary.  
2. Aby uruchomić pliki konfiguracyjne , należy posiadać lub zainstalować program Arduino na swoim komputerze.

Proces instalacji : 
a) Wchodzimy na stronę https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE .
b) Pobieramy najnowszą wersję.
c) Postępuj zgodnie z instrukcjami , które zostały zawarte w przewodniku instalacyjnym.
d) Po zakończeniu procesu instalacji pozostaw zaznaczone pole Run Arduino IDE / Uruchom z pozycji Startu.

Po uruchomieniu Arduino :
1. Wchodzimy na zakładkę File -> Open 
2. Wyświetla się Folder Dokumenty -> Arduino / inny folder
3. Otwieramy wybrany plik z sposród dwóch : Cieślak_Mateusz_9202_m - Połączenie z chmurą obliczeniową.ino , Cieślak_Mateusz_9202_m-Kod urządzenia fizycznego.ino
4. Gotowe


3. Skorzystaj z Terraform, aby uruchomić zasoby w AWS.  
4. Analizuj dane w Excelu, Power BI.

Instrukcja Terraform:
1. Przejdź do folderu `terraform`:
   cd terraform
2. Zainicjuj projekt:
   terraform init
3. (Opcjonalnie) sprawdź plan wdrożenia:
   terraform plan
4. Utwórz zasoby w AWS:
   terraform apply
   i potwierdź wpisując yes.

Uwagi:
- Próbka 20 JSON służy do testów – w realnym działaniu dane są generowane na bieżąco.  

