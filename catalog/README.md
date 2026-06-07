# Catálogo de proyectos — Repo Robótica AJTecnology

**Repo:** https://github.com/elJohn72/Robotica  
**Local:** `~/Repositorios Githup/04-robotica/Robotica`

## Fuente de verdad

| Archivo | Uso |
|---------|-----|
| `catalog/projects.json` | Inventario machine-readable (MCP + agentes) |
| `catalog/LEGACY_MAP.md` | Mapeo carpetas históricas → `id` del catálogo |
| `proyectos/` | Estructura **nueva** escalable (desde 2026-06) |
| Carpetas legacy | Se conservan; no mover masivo sin plan |

## Categorías (`category`)

| Clave | Contenido |
|-------|-----------|
| `kit_venta` | Kits comerciales AJTecnology |
| `competencia` | Seguidor de línea, mini sumo, soccer, etc. |
| `comunidad` | Demos para redes + vitrina web |
| `curso_modulo` | Proyectos del programa educativo por módulo |
| `club_bandera` | EcoGrow, SmartVest (repos dedicados) |
| `escolar` | Proyectos escolares / estudiantes |
| `laboratorio` | Prototipos internos del taller |

## Estados (`status`)

`planificado` · `borrador` · `activo` · `vendible` · `competicion` · `archivado`

## Alta de proyecto nuevo

1. Copiar `proyectos/_plantilla/` → `proyectos/{categoria}/{slug}/`
2. Completar `FICHA.md` y `README.md`
3. Añadir entrada en `catalog/projects.json`
4. Si es bandera o repo aparte: actualizar `GITHUB_PROYECTOS_AJENZA.md` en SSD AJENZA
5. Commit en repo `Robotica` (+ push cuando John apruebe)

## Agente y MCP (workspace AJENZA)

- Agente: `08_AJTecnology_Operacion` (proyectos robótica)
- MCP: `ajenza-robotics-projects` — listar/buscar catálogo y estado git
- Skill: `.cursor/skills/robotics-projects-ajt/`
- Procedimiento: `00_AJENZA_Agentes/08_AJTecnology_Operacion/procedimientos/catalogo_proyectos_robotica.md`
