# Robótica AJTecnology — Repo de proyectos

**GitHub:** https://github.com/elJohn72/Robotica  
**Catálogo (agentes + MCP):** [`catalog/projects.json`](catalog/projects.json) · ver [`catalog/README.md`](catalog/README.md)

## Estructura 2026 (escalable)

| Ruta | Contenido |
|------|-----------|
| `catalog/` | Inventario JSON + mapa legacy |
| `proyectos/` | **Nuevos** proyectos por categoría (kits, competencia, comunidad…) |
| `proyectos/_plantilla/` | Plantilla README + FICHA |
| `Proyectos AJTecnology/` | Proyectos por módulo del programa (legacy) |
| `Proyectos Desarrollados/` | Laboratorio / prototipos (legacy) |
| `Proyectos escolares/` | Pitch escolar, EcoGrow, SmartVest (legacy) |
| `Codigos ejemplos de codigos cursos robotica/` | Ejemplos de cursos |

### Categorías del catálogo

`kit_venta` · `competencia` · `comunidad` · `curso_modulo` · `club_bandera` · `escolar` · `laboratorio`

### Gobernanza (workspace AJENZA en SSD)

- Agente: `08_AJTecnology_Operacion`
- MCP Cursor: `ajenza-robotics-projects`
- Procedimiento: ver SSD `00_AJENZA_Agentes/08_AJTecnology_Operacion/procedimientos/catalogo_proyectos_robotica.md`

---

## Histórico — descripción cursos Arduino

*(README anterior — algunos paths `curso_basico/` pueden no existir aún en disco; priorizar catálogo y carpetas listadas arriba.)*

Bienvenido al repositorio de **códigos y proyectos de Robótica AJTecnology**.

### Requisitos

- Arduino IDE
- Placa Arduino / ESP32 según proyecto
- Componentes del BOM en cada `FICHA.md`

### Clonar

```sh
git clone https://github.com/elJohn72/Robotica.git
cd Robotica
```
