# Projektdokumentation

## Projektstruktur

- `./platformio/`
  - Beinhaltet die Microcontrolleranwendung
- `./kicad/`
  - Beinhaltet das Platinenlayout
- `./docker-compose/`
  - Beinhaltet die docker-compose Datei für Datenbank, haproxy, mqtt-Broker und Grafana
- `./rust-backend/`
  - Beinhaltet das Backend um die Daten zu speichern

## Dockerumgebung

### Installation der benötigten Pakete

- docker-compose lässt sich über die Paketverwaltung der Linux Distribution installieren

### Hinterlegen der Zertifikate

- in `./docker-compose/ha/ssl/certs/` sind die Zertifikate für MQTT sowie Grafana- und EMQX-Webfrontend zu hinterlegen
- Die genauen Pfade sind der Datei `./docker-compose/ha/conf/haproxy.conf/` zu entnehmen.

### Anpassen der docker-compose Datei

- In der Datei `./docker-compose/docker-compose.yaml` sind die Benutzernamen und Passwörter für 
  Datenbank, mqtt und Grafana anzupassen

### Starten der Softwareumgebung

- Als Root-user kann die Softwareumgebung mit `docker-compose up -d` gestartet werden
- Docker downloadet dann alle Container automatisch und startet diese

## Rust Backend

### Installation von cargo

- Cargo ist das build tool von Rust
- Cargo kann über das Paketmanagement der Distribution  oder über [rustup](https://rustup.rs/) installiert werden

### Konfiguration der Datenbankanbindung

- In der Datei `rust-backend/Rocket.toml` sind Benutzername und Passwort für die Datenbankanbindung anzupassen.

### Kompilieren und Ausführen des Programmes

- Mit dem Befehl `cargo build` wird der Quellcode übersetzt und Abhängigkeiten werden automatisch aufgelöst
- Mit `cargo run` wird das Backend gestartet
- Achtung: Die Datenbank muss vorher laufen.

## Microcontrolleranwendung

### Installation von PlatformIO

- PlatformIO kann über den Paketmanger oder nach [der offiziellen Anleitung](https://docs.platformio.org/en/latest/core/installation/methods/index.html) installiert werden.
- Zum Übersetzen wird außerdem das Programm `make` benötigt.

### Kompilieren und Übertragen des Programmes

- Mit `make` wird das Programm kompiliert
- Mit `make flash` wird das Programm auf den Microcontroller übertragen

