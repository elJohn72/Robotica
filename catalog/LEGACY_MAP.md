# Mapa legacy → catálogo

Carpetas creadas antes de junio 2026. **No renombrar en bloque**; usar `id` del catálogo para referencias nuevas.

| id catálogo | Carpeta legacy |
|-------------|----------------|
| ecogrow | `Proyectos escolares/Ecogrow` |
| smartvest | `Proyectos escolares/SmartVest` |
| extrusor-pet | `Proyectos escolares/Extrusor de Botellas PET` |
| mano-robotica-esp32 | `Proyectos AJTecnology/Modulo 3 IOT /Mano Robotica ESP32` |
| robot-evasor-obstaculos | `Proyectos AJTecnology/Modulo 2 …/Robot evasor de obstaculos` |
| oscilador-ne555 | `Proyectos AJTecnology/Modulo 1 …/Oscilador Astable con NE555…` |
| proyecto-ascensor | `Proyectos AJTecnology/Modulo 1 …/Proyecto Ascensor` |
| lampara-automatica | `Proyectos AJTecnology/Modulo 1 …/Proyecto Lámpara automática` |
| sensor-alcohol | `Proyectos Desarrollados/sensor de alcohol` |

## Migración gradual (opcional)

Cuando un proyecto se actualice para estudiantes o competencia:

1. Crear carpeta en `proyectos/{categoria}/{slug}/` con plantilla.
2. Copiar o enlazar código desde legacy (README indica origen).
3. Marcar en `projects.json`: `"path": "proyectos/…"`, mantener `"legacy_path"` hasta retirar duplicado.

## Código de ejemplo de cursos

`Codigos ejemplos de codigos cursos robotica/` — material didáctico suelto; indexar por módulo al migrar, no es un proyecto del catálogo por sí solo.
